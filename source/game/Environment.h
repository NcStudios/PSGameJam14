#pragma once

#include "Core.h"

namespace game
{
// Apply colliders to terrain based on layer
void FinalizeTerrain(nc::ecs::Ecs world);

// World Generation
void RandomlyPopulateTerrain(nc::ecs::Ecs world, nc::Random* random);
auto FilterTerrainEntities(nc::ecs::Ecs world) -> std::vector<nc::Entity>;
auto FilterBorderEntities(nc::ecs::Ecs world) -> std::vector<nc::Entity>;
void GenerateGrass(nc::ecs::Ecs world, nc::Random* random, const std::vector<nc::Entity>& terrain);
void GenerateVegetation(nc::ecs::Ecs world, nc::Random* random, const std::vector<nc::Entity>& terrain);
void GenerateTrees(nc::ecs::Ecs world, nc::Random* random, const std::vector<nc::Entity>& terrain);

void GenerateBorderTrees(nc::ecs::Ecs world, nc::Random* random, const std::vector<nc::Entity>& terrain);
} // namespace game
