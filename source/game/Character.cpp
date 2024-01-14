#include "Character.h"
#include "Assets.h"
#include "Core.h"

namespace
{
using namespace nc;

void DirectionalForceBasedMovement(Entity self, Registry* registry)
{
    static constexpr auto maxVelocitySqMagnitude = 50.0f;
    static constexpr auto moveForce = 1.0f;
    static constexpr auto turnForce = 0.6f;
    static constexpr auto jumpForce = 15.0f;
    static constexpr auto jumpResetTime = 0.35f;
    static auto jumping = false;
    static auto timeUntilJumpReset = 0.0f;
    auto* body = registry->Get<physics::PhysicsBody>(self);
    auto* transform = registry->Get<Transform>(self);

    if(!body || !transform)
        return;

    if (jumping)
    {
        timeUntilJumpReset -= 0.01667f;
        if (timeUntilJumpReset <= 0.0f)
        {
            timeUntilJumpReset = 0.0f;
            jumping = false;
        }
    }

    if(KeyHeld(input::KeyCode::W))
    {
        auto velocity = body->GetVelocity();
        velocity = DirectX::XMVector3LengthSq(velocity);
        const auto currentSqMag = DirectX::XMVectorGetX(velocity);
        if (currentSqMag < maxVelocitySqMagnitude)
            body->ApplyImpulse(transform->Forward() * moveForce);
    }

    if(KeyHeld(input::KeyCode::S))
        body->ApplyImpulse(-transform->Forward() * moveForce);

    if(KeyHeld(input::KeyCode::A))
        body->ApplyTorqueImpulse(-transform->Up() * turnForce);

    if(KeyHeld(input::KeyCode::D))
        body->ApplyTorqueImpulse(transform->Up() * turnForce);

    if(!jumping && KeyDown(input::KeyCode::Space))
    {
        jumping = true;
        timeUntilJumpReset = jumpResetTime;
        body->ApplyImpulse(Vector3::Up() * jumpForce);
    }
}

auto CreateVehicleNode(nc::ecs::Ecs world,
                       const nc::Vector3& position,
                       const nc::Vector3& scale,
                       const std::string& tag,
                       const std::string& mesh,
                       const nc::graphics::PbrMaterial& material, // We may want to keep this guy PBR rendered because toon shading doesn't play well with inorganic shapes
                       float mass) -> nc::Entity
{
    const auto node = world.Emplace<nc::Entity>(nc::EntityInfo
    {
        .position = position,
        .scale = scale * 0.5f,
        .tag = tag,
        .layer = game::Layer::Character,
        .flags = nc::Entity::Flags::NoSerialize
    });

    world.Emplace<nc::graphics::MeshRenderer>(node, mesh, material);
    world.Emplace<nc::physics::Collider>(node, nc::physics::BoxProperties{.center = nc::Vector3{}, .extents = nc::Vector3{2.0f, 2.0f, 4.0f}});
    world.Emplace<nc::physics::PhysicsBody>(node, nc::physics::PhysicsProperties{.mass = mass});
    return node;
}

auto CreateVehicle(nc::ecs::Ecs world, nc::physics::NcPhysics* phys, const nc::Vector3& position) -> nc::Entity
{
    const auto head = CreateVehicleNode(world, position, nc::Vector3::One(), game::VehicleFrontTag, game::BusFrontMesh, game::BusFrontMaterial, 5.0f);
    const auto second = CreateVehicleNode(world, position - nc::Vector3::Front() * 1.62f, nc::Vector3::Splat(0.8f), game::VehicleCarTag, game::BusCarMesh, game::BusCarMaterial, 3.0f);
    const auto third = CreateVehicleNode(world, position - nc::Vector3::Front() * 2.88f, nc::Vector3::Splat(0.6f), game::VehicleCarTag, game::BusCarMesh, game::BusCarMaterial, 1.0f);
    const auto fourth = CreateVehicleNode(world, position - nc::Vector3::Front() * 3.78f, nc::Vector3::Splat(0.4f), game::VehicleCarTag, game::BusCarMesh, game::BusCarMaterial, 0.2f);

    constexpr auto bias = 0.2f;
    constexpr auto softness = 0.1f;
    phys->AddJoint(head, second, Vector3{0.0f, 0.0f, -1.08f}, Vector3{0.0f, 0.0f, 0.9f}, bias, softness);
    phys->AddJoint(second, third, Vector3{0.0f, 0.0f, -0.9f}, Vector3{0.0f, 0.0f, 0.72f}, bias, softness);
    phys->AddJoint(third, fourth, Vector3{0.0f, 0.0f, -0.72f}, Vector3{0.0f, 0.0f, 0.54f}, bias, softness);

    return head;
}
} // namespace

namespace game
{
auto CreateCharacter(nc::ecs::Ecs world, nc::physics::NcPhysics* phys, const nc::Vector3& position) -> nc::Entity
{
    const auto character = ::CreateVehicle(world, phys, position);
    world.Emplace<nc::FixedLogic>(character, &DirectionalForceBasedMovement);
    return character;
}
} // namespace game
