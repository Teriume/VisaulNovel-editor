#pragma once
#include <vector>
#include <string>
#include "Scene.h"

class SceneManager {
public:
    SceneManager() = default;

    bool AddScene(const std::string& name);
    void RemoveSelectedScene();
    void ResetSelection();

    Scene* GetSelectedScene();
    const Scene* GetSelectedScene() const;
    int GetSelectedSceneIndex() const;
    void SelectScene(int index);

    int GetSelectedCharacterIndex() const;
    void SelectCharacter(int index);

    const std::vector<Scene>& GetScenes() const;
    size_t GetSceneCount() const;

private:
    std::vector<Scene> m_scenes;
    int m_selectedSceneIndex = -1;
    int m_selectedCharacterIndex = -1;
};
