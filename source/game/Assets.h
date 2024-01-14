#pragma once

#include "ncengine/graphics/MeshRenderer.h"

namespace nc::config
{
struct AssetSettings;
};

namespace game
{
void LoadAssets(const nc::config::AssetSettings& settings);

/** SFX */
constexpr auto ForestAmbience = "forest.nca";

/** Meshes */
constexpr auto TreeMesh = "tree.nca";
constexpr auto BusFrontMesh = "bus_front.nca";
constexpr auto BusCarMesh = "bus_car.nca";

/** Textures */
constexpr auto BusFrontTexture = "bus_base_color.nca";
constexpr auto BusCarTexture = "bus_car_base_color.nca";
constexpr auto HatchTexture = "hatch.nca";
constexpr auto OverlayTexture = "overlay.nca";

/** Materials */
inline static auto BusFrontMaterial = nc::graphics::PbrMaterial
{
    .baseColor = BusFrontTexture,
    .normal = nc::asset::DefaultNormal,
    .roughness = OverlayTexture,
    .metallic = nc::asset::DefaultMetallic
};

inline static auto BusCarMaterial = nc::graphics::PbrMaterial
{
    .baseColor = BusCarTexture,
    .normal = nc::asset::DefaultNormal,
    .roughness = OverlayTexture,
    .metallic = nc::asset::DefaultMetallic
};
} // namespace game
