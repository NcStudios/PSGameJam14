#pragma once

#include "Event.h"

#include "ncengine/NcEngine.h"
#include "ncengine/ecs/Ecs.h"
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
        void Run(float dt);
        void Clear();

    private:
        inline static GameplayOrchestrator* m_instance = nullptr;
        nc::NcEngine* m_engine;
        nc::ecs::Ecs m_world;
        GameUI* m_ui;
        std::unique_ptr<TreeTracker> m_treeTracker;
        Event m_currentEvent = Event::Intro;
        float m_timeInCurrentEvent = 0.0f;

        bool m_daveEncountered = false;
        bool m_campEncountered = false;
        bool m_spreadStarted = false;

        void SetEvent(Event event);

        void HandleIntro();
        void HandleBegin();
        void HandleDaveEncounter();
        void HandleCampEncounter();
        void HandleNewGame();
        void HandleWin();
        void HandleLose();

        void ProcessTrees(float dt);
};
} // namespace game
