#pragma once

#include "Core.h"

namespace game
{
// Apply colliders to terrain based on layer
void FinalizeTerrain(nc::ecs::Ecs world);


// World Generation
void GenerateGrass(nc::ecs::Ecs world, nc::Random* random);
void GenerateTrees(nc::ecs::Ecs world, nc::Random* random);
} // namespace game
