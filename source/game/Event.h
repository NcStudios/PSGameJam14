#pragma once

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

enum class Event
{
    Intro, // unhandled
    Begin, // signals end of intro
    Win,
    Lose,
    NewGame
};

// Raise an event to be handled by GameplayOrchestrator
void FireEvent(Event event);
} // namespace game
