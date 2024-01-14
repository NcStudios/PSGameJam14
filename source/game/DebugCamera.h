#pragma once

#include "ncengine/ecs/Logic.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/SceneNavigationCamera.h"
#include "ncengine/input/Input.h"

namespace game
{
// SceneNavigationCamera for debug - toggle with F5
inline void CreateDebugCamera(nc::ecs::Ecs world, nc::graphics::NcGraphics* gfx)
{
    const auto debugCamera = world.Emplace<nc::Entity>(
    {
        .position = nc::Vector3{0.0f, 6.1f, -6.5f},
        .rotation = nc::Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f),
        .tag = "DebugCamera",
        .flags = nc::Entity::Flags::NoSerialize
    });

    nc::graphics::Camera* prevCam = nullptr;
    auto toggleFunc = [prevCam, gfx, enabled = false](nc::Entity self, nc::Registry* registry, float dt) mutable
    {
        if (nc::input::KeyDown(nc::input::KeyCode::F5))
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
