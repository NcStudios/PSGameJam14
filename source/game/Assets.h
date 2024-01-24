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
constexpr auto Terrain02Collider = "terrain02_collider.nca";
constexpr auto TerrainCurve01Collider = "terrain_curve01_collider.nca";
constexpr auto TerrainCurve02Collider = "terrain_curve02_collider.nca";
constexpr auto TerrainInletCollider = "terrain_inlet_collider.nca";

/** SFX */
constexpr auto IntroThemeMusic = "intro_theme.nca";
constexpr auto BlightClearedMusic = "blight_cleared.nca";
constexpr auto LoseMusic = "lose.nca";
constexpr auto EndingMusic = "ending.nca";
constexpr auto ForestAmbienceSfx = "forest.nca";
constexpr auto EngineStartSfx = "engine_start.nca";
constexpr auto EngineRunningSfx = "engine_running.nca";
constexpr auto EngineStopSfx = "engine_stop.nca";
constexpr auto SpraySfx = "spray1.nca";
constexpr auto MorphHealthySfx = "morph_healthy.nca";
constexpr auto MorphInfectedSfx = "morph_infected.nca";

/** Cube Maps  - Skybox */
constexpr auto Skybox = "skybox.nca";

/** Meshes */
/** Meshes - Character */
constexpr auto BusCarMesh = "bus_car.nca";
constexpr auto BusFrontMesh = "bus_front.nca";
constexpr auto DaveMesh = "dave.nca";

/** Meshes - Floor */
constexpr auto FloorMesh = "floor.nca";

/** Meshes - Terrain */
constexpr auto AloeMesh = "aloe.nca";
constexpr auto FernMesh = "fern.nca";
constexpr auto FireMesh = "fire.nca";
constexpr auto FirepitMesh = "firepit.nca";
constexpr auto GrassMesh = "grass.nca";
constexpr auto RockMesh = "rock.nca";
constexpr auto StatueMesh = "statue.nca";
constexpr auto Terrain01Mesh = "terrain01.nca";
constexpr auto TerrainCurve01Mesh = "terrain_curve01.nca";
constexpr auto TerrainCurve02Mesh = "terrain_curve02.nca";
constexpr auto TerrainEdgeMesh = "terrain_edge.nca";
constexpr auto Terrain02Mesh = "terrain02.nca";
constexpr auto TerrainInletMesh = "terrain_inlet.nca";
constexpr auto Vegetation01Mesh = "vegetation01.nca";

/** Meshes - Tree */
constexpr auto AspensMesh = "aspens.nca";
constexpr auto PineMesh = "pine.nca";
constexpr auto Tree01Mesh = "tree01.nca";
constexpr auto DecorativeTree01TreeMesh = "decorative_tree01.nca";

/** Animations */
/** Animations - Dave */
constexpr auto DaveWave = "dave_wave.nca";
constexpr auto DaveIdle = "dave_idle.nca";
constexpr auto DaveSittingGround = "dave_idle_sitting_ground.nca";
constexpr auto DaveStandupGround = "dave_idle_sit_to_stand_ground.nca";
constexpr auto DaveSittingStump = "dave_idle_sitting_stump.nca";
constexpr auto DaveStandupStump = "dave_sit_to_stand_stump.nca";
constexpr auto DaveCircleWalk = "dave_walk_in_circle.nca";

/** Animations - Fire */
constexpr auto FireFlicker = "fire_flicker.nca";

/** Textures */
/** Textures - Character */
constexpr auto BusCarTexture = "bus_car_base_color.nca";
constexpr auto BusFrontTexture = "bus_base_color.nca";
constexpr auto DaveTexture = "dave_base_color.nca";

/** Textures - Floor */
constexpr auto FloorTexture = "floor_base_color.nca";

/** Textures - Detail */
constexpr auto HatchTexture = "hatch.nca";
constexpr auto OverlayTexture = "overlay.nca";

