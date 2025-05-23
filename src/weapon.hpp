#pragma once

#include "audio/sound.hpp"
#include "physics/physics.hpp"
#include "render/mesh_renderer.hpp"
#include "script/lua_script.hpp"
#include <system.hpp>
#include <component.hpp>

namespace xen {
class World;
class FilePath;
}

namespace panacea {
class Player;

struct WeaponData {
    std::string name = "Unknown";

    std::string model_path = "assets/meshes/shotgun.glb";
    std::string shoot_sound_path = "assets/sounds/shotgun.wav";

    float damage = 20.0f;
    float range = 40.0f;
    float fire_rate = 0.7f;
    float impulse_strength = 1.5f;
    xen::Vector3f offset_position = xen::Vector3f(0.62f, -2.4f, -1.5f);
    xen::Quaternion offset_rotation = xen::Quaternion(xen::Vector3f(0.f, 1.35f, 0.f));

    xen::Vector3f recoil_position_offset = xen::Vector3f(0.f, 0.f, 0.25f);
    xen::Quaternion recoil_rotation_offset = xen::Quaternion(xen::Vector3f(0.f, 0.f, 0.4f));
    float recoil_duration = 0.35f;

    xen::Vector3f muzzle_offset = xen::Vector3f(2.2f, 1.71f, 0.1f);
    xen::Color muzzle_light_color = xen::Color(0xff8200);
    float muzzle_light_intensity = 1.f;
    float muzzle_light_duration = 0.2f;
    float muzzle_scale;

    uint32_t shots_per_fire = 4;
    xen::Radiansf spread_angle = xen::Degreesf(20.f);

    static WeaponData REVOLVER;
    static WeaponData SHOTGUN;
    static WeaponData RIFLE;
};

class PickapableWeapon {
public:
    PickapableWeapon(Player& player, xen::World& world, xen::Vector3f const& pos, WeaponData const& data);
    ~PickapableWeapon();

    void update(xen::FrameTimeInfo const& time_info);
    void destroy();

private:
    WeaponData data;

    Player* player = nullptr;
    xen::World* world = nullptr;
    xen::Entity* entity = nullptr;
    xen::Transform* transform = nullptr;
    xen::MeshRenderer* mesh_renderer = nullptr;
    xen::LuaScript* script = nullptr;

    bool entered_in_radius = false;

private:
    void load(Player& player, xen::World& world, xen::Vector3f const& pos, WeaponData const& data);
    void switch_player_weapon();
};

class Weapon {
    friend class PickapableWeapon;

public:
    rocket::signal<void()> on_hit;

public:
    Weapon(xen::World& world, WeaponData const& data);
    ~Weapon();

    void shoot(
        xen::PhysicsSystem& physics_system, xen::Entity& shooter, xen::Vector3f const& start_pos,
        xen::Vector3f const& direction
    );

    bool can_shoot(float delta_time)
    {
        time_since_last_shot += delta_time;
        return time_since_last_shot >= data.fire_rate;
    }

    void record_shot() { time_since_last_shot = 0.0f; }

    void update_transform(float delta_time, xen::Transform const& camera_world_transform, float camera_pitch_angle);

    WeaponData const& get_data() const { return data; }

    void destroy();

private:
    xen::World& world;
    xen::Entity& entity;
    xen::Transform& transform;

    WeaponData data;
    float time_since_last_shot = 0.f;

    bool is_recoiling = false;
    float recoil_timer = 0.0f;
    xen::Vector3f current_recoil_position_offset = xen::Vector3f::Zero;
    xen::Quaternion current_recoil_rotation_offset = xen::Quaternion::Identity;

    xen::Transform for_lerp_trans;
    xen::Transform last_trans;

    xen::Entity& muzzle_light_entity;
    float muzzle_light_timer = 0.0f;
    bool do_muzzle = false;

    bool appear = true;

    std::unique_ptr<xen::Sound> shoot_sound;
    std::unique_ptr<xen::Sound> hit_enemy_sound;

    xen::AudioData hit_sound_data;
    std::vector<std::unique_ptr<xen::Sound>> hit_sounds;
};
}