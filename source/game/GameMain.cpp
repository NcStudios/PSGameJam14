#include "MainScene.h"
#include "Assets.h"
#include "GameplayOrchestrator.h"
#include "Tree.h"
#include "UI.h"

#include "ncengine/NcEngine.h"
#include "ncengine/utility/Log.h"

#include <iostream>

int main()
{
    std::unique_ptr<nc::NcEngine> engine;

    try
    {
        const auto config = nc::config::Load("config.ini");
        engine = nc::InitializeNcEngine(config);
        game::LoadAssets(config.assetSettings);
        auto& world = engine->GetRegistry()->GetImpl();
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
