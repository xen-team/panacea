#include "player.hpp"
#include "data/wav_format.hpp"
#include "settings.hpp"

#include <physics/colliders/capsule_collider.hpp>
#include <render/camera.hpp>
#include <render/render_system.hpp>
#include <application.hpp>
#include <physics/kinematic_character.hpp>
#include <physics/physics.hpp>
#include <entity.hpp>
#include <algorithm>
#include <cmath>

namespace panacea {
using namespace xen;

Player::Player(Window& window, World& world) :
    entity(world.add_entity(true)),
    camera(entity.add_component<Camera>(
        world.get_system<RenderSystem>().get_scene_size(), Vector3f(0.f, player_dimensions.stand_height, 0.f),
        Degreesf(fov_params.base), 0.1f, 100.f
    )),
    transform(entity.add_component<Transform>(Vector3f(0.f, 4.f, 0.f), Quaternion(1.f, 0.f, 0.f, 1.f))),
    listener(entity.add_component<Listener>(transform.get_position())),
    kinematic_character(entity.add_component<KinematicCharacter>(
        std::make_unique<CapsuleCollider>(player_dimensions.capsule_radius, player_dimensions.stand_height), 4.f, 0.1f
    )),
    world_ref(world), health(entity.add_component<Health>(100.f)),
    walk_sound(std::make_unique<Sound>(xen::WavFormat::load("assets/sounds/walk.wav"))),
    jump_sound(std::make_unique<Sound>(xen::WavFormat::load("assets/sounds/jump.wav"))),
    slide_sound(std::make_unique<Sound>(xen::WavFormat::load("assets/sounds/slide.wav"))),
    weapon(std::make_unique<Weapon>(world, WeaponData::REVOLVER))
{
    setup_player_movement(window, world);
    jump_sound->set_gain(0.6f);
    walk_sound->set_gain(0.6f);
    slide_sound->set_gain(0.8f);
}

void Player::setup_player_movement(Window& window, World& world)
{
    window.add_key_callback(Keyboard::W, [&](float) {
        if (paused) {
            return;
        }
        forward_input += 1.f;
    });
    window.add_key_callback(Keyboard::S, [&](float) {
        if (paused) {
            return;
        }
        forward_input -= 1.f;
    });
    window.add_key_callback(Keyboard::A, [&](float) {
        if (paused) {
            return;
        }
        strafe_input += 1.f;
    });
    window.add_key_callback(Keyboard::D, [&](float) {
        if (paused) {
            return;
        }
        strafe_input -= 1.f;
    });
    window.add_key_callback(Keyboard::SPACE, [&](float) {
        if (paused) {
            return;
        }
        jump_input = true;
        jump_buffered = true;
        last_jump_press_time = std::chrono::steady_clock::now();
    });
    window.add_key_callback(Keyboard::LEFT_CTRL, [&](float) {
        if (paused) {
            return;
        }
        crouch_input = true;
    });

    static float camera_pitch(0.f);
    window.set_mouse_move_callback([&](double x_move, double y_move) {
        if (paused) {
            return;
        }

        using namespace Literals;

        float pitch_change = (-90_deg * static_cast<float>(y_move) / window.get_height()).value;
        float yaw_change = (-90_deg * static_cast<float>(x_move) / window.get_width()).value;

        float new_camera_pitch = camera_pitch + pitch_change;

        auto max_pitch_rad = Degreesf(89.0f).value;
        auto min_pitch_rad = Degreesf(-89.0f).value;

        if (new_camera_pitch > max_pitch_rad) {
            pitch_change = max_pitch_rad - camera_pitch;
            new_camera_pitch = max_pitch_rad;
        }
        else if (new_camera_pitch < min_pitch_rad) {
            pitch_change = min_pitch_rad - camera_pitch;
            new_camera_pitch = min_pitch_rad;
        }

        camera_transform.rotate(Degreesf(pitch_change), Degreesf(yaw_change));

        camera_pitch = new_camera_pitch;
    });

    window.add_mouse_button_callback(Mouse::LEFT_CLICK, [&](float) {
        if (paused || !weapon) {
            return;
        }
        weapon->shoot(
            world.get_system<PhysicsSystem>(), entity, transform.get_position() + camera.get_offset(),
            camera_transform.get_rotation() * Vector3f::Forward * -1.0f
        );
    });
}

void Player::update(FrameTimeInfo const& time_info)
{
    if (get_settings().master_volume != current_master_volume) {
        listener.set_gain(get_settings().master_volume);
        current_master_volume = get_settings().master_volume;
    }

    if (paused) {
        return;
    }

    walk_sound->set_position(transform.get_position());
    jump_sound->set_position(transform.get_position());
    slide_sound->set_position(transform.get_position());

    listener.set_position(transform.get_position());
    listener.set_orientation(Matrix3(transform.get_rotation().normalize().to_rotation_matrix()));

    update_buffered_inputs();

    float const delta_time = time_info.delta_time;

    determine_movement_state(delta_time);

    handle_grounded_movement(delta_time);
    handle_jumping_and_bhop(delta_time);
    handle_sliding(delta_time);
    handle_air_control();

    apply_movement_to_kinematic_character(delta_time);

    update_visual_effects(delta_time);

    prev_crouch_state = crouch_input;

    reset_frame_inputs();

    if (weapon) {
        Vector3f player_world_pos = transform.get_position();

        Quaternion player_world_rot = transform.get_rotation();

        Vector3f camera_offset_world = player_world_rot * camera.get_offset();
        Vector3f camera_world_pos = player_world_pos + camera_offset_world;

        Transform camera_world_transform(camera_world_pos, player_world_rot);
        Vector3f camera_forward = camera_transform.get_rotation() * Vector3f::Forward * -1.0f;
        float camera_pitch_angle = std::asin(camera_forward.y);

        weapon->update_transform(delta_time, camera_world_transform, camera_pitch_angle);
    }
}

void Player::update_buffered_inputs()
{
    if (jump_buffered && std::chrono::steady_clock::now() - last_jump_press_time > JUMP_BUFFER_TIME) {
        jump_buffered = false;
    }
}

void Player::reset_frame_inputs()
{
    forward_input = 0.f;
    strafe_input = 0.f;
    jump_input = false;
    crouch_input = false;
}

void Player::determine_movement_state(float delta_time)
{
    current_speed =
        (sprint_input && !crouch_input ? movement_speeds.run :
                                         (crouch_input ? movement_speeds.crouch : movement_speeds.walk));
}

void Player::handle_grounded_movement(float delta_time)
{
    if ((forward_input != 0.f || strafe_input != 0.f) && !walk_sound->is_playing() && !sliding && !jump_input &&
        kinematic_character.is_on_ground()) {
        walk_sound->set_pitch(Math::random(1.05f, 1.15f));
        walk_sound->play();
    }

    transform.set_rotation(transform.get_rotation().lerp(camera_transform.get_rotation(), delta_time * 100.f));

    Vector3f camera_forward_vector = camera_transform.get_rotation() * Vector3f::Forward;
    Vector3f camera_right_vector = camera_transform.get_rotation() * Vector3f::Right;

    camera_forward_vector.y = 0;
    camera_right_vector.y = 0;
    camera_forward_vector = camera_forward_vector.normalize();
    camera_right_vector = camera_right_vector.normalize();

    current_forward_speed = forward_input * current_speed;
    current_strafe_speed = strafe_input * current_speed * movement_speeds.strafe_multiplier;

    final_walk_direction.x =
        camera_forward_vector.x * (-current_forward_speed) + camera_right_vector.x * (-current_strafe_speed);
    final_walk_direction.z =
        camera_forward_vector.z * (-current_forward_speed) + camera_right_vector.z * (-current_strafe_speed);
    final_walk_direction.y = 0;
}

void Player::handle_jumping_and_bhop(float delta_time)
{
    bool just_jumped_this_frame = false;

    if ((jump_input || jump_buffered) && kinematic_character.is_on_ground()) {
        kinematic_character.jump({0.0f, movement_speeds.jump_velocity, 0.0f});
        jump_buffered = false;
        just_jumped_this_frame = true;
    }

    if (kinematic_character.is_on_ground() && !just_jumped_this_frame) {
        bhop_speed_multiplier =
            std::lerp(bhop_speed_multiplier, bhop_params.min_speed_multiplier, bhop_params.decay_rate * delta_time);
        if (bhop_speed_multiplier < bhop_params.min_speed_multiplier + 0.01f) {
            bhop_speed_multiplier = bhop_params.min_speed_multiplier;
        }
    }

    if (just_jumped_this_frame) {
        if (!sliding) {
            jump_sound->set_pitch(Math::random(0.7f, 0.9f));
            jump_sound->play();
        }

        bhop_speed_multiplier *= bhop_params.acceleration_factor;
        bhop_speed_multiplier = std::min(bhop_speed_multiplier, bhop_params.max_speed_multiplier);
        Vector3f bhop_impulse_direction = final_walk_direction;
        if (bhop_impulse_direction.length_squared() < 0.001f) {
            bhop_impulse_direction = camera_transform.get_rotation() * Vector3f::Forward * -1.0f;
        }
        bhop_impulse_direction = bhop_impulse_direction.normalize();

        float impulse_magnitude =
            bhop_params.jump_force_base * (bhop_speed_multiplier - bhop_params.min_speed_multiplier + 1.0f);

        kinematic_character.add_force(
            std::make_unique<Force>(bhop_impulse_direction * impulse_magnitude, bhop_params.impulse_duration)

        );
    }
}

void Player::handle_air_control()
{
    if (!kinematic_character.is_on_ground()) {
        final_walk_direction *= bhop_params.air_control_multiplier;
    }
}

void Player::handle_sliding(float delta_time)
{
    current_horizontal_speed =
        Vector3f(kinematic_character.get_linear_velocity().x, 0.f, kinematic_character.get_linear_velocity().z)
            .length() *
        (1.f / KINEMATIC_WALK_DIRECTION_SCALE);

    bool crouch_pressed_this_frame = crouch_input && !prev_crouch_state;
    if (sprint_input && crouch_pressed_this_frame &&
        current_horizontal_speed > (movement_speeds.walk * slide_params.min_sprint_speed_activation_ratio)) {

        sliding = true;
        slide_timer = slide_params.duration;
        slide_direction =
            Vector3f(kinematic_character.get_linear_velocity().x, 0.f, kinematic_character.get_linear_velocity().z)
                .normalize();
        slide_direction.y = -10.f;
    }

    if (sliding) {
        if (!slide_sound->is_playing()) {
            slide_sound->set_pitch(Math::random(0.9f, 1.f));
            slide_sound->play();
        }

        slide_timer -= delta_time;
        if (slide_timer <= 0.f ||
            current_horizontal_speed < (movement_speeds.walk * slide_params.cancel_speed_threshold_walk_ratio)) {
            sliding = false;
        }
        else {
            Vector3f slide_velocity = slide_direction * (current_speed * slide_params.initial_speed_multiplier);
            slide_velocity *= std::max(0.f, slide_timer / slide_params.duration) * KINEMATIC_WALK_DIRECTION_SCALE;

            kinematic_character.set_linear_velocity({slide_velocity.x, slide_velocity.y, slide_velocity.z});
            crouch_input = true;
        }
    }
}

void Player::apply_movement_to_kinematic_character(float delta_time)
{
    float effective_speed = current_speed * bhop_speed_multiplier;

    Vector3f walk_dir_to_apply = final_walk_direction;
    walk_dir_to_apply *= KINEMATIC_WALK_DIRECTION_SCALE;

    float effective_speed_multiplier = current_speed * bhop_speed_multiplier;

    Vector3f normalized_walk_dir = final_walk_direction;

    if (normalized_walk_dir.length_squared() > 0.001f) {
        normalized_walk_dir = normalized_walk_dir.normalize();
    }

    if (!sliding) {
        kinematic_character.set_walk_direction(
            KINEMATIC_WALK_DIRECTION_SCALE * normalized_walk_dir * effective_speed_multiplier
        );
    }

    float const target_player_height =

        (crouch_input || sliding) ? player_dimensions.get_crouch_height() : player_dimensions.stand_height;

    update_player_collider_height(target_player_height);
}

void Player::update_player_collider_height(float target_height) {}

void Player::update_visual_effects(float delta_time)
{
    current_horizontal_speed =
        Vector3f(kinematic_character.get_linear_velocity().x, 0.f, kinematic_character.get_linear_velocity().z)
            .length() *
        (1.f / KINEMATIC_WALK_DIRECTION_SCALE);

    float target_fov_offset = 0.f;

    if (current_horizontal_speed > movement_speeds.walk * 0.8f) {
        target_fov_offset = fov_params.max_sprint_offset * std::min(
                                                               1.0f, (current_horizontal_speed - movement_speeds.walk) /
                                                                         (movement_speeds.run - movement_speeds.walk)
                                                           );
    }
    else if (crouch_input) {
        target_fov_offset = -5.0f;
    }

    float current_fov = Degreesf(camera.get_fov()).value;
    float target_fov = fov_params.base + target_fov_offset;
    float new_fov = std::lerp(current_fov, target_fov, fov_params.lerp_speed * delta_time);
    camera.set_fov(Degreesf(new_fov));

    Vector3f current_camera_offset = Vector3f::Zero;
    if (kinematic_character.is_on_ground() && current_horizontal_speed > 0.05f) {
        float bob_frequency = 0.f;
        float bob_amplitude_x = 0.f;
        float bob_amplitude_y = 0.f;
        if (sprint_input) {
            bob_frequency = head_bob_params.sprint_frequency;
            bob_amplitude_x = head_bob_params.sprint_amplitude_x;
            bob_amplitude_y = head_bob_params.sprint_amplitude_y;
        }
        else if (crouch_input) {
            bob_frequency = head_bob_params.crouch_frequency;
            bob_amplitude_x = head_bob_params.crouch_amplitude_x;
            bob_amplitude_y = head_bob_params.crouch_amplitude_y;
        }
        else {
            bob_frequency = head_bob_params.walk_frequency;
            bob_amplitude_x = head_bob_params.walk_amplitude_x;
            bob_amplitude_y = head_bob_params.walk_amplitude_y;
        }
        head_bob_timer += bob_frequency * current_horizontal_speed * delta_time;
        current_camera_offset.y = std::sin(head_bob_timer) * bob_amplitude_y;
        current_camera_offset.x = std::cos(head_bob_timer * 0.5f) * bob_amplitude_x;
    }
    else {
        head_bob_timer = std::lerp(head_bob_timer, 0.f, 10.0f * delta_time);
        if (std::abs(head_bob_timer) < 0.01f) {
            head_bob_timer = 0.f;
        }
    }
    camera.set_offset(
        Vector3f(
            0.f, (crouch_input || sliding) ? player_dimensions.get_crouch_height() : player_dimensions.stand_height, 0.f
        ) +
        current_camera_offset

    );
}
}