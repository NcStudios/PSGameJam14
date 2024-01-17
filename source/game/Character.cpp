#include "Character.h"
#include "Assets.h"
#include "Core.h"

namespace
{
auto CreateVehicleNode(nc::ecs::Ecs world,
                       const nc::Vector3& position,
                       const nc::Vector3& scale,
                       const std::string& tag,
                       uint8_t layer,
                       const std::string& mesh,
                       const nc::graphics::ToonMaterial& material,
                       float mass,
                       float friction = 0.5f) -> nc::Entity
{
    const auto node = world.Emplace<nc::Entity>(nc::EntityInfo
    {
        .position = position,
        .scale = scale * 0.5f,
        .tag = tag,
        .layer = layer,
        .flags = nc::Entity::Flags::NoSerialize
    });

    world.Emplace<nc::graphics::ToonRenderer>(node, mesh, material);
    world.Emplace<nc::physics::Collider>(node, nc::physics::BoxProperties{.center = nc::Vector3{}, .extents = nc::Vector3{2.0f, 2.0f, 4.0f}});
    world.Emplace<nc::physics::PhysicsBody>(
        node,
        nc::physics::PhysicsProperties{
            .mass = mass,
            .friction = friction
        },
        nc::Vector3::One(),
        nc::Vector3{0.5f, 1.0f, 0.5f} // TODO: play with these values
    );

    return node;
}

auto CreateVehicle(nc::ecs::Ecs world, nc::physics::NcPhysics* phys, const nc::Vector3& position) -> nc::Entity
{
    // TODO: play with mass/friction/restitution values
    //       also consider tweaking PhysicsConstants
    const auto head = CreateVehicleNode(world, position, nc::Vector3::One(), game::tag::VehicleFront, game::layer::Character, game::BusFrontMesh, game::BusFrontMaterial, 15.0f, 0.8f);
    const auto second = CreateVehicleNode(world, position - nc::Vector3{0.0f, -0.1f, 1.62f}, nc::Vector3::Splat(0.8f), game::tag::VehicleCar, game::layer::BoxCar, game::BusCarMesh, game::BusCarMaterial, 3.0f, 0.5f);
    const auto third = CreateVehicleNode(world, position - nc::Vector3{0.0f, -0.1f, 2.88f}, nc::Vector3::Splat(0.6f), game::tag::VehicleCar, game::layer::BoxCar, game::BusCarMesh, game::BusCarMaterial, 1.0f, 0.5f);
    const auto fourth = CreateVehicleNode(world, position - nc::Vector3{0.0f, -0.1f, 3.78f}, nc::Vector3::Splat(0.4f), game::tag::VehicleCar, game::layer::BoxCar, game::BusCarMesh, game::BusCarMaterial, 0.2f, 0.5f);

    // TODO: play with these values
    constexpr auto bias = 0.3f; // lower has more 'spring', too high propagates too much force to front car
    constexpr auto softness = 0.4f; // maybe lower?
    phys->AddJoint(head, second, nc::Vector3{0.0f, -0.1f, -1.08f}, nc::Vector3{0.0f, 0.0f, 0.9f}, bias, softness);
    phys->AddJoint(second, third, nc::Vector3{0.0f, -0.1f, -0.9f}, nc::Vector3{0.0f, 0.0f, 0.72f}, bias, softness);
    phys->AddJoint(third, fourth, nc::Vector3{0.0f, -0.1f, -0.72f}, nc::Vector3{0.0f, 0.0f, 0.54f}, bias, softness);

    return head;
}
} // namespace

