#pragma once

#include "ncengine/scene/Scene.h"

#include <functional>

namespace game
{
class MainScene : public nc::Scene
{
    public:
        MainScene(std::function<void(float)> runOrchestrator)
            : m_runOrchestrator{std::move(runOrchestrator)}
        {
        }

        void Load(nc::Registry* registry, nc::ModuleProvider modules) override;

    private:
        std::function<void(float)> m_runOrchestrator;
};
} // namespace game
