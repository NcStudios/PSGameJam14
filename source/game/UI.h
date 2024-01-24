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
        void ToggleTreeCounter(bool isOpen) { m_counterOpen = isOpen; }
        void SetTreeCounts(size_t healthy, size_t infected) { m_healthyCount = healthy; m_infectedCount = infected; }
        void SetMenuToEndGameMenu() { m_enableEndGameMenu = true; }

    private:
        std::function<void()> m_stopEngine; // could add event instead
        std::vector<std::string> m_dialog;
        size_t m_currentDialogIndex = 0;
        size_t m_currentDialogNextCharacter = 0;
        std::string m_currentDialog = "";
        bool m_menuOpen = false;
        bool m_enableEndGameMenu = false;

        bool m_counterOpen = false;
        size_t m_healthyCount = 0;
        size_t m_infectedCount = 0;

        void DrawMainMenu();
        void DrawEndGameMenu();
        void DrawDialogWindow();
        void DrawTreeCounter();
        void SetDialogPosition(size_t pos);
};
} // namespace game
