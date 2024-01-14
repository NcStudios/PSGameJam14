#include "Event.h"
#include "GameplayOrchestrator.h"

namespace game
{
void FireEvent(Event event)
{
    GameplayOrchestrator::Instance().FireEvent(event);
}
} // namespace game
