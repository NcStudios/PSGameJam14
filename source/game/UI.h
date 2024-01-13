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

    private:
        std::function<void()> m_stopEngine; // could add event instead
        std::vector<std::string> m_dialog;
        size_t m_currentDialog = 0;
        bool m_menuOpen = false;
};
} // namespace game
