#include "Assets.h"

#include "ncengine/asset/Assets.h"
#include "ncengine/config/Config.h"

#include <filesystem>

namespace
{
template<class LoadFunc>
void LoadAssets(const std::filesystem::path& rootDir, nc::asset_flags_type flags, LoadFunc load)
{
    auto paths = std::vector<std::string>{};
    for (auto&& entry : std::filesystem::recursive_directory_iterator{rootDir})
    {
        if (entry.path().extension() != ".nca")
        {
            continue;
        }

        const auto& path = entry.path();
        auto trimmedPath = std::filesystem::path{};
        auto segment = path.begin();
        ++segment; ++segment; ++segment; // super hacky and brittle
        for (; segment != path.end(); ++segment)
        {
            trimmedPath /= *segment;
        }

        paths.push_back(trimmedPath.string());
    }

    load(paths, false, flags);
}
} // anonymous namespace

namespace game
{
void LoadAssets(const nc::config::AssetSettings& settings)
{
    ::LoadAssets(settings.audioClipsPath, nc::AssetFlags::None, &nc::LoadAudioClipAssets);
    ::LoadAssets(settings.meshesPath, nc::AssetFlags::None, &nc::LoadMeshAssets);
    ::LoadAssets(settings.texturesPath, nc::AssetFlags::None, &nc::LoadTextureAssets);
    ::LoadAssets(settings.concaveCollidersPath, nc::AssetFlags::None, &nc::LoadConcaveColliderAssets);
    ::LoadAssets(settings.skeletalAnimationsPath, nc::AssetFlags::None, &nc::LoadSkeletalAnimationAssets);
}
} // namespace game
