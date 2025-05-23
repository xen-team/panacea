#include "menu.hpp"

#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include "rocket.hpp"
#include "animations.hpp"
#include "settings.hpp"

namespace panacea {
using namespace std::chrono_literals;
bool MainMenu::button(char const* label)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImDrawList* drawlist = window->DrawList;
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiID const id = window->GetID(label);

    ImVec2 size = ImGui::CalcTextSize(label);
    ImVec2 const pos = window->DC.CursorPos;
    ImRect const bb(pos, pos + size);
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id)) {
        return false;
    }

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

    float const rect_alpha = imtweens::tween(
        {"button_rect", label}, 0.7f, 1.f, imtweens::easing::QuadEaseInOut, 200ms, hovered || held || pressed
    );

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, rect_alpha));
    ImGui::RenderTextClipped(bb.Min, bb.Max, label, nullptr, nullptr, ImVec2(0.025f, 0.5f));
    ImGui::PopStyleColor();

    if (hovered) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    return pressed;
}

void MainMenu::render()
{
    if (!enabled) {
        game_started = true;
        current_page = Pages::MAIN;
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
            ImGuiWindowFlags_NoMove
    );

    constexpr int buttons_count = 3;
    constexpr float buttons_item_spacing = 20.f;
    constexpr float buttons_spacing_x = 220.f;
    constexpr float header_full_height = 200.f;
    ImVec2 const button_size = ImVec2(400, 40);
    float const full_buttons_height = button_size.y * buttons_count + buttons_item_spacing * (buttons_count - 1);

    ImVec2 const first_button_pos =
        ImVec2(buttons_spacing_x, header_full_height * 0.5f + (size.y - full_buttons_height) * 0.5f);

    switch (current_page) {
    case Pages::MAIN: {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, buttons_item_spacing));
        ImGui::SetCursorPos(first_button_pos);
        ImGui::BeginGroup();

        ImGui::GetWindowDrawList()->AddText(
            xen::Overlay::get_fonts()[1], 84.f, ImVec2(buttons_spacing_x, first_button_pos.y - header_full_height),
            ImColor(255, 255, 255), "panacea"
        );

        if (button(game_started ? "Continue" : "Start Game")) {
            on_continue.invoke();
        }

        if (button("Settings")) {
            current_page = Pages::SETTINGS;
        }
        if (button("Exit")) {
            on_exit.invoke();
        }
        ImGui::EndGroup();
        ImGui::PopStyleVar();

        constexpr float scores_per_kill = 100.f;
        constexpr float time_weight = 800.f;
        constexpr float time_offset = 600.f;

        float score_for_kills = kill_score * scores_per_kill;

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.4f));
        ImGui::RenderText(
            first_button_pos + ImVec2(0, full_buttons_height + buttons_spacing_x),
            ("Kill score: " + std::to_string(static_cast<int>(score_for_kills))).c_str()
        );

        float time_multiplier = time_weight / (time_score + time_offset);

        ImGui::RenderText(
            first_button_pos + ImVec2(0, full_buttons_height + buttons_spacing_x + 30.f),
            ("Time multiplier on score: " + std::to_string(time_multiplier)).c_str()
        );
        ImGui::PopStyleColor();

        ImVec2 const leaderboard_size = ImVec2(600.f, 800.f);
        ImVec2 const leaderboard_pos =
            ImVec2(size.x - buttons_spacing_x - leaderboard_size.x, (size.y - leaderboard_size.y) * 0.5f);

        ImGui::GetWindowDrawList()->AddText(
            xen::Overlay::get_fonts()[1], 32.f, leaderboard_pos, ImColor(255, 255, 255), "Leaderboard"
        );

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, buttons_item_spacing));
        ImGui::SetCursorPos(leaderboard_pos + ImVec2(0, 60.f));
        ImGui::BeginChild("Leaderboard", leaderboard_size - ImVec2(0, 60.f));
        ImGui::Separator();

        auto const leaderboard_entry_count = leaderboard.get_leaderboard().size();
        for (size_t i = 0; i < ImMin(10ul, leaderboard_entry_count); i++) {
            auto const& entry = leaderboard.get_leaderboard()[i];

            ImColor text_color = ImColor(255, 255, 255);

            switch (i) {
            case 0:
                text_color = ImColor(255, 27, 27);
                break;
            case 1:
                text_color = ImColor(255, 113, 27);
                break;
            case 2:
                text_color = ImColor(255, 198, 27);
                break;
            }

            ImGui::TextColored(text_color, entry.name.c_str());
            auto const score_size = ImGui::CalcTextSize(std::to_string(entry.score).c_str());
            ImGui::SameLine(600.f - score_size.x);
            ImGui::TextColored(active_color, "%i", entry.score);
            ImGui::Separator();
        }

        ImGui::EndChild();
        ImGui::PopStyleVar();
    } break;
    case Pages::SETTINGS: {
        ImGui::SetCursorPos(ImVec2(buttons_spacing_x, first_button_pos.y - 60.f));
        if (button("Back to menu")) {
            current_page = Pages::MAIN;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, buttons_item_spacing));
        ImGui::SetCursorPos(first_button_pos);
        ImGui::BeginChild("Settings", ImVec2(800.f, full_buttons_height));

        ImGui::GetWindowDrawList()->AddText(
            ImVec2(buttons_spacing_x, first_button_pos.y - header_full_height), ImColor(255, 255, 255), "panacea"
        );

        ImGui::SliderFloat("Master Volume", &get_settings().master_volume, 0.f, 1.f);
        ImGui::SliderFloat("Ambient Volume", &get_settings().ambient_volume, 0.f, 1.f);
        ImGui::Checkbox("Enable Health Bar", &get_settings().enable_health_bar);

        ImGui::EndChild();
        ImGui::PopStyleVar();
    } break;
    case Pages::GAME_OVER: {
        ImGui::GetWindowDrawList()->AddText(
            xen::Overlay::get_fonts()[1], 84.f, ImVec2(buttons_spacing_x, first_button_pos.y - header_full_height),
            ImColor(255, 255, 255), "panacea"
        );

        ImGui::RenderTextClipped({0, 0}, ImVec2(size.x, size.y), "Game Over", nullptr, nullptr, {0.5f, 0.45f});

        auto const exit_button_size = ImGui::CalcTextSize("Exit");
        ImGui::SetCursorPos((ImVec2(size.x, size.y) - exit_button_size) * 0.5f + ImVec2(0, 20.f));
        if (button("Exit")) {
            on_exit.invoke();
        }
    } break;
    case Pages::GAME_END: {
        ImGui::GetWindowDrawList()->AddText(
            xen::Overlay::get_fonts()[1], 84.f, ImVec2(buttons_spacing_x, first_button_pos.y - header_full_height),
            ImColor(255, 255, 255), "panacea"
        );

        ImGui::RenderTextClipped({0, 0}, ImVec2(size.x, size.y), "You win!", nullptr, nullptr, {0.5f, 0.45f});

        ImVec2 const input_size = ImVec2(200.f, 30.f);
        ImVec2 const input_pos = (ImVec2(size.x, size.y) - input_size) * 0.5f + ImVec2(0, 20.f);
        auto const submit_button_size = ImGui::CalcTextSize("Submit");

        if (!scores_submitted) {
            ImGui::SetCursorPos(input_pos);
            ImGui::SetNextItemWidth(input_size.x);
            ImGui::InputTextWithHint("##", "Nickname", nickname_buf, IM_ARRAYSIZE(nickname_buf));

            ImGui::SetCursorPos(ImVec2((size.x - submit_button_size.x) * 0.5f, input_pos.y + input_size.y + 20.f));
            if (button("Submit")) {
                leaderboard.submit(std::string(nickname_buf), static_cast<int>(game_score));
                leaderboard.fetch_leaderboard();
                scores_submitted = true;
            }
        }

        auto const exit_button_size = ImGui::CalcTextSize("Exit");
        ImGui::SetCursorPos(ImVec2(
            (size.x - exit_button_size.x) * 0.5f, input_pos.y + input_size.y + 20.f + submit_button_size.y + 20.f
        ));
        if (button("Exit")) {
            on_exit.invoke();
        }

        ImVec2 const leaderboard_size = ImVec2(600.f, 800.f);
        ImVec2 const leaderboard_pos =
            ImVec2(size.x - buttons_spacing_x - leaderboard_size.x, (size.y - leaderboard_size.y) * 0.5f);

        ImGui::GetWindowDrawList()->AddText(
            xen::Overlay::get_fonts()[1], 32.f, leaderboard_pos, ImColor(255, 255, 255), "Leaderboard"
        );

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, buttons_item_spacing));
        ImGui::SetCursorPos(leaderboard_pos + ImVec2(0, 60.f));
        ImGui::BeginChild("Leaderboard", leaderboard_size - ImVec2(0, 60.f));
        ImGui::Separator();

        auto const leaderboard_entry_count = leaderboard.get_leaderboard().size();
        for (size_t i = 0; i < ImMin(10ul, leaderboard_entry_count); i++) {
            auto const& entry = leaderboard.get_leaderboard()[i];

            ImColor text_color = ImColor(255, 255, 255);

            switch (i) {
            case 0:
                text_color = ImColor(255, 27, 27);
                break;
            case 1:
                text_color = ImColor(255, 113, 27);
                break;
            case 2:
                text_color = ImColor(255, 198, 27);
                break;
            }

            ImGui::TextColored(text_color, entry.name.c_str());
            auto const score_size = ImGui::CalcTextSize(std::to_string(entry.score).c_str());
            ImGui::SameLine(600.f - score_size.x);
            ImGui::TextColored(active_color, "%i", entry.score);
            ImGui::Separator();
        }

        ImGui::EndChild();
        ImGui::PopStyleVar();
    } break;
    }

    ImGui::End();
}

void MainMenu::update(xen::Vector2ui const& window_size, float delta_time, float kill_score, float time_score)
{
    if (size != window_size) {
        size = window_size;
    }
    this->delta_time = delta_time;
    this->kill_score = kill_score;
    this->time_score = time_score;
}
}