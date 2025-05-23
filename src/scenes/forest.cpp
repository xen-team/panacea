#include "forest.hpp"
#include "data/mesh_format.hpp"
#include "script/lua_script.hpp"

#include "../settings.hpp"

#include <data/mesh.hpp>
#include <data/gltf_format.hpp>
#include <render/mesh_renderer.hpp>
#include <physics/rigidbody.hpp>
#include <entity.hpp>
#include <world.hpp>

namespace panacea {
using namespace xen;
ForestScene::~ForestScene()
{
    unload();
    // ~Sc
}

void ForestScene::load(Player& player, World& world)
{
    this->world = &world;

    load_map();

    ambient_sound->set_repeat(true);
    ambient_sound->play();

    pickapable_weapons.reserve(2);
    pickapable_weapons.emplace_back(player, world, Vector3f(17.3816f, 0.f, -438.437f), WeaponData::SHOTGUN);
    pickapable_weapons.emplace_back(player, world, Vector3f(-10.0094f, 0.f, -1286.59f), WeaponData::RIFLE);

    create_boss_spawn(Vector3f(-2.6658f, 6.f, -1829.77f));

    // First location
    create_enemy_spawn(
        Vector3f(-24.2564f, 6.f, 12.595f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-24.9912f, 6.f, -13.9287f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-19.4849f, 6.f, 61.2281f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-28.4298f, 6.f, 46.4075f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(2.81945f, 6.f, 40.1981f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-20.4961f, 6.f, -53.7316f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(17.6715f, 6.f, -63.4035f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(27.2775f, 6.f, -61.8725f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(11.9652f, 6.f, -96.092f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-6.17532f, 6.f, -103.58f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(13.2761f, 6.f, -103.081f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-3.34628f, 6.f, -100.731f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-14.0425f, 6.f, -92.4521f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-23.9667f, 6.f, -87.2156f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-30.4478f, 6.f, -98.6186f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-26.8077f, 6.f, -105.891f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-17.8466f, 6.f, -117.678f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(13.8548f, 6.f, -144.006f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(24.8048f, 6.f, -135.244f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(11.1827f, 6.f, -117.688f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-17.7976f, 6.f, -125.238f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-26.0857f, 6.f, -126.017f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-28.8499f, 6.f, -144.535f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-25.0398f, 6.f, -147.838f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-8.85751f, 6.f, -160.58f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-2.76095f, 6.f, -158.34f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(15.404f, 6.f, -155.067f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(6.95774f, 6.f, -154.787f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(15.3429f, 6.f, -156.645f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(28.8547f, 6.f, -163.155f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(5.03912f, 6.f, -176.729f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-9.99977f, 6.f, -182.011f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-16.2423f, 6.f, -174.377f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-5.84178f, 6.f, -171.151f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(3.91579f, 6.f, -167.042f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-24.9656f, 6.f, -179.169f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-20.9387f, 6.f, -189.4f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-14.7382f, 6.f, -200.485f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-6.29997f, 6.f, -203.916f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(6.50163f, 6.f, -203.678f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(13.2984f, 6.f, -208.431f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(17.3321f, 6.f, -215.649f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(28.9071f, 6.f, -218.19f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(9.77916f, 6.f, -221.354f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-0.442411f, 6.f, -229.34f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-10.1868f, 6.f, -231.917f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-18.9729f, 6.f, -234.176f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-27.8094f, 6.f, -237.774f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-23.738f, 6.f, -249.617f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-14.8341f, 6.f, -253.641f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-17.7801f, 6.f, -277.964f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-24.7746f, 6.f, -285.274f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-15.8582f, 6.f, -291.186f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-12.753f, 6.f, -300.996f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-3.73965f, 6.f, -294.164f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-1.65958f, 6.f, -283.626f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(3.89392f, 6.f, -277.846f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(14.4728f, 6.f, -281.71f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(14.972f, 6.f, -293.5f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(28.8449f, 6.f, -296.289f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(25.1536f, 6.f, -286.507f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-9.18064f, 6.f, -289.12f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-11.9582f, 6.f, -323.385f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-10.7981f, 6.f, -332.314f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-4.92853f, 6.f, -335.197f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-0.916434f, 6.f, -341.487f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(6.77899f, 6.f, -347.655f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(17.1853f, 6.f, -347.196f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(20.8812f, 6.f, -339.123f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(27.832f, 6.f, -336.079f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(26.6628f, 6.f, -328.139f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(20.8262f, 6.f, -322.364f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(12.1165f, 6.f, -320.221f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(3.67955f, 6.f, -325.121f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-14.3024f, 6.f, -335.575f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-25.2295f, 6.f, -325.962f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-28.3196f, 6.f, -338.751f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-26.64f, 6.f, -345.604f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-16.5382f, 6.f, -354.421f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-19.6037f, 6.f, -360.776f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-28.9769f, 6.f, -361.962f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-27.6212f, 6.f, -374.112f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(20.7448f, 3.f, -437.145f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-18.0898f, 6.f, -376.966f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-9.59222f, 6.f, -377.811f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-0.328023f, 6.f, -383.653f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(10.7329f, 6.f, -387.398f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(17.6524f, 6.f, -388.516f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(24.3455f, 6.f, -396.158f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(17.0312f, 6.f, -399.f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(9.25621f, 6.f, -402.902f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(3.12492f, 6.f, -409.757f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-5.33222f, 6.f, -408.385f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-14.3201f, 6.f, -407.166f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-23.6721f, 6.f, -404.314f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-29.7458f, 6.f, -409.374f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-29.5867f, 6.f, -419.445f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-20.9538f, 6.f, -424.969f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-13.8292f, 6.f, -427.318f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-4.80582f, 6.f, -428.081f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-1.321f, 6.f, -428.38f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(9.65661f, 6.f, -435.783f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(8.85027f, 6.f, -444.558f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(3.79775f, 6.f, -451.627f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(13.1786f, 6.f, -446.692f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(9.21385f, 6.f, -440.985f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(15.3007f, 6.f, -434.722f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(21.6102f, 6.f, -439.879f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(18.7646f, 6.f, -445.297f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(19.9055f, 6.f, -437.433f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(6.72892f, 6.f, -464.882f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(14.5353f, 6.f, -470.427f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(25.2557f, 6.f, -470.462f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(14.8608f, 6.f, -480.996f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(2.3566f, 6.f, -485.884f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-4.58698f, 6.f, -489.982f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-12.3181f, 6.f, -493.666f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-19.4494f, 6.f, -487.488f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-17.2487f, 6.f, -477.968f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-26.8931f, 6.f, -465.595f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-26.5385f, 6.f, -452.373f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-30.6775f, 6.f, -433.925f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-31.2755f, 6.f, -425.89f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-26.8664f, 6.f, -495.318f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-17.0505f, 6.f, -503.783f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-4.61833f, 6.f, -503.117f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(1.79774f, 6.f, -514.586f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(14.4943f, 6.f, -511.841f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(23.8097f, 6.f, -508.488f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(21.2044f, 6.f, -523.995f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(5.6746f, 6.f, -525.651f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-9.97956f, 6.f, -533.832f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-22.2389f, 6.f, -535.065f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-27.7567f, 6.f, -545.127f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-23.6639f, 6.f, -555.63f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-18.1354f, 6.f, -564.814f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-10.1108f, 6.f, -580.791f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(1.06686f, 6.f, -589.238f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(4.17612f, 6.f, -601.477f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-8.97216f, 6.f, -608.596f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-18.9839f, 6.f, -613.359f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-19.6796f, 6.f, -626.948f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-15.0257f, 6.f, -636.843f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(0.0298142f, 6.f, -640.139f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(9.1464f, 6.f, -647.297f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(16.4908f, 6.f, -660.923f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-0.194136f, 6.f, -661.234f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-12.4381f, 6.f, -671.162f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-21.1782f, 6.f, -681.094f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-17.5186f, 6.f, -696.985f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-2.22774f, 6.f, -705.526f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(9.2434f, 6.f, -711.518f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(19.1917f, 6.f, -726.524f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(10.8416f, 6.f, -737.941f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-2.03355f, 6.f, -742.834f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-13.9739f, 6.f, -743.417f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-23.0133f, 6.f, -751.458f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-31.7193f, 6.f, -767.648f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-28.4961f, 6.f, -779.988f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-20.5166f, 6.f, -783.81f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-16.2863f, 6.f, -797.816f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-2.7447f, 6.f, -806.491f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(12.7977f, 6.f, -807.733f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(20.0297f, 6.f, -815.353f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(15.2338f, 6.f, -822.661f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(3.00395f, 6.f, -825.662f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-14.7164f, 6.f, -829.552f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-27.0194f, 6.f, -838.622f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-17.0517f, 6.f, -851.013f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-3.69134f, 6.f, -859.013f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(4.21519f, 6.f, -866.761f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(21.7171f, 6.f, -871.162f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(21.6014f, 6.f, -884.52f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(13.2396f, 6.f, -891.4f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(3.43221f, 6.f, -898.447f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-8.26725f, 6.f, -904.289f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-17.6515f, 6.f, -905.736f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );
    create_enemy_spawn(
        Vector3f(-28.4278f, 6.f, -900.791f), EnemySettings{.model_path = "assets/meshes/ghost.glb", .max_health = 60.f}
    );

    // Second level
    create_enemy_spawn(
        Vector3f(-10.1161f, 6.f, -1015.55f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-2.38843f, 6.f, -1029.54f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(9.66003f, 6.f, -1034.88f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(23.9392f, 6.f, -1041.16f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(23.3358f, 6.f, -1052.74f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(9.43301f, 6.f, -1058.9f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(1.83134f, 6.f, -1064.35f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-12.8245f, 6.f, -1067.67f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-24.3154f, 6.f, -1064.48f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-26.9784f, 6.f, -1074.47f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-26.6668f, 6.f, -1084.02f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-18.4772f, 6.f, -1091.37f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-7.8827f, 6.f, -1094.62f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(5.28635f, 6.f, -1100.88f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(16.0288f, 6.f, -1110.66f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(10.0894f, 6.f, -1120.77f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(1.33663f, 6.f, -1127.97f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-14.5011f, 6.f, -1130.75f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-22.4418f, 6.f, -1138.49f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-26.6782f, 6.f, -1148.88f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-10.8023f, 6.f, -1154.13f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-20.0047f, 6.f, -1168.85f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-9.31837f, 6.f, -1183.04f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-2.9734f, 6.f, -1190.74f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(2.28997f, 6.f, -1202.38f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(12.2252f, 6.f, -1207.71f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(17.2273f, 6.f, -1197.81f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(25.4513f, 6.f, -1189.3f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(23.0456f, 6.f, -1172.32f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-0.122354f, 6.f, -1208.92f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-24.2992f, 6.f, -1218.68f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-20.7334f, 6.f, -1232.98f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-25.6475f, 6.f, -1240.89f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-24.2945f, 6.f, -1249.68f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-13.8008f, 6.f, -1257.05f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-15.4634f, 6.f, -1265.54f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-17.9046f, 6.f, -1279.16f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-12.1829f, 6.f, -1287.15f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-8.99574f, 6.f, -1295.46f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(0.167831f, 6.f, -1298.07f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(8.9853f, 6.f, -1303.56f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(19.1135f, 6.f, -1302.88f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(26.4611f, 6.f, -1298.57f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(15.4486f, 6.f, -1284.91f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(20.9018f, 6.f, -1278.09f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(27.4417f, 6.f, -1268.81f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(26.1238f, 6.f, -1263.72f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(24.0761f, 6.f, -1254.25f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(26.4197f, 6.f, -1246.13f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(16.6974f, 6.f, -1241.18f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(3.72482f, 6.f, -1252.19f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-8.96155f, 6.f, -1253.76f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-18.1485f, 6.f, -1247.19f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-22.3745f, 6.f, -1263.63f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-4.9546f, 6.f, -1295.68f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-11.2215f, 6.f, -1306.7f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-17.765f, 6.f, -1315.76f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-24.3314f, 6.f, -1318.34f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-23.0784f, 6.f, -1329.27f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-20.8011f, 6.f, -1339.65f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-19.8394f, 6.f, -1348.08f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-8.14404f, 6.f, -1352.58f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-2.28335f, 6.f, -1362.68f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(5.66519f, 6.f, -1370.62f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(14.3887f, 6.f, -1370.39f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(19.0754f, 6.f, -1360.06f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(26.4348f, 6.f, -1355.f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(20.6582f, 6.f, -1339.39f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(25.7158f, 6.f, -1328.44f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-7.1905f, 6.f, -1358.21f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-19.5533f, 6.f, -1365.44f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-29.1582f, 6.f, -1372.42f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-22.0346f, 6.f, -1384.69f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-5.55879f, 6.f, -1394.07f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(4.20624f, 6.f, -1403.68f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(24.4634f, 6.f, -1400.9f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(23.6573f, 6.f, -1415.55f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(9.43756f, 6.f, -1416.86f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-7.4139f, 6.f, -1423.93f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-22.5044f, 6.f, -1426.f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-29.6439f, 6.f, -1434.57f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-22.3078f, 6.f, -1446.42f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-6.63089f, 6.f, -1454.72f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(1.84441f, 6.f, -1460.03f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(20.2769f, 6.f, -1458.6f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(17.9879f, 6.f, -1472.05f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(5.4551f, 6.f, -1485.19f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-12.7292f, 6.f, -1493.62f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-24.6046f, 6.f, -1495.49f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-28.0076f, 6.f, -1508.04f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-24.7176f, 6.f, -1523.24f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-11.0214f, 6.f, -1530.86f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-8.87997f, 6.f, -1540.09f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(4.50139f, 6.f, -1543.04f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(22.3068f, 6.f, -1539.92f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(28.1778f, 6.f, -1556.3f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(15.707f, 6.f, -1569.47f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(1.79202f, 6.f, -1579.21f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-12.4219f, 6.f, -1580.91f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-22.3182f, 6.f, -1594.4f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-21.3186f, 6.f, -1610.5f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-26.4875f, 6.f, -1622.27f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-23.7119f, 6.f, -1633.98f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-0.735491f, 6.f, -1638.38f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(16.2342f, 6.f, -1646.72f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(27.3179f, 6.f, -1661.26f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(10.464f, 6.f, -1674.62f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-4.85058f, 6.f, -1675.43f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-15.7067f, 6.f, -1680.29f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-22.6943f, 6.f, -1692.53f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-26.7121f, 6.f, -1705.93f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
    create_enemy_spawn(
        Vector3f(-13.2983f, 6.f, -1721.73f), EnemySettings{.model_path = "assets/meshes/moth.glb", .max_health = 100.f}
    );
}

void ForestScene::unload()
{
    for (auto& entity : entities) {
        world->remove_entity(*entity);
    }

    entities.clear();
    bosses.clear();
    pickapable_weapons.clear();
    enemies.clear();
    enemies_spawns.clear();
    bosses_spawns.clear();
    enemies_death_sounds.clear();
}

void ForestScene::update(xen::FrameTimeInfo const& time_info)
{
    if (paused) {
        return;
    }

    for (auto& pickapable_weapon : pickapable_weapons) {
        pickapable_weapon.update(time_info);
    }

    auto const& player_pos = world->get_player().get_component<Transform>().get_position();
    {
        constexpr float spawn_range = 80.f;
        auto new_end = std::remove_if(enemies_spawns.begin(), enemies_spawns.end(), [&](DeferredEnemy& enemy_spawn) {
            if (enemy_spawn.spawn_pos.distance(player_pos) < spawn_range) {
                spawn_enemy(std::move(enemy_spawn));
                return true;
            }
            return false;
        });

        enemies_spawns.erase(new_end, enemies_spawns.end());
    }
    {
        constexpr float spawn_range = 100.f;
        auto new_end = std::remove_if(bosses_spawns.begin(), bosses_spawns.end(), [&](DeferredBoss& boss_spawn) {
            if (boss_spawn.spawn_pos.distance(player_pos) < spawn_range) {
                spawn_boss(std::move(boss_spawn));
                ambient_sound->load(WavFormat::load("assets/sounds/boss_music.wav"));
                ambient_sound->set_repeat(true);
                ambient_sound->play();
                return true;
            }
            return false;
        });

        bosses_spawns.erase(new_end, bosses_spawns.end());
    }

    enemies_death_sounds.erase(
        std::remove_if(
            enemies_death_sounds.begin(), enemies_death_sounds.end(),
            [](auto& death_sound) { return !death_sound->is_playing(); }
        ),
        enemies_death_sounds.end()
    );

    update_enemies(time_info);
    update_bosses(time_info);
}

void ForestScene::overall_update(xen::FrameTimeInfo const&)
{
    if (get_settings().ambient_volume != current_ambient_volume) {
        ambient_sound->set_gain(get_settings().ambient_volume);
        current_ambient_volume = get_settings().ambient_volume;
    }
}

void ForestScene::load_map()
{
    map_entity = &add_entity();
    map_entity->add_component<xen::Transform>();

    auto [map_mesh, map_mesh_renderer] = MeshFormat::load("assets/meshes/forest.glb");

    GltfFormat::create_map_rigidbody_from_mesh(*map_entity, map_mesh);

    map_entity->add_component<xen::MeshRenderer>(std::move(map_mesh_renderer));
}
}