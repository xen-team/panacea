#pragma once

#include "audio/listener.hpp"
#include "physics/kinematic_character.hpp"
#include "render/camera.hpp"
#include <component.hpp>
#include "render/light.hpp"
#include "utils/health.hpp"
#include "weapon.hpp"
#include <chrono>

namespace xen {
struct FrameTimeInfo;
struct World;
struct KinematicCharacter;
struct Force;
class Window;
class Camera;
class Entity;
class Transform;
};

namespace panacea {

struct HeadBobParams {
    float walk_frequency = 5.0f;
    float walk_amplitude_x = 0.015f;
    float walk_amplitude_y = 0.02f;
    float sprint_frequency = 5.1f;
    float sprint_amplitude_x = 0.015f;
    float sprint_amplitude_y = 0.020f;
    float crouch_frequency = 3.0f;
    float crouch_amplitude_x = 0.005f;
    float crouch_amplitude_y = 0.007f;
    float lerp_speed_return_to_zero = 10.0f;
};

struct BhopParams {
    float acceleration_factor = 1.135f;
    float max_speed_multiplier = 2.5f;
    float decay_rate = 3.f;
    float min_speed_multiplier = 1.0f;
    float impulse_duration = 0.05f;
    float jump_force_base = 5.0f;
    float air_control_multiplier = 0.15f;
};

struct MovementSpeeds {
    float walk = 3.f;
    float run = 5.f;
    float crouch = 1.2f;
    float jump_velocity = 3.1f;
    float strafe_multiplier = 0.8f;
};

struct FovParams {
    float base = 65.0f;
    float max_sprint_offset = 10.0f;
    float crouch_offset = -5.0f;
    float lerp_speed = 5.0f;
};

struct SlideParams {
    float initial_speed_multiplier = 4.6f;
    float duration = 0.8f;
    float cancel_speed_threshold_walk_ratio = 0.5f;
    float min_sprint_speed_activation_ratio = 1.2f;
};

struct Dimensions {
    float stand_height = 0.85f;
    float crouch_height_multiplier = 0.5f;
    float capsule_radius = 0.4f;

    [[nodiscard]] float get_crouch_height() const { return stand_height * crouch_height_multiplier; }
};

constexpr std::chrono::milliseconds JUMP_BUFFER_TIME = std::chrono::milliseconds(200);
constexpr float KINEMATIC_WALK_DIRECTION_SCALE = 0.02f;

class Player : public xen::Component {
    friend class PickapableWeapon;

public:
    Player(xen::Window& window, xen::World& world);
    void update(xen::FrameTimeInfo const& time_info);

    auto& get_entity() { return entity; }
    auto& get_camera() { return camera; }
    auto& get_transform() { return transform; }
    auto& get_kinematic() { return kinematic_character; }
    auto& get_listener() { return listener; }
    auto& get_health() { return health; }
    Weapon* get_weapon() { return weapon.get(); }

    void pause() { paused = true; }
    void unpause() { paused = false; }

private:
    HeadBobParams head_bob_params{};
    BhopParams bhop_params{};
    MovementSpeeds movement_speeds{};
    FovParams fov_params{};
    SlideParams slide_params{};
    Dimensions player_dimensions{};

    xen::Entity& entity;
    xen::Camera& camera;
    xen::Transform& transform;
    xen::KinematicCharacter& kinematic_character;
    xen::World& world_ref;
    xen::Listener& listener;
    xen::Health& health;

    bool paused = false;

    std::unique_ptr<Weapon> weapon = nullptr;
    xen::Transform camera_transform;

    xen::Vector3f final_walk_direction;
    float current_speed;
    float current_horizontal_speed = 0.f;
    float current_forward_speed = 0.f;
    float current_strafe_speed = 0.f;

    float forward_input = 0.f;
    float strafe_input = 0.f;
    bool jump_input = false;
    bool crouch_input = false;
    bool sprint_input = true;
    bool prev_crouch_state = false;
    bool sliding = false;
    float slide_timer = 0.f;
    xen::Vector3f slide_direction = xen::Vector3f::Zero;

    float bhop_speed_multiplier = BhopParams{}.min_speed_multiplier;
    float head_bob_timer = 0.f;
    xen::Vector3f current_velocity = xen::Vector3f::Zero;
    std::chrono::steady_clock::time_point last_jump_press_time;
    bool jump_buffered = false;

    float current_master_volume = 1.f;

    std::unique_ptr<xen::Sound> walk_sound;
    std::unique_ptr<xen::Sound> jump_sound;
    std::unique_ptr<xen::Sound> slide_sound;

private:
    void setup_player_movement(xen::Window& window, xen::World& world);
    void update_buffered_inputs();
    void reset_frame_inputs();
    void determine_movement_state(float delta_time);
    void handle_grounded_movement(float delta_time);
    void handle_jumping_and_bhop(float delta_time);
    void handle_air_control();
    void handle_sliding(float delta_time);
    void apply_movement_to_kinematic_character(float delta_time);
    void update_player_collider_height(float target_height);
    void update_visual_effects(float delta_time);
};
}