#pragma once

#include "Core.h"

namespace game
{
auto CreateCharacter(nc::ecs::Ecs world, nc::physics::NcPhysics* phys, const nc::Vector3& position) -> nc::Entity;

class CharacterController : public nc::FreeComponent
{
    public:
        static constexpr auto fixedDt = 0.01667f;
        static constexpr auto moveAcceleration = 0.4f;
        static constexpr auto maxMoveVelocity = 8.0f;
        static constexpr auto turnAcceleration = 0.05f;
        static constexpr auto maxTurnVelocity = 2.0f;
        static constexpr auto jumpForce = 30.0f;
        static constexpr auto jumpCooldown = 0.35f;
        static constexpr auto purifyCooldown = 3.0f;

        CharacterController(nc::Entity self)
            : nc::FreeComponent{self} {}

        void Run(nc::Entity self, nc::Registry* registry);

    private:
        nc::Entity m_purifier = nc::Entity::Null();
        float m_currentMoveVelocity = 0.0f;
        float m_currentTurnVelocity = 0.0f;
        float m_jumpRemainingCooldownTime = 0.0f;
        float m_purifyRemainingCooldownTime = 0.0f;
        bool m_jumpOnCooldown = false;
        bool m_purifyOnCooldown = false;

        void CreatePurifier(nc::Registry* registry);
};
} // namespace game
