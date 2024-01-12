#include "Core.h"

#include "ncengine/serialize/SceneSerialization.h"

#include <fstream>

namespace game
{
void LoadFragment(std::string_view path, nc::Registry* registry, nc::ModuleProvider modules)
{
    if (auto fragmentFile = std::ifstream{path.data(), std::ios::binary})
    {
        nc::LoadSceneFragment(fragmentFile, registry->GetEcs(), *modules.Get<nc::asset::NcAsset>());
    }
    else
    {
        throw nc::NcError(fmt::format("Failed to open scene '{}'", path));
    }
}
} // namespace game
