#include "MainScene.h"
#include "Assets.h"
#include "GameplayOrchestrator.h"
#include "Tree.h"
#include "ui/UI.h"

#include "ncengine/NcEngine.h"
#include "ncengine/utility/Log.h"

#include <iostream>

auto BuildConfig() -> nc::config::Config
{
    auto config = nc::config::Load("config.ini");

    config.projectSettings = nc::config::ProjectSettings{
        .projectName = "Scootin' Steve and Downy Dave: Origin",
        .logFilePath = "game.log",
        .logMaxFileSize = 100000
    };

    config.assetSettings = nc::config::AssetSettings{
        .audioClipsPath = "assets/nca/audio_clip/",
        .concaveCollidersPath = "assets/nca/concave_collider/",
        .hullCollidersPath = "assets/nca/hull_collider/",
        .meshesPath = "assets/nca/mesh/",
        .shadersPath = "assets/shaders/",
        .skeletalAnimationsPath = "assets/nca/skeletal_animation/",
        .texturesPath = "assets/nca/texture/",
        .cubeMapsPath = "assets/nca/cube_map/",
        .fontsPath = "assets/font/"
    };

    config.memorySettings = nc::config::MemorySettings{
        .maxDynamicColliders = 50,
        .maxStaticColliders = 500,
        .maxNetworkDispatchers = 0,
        .maxParticleEmitters = 100,
        .maxRenderers = 500,
        .maxTransforms = 1000,
        .maxPointLights = 10,
        .maxSkeletalAnimations = 1000,
        .maxTextures = 100
    };

    config.physicsSettings = nc::config::PhysicsSettings{};

    // allow some values in file
    config.graphicsSettings.enabled = true;
    config.graphicsSettings.useNativeResolution = false;
    config.graphicsSettings.targetFPS = 60;
    config.graphicsSettings.useShadows = false;

    return config;
}

int GameMain()
{
    std::unique_ptr<nc::NcEngine> engine;

    try
    {
        const auto config = BuildConfig();
        engine = nc::InitializeNcEngine(config);
        game::LoadAssets(config.assetSettings);
        auto& world = engine->GetComponentRegistry();
        game::RegisterTreeComponents(world);
        auto ui = game::GameUI{engine.get()};
        auto orchestrator = game::GameplayOrchestrator{engine.get(), &ui};
        engine->Start(std::make_unique<game::MainScene>([&orchestrator](float dt) { orchestrator.Run(dt); }));
    }
    catch (std::exception& e)
    {
        NC_LOG_EXCEPTION(e);
    }
    catch (...)
    {
        NC_LOG_ERROR("GameMain.cpp - unknown exception");
        std::cerr << "GameMain.cpp - unknown exception\n";
    }

    return 0;
}

// Just don't have time to wire up output for Win32 app, leave as console for dev builds
#ifdef GAME_PROD_BUILD
#include "ncengine/platform/win32/NcWin32.h"

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    return GameMain();
}

#else

int main()
{
    return GameMain();
}

#endif
