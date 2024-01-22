#include "MainScene.h"
#include "Assets.h"
#include "Character.h"
#include "Core.h"
#include "DebugCamera.h"
#include "Environment.h"
#include "Event.h"
#include "FollowCamera.h"
#include "QuestTrigger.h"
#include "Sasquatch.h"
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
    [[maybe_unused]] auto ncRandom = modules.Get<nc::Random>();

    if (EnableGameplay)
    {
        // Runs the GameplayOrchestrator loop
        auto storyRunner = world.Emplace<nc::Entity>({.tag = "StoryRunner", .flags = nc::Entity::Flags::NoSerialize});
        world.Emplace<nc::FrameLogic>(storyRunner, [this](nc::Entity, nc::Registry*, float dt){ m_runOrchestrator(dt); });
    }

    // Look out, tedium ahead
    // When modifying the scene, do not save to this name. It will get overwritten on install. Save with another filename, and update
    // this path while doing modifications. Once complete, backup the latest 'workspace/scene' directory somewhere in 'workspace/backup'
    // (backup 'workspace/prefab' too, if you changed anything) Then, save your temp scene from 'install/your_scene_name' to
    // 'workspace/scene/terrain' (or 'workspace/prefab/your_prefab').
    auto ncAsset = modules.Get<nc::asset::NcAsset>();
    ::LoadBaseScene(world, ncAsset, "scene/level");

    const auto characterSpawnPos = nc::Vector3{120.0f, 0.0f, -136.0f};
    const auto character = CreateCharacter(world, phys, characterSpawnPos);
    const auto camera = CreateCamera(world, gfx, characterSpawnPos, character);
    ncAudio->RegisterListener(camera);

    // Base objects for initial infected trees; they start with 'healthy' assets
    CreateTreeBase(world, nc::Vector3{5.0f, 0.0f, -12.5f}, nc::Quaternion{}, nc::Vector3::One(), tag::InfectedTree, layer::InfectedTree, Tree01Mesh, HealthyTree01Material);
    CreateTreeBase(world, nc::Vector3{138.0f, 0.0f, -22.0f}, nc::Quaternion{}, nc::Vector3::One(), tag::InfectedTree, layer::InfectedTree, Tree01Mesh, HealthyTree01Material);
    CreateTreeBase(world, nc::Vector3{24.5f, 0.0f, -116.5f}, nc::Quaternion{}, nc::Vector3::One(), tag::InfectedTree, layer::InfectedTree, Tree01Mesh, HealthyTree01Material);

    gfx->SetSkybox(Skybox);

    // Placeholder audio for now. If your audio is wonky, comment out these lines. (and lmk)
    const auto globalAudio = world.Emplace<nc::Entity>({.tag = "GlobalAudio", .flags = nc::Entity::Flags::NoSerialize});
    const auto ambience = world.Emplace<nc::Entity>({.parent = globalAudio, .tag = tag::AmbienceSfx, .flags = nc::Entity::Flags::NoSerialize});
    const auto introTheme = world.Emplace<nc::Entity>({.parent = globalAudio, .tag = tag::IntroThemeMusic, .flags = nc::Entity::Flags::NoSerialize});
    world.Emplace<nc::audio::AudioSource>(ambience, ForestAmbienceSfx, nc::audio::AudioSourceProperties{.gain = 0.3f, .loop = true})->Play();
    world.Emplace<nc::audio::AudioSource>(introTheme, IntroThemeMusic, nc::audio::AudioSourceProperties{.gain = 0.4f, .loop = false})->Play();

    //////////////////////////////////////////////////////
    // Debug environment - just to have stuff in the world
#ifndef GAME_PROD_BUILD
    CreateDebugCamera(world, gfx); // MAKE SURE NOT IN FINAL BUILD
#endif
    // Spawning ops
#if 0
    RandomlyPopulateTerrain(world, ncRandom);

    auto saver = world.Emplace<nc::Entity>({.flags = nc::Entity::Flags::NoSerialize});
    world.Emplace<nc::FrameLogic>(saver, [](nc::Entity, nc::Registry* registry, float)
    {
        if (nc::input::KeyDown(nc::input::KeyCode::F9))
        {
            static auto filter = [](nc::Entity entity) { return entity.Layer() == layer::Foliage; };
            auto fragment = std::ofstream{"foliage", std::ios::binary | std::ios::trunc};
            nc::SaveSceneFragment(fragment, registry->GetEcs(), nc::asset::AssetMap{}, filter);
        }
    });
#endif

    registry->CommitStagedChanges(); // so we can search by tag

    // Not part of debug env, just needs to happen last
    // These modify serialized objects: DO NOT SAVE SCENE WHEN ENABLED!
    if constexpr (EnableGameplay)
    {
        FinalizeTerrain(world);
        AttachSasquatchAnimators(world);
        // Init GameplayManager sequence
        FireEvent(Event::Intro);
    }
}
} // namespace game
