#include "scene.hpp"

#include <data/mesh.hpp>
#include <render/mesh_renderer.hpp>

constexpr std::string_view health_entity_script = R"(
local rotation_speed = 90.0
local pickup_radius = 2.0
local health_count = 15.0
local float_height = 0.5
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

    if pos:distance(player_pos) < pickup_radius then
        local player_health = player:get_health()
        if player_health:get() ~= player_health:get_max() then
            player:get_health():apply_heal(health_count)
            this:destroy()
        end
    end
end)";

namespace panacea {
using namespace xen;

void Scene::try_spawn_health(xen::Vector3f position)
{
    if (Math::random(0, 4) == 0) {
        position.y += 1.f;
        auto& health_entity = world->add_entity_with_component<Transform>(position);
        auto& mesh =
            health_entity.add_component<Mesh>(Sphere(Vector3f(0.f), HEALTH_SPHERE_RADIUS), 16, SphereMeshType::ICO);
        auto& mesh_renderer = health_entity.add_component<MeshRenderer>(mesh);
        mesh_renderer.get_materials().front().get_program().set_attribute(
            Color(1.f, 1.f, 1.f, 0.2f), MaterialAttribute::BaseColor
        );
        mesh_renderer.get_materials().front().get_program().set_attribute(
            Vector3f(3.f, 0.02f, 0.02f), MaterialAttribute::Emissive
        );

        auto& script = health_entity.add_component<LuaScript>(health_entity_script.data());
    }
}
}