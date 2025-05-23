#include "boss.hpp"

#include "application.hpp"
#include "data/mesh.hpp"
#include "data/mesh_format.hpp"
#include "data/wav_format.hpp"
#include "physics/colliders/capsule_collider.hpp"
#include "render/render_system.hpp"

namespace {
constexpr std::string_view boss_script = R"(
        local max_health
        local current_health
        local current_phase = 1
    
        local phase_thresholds = {
            0.60,
            0.40
        }
    
        local melee_damage          = 15.0
        local melee_hit_radius      = 5
        local melee_cooldown        = 1.5
        local last_melee_hit_time   = 0.0
    
        local base_move_speed       = 6.0
        local move_speed_modifier   = 0.02
        local current_move_speed    = base_move_speed
    
        local rotation_speed_degrees = 240.0
        local rad_rotation_speed
    
        local is_charging_attack    = false
        local attack_charge_timer   = 0.0
        local attack_charge_duration = 0.7

        local is_healing_whirlwind  = false
        local healing_whirlwind_timer = 0.0
        local healing_whirlwind_duration = 5.0
        local healing_rate_per_second = 40.0
        local reflected_damage_ratio = 0.4 
        local healing_from_reflected_damage_ratio = 0.2
        local whirlwind_rotation_speed_degrees = 720.0
        local rad_whirlwind_rotation_speed

        -- New variables for Healing Whirlwind cooldown
        local healing_whirlwind_cooldown = 7.0
        local last_healing_whirlwind_end_time = -100.0

        function setup()
            attack_sound = this:get_sound()
            rad_rotation_speed = Radiansf.new(Degreesf.new(rotation_speed_degrees))
            rad_whirlwind_rotation_speed = Radiansf.new(Degreesf.new(whirlwind_rotation_speed_degrees))

            max_health = this:get_health():get_max()
            current_health = this:get_health():get()
        end
    
        function update(time_info)
            local player = this:get_linked_world():get_player()
            local player_health_comp = player:get_health()
            local player_pos = player:get_transform().position
            local pos = this:get_transform().position
            local distance_to_player = pos:distance(player_pos)
    
            local new_health = this:get_health():get()
            if new_health < current_health then
                local damage_dealt = current_health - new_health
                current_health = new_health
                check_phase_change(time_info.global_time)

                if is_healing_whirlwind then
                    player_health_comp:apply_damage(damage_dealt * reflected_damage_ratio)
                    this:get_health():apply_heal(damage_dealt * healing_from_reflected_damage_ratio)
                end
            end
    
            if is_healing_whirlwind then
                this:get_kinematic():set_walk_direction(Vector3f.Zero)
                
                local rotation_amount_radians = rad_whirlwind_rotation_speed.value * time_info.delta_time
                local rotation_quat = Quaternion.new(Vector3f.Up, Radiansf.new(rotation_amount_radians)) 
                this:get_transform().rotation = this:get_transform().rotation * rotation_quat

                this:get_health():apply_heal(healing_rate_per_second * time_info.delta_time)

                healing_whirlwind_timer = healing_whirlwind_timer + time_info.delta_time
                if healing_whirlwind_timer >= healing_whirlwind_duration then
                    is_healing_whirlwind = false
                    last_healing_whirlwind_end_time = time_info.global_time
                    apply_phase_effects() 
                end
            elseif not is_charging_attack then
                local move_direction = (player_pos - pos):normalize()
                this:get_kinematic():set_walk_direction(move_direction * move_speed_modifier * current_move_speed)
    
                local rot = this:get_transform().rotation
                local look_rotation = rot.look_at(move_direction, Vector3f.Up)
                this:get_transform().rotation = rot:slerp(look_rotation, rad_rotation_speed.value * time_info.delta_time)
            else
                this:get_kinematic():set_walk_direction(Vector3f.Zero)
                local move_direction = (player_pos - pos):normalize()
                local rot = this:get_transform().rotation
                local look_rotation = rot.look_at(move_direction, Vector3f.Up)
                this:get_transform().rotation = rot:slerp(look_rotation, rad_rotation_speed.value * time_info.delta_time)
    
                attack_charge_timer = attack_charge_timer + time_info.delta_time
                if attack_charge_timer >= attack_charge_duration then
                    do_melee_attack(player_health_comp)
                    is_charging_attack = false
                    last_melee_hit_time = 0.0
                end
            end
    
            if not is_healing_whirlwind and not is_charging_attack and last_melee_hit_time >= melee_cooldown and distance_to_player < melee_hit_radius then
                is_charging_attack = true
                attack_charge_timer = 0.0
            end
    
            last_melee_hit_time = last_melee_hit_time + time_info.delta_time
        end
    
        function check_phase_change(current_game_time)
            local current_health_ratio = current_health / max_health
    
            if current_phase == 1 and current_health_ratio <= phase_thresholds[1] then
                current_phase = 2
                apply_phase_effects()
            elseif current_phase == 2 
                and current_health_ratio <= phase_thresholds[2] 
                and not is_healing_whirlwind 
                and (current_game_time - last_healing_whirlwind_end_time >= healing_whirlwind_cooldown) then
                start_healing_whirlwind()
            end
        end
    
        function apply_phase_effects()
            if current_phase == 2 then
                current_move_speed = base_move_speed * 1.45
                melee_damage = 30.0
                melee_cooldown = 1.0
                attack_charge_duration = 0.5
            end
        end

        function start_healing_whirlwind()
            is_healing_whirlwind = true
            healing_whirlwind_timer = 0.0
            is_charging_attack = false
        end
    
        function do_melee_attack(player_health_comp)
            if attack_sound then attack_sound:play() end
            player_health_comp:apply_damage(melee_damage)
        end
    )";
}

namespace panacea {
using namespace xen;

Boss::Boss(World& world, Vector3f const& position) :
    world(world), entity(world.add_entity()),
    transform(entity.add_component<Transform>(position, Quaternion::Identity, Vector3f(3.f))),
    kinematic_character(entity.add_component<KinematicCharacter>(std::make_unique<CapsuleCollider>(1.f, 2.f), 4.f, 0.1f)
    ),
    mesh_renderer(entity.add_component<MeshRenderer>(MeshFormat::load("assets/meshes/boss.glb").second)),
    script(entity.add_component<LuaScript>(boss_script.data())), health(entity.add_component<Health>(2500.f)),
    hit_sound(entity.add_component<Sound>(xen::WavFormat::load("assets/sounds/monster_attack.wav")))
{
}

Boss::~Boss()
{
    destroy();
}

void Boss::destroy()
{
    world.remove_entity(entity);
}
}