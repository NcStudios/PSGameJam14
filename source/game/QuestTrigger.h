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
    const auto rootTransform = world.Get<nc::Transform>(parent);
    NC_ASSERT(rootTransform, "expected valid root transform");

    auto triggerIndicator = world.Emplace<nc::Entity>({
        .position = rootTransform->Position() + nc::Vector3::Up() * 5.0f,
        .tag = "QuestTriggerIndicator",
        .flags =nc::Entity::Flags::NoSerialize | nc::Entity::Flags::Static
    });

    world.Emplace<nc::graphics::ParticleEmitter>(triggerIndicator, nc::graphics::ParticleInfo{
        .emission = {
            .initialEmissionCount = 1,
            .periodicEmissionCount = 1,
            .periodicEmissionFrequency = 5.0f
        },
        .init = {
            .lifetime = 10.0f,
            .scaleMin = 1.0f,
            .scaleMax = 1.0f,
            .particleTexturePath = QuestIndicatorParticle
        },
        .kinematic = {
            .rotationMin = 3.14f,
            .rotationMax = 3.14f,
            .rotationOverTimeFactor = 0.0f,
            .scaleOverTimeFactor = -10.0f
        }
    });

    auto trigger = world.Emplace<nc::Entity>({
        .parent = parent,
        .tag = tag::QuestTrigger,
        .layer = layer::QuestTrigger,
        .flags = nc::Entity::Flags::NoSerialize | nc::Entity::Flags::Static
    });

    world.Emplace<nc::physics::Collider>(trigger, nc::physics::BoxProperties{.center = center, .extents = extents}, true);
    auto triggerHandler = [onPlayerHit, triggerIndicator](nc::Entity self, nc::Entity other, nc::Registry* registry)
    {
        if (other.Layer() != layer::Character)
            return;

        FireEvent(onPlayerHit);
        registry->Remove<nc::Entity>(self);
        registry->Remove<nc::Entity>(triggerIndicator);
    };

    world.Emplace<nc::CollisionLogic>(trigger, nullptr, nullptr, triggerHandler, nullptr);



    return trigger;
}
} // namespace game
