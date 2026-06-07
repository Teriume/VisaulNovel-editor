#include "CharacterPresetManager.h"

bool CharacterPresetManager::AddPreset(const std::string& name,
                                      const std::string& emotion,
                                      const std::string& notes,
                                      const std::string& imagePath,
                                      std::unique_ptr<sf::Texture> previewTexture) {
    if (name.empty() || imagePath.empty())
        return false;

    CharacterPreset preset;
    preset.name = name;
    preset.emotion = emotion.empty() ? "Нормальное" : emotion;
    preset.notes = notes;
    preset.imagePath = imagePath;
    preset.previewTexture = std::move(previewTexture);
    m_presets.push_back(std::move(preset));
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
