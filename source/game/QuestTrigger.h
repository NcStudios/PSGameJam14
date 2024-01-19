#pragma once

#include "Assets.h"
#include "Core.h"
#include "Event.h"

namespace game
{
inline auto AttachQuestTrigger(nc::ecs::Ecs world,
                               nc::Entity parent,
                               Event onPlayerHit,
                               const nc::Vector3& center = nc::Vector3::Zero(),
                               const nc::Vector3& extents = nc::Vector3::One()) -> nc::Entity
{
    auto trigger = world.Emplace<nc::Entity>({
        .parent = parent,
        .tag = tag::QuestTrigger,
        .layer = layer::QuestTrigger,
        .flags = nc::Entity::Flags::NoSerialize | nc::Entity::Flags::Static
    });

    // placeholder indicator
    auto material = nc::graphics::ToonMaterial{.baseColor = QuestIndicatorParticle};
    world.Emplace<nc::graphics::ToonRenderer>(trigger, nc::asset::PlaneMesh, material);

    world.Emplace<nc::physics::Collider>(trigger, nc::physics::BoxProperties{.center = center, .extents = extents}, true);
    auto triggerHandler = [onPlayerHit](nc::Entity self, nc::Entity other, nc::Registry* registry)
    {
        if (other.Layer() != layer::Character)
            return;

        FireEvent(onPlayerHit);
        registry->Remove<nc::Entity>(self);
    };

    world.Emplace<nc::CollisionLogic>(trigger, nullptr, nullptr, triggerHandler, nullptr);
    return trigger;
}
} // namespace game
