#pragma once
#include "../scene.hpp"
#include "../weapon.hpp"

namespace xen {
class Entity;
}

namespace panacea {
class ForestScene : public Scene {
public:
    ForestScene() : ambient_sound(std::make_unique<xen::Sound>(xen::WavFormat::load("assets/sounds/ambient_bass.wav")))
    {
    }
    ~ForestScene();

    void load(Player& player, xen::World& world) override;
    void unload() override;

    void update(xen::FrameTimeInfo const& time_info) override;
    void overall_update(xen::FrameTimeInfo const& time_info) override;

private:
    xen::Entity* map_entity = nullptr;
    std::unique_ptr<xen::Sound> ambient_sound;
    std::vector<PickapableWeapon> pickapable_weapons;

    float current_ambient_volume = 0.25f;

private:
    void load_map() override;
};
}