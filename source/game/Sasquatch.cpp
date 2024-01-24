#include "Sasquatch.h"
#include "Assets.h"
#include "QuestTrigger.h"

namespace game
{
void AttachSasquatchAnimators(nc::ecs::Ecs world)
{
    {
        const auto dave = world.GetEntityByTag(tag::Dave);
        world.Emplace<nc::graphics::SkeletalAnimator>(dave, DaveMesh, DaveIdle);
    }

    {
        const auto camp = world.GetEntityByTag(tag::Camp);
        world.Emplace<nc::graphics::SkeletalAnimator>(camp, DaveMesh, DaveIdle);
    }

    {
        const auto elder = world.GetEntityByTag(tag::Elder);
        auto anim = world.Emplace<nc::graphics::SkeletalAnimator>(elder, DaveMesh, DaveIdle);
        anim->LoopImmediate(DaveSittingStump, nc::graphics::anim::Never);
    }

    {
        const auto putter = world.GetEntityByTag(tag::Putter);
        world.Emplace<nc::graphics::SkeletalAnimator>(putter, DaveMesh, DaveIdle);
    }

    {
        const auto s1 = world.GetEntityByTag(tag::Sasquatch1);
        world.Emplace<nc::graphics::SkeletalAnimator>(s1, DaveMesh, DaveSittingGround);

        const auto s2 = world.GetEntityByTag(tag::Sasquatch2);
        world.Emplace<nc::graphics::SkeletalAnimator>(s2, DaveMesh, DaveSittingStump);

        const auto s3 = world.GetEntityByTag(tag::Sasquatch3);
        world.Emplace<nc::graphics::SkeletalAnimator>(s3, DaveMesh, DaveCircleWalk);
    }
}

void SetAnimatorState(nc::ecs::Ecs world, std::string_view animation, std::string_view tag)
{
    const auto entity = world.GetEntityByTag(tag);
    auto animator = world.Get<nc::graphics::SkeletalAnimator>(entity);
    NC_ASSERT(animator, "expected entity to have an animator");
    animator->LoopImmediate(std::string{animation}, [](){ return false; });
}

void SetPlayOnceAnimation(nc::ecs::Ecs world, std::string_view animation, std::string_view tag)
{
    const auto entity = world.GetEntityByTag(tag);
    auto animator = world.Get<nc::graphics::SkeletalAnimator>(entity);
    NC_ASSERT(animator, "expected entity to have an animator");
    animator->PlayOnceImmediate(std::string{animation});
}

void ReturnAnimatorToRootState(nc::ecs::Ecs world, std::string_view tag)
{
    const auto entity = world.GetEntityByTag(tag);
    auto animator = world.Get<nc::graphics::SkeletalAnimator>(entity);
    NC_ASSERT(animator, "expected entity to have an animator");
    animator->StopImmediate([](){ return true; });
}

void MoveSasquatchToCamp(nc::ecs::Ecs world)
{
    // Remove any pending quests? only matters if dave is optional
    // move others?

    auto finalPos = GetComponentByEntityTag<nc::Transform>(world, tag::DaveEndingPosition);
    auto daveTransform = GetComponentByEntityTag<nc::Transform>(world, tag::Dave);
    daveTransform->SetPosition(finalPos->Position());
    daveTransform->SetRotation(finalPos->Rotation());
}

void AttachCampQuestTrigger(nc::ecs::Ecs world)
{
    const auto camp = world.GetEntityByTag(tag::Camp);
    AttachQuestTrigger(world, camp, Event::CampEncounter, nc::Vector3{}, nc::Vector3::Splat(5.0f));
}

void AttachDaveQuestTrigger(nc::ecs::Ecs world)
{
    const auto dave = world.GetEntityByTag(tag::Dave);
    AttachQuestTrigger(world, dave, Event::DaveEncounter, nc::Vector3{}, nc::Vector3::Splat(5.0f));
}

void AttachElderQuestTrigger(nc::ecs::Ecs world)
{
    const auto elder = world.GetEntityByTag(tag::Elder);
    AttachQuestTrigger(world, elder, Event::ElderEncounter, nc::Vector3{}, nc::Vector3::Splat(5.0f));
}

void AttachPutterQuestTrigger(nc::ecs::Ecs world)
{
    const auto putter = world.GetEntityByTag(tag::Putter);
    AttachQuestTrigger(world, putter, Event::PutterEncounter, nc::Vector3{}, nc::Vector3::Splat(5.0f));
}

void AttachFinalQuestTrigger(nc::ecs::Ecs world)
{
    const auto elder = world.GetEntityByTag(tag::Elder);
    AttachQuestTrigger(world, elder, Event::Win, nc::Vector3{}, nc::Vector3::Splat(5.0f));
}
} // namespace game
