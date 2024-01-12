#pragma once

#include "ncengine/NcEngine.h"
#include "ncengine/asset/NcAsset.h"
#include "ncengine/asset/Assets.h"
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
    static constexpr uint8_t Character = 1;
    static constexpr uint8_t Healthy = 2;
    static constexpr uint8_t Sickly = 3;

    // Entity layer internally ignored by engine serialization
    // probably change engine-side
    static constexpr uint8_t DoNotSerialize = UINT8_MAX;
};



// Always set main camera to this tag
const auto MainCameraTag = std::string{"Camera"};
const auto CharacterTag = std::string{"Character"};


void LoadFragment(std::string_view path, nc::Registry* registry, nc::ModuleProvider modules);
} // namespace game
