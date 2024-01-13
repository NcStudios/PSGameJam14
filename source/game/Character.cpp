#include "Character.h"
#include "Core.h"

namespace
{
using namespace nc;

class FollowCamera : public graphics::Camera
{
    public:
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

            // make config..
            constexpr auto followDistance = 10.0f;
            constexpr auto followHeight = 6.0f; // Adjust the height offset as needed

            const auto offset = [followDistance, targetRotation]()
            {
                auto v = Vector3::Back() * followDistance;
                auto v_v = DirectX::XMLoadVector3(&v);
                auto r_v = DirectX::XMLoadQuaternion(&targetRotation);
                v_v = DirectX::XMVector3Rotate(v_v, r_v);
                DirectX::XMStoreVector3(&v, v_v);
                return v;
            }();

            const auto desiredPos = targetPos + offset + Vector3::Up() * followHeight;
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
    static constexpr auto moveForce = 0.7f;
    static constexpr auto turnForce = 0.6f;
    static constexpr auto jumpForce = 4.0f;
    auto* body = registry->Get<physics::PhysicsBody>(self);
    auto* transform = registry->Get<Transform>(self);

    if(!body || !transform)
        return;

    if(KeyHeld(input::KeyCode::W))
        body->ApplyImpulse(transform->Forward() * moveForce);

    if(KeyHeld(input::KeyCode::S))
        body->ApplyImpulse(-transform->Forward() * moveForce);

    if(KeyHeld(input::KeyCode::A))
        body->ApplyImpulse(-transform->Right() * moveForce);

    if(KeyHeld(input::KeyCode::D))
        body->ApplyImpulse(transform->Right() * moveForce);

    if(KeyHeld(input::KeyCode::Space))
        body->ApplyImpulse(Vector3::Up() * jumpForce);

    if(KeyHeld(input::KeyCode::Q))
        body->ApplyTorqueImpulse(-transform->Up() * turnForce);

    if(KeyHeld(input::KeyCode::E))
        body->ApplyTorqueImpulse(transform->Up() * turnForce);
}

auto CreateVehicleNode(nc::ecs::Ecs world,
                       const nc::Vector3& position,
                       const nc::Vector3& scale,
                       const std::string& tag,
                       const std::string& mesh,
                       float mass) -> nc::Entity
{
    const auto node = world.Emplace<nc::Entity>(nc::EntityInfo
    {
        .position = position,
        .scale = scale,
        .tag = tag,
        .layer = game::Layer::Character,
        .flags = nc::Entity::Flags::NoSerialize
    });

    world.Emplace<nc::graphics::MeshRenderer>(node, mesh); // prob add material too
    world.Emplace<nc::physics::Collider>(node, nc::physics::BoxProperties{});
    world.Emplace<nc::physics::PhysicsBody>(node, nc::physics::PhysicsProperties{.mass = mass});
    return node;
}

auto CreateVehicle(const nc::Vector3& position, nc::ecs::Ecs world, nc::ModuleProvider modules) -> nc::Entity
{
    const auto head = CreateVehicleNode(world, position, nc::Vector3::One(), game::CharacterTag, "DefaultCube.nca", 5.0f);
    const auto second = CreateVehicleNode(world, position - nc::Vector3::Front() * 0.9f, nc::Vector3::Splat(0.8f), "BoxCar", "DefaultCube.nca", 3.0f);
    const auto third = CreateVehicleNode(world, position - nc::Vector3::Front() * 1.6f, nc::Vector3::Splat(0.6f), "BoxCar", "DefaultCube.nca", 1.0f);
    const auto fourth = CreateVehicleNode(world, position - nc::Vector3::Front() * 2.1f, nc::Vector3::Splat(0.4f), "BoxCar", "DefaultCube.nca", 0.2f);

    constexpr auto bias = 0.2f;
    constexpr auto softness = 0.1f;
    auto physicsModule = modules.Get<nc::physics::NcPhysics>();
    physicsModule->AddJoint(head, second, Vector3{0.0f, 0.0f, -0.6f}, Vector3{0.0f, 0.0f, 0.5f}, bias, softness);
    physicsModule->AddJoint(second, third, Vector3{0.0f, 0.0f, -0.5f}, Vector3{0.0f, 0.0f, 0.4f}, bias, softness);
    physicsModule->AddJoint(third, fourth, Vector3{0.0f, 0.0f, -0.4f}, Vector3{0.0f, 0.0f, 0.3f}, bias, softness);

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
