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
#include "ncengine/module/ModuleProvider.h"
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/ConcaveCollider.h"
#include "ncengine/physics/NcPhysics.h"
#include "ncengine/physics/PhysicsBody.h"
#include "ncengine/scene/Scene.h"

#include <string_view>

namespace game
{
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
constexpr uint8_t Floor = 7;
constexpr uint8_t BoxCar = 8;
} // namespace layer

namespace hotkey
{
// Character Controls
constexpr auto Forward = nc::input::KeyCode::W;
constexpr auto Back = nc::input::KeyCode::S;
constexpr auto Left = nc::input::KeyCode::A;
constexpr auto Right = nc::input::KeyCode::D;
constexpr auto Jump = nc::input::KeyCode::Space;
constexpr auto Purify = nc::input::KeyCode::LeftShift;

// General Controls
constexpr auto ToggleMenu = nc::input::KeyCode::Escape;

// Debug Controls
constexpr auto ToggleDebugCamera = nc::input::KeyCode::F5;
} // namespace hotkey

namespace map
{
constexpr auto Extent = 300.0f;
constexpr auto HalfExtent = Extent * 0.5f;
const auto Radius = std::sqrt(2.0f * (HalfExtent * HalfExtent));
} // namespace map

// Always set main camera to this tag
namespace tag
{
// IMPORTANT! Do not change tag values, else serialized work will be garbage!
const auto MainCamera = std::string{"Camera"};
const auto VehicleFront = std::string{"VehicleFront"};
const auto VehicleCar = std::string{"BoxCar"};
const auto VehicleAudio = std::string{"VehicleAudio"};
const auto HealthyTree = std::string{"HealthyTree"};
const auto InfectedTree = std::string{"InfectedTree"};
const auto Spreader = std::string{"Spreader"};
const auto Purifier = std::string{"Purifier"};
const auto TreeSystem = std::string{"TreeSystem"};
} // namespace tag

void LoadFragment(std::string_view path, nc::Registry* registry, nc::ModuleProvider modules);

template<class T>
auto GetComponentByEntityTag(nc::ecs::Ecs world, const std::string& tag) -> T*
{
    auto entity = world.GetEntityByTag(tag);
    NC_ASSERT(entity.Valid(), fmt::format("Entity with tag '{}' not found", tag));
    auto component = world.Get<T>(entity);
    NC_ASSERT(component, fmt::format("No component found for Entity with tag '{}'", tag));
    return component;
}

template<class T>
auto GetComponentByEntityTag(nc::Registry* registry, const std::string& tag) -> T*
{
    return GetComponentByEntityTag<T>(registry->GetEcs(), tag);
}
} // namespace game
