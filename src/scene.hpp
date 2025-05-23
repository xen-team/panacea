#pragma once

#include <algorithm>

#include "boss.hpp"
#include "data/wav_format.hpp"
#include "enemy.hpp"
#include "player.hpp"

namespace xen {
class World;
}

namespace panacea {
class Scene {
public:
    struct DeferredEnemy {
        xen::Vector3f spawn_pos = xen::Vector3f(0.f);
        EnemySettings settings = {};
    };
    struct DeferredBoss {
        xen::Vector3f spawn_pos = xen::Vector3f(0.f);
    };

    rocket::signal<void()> on_boss_death;

public:
    Scene() : death_sound_data(xen::Sound(xen::WavFormat::load("assets/sounds/monster_death.wav")).get_data())
    {
        enemies.reserve(50);
        enemies_spawns.reserve(300);
    }
    virtual ~Scene() = default;

    virtual void load(Player& player, xen::World& world) = 0;
    virtual void unload() = 0;

    DeferredEnemy& create_enemy_spawn(xen::Vector3f const& spawn_position, EnemySettings settings = {})
    {
        return enemies_spawns.emplace_back(spawn_position, settings);
    }

    DeferredBoss& create_boss_spawn(xen::Vector3f const& spawn_position)
    {
        return bosses_spawns.emplace_back(spawn_position);
    }

    void spawn_enemy(DeferredEnemy&& deferred_enemy)
    {

        Enemy& enemy =
            *enemies.emplace_back(std::make_unique<Enemy>(*world, deferred_enemy.spawn_pos, deferred_enemy.settings));
        enemy.get_kinematic().start(enemy.get_transform());
        enemy.get_health().on_death.connect([&]() {
            auto const enemy_transform = enemy.get_transform().get_position();

            auto& death_sound = enemies_death_sounds.emplace_back(std::make_unique<xen::Sound>(death_sound_data));
            death_sound->set_position(enemy_transform);
            death_sound->set_gain(2.f);
            death_sound->play();

            destroy_enemy(&enemy);

            try_spawn_health(enemy_transform);

            kill_score += 1.f;
        });
    }

    void spawn_boss(DeferredBoss&& deferred_boss)
    {

        Boss& boss = *bosses.emplace_back(std::make_unique<Boss>(*world, deferred_boss.spawn_pos));
        boss.get_kinematic().start(boss.get_transform());
        boss.get_health().on_death.connect([&]() {
            auto const boss_transform = boss.get_transform().get_position();

            auto& death_sound = enemies_death_sounds.emplace_back(std::make_unique<xen::Sound>(death_sound_data));
            death_sound->set_position(boss_transform);
            death_sound->set_gain(2.f);
            death_sound->play();

            on_boss_death.invoke();
        });
    }

    void destroy_enemy(Enemy* enemy)
    {
        enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [&enemy](std::unique_ptr<Enemy> const& ptr) {
            return (enemy == ptr.get());
        }));
    }

    void pause() { paused = true; }
    void unpause() { paused = false; }

    virtual void update(xen::FrameTimeInfo const& time_info) = 0;
    virtual void overall_update(xen::FrameTimeInfo const& time_info) = 0;

    void update_enemies(xen::FrameTimeInfo const& time_info)
    {
        constexpr float spawn_range = 80.f;
        auto const& player_pos = world->get_player().get_component<xen::Transform>().get_position();

        auto new_end = std::remove_if(enemies.begin(), enemies.end(), [&](std::unique_ptr<Enemy>& enemy_ptr) {
            auto const& enemy_pos = enemy_ptr->get_transform().get_position();

            if (enemy_pos.distance(player_pos) >= spawn_range) {
                enemies_spawns.emplace_back(enemy_pos, enemy_ptr->get_settings());

                return true;
            }
            return false;
        });

        enemies.erase(new_end, enemies.end());
    }

    void update_bosses(xen::FrameTimeInfo const& time_info)
    {
        constexpr float spawn_range = 100.f;
        auto const& player_pos = world->get_player().get_component<xen::Transform>().get_position();

        auto new_end = std::remove_if(bosses.begin(), bosses.end(), [&](std::unique_ptr<Boss>& boss_ptr) {
            auto const& boss_pos = boss_ptr->get_transform().get_position();

            if (boss_pos.distance(player_pos) >= spawn_range) {
                bosses_spawns.emplace_back(boss_pos);

                return true;
            }
            return false;
        });

        bosses.erase(new_end, bosses.end());
    }

    float get_kill_score() const { return kill_score; }

protected:
    std::vector<xen::Entity*> entities;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<DeferredEnemy> enemies_spawns;

    std::vector<std::unique_ptr<Boss>> bosses;
    std::vector<DeferredBoss> bosses_spawns;

    xen::AudioData death_sound_data;
    std::vector<std::unique_ptr<xen::Sound>> enemies_death_sounds;

    xen::World* world = nullptr;
    float kill_score = 0.f;

    bool paused = false;

    static constexpr float HEALTH_SPHERE_RADIUS = 0.4f;

protected:
    xen::Entity& add_entity()
    {
        auto& entity = world->add_entity();
        entities.push_back(&entity);
        return entity;
    }

    virtual void load_map() = 0;

private:
    void try_spawn_health(xen::Vector3f position);
};
}