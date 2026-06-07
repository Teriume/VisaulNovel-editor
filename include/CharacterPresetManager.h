#pragma once
#include <memory>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

struct CharacterPreset {
    std::string name;
    std::string emotion = "Нормальное";
    std::string notes;
    std::string imagePath;
    std::unique_ptr<sf::Texture> previewTexture;
};

class CharacterPresetManager {
public:
    CharacterPresetManager() = default;

    bool AddPreset(const std::string& name,
                   const std::string& emotion,
                   const std::string& notes,
                   const std::string& imagePath,
                   std::unique_ptr<sf::Texture> previewTexture);

    CharacterPreset* GetSelectedPreset();
    const CharacterPreset* GetSelectedPreset() const;
    int GetSelectedPresetIndex() const;
    void SelectPreset(int index);
    bool HasPresets() const;
    const std::vector<CharacterPreset>& GetPresets() const;

private:
    std::vector<CharacterPreset> m_presets;
    int m_selectedPresetIndex = -1;
};
