#include "UI.h"
#include "Assets.h"
#include "Core.h"
#include "Event.h"
#include "UIStyle.h"

#include "ncengine/ui/ImGuiUtility.h"
#include "ncengine/window/Window.h"

namespace
{
constexpr auto g_menuSize = ImVec2{300.0f, 145.0f};
constexpr auto g_endGameMenuSize = ImVec2{300.0f, 100.0f};
constexpr auto g_menuButtonSize = ImVec2{285.0f, 40.0f};
constexpr auto g_windowFlags = ImGuiWindowFlags_NoCollapse |
                               ImGuiWindowFlags_NoTitleBar |
                               ImGuiWindowFlags_NoResize;
} // anonymous namespace

namespace game::ui
{
GameUI::GameUI(nc::NcEngine* engine, nc::Signal<DialogEvent>& onDialog)
    : m_stopEngine{[engine](){ engine->Stop(); }},
      m_dialogUI{onDialog},
      m_dialogFont{nc::AcquireFont(DialogFont)}
{
    engine->GetModuleRegistry()->Get<nc::graphics::NcGraphics>()->SetUi(this);
    SetUIStyle();
}

void GameUI::Draw()
{
    const auto windowDimensions = nc::window::GetDimensions();
    const auto screenExtent = nc::window::GetScreenExtent();

    if (nc::input::KeyDown(hotkey::ToggleMenu))
    {
        m_menuOpen = !m_menuOpen;
    }

    ImGui::PushFont(m_dialogFont.font);
    if (m_menuOpen)
    {
        ImGui::SetNextWindowPos({ windowDimensions.x / 2, windowDimensions.y / 2 }, ImGuiCond_Always, {0.5f, 0.5f});

        if (!m_enableEndGameMenu)
        {
            ImGui::SetNextWindowSize(g_menuSize);
            DrawMainMenu();
        }
        else
        {
            ImGui::SetNextWindowSize(g_endGameMenuSize);
            DrawEndGameMenu();
        }
    }

    m_dialogUI.Draw(windowDimensions, screenExtent);

    if (m_counterOpen)
    {
        ImGui::SetNextWindowPos({ (windowDimensions.x - screenExtent.x) / 2, (windowDimensions.y - screenExtent.y) / 2});
        ImGui::SetNextWindowSize({ 225, 56 });
        DrawTreeCounter();
    }

    ImGui::PopFont();
}

bool GameUI::IsHovered()
{
    return ImGui::GetIO().WantCaptureMouse;
}

void GameUI::Clear()
{
    m_dialogUI.Clear();
    m_menuOpen = false;
    m_enableEndGameMenu = false;
    m_counterOpen = false;
    m_healthyCount = 0;
    m_infectedCount = 0;
}

void GameUI::DrawMainMenu()
{
    if (ImGui::Begin("GameMenu", nullptr, g_windowFlags))
    {
        if (ImGui::Button("Resume", g_menuButtonSize))
        {
            m_menuOpen = false;
        }
        if (ImGui::Button("New Game", g_menuButtonSize))
        {
            m_menuOpen = false;
            FireEvent(Event::NewGame);
        }
        if (ImGui::Button("Quit", g_menuButtonSize))
        {
            m_stopEngine();
        }
    }

    ImGui::End();
}

void GameUI::DrawEndGameMenu()
{
    if (ImGui::Begin("GameMenu", nullptr, g_windowFlags))
    {
        if (ImGui::Button("New Game", g_menuButtonSize))
        {
            m_menuOpen = false;
            FireEvent(Event::NewGame);
        }
        if (ImGui::Button("Quit", g_menuButtonSize))
        {
            m_stopEngine();
        }
    }

    ImGui::End();
}

void GameUI::DrawTreeCounter()
{
    if (ImGui::Begin("Counter", nullptr, g_windowFlags))
    {
        constexpr auto healthyColor = ImVec4{0.196f, 0.659f, 0.321f, 1.0f};
        constexpr auto infectedColor = ImVec4{0.616f, 0.196f, 0.659f, 1.0f};

        ImGui::Text("%s", "Healthy Plumwoods:  ");
            ImGui::SameLine();
            ImGui::TextColored(healthyColor, "%zu", m_healthyCount);

        ImGui::Text("%s", "Blighted Plumwoods: ");
            ImGui::SameLine();
            ImGui::TextColored(infectedColor, "%zu", m_infectedCount);
    }

    ImGui::End();
}
} // namespace game::ui
