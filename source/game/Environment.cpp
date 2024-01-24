#include "Environment.h"
#include "Assets.h"

#include <ranges>

#include <iostream>

namespace
{
constexpr auto Terrain1SpawnExtent = nc::Vector3{11.0f, 1.0f, 11.0f};
constexpr auto Terrain2SpawnExtent = nc::Vector3{11.0f, 1.0f, 11.0f};
constexpr auto TerrainCurve1SpawnExtent = nc::Vector3{3.0f, 1.0f, 11.0f};
constexpr auto TerrainCurve2SpawnExtent = nc::Vector3{11.0f, 1.0f, 3.0f};
constexpr auto TerrainInlet1SpawnExtent = nc::Vector3{10.0f, 1.0f, 20.0f};

constexpr auto BorderSpawnExtent = nc::Vector3{24.0f, 4.0f, 6.0f};

constexpr auto ItemsPerLargeTerrain = 15ull;
constexpr auto ItemsPerSmallTerrain = 6ull;
constexpr auto ItemsPerBorder = 15ull;

using choice_t = std::pair<const char* const, nc::graphics::ToonMaterial>;

const auto TreeChoices = std::array<choice_t, 5 >
{
    choice_t{game::PineMesh, game::PineMaterial},
    choice_t{game::PineMesh, game::PineMaterial},
    choice_t{game::PineMesh, game::PineMaterial},
    choice_t{game::PineMesh, game::PineMaterial},
    choice_t{game::AspensMesh, game::AspensMaterial},
};

const auto VegetationChoices = std::array<choice_t, 3>
{
    // choice_t{game::GrassMesh, game::GrassMaterial},
    // choice_t{game::GrassMesh, game::GrassMaterial},
    choice_t{game::FernMesh, game::FernMaterial},
    choice_t{game::FernMesh, game::FernMaterial},
    choice_t{game::AloeMesh, game::AloeMaterial}
};

auto GetSpawnExtent(uint8_t layer) -> nc::Vector3
{
    switch (layer)
    {
        case game::layer::Terrain1: return Terrain1SpawnExtent;
        case game::layer::Terrain2: return Terrain2SpawnExtent;
        case game::layer::TerrainCurve1: return TerrainCurve1SpawnExtent;
        case game::layer::TerrainCurve2: return TerrainCurve2SpawnExtent;
        case game::layer::TerrainInlet1: return TerrainInlet1SpawnExtent;
        case game::layer::Border: return BorderSpawnExtent;
        default: throw nc::NcError("unhandled spawn extent");
    }
}

auto GetItemsPerTerrain(uint8_t layer) -> size_t
{
    switch (layer)
    {
        case game::layer::Terrain1: return ItemsPerLargeTerrain;
        case game::layer::Terrain2: return ItemsPerLargeTerrain;
        case game::layer::TerrainCurve1: return ItemsPerSmallTerrain;
        case game::layer::TerrainCurve2: return ItemsPerSmallTerrain;
        case game::layer::TerrainInlet1: return ItemsPerLargeTerrain;
        case game::layer::Border: return ItemsPerBorder;
        default: throw nc::NcError("unhandled spawn extent");
    }
}

auto GetLocalSpawnExtents(nc::ecs::Ecs world, nc::Entity entity)
{
    auto maxExtent = ::GetSpawnExtent(entity.Layer());
    auto minExtent = nc::Vector3{-maxExtent.x, maxExtent.y, -maxExtent.z};
    const auto transform = world.Get<nc::Transform>(entity);
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
    world.Emplace<nc::physics::Collider>(entity, nc::physics::BoxProperties{.center = nc::Vector3{0.0f, 0.0f, 0.2f}, .extents = nc::Vector3{24.5f, 5.0f, 24.5f}});
}

void SetupTerrain2(nc::ecs::Ecs world, nc::Entity entity)
{
    world.Emplace<nc::physics::Collider>(entity, nc::physics::BoxProperties{.extents = nc::Vector3{24.5f, 5.0f, 25.0f}});
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
    // const auto terrain = FilterTerrainEntities(world);
    // GenerateGrass(world, random, terrain);
    // GenerateVegetation(world, random, terrain);
    // GenerateTrees(world, random, terrain);

    const auto borders = FilterBorderEntities(world);
    GenerateBorderTrees(world, random, borders);
}

auto FilterTerrainEntities(nc::ecs::Ecs world) -> std::vector<nc::Entity>
{
    auto filter = world.GetAll<nc::Entity>() | std::views::filter([](nc::Entity entity)
    {
        return IsTerrain(entity)
            && entity.Layer();// != layer::TerrainInlet1; // ignore these for now
    });

    return std::vector<nc::Entity>{filter.begin(), filter.end()};
}

auto FilterBorderEntities(nc::ecs::Ecs world) -> std::vector<nc::Entity>
{
    auto filter = world.GetAll<nc::Entity>() | std::views::filter([](nc::Entity entity)
    {
        return entity.Layer() == layer::Border;
    });

    return std::vector<nc::Entity>{filter.begin(), filter.end()};
}

// void GenerateGrass(nc::ecs::Ecs world, nc::Random* random, const std::vector<nc::Entity>&)
// {
//     const auto root = world.Emplace<nc::Entity>({.tag = "[Env] Grass"});

//     auto totalGrass = 200ull;
//     const auto [minPos, maxPos] = std::pair{nc::Vector3{-180.0f, -1.0f, 180.0f}, nc::Vector3{180.0f, -1.0f, 180.0f}};
//     while (totalGrass-- != 0)
//     {
//         const auto grass = world.Emplace<nc::Entity>({
//             .position = random->Between(
//                 minPos,
//                 maxPos
//             ),
//             .rotation = nc::Quaternion::FromAxisAngle(
//                 nc::Vector3::Up(),
//                 random->Between(-1.57f, 1.57f)
//             ),
//             .scale = random->Between(nc::Vector3::Splat(0.3f), nc::Vector3::Splat(3.0f)),
//             .parent = root,
//             .layer = layer::Foliage,
//             .flags = nc::Entity::Flags::Static
//         });

//         world.Emplace<nc::graphics::ToonRenderer>(grass, GrassMesh, GrassMaterial);
//     }
// }

void GenerateVegetation(nc::ecs::Ecs world, nc::Random* random, const std::vector<nc::Entity>& terrain)
{
    const auto root = world.Emplace<nc::Entity>({.tag = "[Env] Vegetation"});
    auto lastChoice = 0ull;

    for (auto entity : terrain)
    {
        const auto grassPerTerrain = ::GetItemsPerTerrain(entity.Layer());
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
                .layer = layer::Foliage,
                .flags = nc::Entity::Flags::Static
            });