/** Textures - Terrain */
constexpr auto AloeTexture = "aloe_base_color.nca";
constexpr auto FernTexture = "fern_base_color.nca";
constexpr auto FireTexture = "fire_base_color.nca";
constexpr auto FirepitTexture = "firepit_base_color.nca";
constexpr auto GrassTexture = "grass_base_color.nca";
constexpr auto RockTexture = "rock_base_color.nca";
constexpr auto StatueTexture = "statue_base_color.nca";
constexpr auto Terrain01Texture = "terrain01_base_color.nca";
constexpr auto Terrain02Texture = "terrain02_base_color.nca";
constexpr auto TerrainCurve01Texture = "terrain_curve_01_base_color.nca";
constexpr auto TerrainCurve02Texture = "terrain_curve_02_base_color.nca";
constexpr auto TerrainEdgeTexture = "terrain_edge_base_color.nca";
constexpr auto TerrainInletTexture = "terrain_inlet_base_color.nca";
constexpr auto Vegetation01Texture = "vegetation01_base_color.nca";

/** Textures - Tree */
constexpr auto AspensTexture = "aspens_base_color.nca";
constexpr auto PineTexture = "pine_base_color.nca";
constexpr auto Tree01Texture = "tree01_base_color.nca";
constexpr auto Tree01InfectedTexture = "tree01_infected_base_color.nca";
constexpr auto DecorativeTree01TreeTexture = "decorative_tree01_base_color.nca";

/** Textures - Particles */
constexpr auto TitleScreenParticle = "title_screen.nca";
constexpr auto QuestIndicatorParticle = "quest_indicator_particle.nca";
constexpr auto RoundemupParticle = "roundemup_particle.nca";
constexpr auto BlightParticle = "blight_particle.nca";
constexpr auto MorphInfectedParticle = "to_infected_particle.nca";
constexpr auto MorphHealthyParticle = "to_healthy_particle.nca";

/** Materials */
inline auto BusFrontMaterial = nc::graphics::ToonMaterial
{
    .baseColor = BusFrontTexture,
    .overlay = nc::asset::DefaultBaseColor,
    .hatching = HatchTexture,
    .hatchingTiling = 6
};

inline auto BusCarMaterial = nc::graphics::ToonMaterial
{
    .baseColor = BusCarTexture,
    .overlay = nc::asset::DefaultBaseColor,
    .hatching = HatchTexture,
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
    .baseColor = Tree01InfectedTexture,
    .overlay = OverlayTexture,
    .hatching = HatchTexture,
    .hatchingTiling = 6
};

inline auto AspensMaterial = nc::graphics::ToonMaterial
{
    .baseColor = AspensTexture,
    .overlay = OverlayTexture,
    .hatching = HatchTexture,
    .hatchingTiling = 9
};

inline auto PineMaterial = nc::graphics::ToonMaterial
{
    .baseColor = PineTexture,
    .overlay = OverlayTexture,
    .hatching = HatchTexture,
    .hatchingTiling = 5
};

inline auto GrassMaterial = nc::graphics::ToonMaterial
{
    .baseColor = GrassTexture,
    .overlay = OverlayTexture,
    .hatching = HatchTexture,
    .hatchingTiling = 6
};

inline auto FernMaterial = nc::graphics::ToonMaterial
{
    .baseColor = FernTexture,
    .overlay = FernTexture,
    .hatching = HatchTexture,
    .hatchingTiling = 5
};

inline auto FloorMaterial = nc::graphics::ToonMaterial
{
    .baseColor = FloorTexture,
    .overlay = OverlayTexture,
    .hatching = nc::asset::DefaultBaseColor,
    .hatchingTiling = 5
};

inline auto AloeMaterial = nc::graphics::ToonMaterial
{
    .baseColor = AloeTexture,
    .overlay = OverlayTexture,
    .hatching = HatchTexture,
    .hatchingTiling = 2
};

inline auto DaveMaterial = nc::graphics::ToonMaterial
{
    .baseColor = DaveTexture,
    .overlay = OverlayTexture,
    .hatching = HatchTexture,
    .hatchingTiling = 6
};

inline auto FirepitMaterial = nc::graphics::ToonMaterial
{
    .baseColor = FirepitTexture,
    .overlay = OverlayTexture,
    .hatching = HatchTexture,
    .hatchingTiling = 6
};

inline auto FireMaterial = nc::graphics::ToonMaterial
{
    .baseColor = FireTexture,
    .overlay = OverlayTexture,
    .hatching = HatchTexture,
    .hatchingTiling = 3
};
} // namespace game
