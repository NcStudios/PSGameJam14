#pragma once

#include "Event.h"

#include "ncengine/NcEngine.h"
#include "ncengine/type/StableAddress.h"

namespace game
{
class GameUI;
class TreeTracker;

// Manager for scripting major game events
class GameplayOrchestrator : public nc::StableAddress
{
    public:
        GameplayOrchestrator(nc::NcEngine* engine, GameUI* ui);
        ~GameplayOrchestrator() noexcept;

        static auto Instance() -> GameplayOrchestrator&;

        void FireEvent(Event event);
        void Run();
        void Clear();

    private:
        inline static GameplayOrchestrator* m_instance = nullptr;
        nc::NcEngine* m_engine;
        GameUI* m_ui;
        std::unique_ptr<TreeTracker> m_treeTracker;

        void HandleBegin();
        void HandleNewGame();
};
} // namespace game
