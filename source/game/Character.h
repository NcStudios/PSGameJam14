#pragma once

#include "Core.h"

namespace game
{
auto CreateCharacter(const nc::Vector3& position, nc::Registry* registry, nc::ModuleProvider modules) -> nc::Entity;
} // namespace game
