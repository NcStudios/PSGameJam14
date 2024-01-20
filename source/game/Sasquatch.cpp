#include "Sasquatch.h"
#include "Assets.h"
#include "QuestTrigger.h"

namespace game
{
void AttachDaveComponents(nc::ecs::Ecs world)
{
    const auto dave = world.GetEntityByTag(tag::Dave);
    world.Emplace<nc::graphics::SkeletalAnimator>(dave, DaveMesh, DaveWave);
    AttachQuestTrigger(world, dave, Event::DaveEncounter, nc::Vector3{}, nc::Vector3::Splat(3.0f));
}

void AttachCampComponents(nc::ecs::Ecs world)
{
    const auto camp = world.GetEntityByTag(tag::Camp);
    AttachQuestTrigger(world, camp, Event::CampEncounter, nc::Vector3{}, nc::Vector3::Splat(3.0f));
}

void AttachElderQuestTrigger(nc::ecs::Ecs world)
{
    const auto elder = world.GetEntityByTag(tag::Elder);
    AttachQuestTrigger(world, elder, Event::ElderEncounter, nc::Vector3{}, nc::Vector3::Splat(3.0f));
}
} // namespace game
