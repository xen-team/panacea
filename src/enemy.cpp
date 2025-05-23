#include "enemy.hpp"

#include <utility>
#include "application.hpp"
#include "data/mesh.hpp"
#include "data/mesh_format.hpp"
#include "data/wav_format.hpp"
#include "physics/colliders/capsule_collider.hpp"
#include "render/render_system.hpp"

namespace {
constexpr std::string_view empty_script = R"(
local damage = 15.0
local hit_radius = 1.8
local hit_cooldown = 1.5
local last_hit_time = 0.0
local move_speed = 5.5
local move_speed_modifier = 0.02
local attack_sound

function setup()
    attack_sound = this:get_sound()
end

function update(time_info)
    local player = this:get_linked_world():get_player()
	local player_pos = player:get_transform().position
    local pos = this:get_transform().position

    local move_direction = (player_pos - pos):normalize()

    local rot = this:get_transform().rotation
    local look_rotation = rot.look_at(move_direction, Vector3f.Up)
    this:get_transform().rotation = look_rotation
    
    this:get_kinematic():set_walk_direction(move_direction * move_speed_modifier * move_speed)

    if last_hit_time > hit_cooldown and pos:distance(player_pos) < hit_radius then
        attack_sound:play()

        local player_health = player:get_health()
        player:get_health():apply_damage(damage)
        last_hit_time = 0.0
    end

    last_hit_time = last_hit_time + time_info.delta_time
end)";
}

namespace panacea {
using namespace xen;
struct SharedEnemyData {
    AudioData hit_sound_data;
};
static auto& get_shared_enemy_data()
{
    static SharedEnemyData data{.hit_sound_data = xen::WavFormat::load("assets/sounds/monster_attack.wav")};
    return data;
}

Enemy::Enemy(World& world, Vector3f const& position, EnemySettings settings) :
    world(world), settings(std::move(settings)), entity(world.add_entity()),
    transform(entity.add_component<Transform>(position)),
    kinematic_character(entity.add_component<KinematicCharacter>(
        std::make_unique<CapsuleCollider>(this->settings.collider_radius, this->settings.height), 4.f, 0.1f
    )),
    mesh_renderer(entity.add_component<MeshRenderer>(MeshFormat::load(this->settings.model_path).second)),
    script(entity.add_component<LuaScript>(empty_script.data())),
    health(entity.add_component<Health>(this->settings.max_health)),
    hit_sound(entity.add_component<Sound>(get_shared_enemy_data().hit_sound_data))
{
}

Enemy::~Enemy()
{
    destroy();
}

void Enemy::destroy()
{
    world.remove_entity(entity);
}
}