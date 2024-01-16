#pragma once

#include "ncengine/NcEngine.h"
#include "ncengine/type/StableAddress.h"
#include "ncengine/ui/IUI.h"

#include <functional>
#include <string>
#include <vector>

namespace game
{
class GameUI : public nc::ui::IUI,
               public nc::StableAddress
{
    public:
        GameUI(nc::NcEngine* engine);

        void Draw() override;
        bool IsHovered() override;

        void Clear();
        void AddNewDialog(std::string dialog);

        void OpenMenu() { m_menuOpen = true; }

    private:
        std::function<void()> m_stopEngine; // could add event instead
        std::vector<std::string> m_dialog;
        size_t m_currentDialogIndex = 0;
        size_t m_currentDialogNextCharacter = 0;
        std::string m_currentDialog = "";
        bool m_menuOpen = false;

        void DrawMainMenu();
        void DrawDialogWindow();
        void SetDialogPosition(size_t pos);
};
} // namespace game
