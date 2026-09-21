#pragma once
#include <string>
#include <vector>

struct BackgroundPreset {
    std::string name;
    std::string imagePath;
};

class BackgroundPresetManager {
public:
    void Add(const std::string& name, const std::string& imagePath);
    const std::vector<BackgroundPreset>& GetPresets() const;
    bool SaveToFile(const std::string& path) const;
    bool LoadFromFile(const std::string& path);
private:
    std::vector<BackgroundPreset> m_presets;
};
