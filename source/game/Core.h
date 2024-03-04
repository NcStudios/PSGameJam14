#pragma once

#include "ncengine/NcEngine.h"
#include "ncengine/asset/NcAsset.h"
#include "ncengine/asset/Assets.h"
#include "ncengine/asset/DefaultAssets.h"
#include "ncengine/audio/AudioSource.h"
#include "ncengine/audio/NcAudio.h"
#include "ncengine/ecs/Logic.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/graphics/Camera.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/graphics/SkeletalAnimator.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/input/Input.h"
#include "ncengine/math/Random.h"
#include "ncengine/module/ModuleProvider.h"
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/ConcaveCollider.h"
#include "ncengine/physics/NcPhysics.h"
#include "ncengine/physics/PhysicsBody.h"
#include "ncengine/physics/PhysicsMaterial.h"
#include "ncengine/scene/Scene.h"

#include <string_view>

namespace game
{
// Enable/disable calls to finalize environment (adding game logic and such) and triggering win/lose condition
// DO NOT SAVE SCENES WITH GAMEPLAY ENABLED!
constexpr auto EnableGameplay = true;

namespace layer
{
// IMPORTANT! Do not change layer values, else serialized work will be garbage!
constexpr uint8_t None = 0;
constexpr uint8_t Default = 1;
constexpr uint8_t Character = 2;
constexpr uint8_t HealthyTree = 3;
constexpr uint8_t InfectedTree = 4;
constexpr uint8_t Spreader = 5;
constexpr uint8_t Purifier = 6;
constexpr uint8_t Ground = 7;
constexpr uint8_t BoxCar = 8;
constexpr uint8_t Terrain = 9; // remove? check not used first
constexpr uint8_t Border = 10; // outer border
constexpr uint8_t Blockade = 11; // obstacle
constexpr uint8_t QuestTrigger = 12;
constexpr uint8_t Dave = 13;
constexpr uint8_t GenericSasquatch = 14;
constexpr uint8_t Foliage = 15;
constexpr uint8_t Detail = 16;

// Reserving [100, 150] for terrain
// so we can script applying stuff (colliders, etcs.) after the fact
constexpr uint8_t Terrain1 = 100;
constexpr uint8_t Terrain2 = 101;

constexpr uint8_t TerrainInlet1 = 110;

constexpr uint8_t TerrainCurve1 = 121;
constexpr uint8_t TerrainCurve2 = 120;
} // namespace layer

namespace hotkey
{
// Character Controls
constexpr auto Forward = nc::input::KeyCode::W;
constexpr auto Back = nc::input::KeyCode::S;
constexpr auto Left = nc::input::KeyCode::A;
constexpr auto Right = nc::input::KeyCode::D;
constexpr auto Lunge = nc::input::KeyCode::Space;
constexpr auto Spray = nc::input::KeyCode::LeftShift;

// General Controls
constexpr auto ToggleMenu = nc::input::KeyCode::Escape;

// Debug Controls
constexpr auto ToggleDebugCamera = nc::input::KeyCode::F5;
constexpr auto SaveFoliageScene = nc::input::KeyCode::F9;
constexpr auto SkipToSpreadEvent = nc::input::KeyCode::F12;
} // namespace hotkey

namespace map
{
constexpr auto Extent = 300.0f;
constexpr auto HalfExtent = Extent * 0.5f;
const auto Radius = std::sqrt(2.0f * (HalfExtent * HalfExtent));
} // namespace map

namespace music
{
constexpr auto IntroThemeIndex = 0u;
constexpr auto BlightClearedIndex = 1u;
constexpr auto LoseIndex = 2u;
constexpr auto EndingIndex = 3u;
} // namespace music

// Always set main camera to this tag
namespace tag
{
// IMPORTANT! Do not change tag values, else serialized work will be garbage!
const auto MainCamera = std::string{"Camera"};
const auto VehicleFront = std::string{"VehicleFront"};
const auto VehicleCar = std::string{"BoxCar"};
const auto VehicleAudio = std::string{"VehicleAudio"};
const auto AmbienceSfx = std::string{"AmbienceSfx"};
const auto Music = std::string{"Music"};
const auto HealthyTree = std::string{"HealthyTree"};
const auto InfectedTree = std::string{"InfectedTree"};
const auto Spreader = std::string{"Spreader"};
const auto Purifier = std::string{"Purifier"};
const auto Ground = std::string{"Ground"};
const auto Terrain = std::string{"Terrain"};
const auto QuestTrigger = std::string{"QuestTrigger"};
const auto Dave = std::string{"Dave"};
const auto Sasquatch = std::string{"Sasquatch"};
const auto Camp = std::string{"Camp"};
const auto Elder = std::string{"Elder"};
const auto Putter = std::string{"Putter"};
const auto Sasquatch1 = std::string{"Sasquatch1"};
const auto Sasquatch2 = std::string{"Sasquatch2"};
const auto Sasquatch3 = std::string{"Sasquatch3"};
const auto Firepit = std::string{"Firepit"};
const auto Fire = std::string{"Fire"};

// Camera focus points
const auto IntroFocusPoint = std::string{"IntroFocusPoint"};
const auto DaveEncounterFocusPoint = std::string{"DaveEncounterFocusPoint"};
const auto CampEncounterFocusPoint = std::string{"CampEncounterFocusPoint"};
const auto ElderEncounterFocusPoint = std::string{"ElderEncounterFocusPoint"};

const auto DaveEndingPosition = std::string{"DaveEndingPosition"};
const auto EndingFocusPoint = std::string{"EndingFocusPoint"};

const auto Light = std::string{"Light"};
} // namespace tag

void LoadFragment(std::string_view path, nc::Registry* registry, nc::ModuleProvider modules);

inline auto IsTerrain(nc::Entity entity) -> bool
{
    return entity.Layer() >= 100 && entity.Layer() <= 150;
}

template<class T>
auto GetComponentByEntityTag(nc::ecs::Ecs world, const std::string& tag) -> T&
{
    auto entity = world.GetEntityByTag(tag);
    return world.Get<T>(entity);
}

template<class T>
auto GetComponentByEntityTag(nc::Registry* registry, const std::string& tag) -> T&
{
    return GetComponentByEntityTag<T>(registry->GetEcs(), tag);
}
} // namespace game
