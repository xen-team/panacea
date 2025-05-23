#pragma once

#include "application.hpp"
#include "audio/audio_system.hpp"
#include "menu.hpp"
#include "physics/physics.hpp"
#include "render/process/pixelization.hpp"
#include "render/process/vignette.hpp"
#include "render/render_system.hpp"
#include "player.hpp"
#include "scene.hpp"
#include "script/lua_script.hpp"
#include "script/script_system.hpp"
#include "hud.hpp"
#include "leaderboard.hpp"

#include <memory>

namespace panacea {
using namespace xen;

class Game {
public:
    Game();

    void load_scene(std::unique_ptr<Scene>&& scene);
    void run();

private:
    static constexpr auto GAME_NAME = "panacea";
    static constexpr uint32_t MAX_ENTITIES_COUNT = 100;
    static constexpr uint32_t AA_SAMPLES_COUNT = 4;

    Application& app;
    World& world;

    RenderSystem& render_system;
    PhysicsSystem& physics_system;
    ScriptSystem& script_system;
    AudioSystem& audio_system;

    Window& window;

    std::unique_ptr<Scene> active_scene;

    HUD* hud = nullptr;
    MainMenu* main_menu = nullptr;
    Leaderboard leaderboard;

    xen::Vignette* health_vignette = nullptr;
    xen::Pixelization* pixelization = nullptr;
    float last_pixelization = 0.95f;
    float current_pixelization = 0.95f;
    float target_pixelization = 0.95f;

    float time_score = 0.f;

    Player player;

    bool started = false;
    bool game_ends = false;
    bool paused = false;

private:
    void setup_window();
    void setup_render();
    void setup_physics();

    void open_menu();
    void close_menu();
};
}