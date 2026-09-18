#include "SceneManager.h"

bool SceneManager::AddScene(const std::string& name) {
    if (name.empty())
        return false;

    m_scenes.emplace_back(name.c_str());
    m_selectedSceneIndex = static_cast<int>(m_scenes.size()) - 1;
    m_selectedCharacterIndex = -1;
    return true;
}

void SceneManager::RemoveSelectedScene() {
    if (m_selectedSceneIndex < 0 || m_selectedSceneIndex >= static_cast<int>(m_scenes.size()))
        return;

    m_scenes.erase(m_scenes.begin() + m_selectedSceneIndex);
    m_selectedSceneIndex = -1;
    m_selectedCharacterIndex = -1;
}

void SceneManager::ResetSelection() {
    m_selectedSceneIndex = -1;
    m_selectedCharacterIndex = -1;
}

Scene* SceneManager::GetSelectedScene() {
    return (m_selectedSceneIndex >= 0 && m_selectedSceneIndex < static_cast<int>(m_scenes.size()))
        ? &m_scenes[m_selectedSceneIndex]
        : nullptr;
}

const Scene* SceneManager::GetSelectedScene() const {
    return (m_selectedSceneIndex >= 0 && m_selectedSceneIndex < static_cast<int>(m_scenes.size()))
        ? &m_scenes[m_selectedSceneIndex]
        : nullptr;
}

int SceneManager::GetSelectedSceneIndex() const {
    return m_selectedSceneIndex;
}

void SceneManager::SelectScene(int index) {
    if (index >= 0 && index < static_cast<int>(m_scenes.size())) {
        if (m_selectedSceneIndex != index) {
            m_selectedCharacterIndex = -1;
        }
        m_selectedSceneIndex = index;
    } else {
        m_selectedSceneIndex = -1;
        m_selectedCharacterIndex = -1;
    }
}

int SceneManager::GetSelectedCharacterIndex() const {
    return m_selectedCharacterIndex;
}

void SceneManager::SelectCharacter(int index) {
    if (Scene* scene = GetSelectedScene()) {
        if (index >= 0 && index < static_cast<int>(scene->GetCharacterCount())) {
            m_selectedCharacterIndex = index;
            return;
        }
    }
    m_selectedCharacterIndex = -1;
}

const std::vector<Scene>& SceneManager::GetScenes() const {
    return m_scenes;
}

size_t SceneManager::GetSceneCount() const {
    return m_scenes.size();
}
