#include "weapon.hpp"

#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <utility>
#include "data/gltf_format.hpp"
#include "data/mesh.hpp"
#include "data/mesh_format.hpp"
#include "data/wav_format.hpp"
#include "physics/rigidbody.hpp"
#include "render/light.hpp"
#include "render/render_system.hpp"
#include "utils/health.hpp"
#include "world.hpp"
#include "player.hpp"

namespace panacea {
using namespace xen;
WeaponData WeaponData::SHOTGUN{
    .name = "Shotgun",

    .model_path = "assets/meshes/shotgun.glb",
    .shoot_sound_path = "assets/sounds/shotgun.wav",

    .damage = 20.0f,
    .range = 30.0f,
    .fire_rate = 0.8f,
    .offset_position = xen::Vector3f(0.62f, -2.4f, -1.5f),
    .offset_rotation = xen::Quaternion(xen::Vector3f(0.f, 1.35f, 0.f)),

    .recoil_position_offset = xen::Vector3f(0.f, 0.f, 0.25f),
    .recoil_rotation_offset = xen::Quaternion(xen::Vector3f(0.f, 0.f, 0.4f)),
    .recoil_duration = 0.35f,

    .muzzle_offset = xen::Vector3f(2.2f, 1.71f, 0.f),
    .muzzle_light_color = xen::Color(0xff8200),
    .muzzle_light_intensity = 1.f,
    .muzzle_light_duration = 0.2f,
    .muzzle_scale = 1.f,

    .shots_per_fire = 5,
    .spread_angle = xen::Degreesf(35.f),
};

WeaponData WeaponData::REVOLVER{
    .name = "Revolver",

    .model_path = "assets/meshes/revolver.glb",
    .shoot_sound_path = "assets/sounds/revolver.wav",

    .damage = 20.0f,
    .range = 40.0f,
    .fire_rate = 0.4f,
    .offset_position = xen::Vector3f(0.62f, -2.2f, -1.8f),
    .offset_rotation = xen::Quaternion(xen::Vector3f(0.f, 1.5f, 0.f)),

    .recoil_position_offset = xen::Vector3f(0.f, 0.f, 0.25f),
    .recoil_rotation_offset = xen::Quaternion(xen::Vector3f(0.f, 0.f, 0.4f)),
    .recoil_duration = 0.35f,

    .muzzle_offset = xen::Vector3f(2.2f, 1.45f, 0.f),
    .muzzle_light_color = xen::Color(0xff8200),
    .muzzle_light_intensity = 1.f,
    .muzzle_light_duration = 0.2f,
    .muzzle_scale = 0.6f,

    .shots_per_fire = 1,
    .spread_angle = xen::Degreesf(0.f),
};
WeaponData WeaponData::RIFLE{
    .name = "Rifle",

    .model_path = "assets/meshes/rifle.glb",
    .shoot_sound_path = "assets/sounds/rifle.wav",

    .damage = 20.0f,
    .range = 80.0f,
    .fire_rate = 0.2f,
    .offset_position = xen::Vector3f(0.62f, -3.6f, -1.f),
    .offset_rotation = xen::Quaternion(xen::Vector3f(0.f, 1.5f, 0.f)),

    .recoil_position_offset = xen::Vector3f(0.f, 0.f, 0.25f),
    .recoil_rotation_offset = xen::Quaternion(xen::Vector3f(0.f, 0.f, 0.2f)),
    .recoil_duration = 0.15f,

    .muzzle_offset = xen::Vector3f(2.2f, 3.f, 0.f),
    .muzzle_light_color = xen::Color(0xff8200),
    .muzzle_light_intensity = 1.f,
    .muzzle_light_duration = 0.12f,
    .muzzle_scale = 0.6f,

    .shots_per_fire = 1,
    .spread_angle = xen::Degreesf(0.f),
};

constexpr std::string_view pickapable_weapon_script = R"(
    local rotation_speed = 90.0
    local float_height = 0.2
    local float_speed = 2.0
    
    local rad_rotation_speed = Radiansf.new(Degreesf.new(rotation_speed))
    local start_pos
    
    function setup()
        start_pos = this:get_transform().position
    end
    
    function update(time_info)
        local player = this:get_linked_world():get_player()
        local player_pos = player:get_transform().position
        local pos = this:get_transform().position
    
        local vertical_offset = float_height * math.sin(time_info.global_time * float_speed)
        this:get_transform().position = start_pos + Vector3f.new(0.0, vertical_offset, 0.0)
    
        local angle_increment = rad_rotation_speed * time_info.delta_time
        this:get_transform():rotate(Quaternion.new(Vector3f.Up, angle_increment))
    end)";

PickapableWeapon::PickapableWeapon(Player& player, World& world, xen::Vector3f const& pos, WeaponData const& data)
{
    load(player, world, pos, data);
}

PickapableWeapon::~PickapableWeapon()
{
    destroy();
}

