#pragma once
#include "render/process/vignette.hpp"
#include "settings.hpp"
#include "utils/health.hpp"
#define IMGUI_DEFINE_MATH_OPERATORS

#include <utility>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

#include "render/overlay.hpp"

namespace panacea {
using namespace std::chrono_literals;

class HUD : public xen::OverlayWindow {
public:
    explicit HUD(
        xen::Health& player_health, xen::Vignette& health_vignette, std::string title,
        xen::Vector2f const& init_size = xen::Vector2f(0.f), xen::Vector2f const& init_pos = xen::Vector2f(0.f)
    ) :
        player_health{player_health}, health_vignette{health_vignette}, visible_health{player_health.get()},
        xen::OverlayWindow(std::move(title), init_size, init_pos), size{init_size} {};

    void render() override
    {
        if (!enabled) {
            return;
        }

        if (update_size) {
            ImGui::SetNextWindowSize(ImVec2(current_size.x, current_size.y));
            update_size = false;
        }
        ImGui::SetNextWindowPos(ImVec2(current_pos.x, current_pos.y), ImGuiCond_Once);
        ImGui::Begin(
            title.c_str(), nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground |
                ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove
        );

        constexpr float mark_radius = 4.f;
        auto const circle_pos = current_size * 0.5f - mark_radius;

        auto* drawlist = ImGui::GetWindowDrawList();

        ImVec4 animated_col = ImColor(255, 255, 255, 125);
        if (hitmark) {
            float const anim_modif = last_hitmark_time / hitmark_duration;
            animated_col.y = animated_col.y * 0.5f + animated_col.y * 0.5f * anim_modif;
            animated_col.z = animated_col.z * 0.5f + animated_col.z * 0.5f * anim_modif;
        }

        drawlist->AddCircleFilled(ImVec2(circle_pos.x, circle_pos.y), mark_radius, ImGui::GetColorU32(animated_col));

        if (hitmark) {
            float const hitmark_disappear_progress = (1.f - last_hitmark_time / hitmark_duration);
            drawlist->AddCircle(
                ImVec2(circle_pos.x, circle_pos.y), mark_radius * 2.5f,
                ImColor(255, 125, 125, static_cast<int>(hitmark_disappear_progress * 155)), 0, 2.f
            );

            last_hitmark_time += std::chrono::duration<float>(delta_time);
            if (last_hitmark_time >= hitmark_duration) {
                hitmark = false;
                last_hitmark_time = 0s;
            }
        }

        visible_health = std::lerp(visible_health, player_health.get(), 3.f * delta_time);
        float vignette_clear_modifier = (visible_health / player_health.get_max());
        float vignette_modifier = (1.f - vignette_clear_modifier) * 0.35f;

        health_vignette.set_strength(vignette_modifier);

        if (get_settings().enable_health_bar) {
            constexpr float rect_spacing_y = 25.f;
            constexpr float rect_height = 12.f;
            constexpr float rect_width = 800.f;
            ImVec2 const rect_pos = ImVec2((size.x - rect_width) * 0.5f, size.y - rect_height - rect_spacing_y);
            drawlist->AddRectFilled(rect_pos, rect_pos + ImVec2(rect_width, rect_height), ImColor(29, 29, 29));
            drawlist->AddRectFilled(
                rect_pos, rect_pos + ImVec2(rect_width * vignette_clear_modifier, rect_height), ImColor(250, 103, 103)
            );
        }

        ImGui::End();
    }

    void update(xen::Vector2ui const& window_size, float delta_time)
    {
        if (size != window_size) {
            size = window_size;
        }
        this->delta_time = delta_time;
    }

    void show_hitmark(bool value) { hitmark = value; }

private:
    xen::Vector2ui size;
    xen::Health& player_health;
    xen::Vignette& health_vignette;
    float visible_health;
    float delta_time;
    bool update_size = true;
    bool hitmark = false;
    std::chrono::duration<float> hitmark_duration = 180ms;
    std::chrono::duration<float> last_hitmark_time = 0ms;
};
}