#pragma once

#include "ncengine/NcEngine.h"
#include "ncengine/type/StableAddress.h"
#include "ncengine/ui/IUI.h"

#include <functional>

namespace game
{
class GameUI : public nc::ui::IUI,
               public nc::StableAddress
{
    public:
        GameUI(nc::NcEngine* engine);

        void Draw() override;
        bool IsHovered() override;

    private:
        std::function<void()> m_stopEngine;
        bool m_menuOpen = false;
};
} // namespace game