void PickapableWeapon::load(Player& player, xen::World& world, xen::Vector3f const& pos, WeaponData const& data)
{
    this->player = &player;
    this->world = &world;
    entity = &world.add_entity();
    transform = &entity->add_component<Transform>(pos);
    mesh_renderer = &entity->add_component<MeshRenderer>(MeshFormat::load(data.model_path).second);
    this->data = data;
    script = &entity->add_component<xen::LuaScript>(pickapable_weapon_script.data());
}

void PickapableWeapon::destroy()
{
    world->remove_entity(*entity);
}

void PickapableWeapon::update(xen::FrameTimeInfo const&)
{
    constexpr float pickup_radius = 4.5f;

    auto const& player_pos = world->get_player().get_component<Transform>().get_position();
    bool const player_is_near = (transform->get_position().distance(player_pos) < pickup_radius);

    if (!entered_in_radius && player_is_near) {
        switch_player_weapon();
        entered_in_radius = true;
    }
    else if (!player_is_near) {
        entered_in_radius = false;
    }
}

void PickapableWeapon::switch_player_weapon()
{
    auto player_weapon = std::move(player->weapon);

    player->weapon = std::make_unique<Weapon>(*world, std::move(data));

    destroy();
    load(*player, *world, transform->get_position(), player_weapon->data);
}

Weapon::Weapon(World& world, WeaponData const& data) :
    world(world), entity(world.add_entity()), transform(entity.add_component<Transform>()), data(data),
    muzzle_light_entity(world.add_entity_with_component<Transform>(Vector3f(0.f), Quaternion::Identity, Vector3f(0.f))),
    shoot_sound(std::make_unique<Sound>(xen::WavFormat::load(data.shoot_sound_path))),
    hit_enemy_sound(std::make_unique<Sound>(xen::WavFormat::load("assets/sounds/hit_enemy.wav"))),
    hit_sound_data(Sound(xen::WavFormat::load("assets/sounds/bullet_hit.wav")).get_data()),
    time_since_last_shot(data.fire_rate)
{
    auto& mesh_renderer = entity.add_component<MeshRenderer>(MeshFormat::load(data.model_path).second);
    mesh_renderer.set_skip_depth(true);
    muzzle_light_entity.add_component<Light>(
        LightType::SPOT, Vector3f(1.f), 0.f, Radiansf(Degreesf(-90.f)), data.muzzle_light_color
    );
    auto& muzzle_mesh_renderer =
        muzzle_light_entity.add_component<MeshRenderer>(MeshFormat::load("assets/meshes/muzzle.obj").second);
    muzzle_mesh_renderer.set_skip_depth(true);

    shoot_sound->set_gain(2.f);

    hit_sounds.reserve(data.shots_per_fire);
}

Weapon::~Weapon()
{
    destroy();
}

void Weapon::destroy()
{
    world.remove_entity(entity);
    world.remove_entity(muzzle_light_entity);
}

void Weapon::shoot(
    xen::PhysicsSystem& physics_system, xen::Entity& shooter, xen::Vector3f const& start_pos,
    xen::Vector3f const& direction
)
{
    if (time_since_last_shot < data.fire_rate) {
        return;
    }

    bool hit_enemy_sound_played = false;

    for (int i = 0; i < data.shots_per_fire; ++i) {
        Radiansf yaw_offset(Math::random(-data.spread_angle.value / 2.0f, data.spread_angle.value / 2.0f));
        Radiansf pitch_offset(Math::random(-data.spread_angle.value / 2.0f, data.spread_angle.value / 2.0f));

        xen::Quaternion spread_rotation = xen::Quaternion(xen::Vector3f(0.0f, 1.0f, 0.0f), yaw_offset) *
                                          xen::Quaternion(xen::Vector3f(1.0f, 0.0f, 0.0f), pitch_offset);

        xen::Vector3f spread_direction = spread_rotation * direction;

        Vector3f end_pos = start_pos + spread_direction * data.range;

        Raycast hit = physics_system.raytest(start_pos, end_pos);

        if (hit.has_hit()) {
            CollisionObject* hit_object = hit.get_collision_object();
            Vector3f const& hit_point = hit.get_point_world();
            if (hit_object) {
                Entity* hit_entity = hit_object->get_entity();

                if (hit_entity && hit_entity != &shooter) {
                    if (hit_entity->has_component<Health>()) {
                        hit_entity->get_component<Health>().apply_damage(data.damage);

                        if (!hit_enemy_sound_played) {
                            hit_enemy_sound->play();
                            hit_enemy_sound->set_position(hit_point);
                            hit_enemy_sound_played = true;
                        }

                        on_hit.invoke();
                    }
                    else {
                        auto& hit_sound = hit_sounds.emplace_back(std::make_unique<Sound>(hit_sound_data));
                        hit_sound->set_position(hit_point);
                        hit_sound->set_gain(0.3f);
                        hit_sound->play();
                    }
                }
            }
        }
        else {
            // Miss
        }
    }

    is_recoiling = true;
    recoil_timer = 0.0f;
    time_since_last_shot = 0.f;

    muzzle_light_timer = 0.0f;
    do_muzzle = true;

    shoot_sound->stop();

    muzzle_light_entity.get_component<Transform>().set_position(start_pos);
    muzzle_light_entity.get_component<Light>().set_direction(direction);
    muzzle_light_entity.get_component<Light>().set_color(data.muzzle_light_color);
}

