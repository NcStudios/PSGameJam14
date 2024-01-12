#pragma once

#include "ncengine/NcEngine.h"
#include "ncengine/asset/NcAsset.h"
#include "ncengine/asset/Assets.h"
#include "ncengine/audio/AudioSource.h"
#include "ncengine/audio/NcAudio.h"
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
// Entity layer internally ignored by engine serialization
constexpr auto DoNotSerialize = UINT8_MAX;

void LoadFragment(std::string_view path, nc::Registry* registry, nc::ModuleProvider modules);
} // namespace game
