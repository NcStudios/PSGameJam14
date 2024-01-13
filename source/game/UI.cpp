#include "UI.h"
#include "GameplayOrchestrator.h"

#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/input/Input.h"
#include "ncengine/ui/ImGuiStyle.h"
#include "ncengine/ui/ImGuiUtility.h"
#include "ncengine/window/Window.h"

namespace
{
constexpr auto g_openMenuHotkey = nc::input::KeyCode::Escape;
constexpr auto g_narrativeWindowHeight = 100.0f;
constexpr auto g_menuSize = ImVec2{300.0f, 145.0f};
constexpr auto g_menuButtonSize = ImVec2{285.0f, 40.0f};
constexpr auto g_windowFlags = ImGuiWindowFlags_NoCollapse |
                               ImGuiWindowFlags_NoTitleBar |
                               ImGuiWindowFlags_NoResize;
} // anonymous namespace

namespace game
{
GameUI::GameUI(nc::NcEngine* engine)
    : m_stopEngine{[engine](){ engine->Stop(); }}
{
    engine->GetModuleRegistry()->Get<nc::graphics::NcGraphics>()->SetUi(this);
    nc::ui::SetDefaultUIStyle();
}

void GameUI::Draw()
{
    const auto windowDimensions = nc::window::GetDimensions();
    const auto screenExtent = nc::window::GetScreenExtent();

    if (nc::input::KeyDown(g_openMenuHotkey))
    {
        m_menuOpen = !m_menuOpen;
    }

    if (m_menuOpen)
    {
        ImGui::SetNextWindowPos({ windowDimensions.x / 2, windowDimensions.y / 2 }, ImGuiCond_Always, {0.5f, 0.5f});
        ImGui::SetNextWindowSize(g_menuSize);

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


    ImGui::SetNextWindowPos({ (windowDimensions.x - screenExtent.x) / 2, windowDimensions.y - g_narrativeWindowHeight });
    ImGui::SetNextWindowSize({ screenExtent.x, g_narrativeWindowHeight });
    if (ImGui::Begin("GameUI", nullptr, g_windowFlags))
    {
        // fix? I think its rendering the current frame in a 'cleared' state, and doesn't get initial dialog until next frame
        // NC_ASSERT(m_currentDialog < m_dialog.size(), "dialog out of sync");
        if (m_currentDialog < m_dialog.size())
            ImGui::Text("%s", m_dialog.at(m_currentDialog).c_str());
    }

    ImGui::End();
}

bool GameUI::IsHovered()
{
    return ImGui::GetIO().WantCaptureMouse;
}

void GameUI::Clear()
{
    m_dialog.clear();
    m_currentDialog = 0;
    m_menuOpen = false;
}

void GameUI::AddNewDialog(std::string dialog)
{
    m_dialog.push_back(std::move(dialog));
    m_currentDialog = m_dialog.size() - 1;
}

} // namespace game
