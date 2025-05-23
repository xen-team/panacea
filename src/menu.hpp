#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS

#include <utility>

#include "imgui.h"
#include "rocket.hpp"
#include "render/overlay.hpp"
#include "leaderboard.hpp"

namespace panacea {
class MainMenu : public xen::OverlayWindow {
public:
    rocket::signal<void()> on_continue;
    rocket::signal<void()> on_exit;

    enum class Pages { MAIN, SETTINGS, GAME_OVER, GAME_END };

public:
    explicit MainMenu(
        Leaderboard& leaderboard, std::string title, xen::Vector2f const& init_size = xen::Vector2f(0.f),
        xen::Vector2f const& init_pos = xen::Vector2f(0.f)
    ) : leaderboard{leaderboard}, xen::OverlayWindow(std::move(title), init_size, init_pos), size{init_size} {};

    void render() override;

    void update(xen::Vector2ui const& window_size, float delta_time, float kill_score, float time_time);

    void set_page(Pages new_page) { current_page = new_page; }

    void set_game_score(float game_score) { this->game_score = game_score; }

private:
    Leaderboard& leaderboard;

    xen::Vector2ui size;
    float delta_time;
    bool update_size = true;
    bool game_started = false;
    float game_score = 0.f;
    float time_score = 0.f;
    float kill_score = 0.f;

    Pages current_page = Pages::MAIN;

    ImVec4 active_color = ImColor(210, 103, 103);
    ImVec4 bg_color = ImColor(29, 29, 29);
    float rounding = 0.f;

    char nickname_buf[32] = "\0";
    bool scores_submitted = false;

private:
    bool button(char const* label);
};
}