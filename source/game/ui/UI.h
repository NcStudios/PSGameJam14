#pragma once

#include "DialogUI.h"

#include "ncengine/NcEngine.h"
#include "ncengine/asset/Assets.h"
#include "ncengine/type/StableAddress.h"
#include "ncengine/ui/IUI.h"

#include <functional>
#include <string>

namespace game::ui
{
class GameUI : public nc::ui::IUI,
               public nc::StableAddress
{
    public:
        GameUI(nc::NcEngine* engine, nc::Signal<DialogEvent>& onDialog);

        void Draw() override;
        bool IsHovered() override;

        void Clear();

        void OpenMenu() { m_menuOpen = true; }
        void ToggleTreeCounter(bool isOpen) { m_counterOpen = isOpen; }
        void SetTreeCounts(size_t healthy, size_t infected) { m_healthyCount = healthy; m_infectedCount = infected; }
        void SetMenuToEndGameMenu() { m_enableEndGameMenu = true; }

    private:
        std::function<void()> m_stopEngine; // could add event instead
        DialogUI m_dialogUI;
        nc::FontView m_dialogFont;
        bool m_menuOpen = false;
        bool m_enableEndGameMenu = false;

        bool m_counterOpen = false;
        size_t m_healthyCount = 0;
        size_t m_infectedCount = 0;

        void DrawMainMenu();
        void DrawEndGameMenu();
        void DrawDialogWindow();
        void DrawTreeCounter();
};
} // namespace game::ui