void Weapon::update_transform(float delta_time, xen::Transform const& camera_world_transform, float camera_pitch_angle)
{
    hit_sounds.erase(
        std::remove_if(hit_sounds.begin(), hit_sounds.end(), [](auto& hit_sound) { return !hit_sound->is_playing(); }),
        hit_sounds.end()
    );

    xen::Vector3f const& camera_pos = camera_world_transform.get_position();
    xen::Quaternion const& camera_rot = camera_world_transform.get_rotation();

    xen::Vector3f adjusted_offset_position = data.offset_position;

    adjusted_offset_position.y += camera_pitch_angle * 0.1f;
    adjusted_offset_position.z += camera_pitch_angle * -0.5f;
    adjusted_offset_position.x += camera_pitch_angle * -0.3f;

    time_since_last_shot += delta_time;

    if (is_recoiling) {
        recoil_timer += delta_time;

        float t = std::min(1.0f, recoil_timer / data.recoil_duration);

        if (t < 0.5f) {
            float sub_t = t * 2.0f;
            float eased_sub_t = 1.0f - (1.0f - sub_t) * (1.0f - sub_t);

            current_recoil_position_offset = xen::Vector3f::Zero.lerp(data.recoil_position_offset, eased_sub_t);
            current_recoil_rotation_offset = xen::Quaternion::Identity.slerp(data.recoil_rotation_offset, eased_sub_t);
        }
        else {
            float sub_t = (t - 0.5f) * 2.0f;
            float eased_sub_t = 1.0f - (1.0f - sub_t) * (1.0f - sub_t);

            current_recoil_position_offset = data.recoil_position_offset.lerp(xen::Vector3f::Zero, eased_sub_t);
            current_recoil_rotation_offset = data.recoil_rotation_offset.slerp(xen::Quaternion::Identity, eased_sub_t);
        }

        if (recoil_timer >= data.recoil_duration) {
            is_recoiling = false;
            current_recoil_position_offset = xen::Vector3f::Zero;
            current_recoil_rotation_offset = xen::Quaternion::Identity;
        }
    }

    adjusted_offset_position += current_recoil_position_offset;

    xen::Quaternion pitch_correction_quat =
        xen::Quaternion(xen::Vector3f::Right, xen::Radiansf(camera_pitch_angle * 0.1f));

    xen::Quaternion weapon_final_rotation =
        camera_rot * data.offset_rotation * pitch_correction_quat * current_recoil_rotation_offset;

    xen::Vector3f weapon_final_position = camera_pos + (camera_rot * adjusted_offset_position);

    if (appear) {
        transform.set_position(weapon_final_position);
        transform.set_rotation(weapon_final_rotation);
        appear = false;
    }

    constexpr float lerp_speed = 20.f;
    transform.set_position(weapon_final_position);
    transform.set_rotation(transform.get_rotation().lerp(weapon_final_rotation, lerp_speed * delta_time));

    if (do_muzzle) {
        muzzle_light_timer += delta_time;

        if (muzzle_light_timer >= data.muzzle_light_duration) {
            muzzle_light_timer = 0.f;
            muzzle_light_entity.get_component<Light>().set_color(Color(0, 0, 0, 0));
            muzzle_light_entity.get_component<Light>().set_energy(0.0f);
            muzzle_light_entity.get_component<Transform>().set_scale(Vector3f(0.0f));
            world.get_system<RenderSystem>().update_lights();
            do_muzzle = false;
        }
        else {
            xen::Quaternion current_weapon_rotation = transform.get_rotation();
            xen::Vector3f current_weapon_position = transform.get_position();

            xen::Quaternion correction_rot = xen::Quaternion(xen::Vector3f::Up, xen::Radiansf(xen::Degreesf(180.0f)));

            xen::Vector3f muzzle_world_pos = current_weapon_position + current_weapon_rotation * data.muzzle_offset;

            muzzle_light_entity.get_component<Transform>().set_position(muzzle_world_pos);

            shoot_sound->set_position(muzzle_world_pos);
            if (!shoot_sound->is_playing()) {
                shoot_sound->set_pitch(Math::random(0.85f, 1.15f));
                shoot_sound->play();
            }

            xen::Vector3f light_direction = weapon_final_rotation * xen::Vector3f::Forward;

            muzzle_light_entity.get_component<Light>().set_direction(light_direction);

            muzzle_light_entity.get_component<Transform>().set_rotation(transform.get_rotation() * correction_rot);

            float const alpha = 1.f - (muzzle_light_timer / data.muzzle_light_duration);
            muzzle_light_entity.get_component<Light>().set_energy(data.muzzle_light_intensity * alpha);

            muzzle_light_entity.get_component<Transform>().set_scale(Vector3f((1.f - alpha) * data.muzzle_scale));

            world.get_system<RenderSystem>().update_lights();
        }
    }
}
}