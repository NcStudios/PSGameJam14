#include "Event.h"
#include "GameplayOrchestrator.h"

namespace
{
// auto g_onDialog = nc::Signal<game::DialogEvent>{};
}

namespace game
{
void FireEvent(Event event)
{
    GameplayOrchestrator::Instance().FireEvent(event);
}

// auto OnDialog() -> nc::Signal<DialogEvent>&
// {
//     return g_onDialog;
// }
} // namespace game
