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
        static constexpr auto moveVelocityUpperBound = 8.0f;
        static constexpr auto moveVelocityLowerBound = 2.0f;
        static constexpr auto turnAcceleration = 0.05f;
        static constexpr auto maxTurnVelocity = 1.0f;
        static constexpr auto jumpForce = 30.0f;
        static constexpr auto jumpCooldown = 0.35f;
        static constexpr auto purifyCooldown = 1.0f;

        CharacterController(nc::Entity self)
            : nc::FreeComponent{self} {}

        void Run(nc::Entity self, nc::Registry* registry);

    private:
        nc::Entity m_purifier = nc::Entity::Null();
        float m_currentMoveVelocity = 0.0f;
        float m_timeAtMoveBound = 0.0f;
        float m_currentTurnVelocity = 0.0f;
        float m_jumpRemainingCooldownTime = 0.0f;
        float m_purifyRemainingCooldownTime = 0.0f;
        bool m_inchDecelerating = false;
        bool m_jumpOnCooldown = false;
        bool m_purifyOnCooldown = false;

        void CreatePurifier(nc::Registry* registry);
};

enum class VehicleState
{
    Idle,
    StartForward,
    Forward,
    StopForward,
    Back,
    Turning
};

class CharacterAudio : public nc::FreeComponent
{
    public:
        CharacterAudio(nc::Entity self, nc::Entity character);

        void Init(nc::ecs::Ecs world);
        void Run(nc::Entity self, nc::Registry* registry, float);
        void SetState(VehicleState state);
        void PlayPurifySfx(nc::ecs::Ecs world);

    private:
        VehicleState m_currentState = VehicleState::Idle;
        VehicleState m_nextState = VehicleState::Idle;
        nc::Entity m_engineStartPlayer;
        nc::Entity m_engineRunningPlayer;
        nc::Entity m_engineStopPlayer;
        nc::Entity m_currentEnginePlayer = nc::Entity::Null();
        nc::Entity m_purifyPlayer;
};
} // namespace game
