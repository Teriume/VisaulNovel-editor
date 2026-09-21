#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <SFML/Graphics.hpp>

struct CharacterPreset {
    std::string name;
    std::string notes;
    std::string defaultEmotion = "Нормальное";
    
    std::unordered_map<std::string, std::string> emotions;
    
    // Заменили unique_ptr на shared_ptr, чтобы vector не поршил память при реаллокации
    std::shared_ptr<sf::Texture> previewTexture;
};

class CharacterPresetManager {
public:
    CharacterPresetManager() = default;

    bool AddPreset(const std::string& name,
                   const std::unordered_map<std::string, std::string>& emotions,
                   const std::string& defaultEmotion,
                   const std::string& notes,
                   std::shared_ptr<sf::Texture> previewTexture);

    CharacterPreset* GetSelectedPreset();
    const CharacterPreset* GetSelectedPreset() const;
    int GetSelectedPresetIndex() const;
    void SelectPreset(int index);
    bool HasPresets() const;
    const std::vector<CharacterPreset>& GetPresets() const;
    
    bool SaveToFile(const std::string& path) const;
    bool LoadFromFile(const std::string& path);   // Очищает и загружает заново
    bool ImportFromFile(const std::string& path); // Добавляет/объединяет пресеты

private:
    std::vector<CharacterPreset> m_presets;
    int m_selectedPresetIndex = -1;
};