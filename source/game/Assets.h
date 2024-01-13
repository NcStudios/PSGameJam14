#pragma once

namespace nc::config
{
struct AssetSettings;
};

namespace game
{
void LoadAssets(const nc::config::AssetSettings& settings);

constexpr auto TreeMesh = "tree.nca";
} // namespace game
