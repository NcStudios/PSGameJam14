#pragma once

#include "Dialog.h"

#include "ncengine/utility/Signal.h"

#include <string_view>
#include <variant>

namespace game
{
enum class Event
{
    None, // for GameplayOrchestrator internal use
    TitleScreen,
    Intro,
    Begin, // display controls
    DaveEncounter,
    HeadToCamp,
    CampEncounter,
    ElderEncounter,
    PutterEncounter,
    StartSpread,
    FlavorDialog,
    TreesCleared,
    Win,
    Lose,
    NewGame
};

// Raise an event to be handled by GameplayOrchestrator
void FireEvent(Event event);

// Dialog Events
struct OneShotDialogEvent
{
    dialog::DialogEntry dialog;
    float openDuration = 8.0f;
};

struct StartDialogEvent
{
    dialog::DialogEntry dialog;
};

struct NextDialogEvent
{
    dialog::DialogEntry dialog;
};

struct EndDialogEvent {};

using DialogEvent = std::variant<OneShotDialogEvent,
                                 StartDialogEvent,
                                 NextDialogEvent,
                                 EndDialogEvent>;

struct GameEvents
{
    nc::Signal<DialogEvent> onDialog;
};
} // namespace game
