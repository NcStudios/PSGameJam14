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
struct Layer
{
    static constexpr uint8_t None = 0;
    static constexpr uint8_t Default = 1;
    static constexpr uint8_t Character = 2;
    static constexpr uint8_t HealthyTree = 3;
    static constexpr uint8_t InfectedTree = 4;
    static constexpr uint8_t Spreader = 5;
};

constexpr auto MapExtent = 300.0f;
constexpr auto MapHalfExtent = MapExtent * 0.5f;
const auto MapRadius = std::sqrt(2.0f * (MapHalfExtent * MapHalfExtent));

// Always set main camera to this tag
const auto MainCameraTag = std::string{"Camera"};
const auto VehicleFrontTag = std::string{"VehicleFront"};
const auto VehicleCarTag = std::string{"BoxCar"};
const auto HealthyTreeTag = std::string{"HealthyTree"};
const auto InfectedTreeTag = std::string{"InfectedTree"};

void LoadFragment(std::string_view path, nc::Registry* registry, nc::ModuleProvider modules);
} // namespace game
