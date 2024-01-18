#pragma once

#include "Core.h"

#include "ncengine/graphics/SceneNavigationCamera.h"

namespace game
{
// SceneNavigationCamera for debug - toggle with F5
inline void CreateDebugCamera(nc::ecs::Ecs world, nc::graphics::NcGraphics* gfx)
{
    const auto debugCamera = world.Emplace<nc::Entity>(
    {
        .position = nc::Vector3{15.0f, 80.0f, -200.0f},
        .rotation = nc::Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f),
        .tag = "DebugCamera",
        .flags = nc::Entity::Flags::NoSerialize
    });

    nc::graphics::Camera* prevCam = nullptr;
    auto toggleFunc = [prevCam, gfx, enabled = false](nc::Entity self, nc::Registry* registry, float dt) mutable
    {
        if (nc::input::KeyDown(hotkey::ToggleDebugCamera))
        {
            if (enabled)
            {
                registry->Remove<nc::graphics::SceneNavigationCamera>(self);
                gfx->SetCamera(prevCam);
            }
            else
            {
                prevCam = gfx->GetCamera();
                auto camera = registry->Add<nc::graphics::SceneNavigationCamera>(self);
                gfx->SetCamera(camera);
            }

            enabled = !enabled;
        }

        if (enabled)
        {
            registry->Get<nc::graphics::SceneNavigationCamera>(self)->Run(self, registry, dt);
        }
    };

    world.Emplace<nc::FrameLogic>(debugCamera, toggleFunc);
}
} // namespace game
