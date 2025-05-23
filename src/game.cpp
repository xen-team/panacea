#include "game.hpp"
#include "data/image.hpp"
#include "data/image_format.hpp"
#include "hud.hpp"
#include "physics/colliders/capsule_collider.hpp"
#include <physics/rigidbody.hpp>
#include <xen2.hpp>

namespace {
constexpr std::string_view fog_shader_source = {
#include "fog.frag.embed"
};
}

namespace panacea {
Game::Game() :
    app(Application::get()), world(app.add_world(MAX_ENTITIES_COUNT)),
    render_system(
        world.add_system<RenderSystem>(Vector2ui(1920u, 1080u), GAME_NAME, WindowSetting::MAXIMIZED, AA_SAMPLES_COUNT)
    ),
    window(render_system.get_window()), physics_system(world.add_system<PhysicsSystem>()),
    script_system(world.add_system<ScriptSystem>()), audio_system(world.add_system<AudioSystem>()),
    player(window, world), paused(true)
{

    setup_render();
    setup_window();
    setup_physics();
    world.set_player(player.get_entity());
    player.pause();
    physics_system.pause();
    script_system.pause();

    leaderboard.fetch_leaderboard();

    player.get_health().on_death.connect([&]() {
        game_ends = true;
        main_menu->set_page(MainMenu::Pages::GAME_OVER);
        open_menu();
    });
}

void Game::load_scene(std::unique_ptr<Scene>&& scene)
{
    if (active_scene != nullptr) {
        active_scene->unload();
    }
    active_scene = std::move(scene);
    active_scene->load(player, world);
    active_scene->pause();

    active_scene->on_boss_death.connect([&]() {
        auto calculate_player_score = [](float kill_score, float time_score) {
            constexpr float scores_per_kill = 100.f;
            constexpr float time_weight = 600.f;
            constexpr float time_offset = 600.f;

            float score_for_kills = kill_score * scores_per_kill;

            float time_multiplier = time_weight / (time_score + time_offset);

            float total_score = score_for_kills * time_multiplier;
            return total_score;
        };

        game_ends = true;
        main_menu->set_game_score(calculate_player_score(active_scene->get_kill_score(), time_score));
        main_menu->set_page(MainMenu::Pages::GAME_END);
        open_menu();
    });
}

void Game::run()
{
    if (!started) {
        render_system.update_lights();
        render_system.update_materials();
        render_system.update_shaders();

        world.refresh();
        physics_system.start();
        started = true;
    }

    app.run([&](FrameTimeInfo const& time_info) {
        constexpr float pixelization_speed = 10.f;

        float normalized_delta_time = pixelization_speed * time_info.delta_time;
        current_pixelization = std::lerp(current_pixelization, target_pixelization, normalized_delta_time);

        if (current_pixelization != last_pixelization) {
            pixelization->set_strength(current_pixelization);
        }

        last_pixelization = current_pixelization;

        if (game_ends) {
            return;
        }

        player.update(time_info);
        active_scene->overall_update(time_info);
        active_scene->update(time_info);
        hud->update(window.get_size(), time_info.delta_time);
        main_menu->update(window.get_size(), time_info.delta_time, active_scene->get_kill_score(), time_score);

        if (!paused) {
            time_score += time_info.delta_time;
        }
    });
}

void Game::setup_window()
{
    window.set_icon(ImageFormat::load("assets/icons/xen_logo_32.png"));

    window.add_key_callback(
        Keyboard::F11,
        [&](float) {
            static bool is_fullscreen = false;
            (is_fullscreen ? window.make_windowed() : window.make_fullscreen());
            is_fullscreen = !is_fullscreen;
        },
        Input::ONCE
    );

    window.set_close_callback([&]() { app.quit(); });

    hud = dynamic_cast<HUD*>(window.get_overlay().add_window(
        std::make_unique<HUD>(player.get_health(), *health_vignette, "HUD", window.get_size())
    ));
    hud->disable();
    player.get_weapon()->on_hit.connect([&]() { hud->show_hitmark(true); });

    main_menu = dynamic_cast<MainMenu*>(
        window.get_overlay().add_window(std::make_unique<MainMenu>(leaderboard, "MainMenu", window.get_size()))
    );

    main_menu->on_continue.connect([&]() {
        if (!game_ends) {
            close_menu();
        }
    });

    main_menu->on_exit.connect([&]() { app.quit(); });

    window.add_key_callback(
        Keyboard::ESCAPE, [](float) {}, Input::ONCE,
        [&]() {
            if (main_menu->is_enabled() && !game_ends) {
                close_menu();
            }
            else {
                open_menu();
            }
        }
    );
}

void Game::setup_render()
{
    window.disable_face_culling();

    Entity& sun = world.add_entity();
    sun.add_component<Light>(LightType::SPOT, Vector3f(0.f, -1.f, 1.f).normalize(), 0.2f, Color::Red);
    sun.add_component<Transform>(Vector3f(0.f, 200.f, 0.f));

    RenderGraph& render_graph = render_system.get_render_graph();
    RenderPass& geometry_pass = render_graph.get_geometry_pass();

    auto const depth_buffer = Texture2D::create(render_system.get_scene_size(), TextureColorspace::DEPTH);
    auto const color_buffer = Texture2D::create(render_system.get_scene_size(), TextureColorspace::RGBA);

    geometry_pass.set_write_depth_texture(depth_buffer);
    geometry_pass.add_write_color_texture(color_buffer, 0);

    RenderPass& fog_pass = render_graph.add_node(FragmentShader::load_from_source(fog_shader_source), "Fog");
    fog_pass.add_read_texture(depth_buffer, "uniSceneBuffers.depth");
    fog_pass.add_read_texture(color_buffer, "uniSceneBuffers.color");
    fog_pass.get_program().set_attribute(sun.get_component<Light>().get_direction(), "uniSunDir");
    fog_pass.get_program().set_attribute(1.f, "uniFogDensity");
    fog_pass.get_program().send_attributes();

    geometry_pass.add_children(fog_pass);

    auto after_fog_color_buffer = Texture2D::create(render_system.get_scene_size(), TextureColorspace::RGBA);
    fog_pass.add_write_color_texture(after_fog_color_buffer, 0);

    auto& bloom = render_graph.add_render_process<Bloom>();
    bloom.set_input_color_buffer(after_fog_color_buffer);
    bloom.set_output_buffer(color_buffer);
    bloom.set_threshold_value(1.f);
    bloom.add_parent(fog_pass);

    auto& vignette = render_graph.add_render_process<xen::Vignette>();
    vignette.set_input_buffer(color_buffer);
    vignette.set_output_buffer(after_fog_color_buffer);
    vignette.add_parent(bloom);

    health_vignette = &render_graph.add_render_process<xen::Vignette>();
    health_vignette->set_input_buffer(after_fog_color_buffer);
    health_vignette->set_output_buffer(color_buffer);
    health_vignette->add_parent(vignette);
    health_vignette->set_color(Color(1.f, 0.2f, 0.2f));
    health_vignette->set_strength(0.f);

    pixelization = &render_graph.add_render_process<xen::Pixelization>();
    pixelization->set_input_buffer(color_buffer);
    pixelization->add_parent(*health_vignette);
    pixelization->set_strength(current_pixelization);
}

void Game::setup_physics() {}

void Game::open_menu()
{
    leaderboard.fetch_leaderboard();
    main_menu->enable();
    hud->disable();
    health_vignette->set_strength(0.f);
    window.show_cursor();
    physics_system.pause();
    script_system.pause();
    player.pause();
    active_scene->pause();
    target_pixelization = 0.95f;
    paused = true;
}
void Game::close_menu()
{
    hud->enable();
    main_menu->disable();
    window.disable_cursor();
    physics_system.unpause();
    script_system.unpause();
    player.unpause();
    active_scene->unpause();
    target_pixelization = 0.85f;
    paused = false;
}
}