#include "Environment.h"
#include "Assets.h"

#include <ranges>

#include <iostream>

namespace
{
constexpr auto Terrain1SpawnExtent = nc::Vector3{12.0f, 1.0f, 12.0f};
constexpr auto Terrain2SpawnExtent = nc::Vector3{12.0f, 1.0f, 12.0f};
constexpr auto TerrainCurve1SpawnExtent = nc::Vector3{3.0f, 1.0f, 11.0f};
constexpr auto TerrainCurve2SpawnExtent = nc::Vector3{3.0f, 1.0f, 11.0f};
constexpr auto TerrainInlet1SpawnExtent = nc::Vector3{10.0f, 1.0f, 20.0f};

auto GetSpawnExtent(uint8_t layer) -> nc::Vector3
{
    switch (layer)
    {
        case game::layer::Terrain1: return Terrain1SpawnExtent;
        case game::layer::Terrain2: return Terrain2SpawnExtent;
        case game::layer::TerrainCurve1: return TerrainCurve1SpawnExtent;
        case game::layer::TerrainCurve2: return TerrainCurve2SpawnExtent;
        case game::layer::TerrainInlet1: return TerrainInlet1SpawnExtent;
        default: throw nc::NcError("unhandled spawn extent");
    }
}

auto GetLocalSpawnExtents(nc::ecs::Ecs world, nc::Entity entity)
{
    auto maxExtent = ::GetSpawnExtent(entity.Layer());
    auto minExtent = -maxExtent;
    const auto transform = world.Get<nc::Transform>(entity);
    const auto pos = transform->Position();
    const auto& m = transform->TransformationMatrix();

    auto v = DirectX::XMLoadVector3(&maxExtent);
    v = DirectX::XMVector3Transform(v, m);
    DirectX::XMStoreVector3(&maxExtent, v);

    v = DirectX::XMLoadVector3(&minExtent);
    v = DirectX::XMVector3Transform(v, m);
    DirectX::XMStoreVector3(&minExtent, v);

    return std::pair{minExtent, maxExtent};
}
} // anonymous namespace

namespace game
{
void SetupTerrain1(nc::ecs::Ecs world, nc::Entity entity)
{
    world.Emplace<nc::physics::ConcaveCollider>(entity, Terrain01Collider);
}

void SetupTerrain2(nc::ecs::Ecs world, nc::Entity entity)
{
    world.Emplace<nc::physics::ConcaveCollider>(entity, Terrain02Collider);
}

void SetupTerrainCurve1(nc::ecs::Ecs world, nc::Entity entity)
{
    world.Emplace<nc::physics::ConcaveCollider>(entity, TerrainCurve01Collider);
}

void SetupTerrainCurve2(nc::ecs::Ecs world, nc::Entity entity)
{
    world.Emplace<nc::physics::ConcaveCollider>(entity, TerrainCurve02Collider);
}

void SetupTerrainInlet(nc::ecs::Ecs world, nc::Entity entity)
{
    world.Emplace<nc::physics::ConcaveCollider>(entity, TerrainInletCollider);
}

void FinalizeTerrain(nc::ecs::Ecs world)
{
    for (auto entity : world.GetAll<nc::Entity>())
    {
        if (!IsTerrain(entity))
            continue;

        switch (entity.Layer())
        {
            case layer::Terrain1:      { SetupTerrain1(world, entity); break; }
            case layer::Terrain2:      { SetupTerrain2(world, entity); break; }
            case layer::TerrainCurve1: { SetupTerrainCurve1(world, entity); break; }
            case layer::TerrainCurve2: { SetupTerrainCurve2(world, entity); break; }
            case layer::TerrainInlet1: { SetupTerrainInlet(world, entity); break; }
            default: throw nc::NcError(fmt::format("Unhandled terrain layer '{}'", (int)entity.Layer()));
        }
    }
}

void RandomlyPopulateTerrain(nc::ecs::Ecs world, nc::Random* random)
{
    const auto terrain = FilterTerrainEntities(world);
    GenerateGrass(world, random, terrain);
    GenerateTrees(world, random, terrain);
}

auto FilterTerrainEntities(nc::ecs::Ecs world) -> std::vector<nc::Entity>
{
    auto filter = world.GetAll<nc::Entity>() | std::views::filter([](nc::Entity entity)
    {
        return IsTerrain(entity)
            && entity.Layer() != layer::TerrainInlet1; // ignore these for now
    });

    return std::vector<nc::Entity>{filter.begin(), filter.end()};
}

void GenerateGrass(nc::ecs::Ecs world, nc::Random* random, const std::vector<nc::Entity>& terrain)
{
    constexpr auto grassPerTerrain = 50ull;
    const auto root = world.Emplace<nc::Entity>({.tag = "[Env] Grass"});

    for (auto entity : terrain)
    {
        const auto [minPos, maxPos] = ::GetLocalSpawnExtents(world, entity);
        for ([[maybe_unused]] auto _ : std::views::iota(0ull, grassPerTerrain))
        {
            const auto grass = world.Emplace<nc::Entity>({
                .position = random->Between(
                    minPos,
                    maxPos
                ),
                .rotation = nc::Quaternion::FromAxisAngle(
                    nc::Vector3::Up(),
                    random->Between(-1.57f, 1.57f)
                ),
                .scale = random->Between(nc::Vector3::Splat(0.3f), nc::Vector3::Splat(3.0f)),
                .parent = root,
                .flags = nc::Entity::Flags::Static
            });

            world.Emplace<nc::graphics::ToonRenderer>(grass, GrassMesh, GrassMaterial);
        }
    }
}

void GenerateTrees(nc::ecs::Ecs world, nc::Random* random, const std::vector<nc::Entity>& terrain)
{
    constexpr auto treesPerTerrain = 10ull;
    const auto root = world.Emplace<nc::Entity>({.tag = "[Env] Trees"});

    for (auto entity : terrain)
    {
        const auto [minPos, maxPos] = ::GetLocalSpawnExtents(world, entity);

        for ([[maybe_unused]] auto _ : std::views::iota(0ull, treesPerTerrain))
        {
            const auto tree = world.Emplace<nc::Entity>({
                .position = random->Between(
                    minPos,
                    maxPos
                ),
                .rotation = nc::Quaternion::FromAxisAngle(
                    nc::Vector3::Up(),
                    random->Between(-1.57f, 1.57f)
                ),
                .scale = nc::Vector3::Splat(random->Between(0.3f, 2.0f)),
                .parent = root,
                .flags = nc::Entity::Flags::Static
            });

            world.Emplace<nc::graphics::ToonRenderer>(tree, Tree01Mesh, HealthyTree01Material);
        }
    }
}
} // namespace game
