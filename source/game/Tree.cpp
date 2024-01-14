#include "Tree.h"
#include "Assets.h"

#include <algorithm>
#include <ranges>

namespace
{
// Copied from engine, consider exposing this somehow
auto ToSphereProperties(const nc::physics::VolumeInfo& in) noexcept -> nc::physics::SphereProperties
{
    return nc::physics::SphereProperties{in.offset, in.scale.x * 0.5f};
}
} // anonymous namespace

namespace game
{
void InfectedTree::Update(nc::ecs::Ecs world, float dt)
{
    m_timeSinceLastSpread += dt;
    if (m_timeSinceLastSpread < RadiusSpreadTime)
        return;

    m_timeSinceLastSpread = 0.0f;
    auto transform = world.Get<nc::Transform>(ParentEntity());
    auto children = transform->Children();
    NC_ASSERT(children.size() == 1, "expected only one child");
    auto collider = world.Get<nc::physics::Collider>(children[0]);
    NC_ASSERT(collider, "expected a collider");
    auto props = ::ToSphereProperties(collider->GetInfo());
    if (props.radius < map::Radius)
    {
        props.radius += RadiusGrowthAmount;
        collider->SetProperties(props);
    }

    auto emitter = world.Get<nc::graphics::ParticleEmitter>(ParentEntity());
    NC_ASSERT(emitter, "expected a particle emitter");
    auto particleInfo = emitter->GetInfo();
    if (particleInfo.init.positionMax.x < map::Extent / 2.0f)
    {
        particleInfo.init.positionMin -= nc::Vector3::Splat(RadiusGrowthAmount);
        particleInfo.init.positionMax += nc::Vector3::Splat(RadiusGrowthAmount);
    }
    if (particleInfo.emission.periodicEmissionCount < MaxEmissionCount) // nc::Clamp needs template adjustment
    {
        particleInfo.emission.periodicEmissionCount += 1;
    }

    emitter->SetInfo(particleInfo);
}

auto CreateTreeBase(nc::ecs::Ecs world,
                    const nc::Vector3& position,
                    const nc::Quaternion& rotation,
                    const nc::Vector3& scale,
                    const std::string& tag,
                    nc::Entity::layer_type layer,
                    const std::string& mesh) -> nc::Entity
{
    const auto tree = world.Emplace<nc::Entity>(nc::EntityInfo{
        .position = position,
        .rotation = rotation,
        .scale = scale,
        .tag = tag,
        .layer = layer,
        .flags = nc::Entity::Flags::NoSerialize
    });

    world.Emplace<nc::graphics::ToonRenderer>(tree, mesh);
    world.Emplace<nc::physics::Collider>(tree, nc::physics::BoxProperties{});
    world.Emplace<nc::physics::PhysicsBody>(tree, nc::physics::PhysicsProperties{.isKinematic = true});
    return tree;
}

void AttachHealthyTree(nc::ecs::Ecs world, nc::Entity tree)
{
    world.Emplace<HealthyTree>(tree);
    auto onTriggerEnter = [](nc::Entity self, nc::Entity other, nc::Registry* registry)
    {
        if (other.Layer() != layer::Spreader)
            return;

        auto tree = registry->Get<HealthyTree>(self);
        NC_ASSERT(tree, "expected tree");
        tree->Infect();
    };

    auto onTriggerExit = [](nc::Entity self, nc::Entity other, nc::Registry* registry)
    {
        if (other.Layer() != layer::Spreader)
            return;

        auto tree = registry->Get<HealthyTree>(self);
        NC_ASSERT(tree, "expected tree");
        tree->Disinfect();
    };

    world.Emplace<nc::CollisionLogic>(tree, nullptr, nullptr, onTriggerEnter, onTriggerExit);
}

void AttachInfectedTree(nc::ecs::Ecs world, nc::Entity tree)
{
    world.Emplace<InfectedTree>(tree);
    // may want two emitters with different particles, like an orb and smoke/haze
    world.Emplace<nc::graphics::ParticleEmitter>(tree, nc::graphics::ParticleInfo{
        .emission = nc::graphics::ParticleEmissionInfo{
            .periodicEmissionCount = 1,
            .periodicEmissionFrequency = 0.1f
        },
        .init = nc::graphics::ParticleInitInfo{
            .lifetime = 3.0f,
            .positionMin = nc::Vector3::Splat(-0.5f), // keep in sync with init collider size
            .positionMax = nc::Vector3::Splat(0.5f),
            .rotationMin = -0.157f,
            .rotationMax = 0.157f,
            .scaleMin = 0.01f,
            .scaleMax = 0.8f,
        },
        .kinematic = nc::graphics::ParticleKinematicInfo{
            .velocityMin = nc::Vector3::One() * -0.25f,
            .velocityMax = nc::Vector3::One() * 0.25f,
            .scaleOverTimeFactor = -20.0f
        }
    });

    auto onCollisionEnter = [](nc::Entity self, nc::Entity other, nc::Registry* registry)
    {
        if (other.Layer() != layer::Character)
            return;

        // maybe want this to be time-related as well. if so, will have to be moved to system
        MorphTreeToHealthy(registry->GetEcs(), self);
    };

    world.Emplace<nc::CollisionLogic>(tree, onCollisionEnter, nullptr, nullptr, nullptr);

    const auto spreader = world.Emplace<nc::Entity>(nc::EntityInfo
    {
        .parent = tree,
        .tag = tag::Spreader,
        .layer = layer::Spreader // prob don't need layer on both, but...
    });

    world.Emplace<nc::physics::Collider>(spreader, nc::physics::SphereProperties{}, true);
}

void FinalizeTrees(nc::ecs::Ecs world)
{
    for (auto entity : world.GetAll<nc::Entity>())
    {
        if (entity.Layer() == layer::HealthyTree)
            AttachHealthyTree(world, entity);
        else if (entity.Layer() == layer::InfectedTree)
            AttachInfectedTree(world, entity);
    }
}

void MorphTreeToHealthy(nc::ecs::Ecs world, nc::Entity target)
{
    const auto transform = world.Get<nc::Transform>(target);
    const auto pos = transform->Position();
    const auto rot = transform->Rotation();
    const auto scl = transform->Scale();
    world.Remove<nc::Entity>(target);
    auto tree = CreateTreeBase(world, pos, rot, scl, tag::HealthyTree, layer::HealthyTree, TreeMesh);
    AttachHealthyTree(world, tree);
}

void MorphTreeToInfected(nc::ecs::Ecs world, nc::Entity target)
{
    const auto transform = world.Get<nc::Transform>(target);
    NC_ASSERT(transform, "expected transform");
    const auto pos = transform->Position();
    const auto rot = transform->Rotation();
    const auto scl = transform->Scale();
    world.Remove<nc::Entity>(target);
    auto tree = CreateTreeBase(world, pos, rot, scl, tag::InfectedTree, layer::InfectedTree, nc::asset::SphereMesh);
    AttachInfectedTree(world, tree);
}

void RegisterTreeComponents(nc::ecs::ComponentRegistry& registry)
{
    const auto maxEntities = registry.GetMaxEntities();
    registry.RegisterType<game::HealthyTree>(maxEntities, nc::ComponentHandler<game::HealthyTree>{
        .name = "HealthyTree",
        .drawUI = [](HealthyTree&) {}
    });

    registry.RegisterType<game::InfectedTree>(maxEntities, nc::ComponentHandler<game::InfectedTree>{
        .name = "InfectedTree",
        .drawUI = [](InfectedTree&) {}
    });
}

void ProcessTrees(nc::Entity, nc::Registry* registry, float dt)
{
    auto world = registry->GetEcs();
    for (auto& infected : world.GetAll<InfectedTree>())
    {
        infected.Update(world, dt);
    }

    for (auto& healthy : world.GetAll<HealthyTree>())
    {
        healthy.Update(dt);
        if (healthy.ShouldMorph())
        {
            MorphTreeToInfected(world, healthy.ParentEntity());
        }
    }
}
} // namespace game
