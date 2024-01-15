#pragma once

#include "Core.h"

namespace game
{
class FollowCamera : public nc::graphics::Camera
{
    public:
        static constexpr auto FollowDistance = 12.0f;
        static constexpr auto FollowHeight = 6.0f;
        static constexpr auto FollowSpeed = 4.0f;

        FollowCamera(nc::Entity self, nc::Entity target)
            : nc::graphics::Camera{self}, m_target{target} {}

        void SetTarget(nc::Entity target) noexcept { m_target = target; }
        void Run(nc::Entity entity, nc::Registry* registry, float dt);

    private:
        nc::Entity m_target;
};

auto CreateCamera(nc::ecs::Ecs world, nc::graphics::NcGraphics* gfx, nc::Entity initialTarget) -> nc::Entity;
} // namespace game
