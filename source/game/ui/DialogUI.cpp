#include "DialogUI.h"

#include "ncengine/time/Time.h"
#include "ncengine/ui/ImGuiUtility.h"

namespace
{
constexpr auto g_narrativeWindowHeight = 120.0f;
constexpr auto g_dialogButtonSize = ImVec2{40.0f, g_narrativeWindowHeight - 20.0f};
constexpr auto g_windowFlags = ImGuiWindowFlags_NoCollapse  |
                               ImGuiWindowFlags_NoTitleBar  |
                               ImGuiWindowFlags_NoScrollbar |
                               ImGuiWindowFlags_NoResize;
}

namespace game::ui
{
DialogUI::DialogUI(nc::Signal<DialogEvent>& onDialog)
    : m_onDialogConnection{onDialog.Connect(this, &DialogUI::HandleDialogEvent)}
{
}

void DialogUI::Draw(const nc::Vector2& windowDimensions, const nc::Vector2& screenExtent)
{
    if (m_state == DialogState::Closed)
        return;

    const auto dt = nc::time::DeltaTime();
    const auto transitionFactor  = ProcessTransition(dt);
    auto& style = ImGui::GetStyle();
    const auto alphaRestore = style.Alpha;
    style.Alpha = transitionFactor;

    const auto height = transitionFactor * g_narrativeWindowHeight;
    const auto size = ImVec2{screenExtent.x * 0.75f, height};
    const auto pos = ImVec2{(windowDimensions.x - size.x) * 0.5f, windowDimensions.y - height};
    ImGui::SetNextWindowPos(ImVec2{pos});
    ImGui::SetNextWindowSize(ImVec2{size});

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

        if (m_state != DialogState::Opening && m_currentDialogIndex < m_dialog.size())
        {
            const auto& fullDialog = m_dialog.at(m_currentDialogIndex);
            if (m_currentDialogNextCharacter < fullDialog.size())
            {
                m_currentDialog.push_back(fullDialog.at(m_currentDialogNextCharacter++));
            }

            ImGui::TextWrapped("%s", m_currentDialog.c_str());
        }
    }

    ImGui::GetStyle().Alpha = alphaRestore;
    ImGui::End();
}

void DialogUI::Clear()
{
    SetState(DialogState::Closed);
    m_totalOneShotOpenTime = 0.0f;
    m_inOneShot = false;
    m_dialog.clear();
    m_currentDialogIndex = 0;
    SetDialogPosition(0);
}

void DialogUI::HandleDialogEvent(DialogEvent event)
{
    std::visit([this](auto&& unpackedEvent)
    {
        using event_t = std::decay_t<decltype(unpackedEvent)>;
        if constexpr (std::same_as<event_t, OneShotDialogEvent>)
        {
            SetState(DialogState::Opening);
            AddNewDialog(unpackedEvent.dialog.data());
            m_totalOneShotOpenTime = unpackedEvent.openDuration;
            m_inOneShot = true;
        }
        else if constexpr (std::same_as<event_t, StartDialogEvent>)
        {
            SetState(DialogState::Opening);
            AddNewDialog(unpackedEvent.dialog.data());
        }
        else if constexpr (std::same_as<event_t, NextDialogEvent>)
        {
            AddNewDialog(unpackedEvent.dialog.data());
        }
        else if constexpr (std::same_as<event_t, EndDialogEvent>)
        {
            SetState(DialogState::Closing);
        }
    }, event);
}

void DialogUI::AddNewDialog(std::string dialog)
{
    m_dialog.push_back(std::move(dialog));
    SetDialogPosition(m_dialog.size() - 1);
}

void DialogUI::SetState(DialogState state)
{
    m_state = state;
    m_currentOneShotOpenTime = 0.0f;
    m_currentTransitionTime = 0.0f;
}

auto DialogUI::ProcessTransition(float dt) -> float
{
    switch (m_state)
    {
        case DialogState::Open:
        {
            if (m_inOneShot)
            {
                m_currentOneShotOpenTime += dt;
                if (m_currentOneShotOpenTime > m_totalOneShotOpenTime)
                {
                    SetState(DialogState::Closing);
                    m_totalOneShotOpenTime = 0.0f;
                    m_inOneShot = false;
                }
            }

            return 1.0f;
        }
        case DialogState::Opening:
        {
            m_currentTransitionTime += dt;
            const auto transitionFactor = m_currentTransitionTime / TransitionTime;
            if (m_currentTransitionTime >= TransitionTime)
                SetState(DialogState::Open);

            return transitionFactor;
        }
        case DialogState::Closing:
        {
            m_currentTransitionTime += dt;
            const auto transitionFactor = 1.0f - (m_currentTransitionTime / TransitionTime);
            if (m_currentTransitionTime >= TransitionTime)
                SetState(DialogState::Closed);

            return transitionFactor;
        }
        case DialogState::Closed:
        {
            return 0.0f; // don't expect to be here
        }
    }

    return 0.0f;
}

void DialogUI::SetDialogPosition(size_t pos)
{
    if (pos >= m_dialog.size())
        return;

    m_currentDialogIndex = pos;
    m_currentDialogNextCharacter = 0;
    m_currentDialog = "";
}
} // namespace game::ui
