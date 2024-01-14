#include "FollowCamera.h"

namespace game
{
void FollowCamera::Run(nc::Entity entity, nc::Registry* registry, float dt)
{
    auto self = registry->Get<nc::Transform>(entity);
    auto target = registry->Get<nc::Transform>(m_target);
    if (!self || !target)
    {
        return;
    }

    auto selfPos = self->Position();
    const auto targetPos = target->Position();
    const auto targetRotation = target->Rotation();
    const auto offset = [followDistance = FollowDistance, targetRotation]()
    {
        auto v = nc::Vector3::Back() * FollowCamera::FollowDistance;
        auto v_v = DirectX::XMLoadVector3(&v);
        auto r_v = DirectX::XMLoadQuaternion(&targetRotation);
        v_v = DirectX::XMVector3Rotate(v_v, r_v);
        DirectX::XMStoreVector3(&v, v_v);
        return v;
    }();

    const auto desiredPos = targetPos + offset + nc::Vector3::Up() * FollowHeight;
    self->Translate((desiredPos - selfPos) * dt * FollowSpeed);

    const auto camToTarget = targetPos - selfPos;
    const auto forward = nc::Normalize(camToTarget);
    const auto cosTheta = nc::Dot(nc::Vector3::Front(), forward);
    const auto angle = std::acos(cosTheta);
    const auto axis = nc::Normalize(nc::CrossProduct(nc::Vector3::Front(), forward));
    self->SetRotation(nc::Quaternion::FromAxisAngle(axis, angle));
}

auto CreateCamera(nc::ecs::Ecs world, nc::graphics::NcGraphics* gfx, nc::Entity initialTarget) -> nc::Entity
{
    const auto handle = world.Emplace<nc::Entity>(nc::EntityInfo
    {
        .position = nc::Vector3{0.0f, 5.0f, -11.0f},
        .rotation = nc::Quaternion::FromEulerAngles(0.35f, 0.0f, 0.0f),
        .tag = tag::MainCamera,
        .flags = nc::Entity::Flags::NoSerialize
    });

    const auto camera = world.Emplace<FollowCamera>(handle, initialTarget);
    world.Emplace<nc::FrameLogic>(handle, nc::InvokeFreeComponent<FollowCamera>());
    gfx->SetCamera(camera);
    return handle;
}
} // namespace game
