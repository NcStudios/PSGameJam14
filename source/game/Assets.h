#pragma once

#include "ncengine/graphics/ToonRenderer.h"

namespace nc::config
{
struct AssetSettings;
};

namespace game
{
void LoadAssets(const nc::config::AssetSettings& settings);

/** Colliders */
constexpr auto Terrain01Collider = "terrain01_collider.nca";

/** SFX */
constexpr auto ForestAmbienceSfx = "forest.nca";
constexpr auto EngineStartSfx = "engine_start.nca";
constexpr auto EngineRunningSfx = "engine_running.nca";
constexpr auto EngineStopSfx = "engine_stop.nca";
constexpr auto PurifySfx = "purify.nca";
constexpr auto MorphHealthySfx = "morph_healthy.nca";
constexpr auto MorphInfectedSfx = "morph_infected.nca";

/** Meshes */
constexpr auto FloorMesh = "floor.nca";
constexpr auto Terrain01Mesh = "terrain01.nca";
constexpr auto Terrain02Mesh = "terrain02.nca";
constexpr auto TerrainInletMesh = "terrain_inlet.nca";
constexpr auto TreeMesh = "tree.nca";
constexpr auto Tree01Mesh = "tree01.nca";
constexpr auto BusFrontMesh = "bus_front.nca";
constexpr auto BusCarMesh = "bus_car.nca";

/** Textures */
constexpr auto FloorTexture = "floor_base_color.nca";
constexpr auto Terrain01Texture = "terrain01_base_color.nca";
constexpr auto Terrain02Texture = "terrain02_base_color.nca";
constexpr auto TerrainInletTexture = "terrain_inlet_base_color.nca";
constexpr auto Tree01Texture = "tree01_base_color.nca";
constexpr auto BusFrontTexture = "bus_base_color.nca";
constexpr auto BusCarTexture = "bus_car_base_color.nca";
constexpr auto HatchTexture = "hatch.nca";
constexpr auto OverlayTexture = "overlay.nca";

/** Materials */
inline auto BusFrontMaterial = nc::graphics::ToonMaterial
{
    .baseColor = BusFrontTexture,
    .overlay = nc::asset::DefaultBaseColor,
    .hatching = nc::asset::DefaultBaseColor,
    .hatchingTiling = 6
};

inline auto BusCarMaterial = nc::graphics::ToonMaterial
{
    .baseColor = BusCarTexture,
    .overlay = nc::asset::DefaultBaseColor,
    .hatching = nc::asset::DefaultBaseColor,
    .hatchingTiling = 6
};

inline auto HealthyTree01Material = nc::graphics::ToonMaterial
{
    .baseColor = Tree01Texture,
    .overlay = OverlayTexture,
    .hatching = HatchTexture,
    .hatchingTiling = 6
};

inline auto InfectedTree01Material = nc::graphics::ToonMaterial
{
    .baseColor = nc::asset::DefaultBaseColor,
    .overlay = OverlayTexture,
    .hatching = HatchTexture,
    .hatchingTiling = 6
};
} // namespace game
