#include "MainScene.h"
#include "Assets.h"
#include "Character.h"
#include "Core.h"
#include "DebugCamera.h"
#include "Event.h"
#include "FollowCamera.h"
#include "Tree.h"

#include "ncengine/serialize/SceneSerialization.h"

#include <fstream>

void LoadBaseScene(nc::ecs::Ecs world, nc::asset::NcAsset* ncAsset, const std::string& path)
{
    auto file = std::ifstream{path, std::ios::binary};
    NC_ASSERT(file.is_open(), fmt::format("Scene fragment '{}' not found.", path));
    nc::LoadSceneFragment(file, world, *ncAsset);
}

namespace game
{
void MainScene::Load(nc::Registry* registry, nc::ModuleProvider modules)
{
    auto world = registry->GetEcs();
    auto gfx = modules.Get<nc::graphics::NcGraphics>();
    auto phys = modules.Get<nc::physics::NcPhysics>();
    auto ncAudio = modules.Get<nc::audio::NcAudio>();

    auto storyRunner = world.Emplace<nc::Entity>({.tag = "StoryRunner"});
    world.Emplace<nc::FrameLogic>(storyRunner, [this](nc::Entity, nc::Registry*, float dt)
    {
        m_runOrchestrator(dt);
    });

    // To overlay a saved scene: copy from fragment from install/game to workspace/scene/your_file and change the path below
    // auto ncAsset = modules.Get<nc::asset::NcAsset>();
    // ::LoadBaseScene(world, ncAsset, "scene/terrain");

    const auto character = CreateCharacter(world, phys, nc::Vector3{0.0f, 0.5f, 0.0f});
    const auto camera = CreateCamera(world, gfx, character);
    ncAudio->RegisterListener(camera);

    const auto treeSystem = world.Emplace<nc::Entity>(nc::EntityInfo{.tag = tag::TreeSystem, .flags = nc::Entity::Flags::NoSerialize});
    world.Emplace<nc::FrameLogic>(treeSystem, &ProcessTrees);

    // Placeholder audio for now. If your audio is wonky, comment out these lines. (and lmk)
    const auto globalAudio = world.Emplace<nc::Entity>({.tag = "GlobalAudio", .flags = nc::Entity::Flags::NoSerialize});
    const auto ambience = world.Emplace<nc::Entity>({.parent = globalAudio, .tag = "Ambience", .flags = nc::Entity::Flags::NoSerialize});
    world.Emplace<nc::audio::AudioSource>(ambience, ForestAmbience, nc::audio::AudioSourceProperties{.gain = 0.5f, .loop = true})->Play();

    // Init GameplayManager sequence
    FireEvent(Event::Intro);

    //////////////////////////////////////////////////////
    // Debug environment - just to have stuff in the world
#ifndef GAME_PROD_BUILD
    CreateDebugCamera(world, gfx); // MAKE SURE NOT IN FINAL BUILD
#endif

    const auto light = world.Emplace<nc::Entity>(nc::EntityInfo
    {
        .position = nc::Vector3{1.20484f, 9.4f, -8.48875f},
        .tag = "Light",
        .flags = nc::Entity::Flags::NoSerialize
    });

    world.Emplace<nc::graphics::PointLight>(light, nc::Vector3{0.443f, 0.412f, 0.412f}, nc::Vector3{0.4751f, 0.525f, 1.0f}, 600.0f);

    const auto floor = world.Emplace<nc::Entity>(nc::EntityInfo
    {
        .position = nc::Vector3::Up() * -1.0f,
        .scale = nc::Vector3{300.0f, 1.0f, 300.0f},
        .tag = tag::Ground,
        .layer = layer::Ground,
        .flags = nc::Entity::Flags::NoSerialize | nc::Entity::Flags::Static
    });

    world.Emplace<nc::graphics::MeshRenderer>(floor);
    world.Emplace<nc::physics::Collider>(floor, nc::physics::BoxProperties{});
    world.Emplace<nc::physics::PhysicsBody>(floor, nc::physics::PhysicsProperties{.isKinematic = true});

    const auto treeRotation = nc::Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f);
    CreateTreeBase(world, nc::Vector3{10.0f, 0.0f, 0.0f}, treeRotation, nc::Vector3::One(), tag::HealthyTree, layer::HealthyTree, TreeMesh);
    CreateTreeBase(world, nc::Vector3{-10.0f, 0.0f, 0.0f}, treeRotation, nc::Vector3::One(), tag::HealthyTree, layer::HealthyTree, TreeMesh);
    CreateTreeBase(world, nc::Vector3{0.0f, 0.0f, 10.0f}, treeRotation, nc::Vector3::One(), tag::InfectedTree, layer::InfectedTree, nc::asset::SphereMesh);
    CreateTreeBase(world, nc::Vector3{0.0f, 0.0f, -10.0f}, treeRotation, nc::Vector3::One(), tag::InfectedTree, layer::InfectedTree, nc::asset::SphereMesh);

    // Not part of debug env, just needs to happen last
    FinalizeTrees(world);
}
} // namespace game
