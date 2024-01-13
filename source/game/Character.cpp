#include "Character.h"
#include "Assets.h"
#include "Core.h"

namespace
{
using namespace nc;

class FollowCamera : public graphics::Camera
{
    public:
        static constexpr auto FollowDistance = 10.0f;
        static constexpr auto FollowHeight = 4.0f;

        FollowCamera(Entity self, Entity target)
            : graphics::Camera{self}, m_target{target}
        {
        }

        void Run(Entity entity, Registry* registry, float dt)
        {
            auto self = registry->Get<Transform>(entity);
            auto target = registry->Get<Transform>(m_target);
            if (!self || !target)
            {
                return;
            }

            auto selfPos = self->Position();
            const auto targetPos = target->Position();
            const auto targetRotation = target->Rotation();
            const auto offset = [followDistance = FollowDistance, targetRotation]()
            {
                auto v = Vector3::Back() * FollowCamera::FollowDistance;
                auto v_v = DirectX::XMLoadVector3(&v);
                auto r_v = DirectX::XMLoadQuaternion(&targetRotation);
                v_v = DirectX::XMVector3Rotate(v_v, r_v);
                DirectX::XMStoreVector3(&v, v_v);
                return v;
            }();

            const auto desiredPos = targetPos + offset + Vector3::Up() * FollowHeight;
            self->Translate((desiredPos - selfPos) * dt);

            const auto camToTarget = targetPos - selfPos;
            const auto forward = Normalize(camToTarget);
            const auto cosTheta = Dot(Vector3::Front(), forward);
            const auto angle = std::acos(cosTheta);
            const auto axis = Normalize(CrossProduct(Vector3::Front(), forward));
            self->SetRotation(Quaternion::FromAxisAngle(axis, angle));
        }

    private:
        Entity m_target;
};

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

auto CreateVehicle(const nc::Vector3& position, nc::ecs::Ecs world, nc::ModuleProvider modules) -> nc::Entity
{
    const auto head = CreateVehicleNode(world, position, nc::Vector3::One(), game::CharacterTag, game::BusFrontMesh, game::BusFrontMaterial, 5.0f);
    const auto second = CreateVehicleNode(world, position - nc::Vector3::Front() * 1.62f, nc::Vector3::Splat(0.8f), "BoxCar", game::BusCarMesh, game::BusCarMaterial, 3.0f);
    const auto third = CreateVehicleNode(world, position - nc::Vector3::Front() * 2.88f, nc::Vector3::Splat(0.6f), "BoxCar", game::BusCarMesh, game::BusCarMaterial, 1.0f);
    const auto fourth = CreateVehicleNode(world, position - nc::Vector3::Front() * 3.78f, nc::Vector3::Splat(0.4f), "BoxCar", game::BusCarMesh, game::BusCarMaterial, 0.2f);

    constexpr auto bias = 0.2f;
    constexpr auto softness = 0.1f;
    auto physicsModule = modules.Get<nc::physics::NcPhysics>();
    physicsModule->AddJoint(head, second, Vector3{0.0f, 0.0f, -1.08f}, Vector3{0.0f, 0.0f, 0.9f}, bias, softness);
    physicsModule->AddJoint(second, third, Vector3{0.0f, 0.0f, -0.9f}, Vector3{0.0f, 0.0f, 0.72f}, bias, softness);
    physicsModule->AddJoint(third, fourth, Vector3{0.0f, 0.0f, -0.72f}, Vector3{0.0f, 0.0f, 0.54f}, bias, softness);

    return head;
}
} // namespace

namespace game
{
auto CreateCharacter(const nc::Vector3& position, nc::Registry* registry, nc::ModuleProvider modules) -> nc::Entity
{
    auto world = registry->GetEcs();
    const auto character = ::CreateVehicle(position, world, modules);
    world.Emplace<nc::FixedLogic>(character, &DirectionalForceBasedMovement);

    ///
    const auto cameraHandle = world.Emplace<nc::Entity>(nc::EntityInfo
    {
        .position = nc::Vector3{0.0f, 5.0f, -11.0f},
        .rotation = nc::Quaternion::FromEulerAngles(0.35f, 0.0f, 0.0f),
        .tag = MainCameraTag,
        .flags = nc::Entity::Flags::NoSerialize
    });
    const auto camera = world.Emplace<FollowCamera>(cameraHandle, character);
    world.Emplace<nc::FrameLogic>(cameraHandle, nc::InvokeFreeComponent<FollowCamera>());
    modules.Get<nc::graphics::NcGraphics>()->SetCamera(camera);

    return character;
}
} // namespace game