            if (lastChoice == ::VegetationChoices.size())
                lastChoice = 0;

            const auto& [mesh, material] = ::VegetationChoices.at(lastChoice++);
            world.Emplace<nc::graphics::ToonRenderer>(grass, mesh, material);
        }
    }
}

void GenerateTrees(nc::ecs::Ecs world, nc::Random* random, const std::vector<nc::Entity>& terrain)
{
    const auto root = world.Emplace<nc::Entity>({.tag = "[Env] Trees"});
    auto lastChoice = 0ull;

    for (auto entity : terrain)
    {
        const auto treesPerTerrain = ::GetItemsPerTerrain(entity.Layer());
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
                .layer = layer::Foliage,
                .flags = nc::Entity::Flags::Static
            });

            if (lastChoice == ::TreeChoices.size())
                lastChoice = 0;

            const auto& [mesh, material] = ::TreeChoices.at(lastChoice++);
            world.Emplace<nc::graphics::ToonRenderer>(tree, mesh, material);
        }
    }
}

void GenerateBorderTrees(nc::ecs::Ecs world, nc::Random* random, const std::vector<nc::Entity>& terrain)
{
    const auto root = world.Emplace<nc::Entity>({.tag = "[Env] Border Trees"});

    for (auto entity : terrain)
    {
        const auto treesPerTerrain = ::GetItemsPerTerrain(entity.Layer());
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
                .scale = nc::Vector3::Splat(random->Between(0.45f, 5.0f)),
                .parent = root,
                .layer = layer::Foliage,
                .flags = nc::Entity::Flags::Static
            });


            world.Emplace<nc::graphics::ToonRenderer>(tree, PineMesh, PineMaterial);
        }
    }
}
} // namespace game
