#pragma once

#include "Event.h"
#include "ncmath/Vector.h"

#include <string>
#include <vector>

namespace game::ui
{
enum class DialogState
{
    Closed,
    Open,
    Opening,
    Closing
};

class DialogUI
{
    static constexpr auto TransitionTime = 0.5f;

    public:
        DialogUI(nc::Signal<DialogEvent>& onDialog);

        void Draw(const nc::Vector2& dimensions, const nc::Vector2& screenExtent);
        void Clear();

    private:
        std::vector<std::pair<std::string, std::string>> m_dialog;
        size_t m_currentDialogIndex = 0;
        size_t m_currentDialogNextCharacter = 0;
        std::string m_currentDialog = "";
        nc::Connection<DialogEvent> m_onDialogConnection;

        DialogState m_state = DialogState::Closed;
        float m_currentTransitionTime = 0.0f;
        float m_currentOneShotOpenTime = 0.0f;
        float m_totalOneShotOpenTime = 0.0f;
        bool m_inOneShot = false;

        void HandleDialogEvent(DialogEvent event);
        void AddNewDialog(dialog::DialogEntry dialog);
        void SetState(DialogState state);
        auto ProcessTransition(float dt) -> float;
        void SetDialogPosition(size_t pos);
};
} // namespace game::ui
