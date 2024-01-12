#pragma once

#include "ncengine/scene/Scene.h"

namespace game
{
class MainScene : public nc::Scene
{
    void Load(nc::Registry* registry, nc::ModuleProvider modules) override;
};
} // namespace game
