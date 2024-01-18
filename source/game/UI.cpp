#include "UI.h"
#include "Core.h"
#include "Event.h"

#include "ncengine/ui/ImGuiStyle.h"
#include "ncengine/ui/ImGuiUtility.h"
#include "ncengine/window/Window.h"

namespace
{
constexpr auto g_narrativeWindowHeight = 100.0f;
constexpr auto g_dialogButtonSize = ImVec2{40.0f, g_narrativeWindowHeight - 20.0f};
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

    if (nc::input::KeyDown(hotkey::ToggleMenu))
    {
        m_menuOpen = !m_menuOpen;
    }

#ifndef GAME_PROD_BUILD
    ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
    ImGui::SetNextWindowSize({80, 30});
    if (ImGui::Begin("DebugUI", nullptr, g_windowFlags))
    {
        ImGui::Text("fps: %.1f", ImGui::GetIO().Framerate);
    }

    ImGui::End();
#endif

    if (m_menuOpen)
    {
        ImGui::SetNextWindowPos({ windowDimensions.x / 2, windowDimensions.y / 2 }, ImGuiCond_Always, {0.5f, 0.5f});
        ImGui::SetNextWindowSize(g_menuSize);
        DrawMainMenu();
    }

    ImGui::SetNextWindowPos({ (windowDimensions.x - screenExtent.x) / 2, windowDimensions.y - g_narrativeWindowHeight });
    ImGui::SetNextWindowSize({ screenExtent.x, g_narrativeWindowHeight });
    DrawDialogWindow();
}

bool GameUI::IsHovered()
{
    return ImGui::GetIO().WantCaptureMouse;
}

void GameUI::Clear()
{
    m_dialog.clear();
    m_currentDialogIndex = 0;
    SetDialogPosition(0);
    m_menuOpen = false;
}

void GameUI::AddNewDialog(std::string dialog)
{
    m_dialog.push_back(std::move(dialog));
    SetDialogPosition(m_dialog.size() - 1);
}

void GameUI::SetDialogPosition(size_t pos)
{
    if (pos >= m_dialog.size())
        return;

    m_currentDialogIndex = pos;
    m_currentDialogNextCharacter = 0;
    m_currentDialog = "";
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

void GameUI::DrawDialogWindow()
{
    if (ImGui::Begin("GameUI", nullptr, g_windowFlags))
    {
        ImGui::BeginDisabled(m_currentDialogIndex == 0);
        if (ImGui::Button("<", g_dialogButtonSize))
        {
            if (m_currentDialogIndex != 0)
                SetDialogPosition(m_currentDialogIndex - 1);
        }
        ImGui::EndDisabled();

        ImGui::SameLine();

        ImGui::BeginDisabled(m_currentDialogIndex + 1 == m_dialog.size());
        if (ImGui::Button(">", g_dialogButtonSize))
        {
            SetDialogPosition(m_currentDialogIndex + 1);
        }
        ImGui::EndDisabled();

        ImGui::SameLine();

        // fix? I think its rendering the current frame in a 'cleared' state, and doesn't get initial dialog until next frame
        // NC_ASSERT(m_currentDialog < m_dialog.size(), "dialog out of sync");
        if (m_currentDialogIndex < m_dialog.size())
        {
            const auto& fullDialog = m_dialog.at(m_currentDialogIndex);
            if (m_currentDialogNextCharacter < fullDialog.size())
            {
                m_currentDialog.push_back(fullDialog.at(m_currentDialogNextCharacter++));
            }

            ImGui::Text("%s", m_currentDialog.c_str());
        }
    }

    ImGui::End();
}
} // namespace game
