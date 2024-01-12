#include "MainScene.h"
#include "Character.h"
#include "Core.h"

namespace game
{
void MainScene::Load(nc::Registry* registry, nc::ModuleProvider modules)
{
    auto world = registry->GetEcs();

    constexpr auto characterPosition = nc::Vector3::Up();
    CreateCharacter(characterPosition, registry, modules);

    const auto floor = world.Emplace<nc::Entity>(nc::EntityInfo
    {
        .scale = nc::Vector3{100.0f, 1.0f, 100.0f},
        .tag = "Floor",
        .layer = game::Layer::DoNotSerialize
    });

    world.Emplace<nc::graphics::MeshRenderer>(floor);
    world.Emplace<nc::physics::Collider>(floor, nc::physics::BoxProperties{});
    world.Emplace<nc::physics::PhysicsBody>(floor, nc::physics::PhysicsProperties{.isKinematic = true});
}
} // namespace game
