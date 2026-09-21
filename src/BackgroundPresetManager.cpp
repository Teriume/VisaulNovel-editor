#include "BackgroundPresetManager.h"
#include <fstream>
#include <iomanip>

void BackgroundPresetManager::Add(const std::string& name, const std::string& imagePath) {
    if (!name.empty() && !imagePath.empty())
        m_presets.push_back({name, imagePath});
}

const std::vector<BackgroundPreset>& BackgroundPresetManager::GetPresets() const {
    return m_presets;
}

bool BackgroundPresetManager::SaveToFile(const std::string& path) const {
    std::ofstream file(path);
    if (!file) return false;
    file << "NANA_BACKGROUND_PRESETS_1\n" << m_presets.size() << '\n';
    for (const auto& preset : m_presets)
        file << std::quoted(preset.name) << ' ' << std::quoted(preset.imagePath) << '\n';
    return true;
}

bool BackgroundPresetManager::LoadFromFile(const std::string& path) {
    std::ifstream file(path);
    std::string header;
    size_t count = 0;
    if (!file || !(file >> header) || header != "NANA_BACKGROUND_PRESETS_1" || !(file >> count))
        return false;
    std::vector<BackgroundPreset> loaded;
    for (size_t index = 0; index < count; ++index) {
        BackgroundPreset preset;
        if (!(file >> std::quoted(preset.name) >> std::quoted(preset.imagePath))) return false;
        loaded.push_back(std::move(preset));
    }
    m_presets = std::move(loaded);
    return true;
}
