#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include <memory> // Добавлено для std::unique_ptr
#include "Scene.h"

class SceneManager {
public:
    SceneManager() = default;

    bool AddScene(const std::string& name);
    void RemoveSelectedScene();
    void MoveSelectedSceneUp();
    void MoveSelectedSceneDown();
    void ResetSelection();

    Scene* GetSelectedScene();
    const Scene* GetSelectedScene() const;
    int GetSelectedSceneIndex() const;
    void SelectScene(int index);

    int GetSelectedCharacterIndex() const;
    void SelectCharacter(int index);

    // Исправлен тип возвращаемого значения
    const std::vector<std::unique_ptr<Scene>>& GetScenes() const;

    size_t GetSceneCount() const;
    Scene* GetScene(const std::string& name);
    const Scene* GetScene(const std::string& name) const;
    bool SelectSceneByName(const std::string& name);
    bool SaveToFile(const std::string& path) const;
    bool LoadFromFile(const std::string& path);
    void SetProjectDirectory(const std::filesystem::path& directory);
    bool SaveGameState(const std::string& path) const;
    bool LoadGameState(const std::string& path);

private:
    std::vector<std::unique_ptr<Scene>> m_scenes;
    int m_selectedSceneIndex = -1;
    int m_selectedCharacterIndex = -1;
    std::filesystem::path m_projectDirectory;
};