#pragma once

#include "Core.h"

namespace game
{
auto CreateHealthyNode(nc::ecs::Ecs world) -> nc::Entity;
auto CreateSicklyNode(nc::ecs::Ecs world) -> nc::Entity;
void UpdateNode(nc::Entity target, nc::ecs::Ecs world);
} // namespace game
