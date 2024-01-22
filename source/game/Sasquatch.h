#pragma once

#include "Core.h"

namespace game
{
void AttachSasquatchAnimators(nc::ecs::Ecs world);

void SetAnimatorState(nc::ecs::Ecs world, std::string_view animation, std::string_view tag);
void SetPlayOnceAnimation(nc::ecs::Ecs world, std::string_view animation, std::string_view tag);
void ReturnAnimatorToRootState(nc::ecs::Ecs world, std::string_view tag);

void MoveSasquatchToCamp(nc::ecs::Ecs world);

void AttachDaveQuestTrigger(nc::ecs::Ecs world);
void AttachCampQuestTrigger(nc::ecs::Ecs world);
void AttachElderQuestTrigger(nc::ecs::Ecs world);
void AttachPutterQuestTrigger(nc::ecs::Ecs world);
void AttachFinalQuestTrigger(nc::ecs::Ecs world);
} // namespace game
