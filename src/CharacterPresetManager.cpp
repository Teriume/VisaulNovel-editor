#include "CharacterPresetManager.h"
#include <fstream>
#include <iomanip>
#include <algorithm>

bool CharacterPresetManager::AddPreset(const std::string& name,
                                       const std::unordered_map<std::string, std::string>& emotions,
                                       const std::string& defaultEmotion,
                                       const std::string& notes,
                                       std::shared_ptr<sf::Texture> previewTexture) {
    if (name.empty() || emotions.empty())
        return false;

    CharacterPreset preset;
    preset.name = name;
    preset.emotions = emotions;
    preset.defaultEmotion = defaultEmotion.empty() ? "Нормальное" : defaultEmotion;
    preset.notes = notes;
    preset.previewTexture = previewTexture;

    // Если пресет с таким именем уже существует — обновляем его, иначе добавляем
    auto it = std::find_if(m_presets.begin(), m_presets.end(), [&](const CharacterPreset& p) {
        return p.name == name;
    });

    if (it != m_presets.end()) {
        *it = std::move(preset);
    } else {
        m_presets.push_back(std::move(preset));
    }

    m_selectedPresetIndex = static_cast<int>(m_presets.size()) - 1;
    return true;
}

CharacterPreset* CharacterPresetManager::GetSelectedPreset() {
    return (m_selectedPresetIndex >= 0 && m_selectedPresetIndex < static_cast<int>(m_presets.size()))
        ? &m_presets[m_selectedPresetIndex]
        : nullptr;
}

const CharacterPreset* CharacterPresetManager::GetSelectedPreset() const {
    return (m_selectedPresetIndex >= 0 && m_selectedPresetIndex < static_cast<int>(m_presets.size()))
        ? &m_presets[m_selectedPresetIndex]
        : nullptr;
}

int CharacterPresetManager::GetSelectedPresetIndex() const {
    return m_selectedPresetIndex;
}

void CharacterPresetManager::SelectPreset(int index) {
    if (index >= 0 && index < static_cast<int>(m_presets.size())) {
        m_selectedPresetIndex = index;
    } else {
        m_selectedPresetIndex = -1;
    }
}

bool CharacterPresetManager::HasPresets() const {
    return !m_presets.empty();
}

const std::vector<CharacterPreset>& CharacterPresetManager::GetPresets() const {
    return m_presets;
}

bool CharacterPresetManager::SaveToFile(const std::string& path) const {
    std::ofstream file(path);
    if (!file) return false;

    file << "NANA_CHARACTER_PRESETS_2\n" << m_presets.size() << '\n';
    for (const auto& preset : m_presets) {
        file << std::quoted(preset.name) << ' ' 
             << std::quoted(preset.defaultEmotion) << ' '
             << std::quoted(preset.notes) << ' ' 
             << preset.emotions.size() << '\n';

        for (const auto& [emoName, emoPath] : preset.emotions) {
            file << std::quoted(emoName) << ' ' << std::quoted(emoPath) << '\n';
        }
    }
    return true;
}

bool CharacterPresetManager::LoadFromFile(const std::string& path) {
    m_presets.clear();
    m_selectedPresetIndex = -1;
    return ImportFromFile(path);
}

bool CharacterPresetManager::ImportFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) return false;

    std::string header;
    file >> header;

    std::vector<CharacterPreset> loaded;

    if (header == "NANA_CHARACTER_PRESETS_2") {
        size_t count = 0;
        if (!(file >> count)) return false;

        for (size_t index = 0; index < count; ++index) {
            CharacterPreset preset;
            size_t emotionsCount = 0;

            if (!(file >> std::quoted(preset.name) 
                       >> std::quoted(preset.defaultEmotion)
                       >> std::quoted(preset.notes) 
                       >> emotionsCount)) {
                return false;
            }

            for (size_t e = 0; e < emotionsCount; ++e) {
                std::string emoName, emoPath;
                if (!(file >> std::quoted(emoName) >> std::quoted(emoPath))) {
                    return false;
                }
                preset.emotions[emoName] = emoPath;
            }

            auto it = preset.emotions.find(preset.defaultEmotion);
            std::string previewPath = (it != preset.emotions.end()) ? it->second : (preset.emotions.empty() ? "" : preset.emotions.begin()->second);
            
            if (!previewPath.empty()) {
                preset.previewTexture = std::make_shared<sf::Texture>();
                if (!preset.previewTexture->loadFromFile(previewPath)) {
                    preset.previewTexture.reset();
                }
            }

            loaded.push_back(std::move(preset));
        }
    } else if (header == "NANA_CHARACTER_PRESETS_1") {
        size_t count = 0;
        if (!(file >> count)) return false;

        for (size_t index = 0; index < count; ++index) {
            CharacterPreset preset;
            std::string oldEmotion, oldPath;

            if (!(file >> std::quoted(preset.name) >> std::quoted(oldEmotion)
                  >> std::quoted(preset.notes) >> std::quoted(oldPath))) {
                return false;
            }

            preset.defaultEmotion = oldEmotion.empty() ? "Нормальное" : oldEmotion;
            preset.emotions[preset.defaultEmotion] = oldPath;

            preset.previewTexture = std::make_shared<sf::Texture>();
            if (!preset.previewTexture->loadFromFile(oldPath)) {
                preset.previewTexture.reset();
            }

            loaded.push_back(std::move(preset));
        }
    } else {
        return false;
    }

    // Объединяем импортированные пресеты с имеющимися
    for (auto& newPreset : loaded) {
        auto it = std::find_if(m_presets.begin(), m_presets.end(), [&](const CharacterPreset& p) {
            return p.name == newPreset.name;
        });

        if (it != m_presets.end()) {
            *it = std::move(newPreset);
        } else {
            m_presets.push_back(std::move(newPreset));
        }
    }

    if (m_selectedPresetIndex == -1 && !m_presets.empty()) {
        m_selectedPresetIndex = 0;
    }

    return true;
}