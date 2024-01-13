#include "MainScene.h"
#include "Character.h"
#include "Core.h"
#include "Tree.h"

namespace game
{
void MainScene::Load(nc::Registry* registry, nc::ModuleProvider modules)
{
    auto world = registry->GetEcs();

    const auto light = world.Emplace<nc::Entity>(nc::EntityInfo
    {
        .position = nc::Vector3{1.20484f, 9.4f, -8.48875f},
        .tag = "Light",
        .flags = nc::Entity::Flags::NoSerialize
    });

    world.Emplace<nc::graphics::PointLight>(light, nc::Vector3{0.443f, 0.412f, 0.412f}, nc::Vector3{0.4751f, 0.525f, 1.0f}, 600.0f);

    constexpr auto characterPosition = nc::Vector3::Up();
    CreateCharacter(characterPosition, registry, modules);

    const auto treeSystem = world.Emplace<nc::Entity>(nc::EntityInfo{.tag = "TreeSystem", .flags = nc::Entity::Flags::NoSerialize});
    world.Emplace<nc::FrameLogic>(treeSystem, &ProcessTrees);

    // Debug environment - just to have stuff in the world
    const auto floor = world.Emplace<nc::Entity>(nc::EntityInfo
    {
        .position = nc::Vector3::Up() * -1.0f,
        .scale = nc::Vector3{500.0f, 1.0f, 500.0f},
        .tag = "Floor",
        .flags = nc::Entity::Flags::NoSerialize
    });

    world.Emplace<nc::graphics::MeshRenderer>(floor);
    world.Emplace<nc::physics::Collider>(floor, nc::physics::BoxProperties{});
    world.Emplace<nc::physics::PhysicsBody>(floor, nc::physics::PhysicsProperties{.isKinematic = true});

    const auto treeRotation = nc::Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f);
    CreateHealthyTree(world, nc::Vector3{10.0f, 0.0f, 0.0f}, treeRotation, nc::Vector3::One());
    CreateHealthyTree(world, nc::Vector3{-10.0f, 0.0f, 0.0f}, treeRotation, nc::Vector3::One());

    CreateSicklyTree(world, nc::Vector3{0.0f, 0.0f, 10.0f}, treeRotation, nc::Vector3::One());
    CreateSicklyTree(world, nc::Vector3{0.0f, 0.0f, -10.0f}, treeRotation, nc::Vector3::One());

}
} // namespace game
