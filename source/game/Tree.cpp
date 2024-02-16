#include "Tree.h"
#include "Assets.h"
#include "ncengine/graphics/SkeletalAnimator.h"
#include "Event.h"

#include <algorithm>
#include <ranges>

namespace
{
// Copied from engine, consider exposing this somehow
auto ToSphereProperties(const nc::physics::VolumeInfo& in) noexcept -> nc::physics::SphereProperties
{
    return nc::physics::SphereProperties{in.offset, in.scale.x * 0.5f};
}

auto GetSpreaderFromParent(nc::ecs::Ecs world, nc::Entity parent)
{
    auto& hierarchy = world.Get<nc::Hierarchy>(parent);
    auto pos = std::ranges::find_if(hierarchy.children, [](auto entity) { return entity.Layer() == game::layer::Spreader; });
    NC_ASSERT(pos != std::end(hierarchy.children), "expected a spreader");
    return *pos;
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
    const auto spreader = ::GetSpreaderFromParent(world, ParentEntity());
    auto& collider = world.Get<nc::physics::Collider>(spreader);
    auto props = ::ToSphereProperties(collider.GetInfo());
    if (props.radius < MaxSpreadRadius)
    {
        props.radius += RadiusGrowthAmount;
        collider.SetProperties(props);
    }

    auto& emitter = world.Get<nc::graphics::ParticleEmitter>(ParentEntity());
    auto particleInfo = emitter.GetInfo();
    if (particleInfo.kinematic.velocityMax.y < 10.0f)
    {
        particleInfo.kinematic.velocityMin -= nc::Vector3{0.1f, 0.0f, 0.1f};
        particleInfo.kinematic.velocityMax += nc::Vector3{0.1f, 0.1f, 0.1f};
    }
    if (particleInfo.emission.periodicEmissionCount < MaxEmissionCount) // nc::Clamp needs template adjustment
    {
        particleInfo.emission.periodicEmissionCount += 1;
    }

    emitter.SetInfo(particleInfo);
}

auto CreateTreeBase(nc::ecs::Ecs world,
                    const nc::Vector3& position,
                    const nc::Quaternion& rotation,
                    const nc::Vector3& scale,
                    const std::string& tag,
                    nc::Entity::layer_type layer,
                    const std::string& mesh,
                    const nc::graphics::ToonMaterial& material) -> nc::Entity
{
    const auto tree = world.Emplace<nc::Entity>(nc::EntityInfo{
        .position = position,
        .rotation = rotation,
        .scale = scale,
        .tag = tag,
        .layer = layer,
        .flags = nc::Entity::Flags::NoSerialize | nc::Entity::Flags::Static
    });

    world.Emplace<nc::graphics::ToonRenderer>(tree, mesh, material);
    world.Emplace<nc::physics::Collider>(tree, nc::physics::BoxProperties{});
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

    world.Emplace<nc::audio::AudioSource>(tree, std::vector<std::string>{MorphHealthySfx}, nc::audio::AudioSourceProperties{
        .gain = 2.0f,
        .outerRadius = 60.0f,
        .flags = nc::audio::AudioSourceFlags::Spatial
    }).Play();
}

void AttachInfectedTree(nc::ecs::Ecs world, nc::Entity tree)
{
    world.Emplace<InfectedTree>(tree);
    world.Emplace<nc::graphics::ParticleEmitter>(tree, nc::graphics::ParticleInfo{
        .emission = nc::graphics::ParticleEmissionInfo{
            .periodicEmissionCount = 1,
            .periodicEmissionFrequency = 0.5f
        },
        .init = nc::graphics::ParticleInitInfo{
            .lifetime = 4.0f,
            .positionMin = nc::Vector3{0.0f, 0.0f, 0.0f}, // spawn inside tree so can't see them blink in
            .positionMax = nc::Vector3{0.0f, 5.0f, 0.0f},
            .rotationMin = -0.157f,
            .rotationMax = 0.157f,
            .scaleMin = 0.005f,
            .scaleMax = 0.6f,
            .particleTexturePath = BlightParticle
        },
        .kinematic = nc::graphics::ParticleKinematicInfo{
            .velocityMin = nc::Vector3{-0.35f, 0.05f, -0.35f},
            .velocityMax = nc::Vector3{0.35f, 0.5f, 0.35f},
            .rotationMin = -1.0f,
            .rotationMax = 1.0f,
            .rotationOverTimeFactor = 0.0f,
            .scaleOverTimeFactor = -20.0f
        }
    });

    auto onTriggerEnter = [](nc::Entity self, nc::Entity other, nc::Registry* registry)
    {
        if (other.Layer() != layer::Purifier)
            return;

        // maybe want this to be time-related as well. if so, will have to be moved to system
        MorphTreeToHealthy(registry->GetEcs(), self);
    };

    world.Emplace<nc::CollisionLogic>(tree, nullptr, nullptr, onTriggerEnter, nullptr);

    world.Emplace<nc::audio::AudioSource>(tree, std::vector<std::string>{MorphInfectedSfx}, nc::audio::AudioSourceProperties{
        .gain = 2.0f,
        .outerRadius = 60.0f,
        .flags = nc::audio::AudioSourceFlags::Spatial
    }).Play();

    const auto spreader = world.Emplace<nc::Entity>(nc::EntityInfo
    {
        .parent = tree,
        .tag = tag::Spreader,
        .layer = layer::Spreader,
        .flags = nc::Entity::Flags::NoSerialize | nc::Entity::Flags::Static
    });

    world.Emplace<nc::physics::Collider>(spreader, nc::physics::SphereProperties{}, true);
    world.Emplace<nc::physics::PhysicsBody>(spreader, nc::physics::PhysicsProperties{.isKinematic = true});
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

void AttachMorphParticles(nc::ecs::Ecs world, nc::Entity parent, std::string_view particleTexture)
{
    auto explosion = world.Emplace<nc::Entity>({.parent = parent, .flags = nc::Entity::Flags::NoSerialize});
    world.Emplace<nc::graphics::ParticleEmitter>(explosion, nc::graphics::ParticleInfo{
        .emission = nc::graphics::ParticleEmissionInfo{
            .initialEmissionCount = 50,
            .periodicEmissionCount = 0,
            .periodicEmissionFrequency = 0.0f
        },
        .init = nc::graphics::ParticleInitInfo{
            .lifetime = 3.0f,
            .positionMin = nc::Vector3{0.0f, 0.0f, 0.0f}, // spawn inside tree so can't see them blink in
            .positionMax = nc::Vector3{0.0f, 5.0f, 0.0f},
            .rotationMin = -0.157f,
            .rotationMax = 0.157f,
            .scaleMin = 0.05f,
            .scaleMax = 0.5f,
            .particleTexturePath = std::string{particleTexture}
        },
        .kinematic = nc::graphics::ParticleKinematicInfo{
            .velocityMin = nc::Vector3{-10.0f, 0.05f, -10.f},
            .velocityMax = nc::Vector3{10.0f, 10.0f, 10.0f},
            .rotationMin = -3.0f,
            .rotationMax = -6.0f,
            .rotationOverTimeFactor = 0.0f,
            .scaleOverTimeFactor = -25.0f
        }
    });
}

void MorphTreeToHealthy(nc::ecs::Ecs world, nc::Entity target)
{
    const auto& transform = world.Get<nc::Transform>(target);
    const auto pos = transform.Position();
    const auto rot = transform.Rotation();
    const auto scl = transform.Scale();
    world.Remove<nc::Entity>(target);
    auto tree = CreateTreeBase(world, pos, rot, scl, tag::HealthyTree, layer::HealthyTree, Tree01Mesh, HealthyTree01Material);
    AttachHealthyTree(world, tree);
    AttachMorphParticles(world, tree, MorphHealthyParticle);
}

void MorphTreeToInfected(nc::ecs::Ecs world, nc::Entity target)
{
    const auto& transform = world.Get<nc::Transform>(target);
    const auto pos = transform.Position();
    const auto rot = transform.Rotation();
    const auto scl = transform.Scale();
    world.Remove<nc::Entity>(target);
    auto tree = CreateTreeBase(world, pos, rot, scl, tag::InfectedTree, layer::InfectedTree, Tree01Mesh, InfectedTree01Material);
    AttachInfectedTree(world, tree);
    AttachMorphParticles(world, tree, MorphInfectedParticle);
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
} // namespace game
