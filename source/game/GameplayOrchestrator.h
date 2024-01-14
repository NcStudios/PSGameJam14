#pragma once

#include "ncengine/NcEngine.h"
#include "ncengine/type/StableAddress.h"

namespace game
{

//  Loosely thinking the events just handle sequencing of larger things like:
//    Intro (have camera follow poison impact thing and display intro dialog)
//    Begin (setup controller and switch camera, display controls)
//    DaveEncounter (could be fun to have some flavor text on first encounter or something)
//    Win/Lose (lose is probably just a NewGame/Quit menu, maybe win is a new scene, if time allows)
//    etc.
//
// We may also want an enum to indicate where we are at, in addition to the events.

// Could move event stuff?
enum class Event
{
    Intro, // unhandled
    Begin, // signals end of intro
    Win, // unhandled
    Lose, // unhandled
    NewGame
};

void FireEvent(Event event);

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
