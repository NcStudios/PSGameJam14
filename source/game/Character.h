#pragma once

#include "Core.h"

namespace game
{
auto CreateCharacter(nc::ecs::Ecs world, nc::physics::NcPhysics* phys, const nc::Vector3& position) -> nc::Entity;

enum class VehicleState
{
    Idle,
    StartForward,
    Forward,
    StopForward,
    Back,
    Turning
};

class CharacterController : public nc::FreeComponent
{
    public:
        static constexpr auto moveAcceleration = 0.5f;
        static constexpr auto moveVelocityUpperBound = 10.0f;
        static constexpr auto moveVelocityLowerBound = 2.5f;
        static constexpr auto lungeAcceleration = moveAcceleration * 1.75f;
        static constexpr auto lungeVelocityUpperBound = moveVelocityUpperBound * 1.75f;

        static constexpr auto turnAcceleration = 0.05f;
        static constexpr auto maxStationaryTurnVelocity = 1.5f;
        static constexpr auto maxMovingTurnVelocity = maxStationaryTurnVelocity;

        static constexpr auto lungeCooldown = 0.35f;
        static constexpr auto sprayCooldown = 1.0f;

        CharacterController(nc::Entity self)
            : nc::FreeComponent{self} {}

        void Run(nc::Entity self, nc::Registry* registry);

        void EquipSprayer() { m_sprayerEquipped = true; }

    private:
        nc::Entity m_purifier = nc::Entity::Null();
        float m_currentMoveVelocity = 0.0f;
        float m_timeAtMoveBound = 0.0f;
        float m_currentTurnVelocity = 0.0f;
        float m_lungeRemainingCooldownTime = 0.0f;
        float m_sprayRemainingCooldownTime = 0.0f;
        bool m_inchDecelerating = false;
        bool m_lungeOnCooldown = false;
        bool m_sprayOnCooldown = false;
        bool m_sprayerEquipped = false;

        void CreatePurifier(nc::Registry* registry, float moveVelocity);
        void SetAudioState(nc::Registry* registry, VehicleState state);
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
