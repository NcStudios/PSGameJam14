#pragma once

#include "Core.h"

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

        InfectedTree(nc::Entity self)
            : nc::ComponentBase{self} {}

        void Update(nc::ecs::Ecs, float dt);

    private:
        float m_timeSinceLastSpread = 0.0f;
};

void RegisterTreeComponents(nc::ecs::ComponentRegistry& registry);
void ProcessTrees(nc::Entity, nc::Registry* registry, float dt);

auto CreateHealthyTree(nc::ecs::Ecs world,
                       const nc::Vector3& position,
                       const nc::Quaternion& rotation,
                       const nc::Vector3& scale) -> nc::Entity;

auto CreateSicklyTree(nc::ecs::Ecs world,
                      const nc::Vector3& position,
                      const nc::Quaternion& rotation,
                      const nc::Vector3& scale) -> nc::Entity;

void UpdateTree(nc::Entity target, nc::ecs::Ecs world);
} // namespace game
