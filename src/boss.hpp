#pragma once

#include "audio/sound.hpp"
#include "physics/kinematic_character.hpp"
#include "render/mesh_renderer.hpp"
#include "script/lua_script.hpp"
#include "utils/health.hpp"
#include "world.hpp"
namespace panacea {
class Boss {
public:
    Boss(xen::World& world, xen::Vector3f const& position);
    ~Boss();

    auto& get_entity() { return entity; }
    auto& get_transform() { return transform; }
    auto& get_kinematic() { return kinematic_character; }
    auto& get_mesh_renderer() { return mesh_renderer; }
    auto& get_script() { return script; }
    auto& get_health() { return health; }

    void update(xen::FrameTimeInfo const& time_info);
    void destroy();

private:
    xen::World& world;
    xen::Entity& entity;
    xen::Transform& transform;
    xen::KinematicCharacter& kinematic_character;
    xen::MeshRenderer& mesh_renderer;
    xen::LuaScript& script;
    xen::Sound& hit_sound;
    xen::Health& health;
};
}