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
constexpr auto ForestAmbience = "forest.nca";
constexpr auto EngineStart = "engine_start.nca";
constexpr auto EngineRunning = "engine_running.nca";
constexpr auto EngineStop = "engine_stop.nca";
constexpr auto VehicleCrash01 = "vehicle_crash01.nca";
constexpr auto VehicleCrash02 = "vehicle_crash02.nca";
constexpr auto VehicleCrash03 = "vehicle_crash03.nca";

/** Meshes */
constexpr auto Terrain01Mesh = "terrain01.nca";
constexpr auto TreeMesh = "tree.nca";
constexpr auto Tree01Mesh = "tree01.nca";
constexpr auto BusFrontMesh = "bus_front.nca";
constexpr auto BusCarMesh = "bus_car.nca";

/** Textures */
constexpr auto Terrain01Texture = "terrain01_base_color.nca";
constexpr auto Tree01Texture = "tree01_base_color.nca";
constexpr auto BusFrontTexture = "bus_base_color.nca";
constexpr auto BusCarTexture = "bus_car_base_color.nca";
constexpr auto HatchTexture = "hatch.nca";
constexpr auto OverlayTexture = "overlay.nca";

/** Materials */
inline static auto BusFrontMaterial = nc::graphics::ToonMaterial
{
    .baseColor = BusFrontTexture,
    .overlay = nc::asset::DefaultBaseColor,
    .hatching = nc::asset::DefaultBaseColor,
    .hatchingTiling = 6
};

inline static auto BusCarMaterial = nc::graphics::ToonMaterial
{
    .baseColor = BusCarTexture,
    .overlay = nc::asset::DefaultBaseColor,
    .hatching = nc::asset::DefaultBaseColor,
    .hatchingTiling = 6
};

} // namespace game
