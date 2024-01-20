#pragma once

#include "Event.h"

#include "ncengine/NcEngine.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/type/StableAddress.h"

namespace game
{
class GameUI;
class TreeTracker;

class Cutscene
{
    public:
        void Enter(nc::ecs::Ecs world, std::string_view focusPointTag, std::span<const std::string_view> dialogSequence);
        void Exit(nc::ecs::Ecs world);
        auto IsRunning() -> bool;
        void Update(nc::ecs::Ecs world, GameUI* ui);

    private:
        std::span<const std::string_view> m_dialogSequence;
        size_t m_currentDialog = 0ull;
        bool m_running = false;
        bool m_initialDialogPlayed = false;
};

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
        Cutscene m_currentCutscene;
        float m_timeInCurrentEvent = 0.0f;
        size_t m_currentFlavorDialogIndex = 0ull;
        bool m_spreadStarted = false;

        void SetEvent(Event event);

        void HandleIntro();
        void HandleBegin();
        void HandleDaveEncounter();
        void HandleHeadToCamp();
        void HandleCampEncounter();
        void HandleElderEncounter();
        void HandlePutterEncounter();
        void HandleStartSpread();
        void HandleFlavorDialog();
        void HandleNewGame();
        void HandleWin();
        void HandleLose();

        void StartCutsence(std::string_view focusPointTag);
        void EndCutScene();

        void ProcessTrees(float dt);
};
} // namespace game
