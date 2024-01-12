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
    static constexpr float force = 0.7f;
    auto* body = registry->Get<physics::PhysicsBody>(self);
    auto* transform = registry->Get<Transform>(self);

    if(!body || !transform)
        return;

    if(KeyHeld(input::KeyCode::W))
        body->ApplyImpulse(transform->Forward() * force);

    if(KeyHeld(input::KeyCode::S))
        body->ApplyImpulse(-transform->Forward() * force);

    if(KeyHeld(input::KeyCode::A))
        body->ApplyImpulse(-transform->Right() * force);
    
    if(KeyHeld(input::KeyCode::D))
        body->ApplyImpulse(transform->Right() * force);

    if(KeyHeld(input::KeyCode::Space))
        body->ApplyImpulse(Vector3::Up() * 4.0f);

    if(KeyHeld(input::KeyCode::Q))
        body->ApplyTorqueImpulse(-transform->Up() * 0.6f);

    if(KeyHeld(input::KeyCode::E))
        body->ApplyTorqueImpulse(transform->Up() * 0.6f);
}

auto CreateCharacter(const nc::Vector3& position, nc::ecs::Ecs world, nc::ModuleProvider) -> nc::Entity
{
    const auto character = world.Emplace<nc::Entity>(nc::EntityInfo
    {
        .position = position,
        .tag = game::CharacterTag,
        .layer = game::Layer::DoNotSerialize
    });

    world.Emplace<nc::graphics::MeshRenderer>(character);
    world.Emplace<nc::physics::Collider>(character, nc::physics::BoxProperties{});
    world.Emplace<nc::physics::PhysicsBody>(character, nc::physics::PhysicsProperties{});
    world.Emplace<nc::FixedLogic>(character, &DirectionalForceBasedMovement);
    return character;
}
} // namespace

namespace game
{
auto CreateCharacter(const nc::Vector3& position, nc::Registry* registry, nc::ModuleProvider modules) -> nc::Entity
{
    auto world = registry->GetEcs();
    const auto character = ::CreateCharacter(position, world, modules);

    ///
    const auto cameraHandle = world.Emplace<nc::Entity>(nc::EntityInfo
    {
        .position = nc::Vector3{0.0f, 5.0f, -11.0f},
        .rotation = nc::Quaternion::FromEulerAngles(0.35f, 0.0f, 0.0f),
        .tag = MainCameraTag,
        .layer = Layer::DoNotSerialize
    });
    const auto camera = world.Emplace<FollowCamera>(cameraHandle, character);
    world.Emplace<nc::FrameLogic>(cameraHandle, nc::InvokeFreeComponent<FollowCamera>());
    modules.Get<nc::graphics::NcGraphics>()->SetCamera(camera);

    return character;
}
} // namespace game
