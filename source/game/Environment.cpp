#include "Environment.h"
#include "Assets.h"

#include <ranges>

#include <iostream>

namespace
{
constexpr auto Terrain1SpawnExtent = nc::Vector3{12.0f, 1.0f, 12.0f};
constexpr auto Terrain2SpawnExtent = nc::Vector3{12.0f, 1.0f, 12.0f};
constexpr auto TerrainCurve1SpawnExtent = nc::Vector3{1.0f, 1.0f, 1.0f};
constexpr auto TerrainCurve2SpawnExtent = nc::Vector3{1.0f, 1.0f, 1.0f};
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

void GenerateGrass(nc::ecs::Ecs world, nc::Random* random)
{
    constexpr auto grassPerTerrain = 5;

    // We can leave this serializable if we want to use a development tool (generate, position, save)
    // or turn off serialization if 'full blown random all the time' works
    auto root = world.Emplace<nc::Entity>({.tag = "[Env] Grass"});

    auto terrain = world.GetAll<nc::Entity>()
        | std::views::filter([](nc::Entity entity) { return IsTerrain(entity); });

    auto terrainCount = 0ull;

    for (auto entity : terrain)
    {
        ++terrainCount;
        auto grassCount = 0;

        auto terrainTransform = world.Get<nc::Transform>(entity);
        const auto pos = terrainTransform->Position();
        const auto extent = ::GetSpawnExtent(entity.Layer());
        const auto minPos = pos - extent;
        const auto maxPos = pos + extent;

        for (auto i = 0ull; i < grassPerTerrain; ++i)
        {
            ++grassCount;
            auto scale = random->Between(nc::Vector3::Splat(0.3f), nc::Vector3::Splat(3.0f));

            auto grass = world.Emplace<nc::Entity>({
                .position = random->Between(
                    minPos,
                    maxPos
                ),
                .rotation = nc::Quaternion::FromAxisAngle(
                    nc::Vector3::Up(),
                    random->Between(-1.57f, 1.57f)
                ),
                .scale = scale,
                .parent = root,
                .flags = nc::Entity::Flags::Static
            });

            world.Emplace<nc::graphics::ToonRenderer>(grass, GrassMesh, GrassMaterial);
        }

        std::cerr << "terrain no. " << terrainCount << ", grassSpawned: " << grassCount << '\n';
    }

    auto childCount = world.Get<nc::Transform>(root)->Children().size();
    std::cerr << "actualChildCount: " << childCount << '\n';
}

void GenerateTrees(nc::ecs::Ecs world, nc::Random* random)
{
    constexpr auto treesPerTerrain = 5ull;
    auto root = world.Emplace<nc::Entity>({.tag = "[Env] Trees"});

    auto terrain = world.GetAll<nc::Entity>()
        | std::views::filter([](nc::Entity entity) { return IsTerrain(entity); });

    for (auto entity : terrain)
    {
        auto terrainTransform = world.Get<nc::Transform>(entity);
        const auto pos = terrainTransform->Position();
        const auto extent = ::GetSpawnExtent(entity.Layer());
        const auto minPos = pos - extent;
        const auto maxPos = pos + extent;

        for (auto i = 0; i < treesPerTerrain; ++i)
        {
            auto scale = nc::Vector3::Splat(random->Between(0.3f, 2.0f));

            auto tree = world.Emplace<nc::Entity>({
                .position = random->Between(
                    minPos,
                    maxPos
                ),
                .rotation = nc::Quaternion::FromAxisAngle(
                    nc::Vector3::Up(),
                    random->Between(-1.57f, 1.57f)
                ),
                .scale = scale,
                .parent = root,
                .flags = nc::Entity::Flags::Static
            });

            world.Emplace<nc::graphics::ToonRenderer>(tree, Tree01Mesh, HealthyTree01Material);
        }
    }
}
} // namespace game
