#pragma once

#include "Core.h"

namespace game
{
class FollowCamera : public nc::graphics::Camera
{
    public:
        static constexpr auto DefaultFollowDistance = 10.0f;
        static constexpr auto DefaultFollowHeight = 5.0f;
        static constexpr auto DefaultFollowSpeed = 4.0f;

        FollowCamera(nc::Entity self, nc::Entity target)
            : nc::graphics::Camera{self}, m_target{target} {}

        void Run(nc::Entity entity, nc::Registry* registry, float dt);

        void SetTarget(nc::Entity target) noexcept { m_target = target; }
        void SetFollowDistance(float distance) noexcept { m_followDistance = distance; }
        void SetFollowHeight(float height) noexcept { m_followHeight = height; }
        void SetFollowSpeed(float speed) noexcept { m_followSpeed = speed; }

    private:
        nc::Entity m_target;
        float m_followDistance = DefaultFollowDistance;
        float m_followHeight = DefaultFollowHeight;
        float m_followSpeed = DefaultFollowSpeed;
};

auto CreateCamera(nc::ecs::Ecs world, nc::graphics::NcGraphics* gfx, const nc::Vector3& initialPosition, nc::Entity initialTarget) -> nc::Entity;
} // namespace game
