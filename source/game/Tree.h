#pragma once

#include "Core.h"

#include "ncengine/utility/Signal.h"

#include <iostream>

namespace game
{
class HealthyTree : public nc::ComponentBase
{
    public:
        static constexpr float InfectThresholdSeconds = 5.0f;

        HealthyTree(nc::Entity self)
            : nc::ComponentBase{self} {}

        void Update(float dt)
        {
            if (m_infectedByCount > 0)
                m_timeInfected += dt;
            else
                m_timeInfected = 0.0f; // decrement with time?
        }

        void Infect() { ++m_infectedByCount; }

        void Disinfect()
        {
            NC_ASSERT(m_infectedByCount != 0, "expected at least one infected reference");
            --m_infectedByCount;
        }

        auto ShouldMorph() -> bool { return m_timeInfected > InfectThresholdSeconds; }

    private:
        size_t m_infectedByCount = 0;
        bool m_infected = false;
        float m_timeInfected = 0.0f;
};

class InfectedTree : public nc::ComponentBase
{
    public:
        static constexpr float RadiusGrowthAmount = 1.0f;
        static constexpr float RadiusSpreadTime = 1.0f;
        static constexpr unsigned MaxEmissionCount = 100;

        InfectedTree(nc::Entity self)
            : nc::ComponentBase{self} {}

        void Update(nc::ecs::Ecs, float dt);

    private:
        float m_timeSinceLastSpread = 0.0f;
};
} // namespace game

namespace nc
{
// Really annoying we have to put this in nc namespace, consider updating
template<>
struct StoragePolicy<game::HealthyTree> : DefaultStoragePolicy
{
    static constexpr bool EnableOnAddCallbacks = true;
    static constexpr bool EnableOnRemoveCallbacks = true;
};

template<>
struct StoragePolicy<game::InfectedTree> : DefaultStoragePolicy
{
    static constexpr bool EnableOnAddCallbacks = true;
    static constexpr bool EnableOnRemoveCallbacks = true;
};
} // namespace nc

namespace game
{
class TreeTracker
{
    public:
        TreeTracker(nc::ecs::ComponentRegistry& registry)
            : m_onAddHealthyConnection{registry.GetPool<HealthyTree>().OnAdd().Connect(this, &TreeTracker::AddHealthy)},
              m_onRemoveHealthyConnection{registry.GetPool<HealthyTree>().OnRemove().Connect(this, &TreeTracker::RemoveHealthy)},
              m_onAddInfectedConnection{registry.GetPool<InfectedTree>().OnAdd().Connect(this, &TreeTracker::AddInfected)},
              m_onRemoveInfectedConnection{registry.GetPool<InfectedTree>().OnRemove().Connect(this, &TreeTracker::RemoveInfected)}
        {
        }

        auto GetHealthyCount() const noexcept { return m_numHealthy; }
        auto GetInfectedCount() const noexcept { return m_numInfected; }

        void AddHealthy(HealthyTree&) { std::cerr << "add healthy\n"; ++m_numHealthy; }
        void AddInfected(InfectedTree&) { std::cerr << "add infected\n"; ++m_numInfected; }
        void RemoveHealthy(nc::Entity) { std::cerr << "remove healthy\n"; NC_ASSERT(m_numHealthy > 0, "invalid design"); --m_numHealthy; }
        void RemoveInfected(nc::Entity) { std::cerr << "remove infefcted\n"; NC_ASSERT(m_numInfected > 0, "invalid design");--m_numInfected; }

    private:
        size_t m_numHealthy = 0;
        size_t m_numInfected = 0;
        nc::Connection<HealthyTree&> m_onAddHealthyConnection;
        nc::Connection<nc::Entity> m_onRemoveHealthyConnection;
        nc::Connection<InfectedTree&> m_onAddInfectedConnection;
        nc::Connection<nc::Entity> m_onRemoveInfectedConnection;
};

// Create entity with renderer + collider
auto CreateTreeBase(nc::ecs::Ecs world,
                    const nc::Vector3& position,
                    const nc::Quaternion& rotation,
                    const nc::Vector3& scale,
                    const std::string& tag,
                    nc::Entity::layer_type layer,
                    const std::string& mesh) -> nc::Entity;

// Attach logic to anything with HealthyTree/InfectedTree layers
void FinalizeTrees(nc::ecs::Ecs world);

// Replace target with a healthy tree
void MorphTreeToHealthy(nc::ecs::Ecs world, nc::Entity target);

// Replace target with an infected tree
void MorphTreeToInfected(nc::ecs::Ecs world, nc::Entity target);

void RegisterTreeComponents(nc::ecs::ComponentRegistry& registry);

// System for tree update loop
void ProcessTrees(nc::Entity, nc::Registry* registry, float dt);
} // namespace game
