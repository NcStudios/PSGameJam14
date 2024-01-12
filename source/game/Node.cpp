#include "Node.h"

namespace game
{
auto CreateHealthyNode(nc::ecs::Ecs world) -> nc::Entity
{
    (void)world;
    return nc::Entity{};
}

auto CreateSicklyNode(nc::ecs::Ecs world) -> nc::Entity
{
    (void)world;
    return nc::Entity{};
}

void UpdateNode(nc::Entity target, nc::ecs::Ecs world)
{
    (void)target;
    (void)world;
}
} // namespace game