namespace game
{
auto CreateCharacter(nc::ecs::Ecs world, nc::physics::NcPhysics* phys, const nc::Vector3& position) -> nc::Entity
{
    const auto character = ::CreateVehicle(world, phys, position);
    world.Emplace<CharacterController>(character);
    world.Emplace<nc::FixedLogic>(character, nc::InvokeFreeComponent<CharacterController>{});

    const auto characterAudio = world.Emplace<nc::Entity>({.parent = character, .tag = tag::VehicleAudio});
    auto p = world.Emplace<CharacterAudio>(characterAudio, character);
    p->Init(world);
    world.Emplace<nc::FrameLogic>(characterAudio, nc::InvokeFreeComponent<CharacterAudio>{});

    const auto transform = world.Get<nc::Transform>(character);
    const auto forward = transform->Forward();
    const auto forwardMin = transform->ToLocalSpace(nc::Vector3{-1.0f, -1.0f, 5.0f}) * 5.0f;
    const auto forwardMax = transform->ToLocalSpace(nc::Vector3{-1.0f, -1.0f, 10.0f}) * 10.0f;

    const auto purifyParticles = world.Emplace<nc::Entity>({.parent = character, .tag = "PurifyParticles"});
    world.Emplace<nc::graphics::ParticleEmitter>(purifyParticles, nc::graphics::ParticleInfo{
        .init = nc::graphics::ParticleInitInfo{
            .lifetime = 1.0f,
            .positionMin = nc::Vector3::Splat(-0.1f),
            .positionMax = nc::Vector3::Splat(0.1f),
            .rotationMin = -0.157f,
            .rotationMax = 0.157f,
            .scaleMin = 0.05f,
            .scaleMax = 0.5f
        },
        .kinematic = nc::graphics::ParticleKinematicInfo{
            .velocityMin = forwardMin,
            .velocityMax = forwardMax,
            .rotationMin = -1.0f,
            .rotationMax = 1.0f,
            .scaleOverTimeFactor = -30.0f
        }
    });

    return character;
}

void CharacterController::Run(nc::Entity self, nc::Registry* registry)
{
    auto* body = registry->Get<nc::physics::PhysicsBody>(self);
    auto* transform = registry->Get<nc::Transform>(self);

    if(!body || !transform)
        return;

    if (m_jumpOnCooldown)
    {
        m_jumpRemainingCooldownTime -= fixedDt;
        if (m_jumpRemainingCooldownTime <= 0.0f)
        {
            m_jumpRemainingCooldownTime = 0.0f;
            m_jumpOnCooldown = false;
        }
    }

    if (m_purifyOnCooldown)
    {
        m_purifyRemainingCooldownTime -= fixedDt;
        if (m_purifyRemainingCooldownTime <= 0.0f)
        {
            m_purifyRemainingCooldownTime = 0.0f;
            m_purifyOnCooldown = false;
            registry->Remove<nc::Entity>(m_purifier);
            m_purifier = nc::Entity::Null();
        }
    }

    if (KeyDown(game::hotkey::Forward))
    {
        auto characterAudio = GetComponentByEntityTag<CharacterAudio>(registry, tag::VehicleAudio);
        characterAudio->SetState(VehicleState::StartForward);
    }
    else if (KeyUp(game::hotkey::Forward))
    {
        auto characterAudio = GetComponentByEntityTag<CharacterAudio>(registry, tag::VehicleAudio);
        characterAudio->SetState(VehicleState::StopForward);
    }

    if (KeyHeld(game::hotkey::Forward))
    {
        if (m_currentMoveVelocity < maxMoveVelocity)
        {
            m_currentMoveVelocity += moveAcceleration;
        }
        transform->Translate(transform->Forward() * m_currentMoveVelocity * fixedDt);
    }
    else if (!nc::FloatEqual(m_currentMoveVelocity, 0.0f))
    {
        m_currentMoveVelocity -= moveAcceleration * 1.5f;
        if (m_currentMoveVelocity < 0.0f)
        {
            m_currentMoveVelocity = 0.0f;
        }
        else
        {
            transform->Translate(transform->Forward() * m_currentMoveVelocity * fixedDt);
        }
    }

    if (KeyHeld(game::hotkey::Back))
    {
        transform->Translate(-transform->Forward() * maxMoveVelocity * 0.5f * fixedDt);
    }

    auto turning = false;
    if (KeyHeld(game::hotkey::Left))
    {
        turning = true;
        if (m_currentTurnVelocity > -maxTurnVelocity)
            m_currentTurnVelocity -= turnAcceleration;
    }

    if (KeyHeld(game::hotkey::Right))
    {
        turning = true;
        if (m_currentTurnVelocity < maxTurnVelocity)
            m_currentTurnVelocity += turnAcceleration;
    }

    if (turning)
    {
        // TODO: Torque may be ok/better for rotation - requires tinkering, but is probably a 'nice to have'
        transform->Rotate(transform->Up(), m_currentTurnVelocity * fixedDt);
    }
    else if (!nc::FloatEqual(m_currentTurnVelocity, 0.0f))
    {
        m_currentTurnVelocity = 0.0f;
    }

    if (!m_jumpOnCooldown && KeyDown(game::hotkey::Jump))
    {
        m_jumpOnCooldown = true;
        m_jumpRemainingCooldownTime = jumpCooldown;
        body->ApplyImpulse(nc::Vector3::Up() * jumpForce);
    }

    if (!m_purifyOnCooldown && KeyDown(game::hotkey::Purify))
    {
        auto characterAudio = GetComponentByEntityTag<CharacterAudio>(registry, tag::VehicleAudio);
        characterAudio->PlayPurifySfx(registry->GetEcs());

        auto purifyParticles = GetComponentByEntityTag<nc::graphics::ParticleEmitter>(registry, "PurifyParticles");
        auto props = purifyParticles->GetInfo();
        const auto moveVel = transform->ToLocalSpace(nc::Vector3::Front()) * m_currentMoveVelocity;
        const auto baseVelMin = transform->ToLocalSpace(nc::Vector3{-1.0f, -1.0f, 5.0f}) * 5.0f;
        const auto baseVelMax = transform->ToLocalSpace(nc::Vector3{1.0f, 1.0f, 10.0f}) * 10.0f;

        props.kinematic.velocityMin = moveVel + baseVelMin;
        props.kinematic.velocityMax = moveVel + baseVelMax;
        purifyParticles->SetInfo(props);
        purifyParticles->Emit(30);
        CreatePurifier(registry);
    }
}

void CharacterController::CreatePurifier(nc::Registry* registry)
{
    m_purifyOnCooldown = true;
    m_purifyRemainingCooldownTime = purifyCooldown;

    m_purifier = registry->Add<nc::Entity>(
    {
        .position = nc::Vector3::Front() * 3.0f,
        .scale = nc::Vector3::One() * 5.0f,
        .parent = ParentEntity(),
        .tag = tag::Purifier,
        .layer = layer::Purifier
    });

    const auto transform = registry->Get<nc::Transform>(ParentEntity());
    const auto forward = transform->Forward();

    registry->Add<nc::physics::Collider>(m_purifier, nc::physics::SphereProperties{}, true);
    registry->Add<nc::physics::PhysicsBody>(m_purifier, nc::physics::PhysicsProperties{.isKinematic = true});
}

CharacterAudio::CharacterAudio(nc::Entity self, nc::Entity)
    : nc::FreeComponent(self)
{
}

void CharacterAudio::Init(nc::ecs::Ecs world)
{
    constexpr auto innerRadius = 1.0f;
    constexpr auto outerRadius = 20.0f;
    const auto self = ParentEntity();

    m_engineStartPlayer = world.Emplace<nc::Entity>({.parent = self});
    world.Emplace<nc::audio::AudioSource>(m_engineStartPlayer, EngineStartSfx, nc::audio::AudioSourceProperties{
        .gain = 0.8f,
        .innerRadius = innerRadius,
        .outerRadius = outerRadius,
        .spatialize = true
    });

    m_engineRunningPlayer = world.Emplace<nc::Entity>({.parent = self});
    world.Emplace<nc::audio::AudioSource>(m_engineRunningPlayer, EngineRunningSfx, nc::audio::AudioSourceProperties{
        .gain = 0.5f,
        .innerRadius = innerRadius,
        .outerRadius = outerRadius,
        .spatialize = true
    });

    m_engineStopPlayer = world.Emplace<nc::Entity>({.parent = self});
    world.Emplace<nc::audio::AudioSource>(m_engineStopPlayer, EngineStopSfx, nc::audio::AudioSourceProperties{
        .gain = 0.6f,
        .innerRadius = innerRadius,
        .outerRadius = outerRadius,
        .spatialize = true
    });

    m_purifyPlayer = world.Emplace<nc::Entity>({.parent = self});
    world.Emplace<nc::audio::AudioSource>(m_purifyPlayer, PurifySfx, nc::audio::AudioSourceProperties{
        .gain = 1.0f,
        .innerRadius = innerRadius,
        .outerRadius = outerRadius,
        .spatialize = true
    });
}

void CharacterAudio::PlayPurifySfx(nc::ecs::Ecs world)
{
    auto player = world.Get<nc::audio::AudioSource>(m_purifyPlayer);
    NC_ASSERT(player, "No AudioSource");
    player->Play();
}

void CharacterAudio::Run(nc::Entity, nc::Registry* registry, float)
{
    if (m_currentState != m_nextState)
    {
        if (m_currentEnginePlayer.Valid())
        {
            auto player = registry->Get<nc::audio::AudioSource>(m_currentEnginePlayer);
            if (player->IsPlaying()) player->Stop();
        }

        switch (m_nextState)
        {
            case VehicleState::StartForward:
            {
                m_currentEnginePlayer = m_engineStartPlayer;
                auto player = registry->Get<nc::audio::AudioSource>(m_currentEnginePlayer);
                player->Play();
                break;
            }
            case VehicleState::Forward:
            {
                m_currentEnginePlayer = m_engineRunningPlayer;
                auto player = registry->Get<nc::audio::AudioSource>(m_currentEnginePlayer);
                player->Play();
                break;
            }
            case VehicleState::StopForward:
            {
                m_currentEnginePlayer = m_engineStopPlayer;
                auto player = registry->Get<nc::audio::AudioSource>(m_currentEnginePlayer);
                player->Play();
                break;
            }
            default: break;
        }

        m_currentState = m_nextState;
        return;
    }

    switch (m_currentState)
    {
        case VehicleState::StartForward:
        {
            auto player = registry->Get<nc::audio::AudioSource>(m_engineStartPlayer);
            if (!player->IsPlaying()) SetState(VehicleState::Forward);
            break;
        }
        case VehicleState::StopForward:
        {
            auto player = registry->Get<nc::audio::AudioSource>(m_engineStopPlayer);
            if (!player->IsPlaying()) SetState(VehicleState::Idle);
            break;
        }
        default: break;
    }
}

void CharacterAudio::SetState(VehicleState state)
{
    m_nextState = state;
}
} // namespace game
