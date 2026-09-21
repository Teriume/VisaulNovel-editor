#pragma once
#include <SFML/Graphics.hpp>
#include "SceneManager.h"
#include "CharacterPresetManager.h"
#include "ScenePreview.h"
#include "BackgroundPresetManager.h"

class Editor {
private:
    sf::RenderWindow* m_window = nullptr;
    SceneManager m_sceneManager;
    CharacterPresetManager m_presetManager;
    BackgroundPresetManager m_backgroundPresetManager;
    ScenePreview m_scenePreview;

    bool m_showPresetWindow = true;
    bool m_showNewSceneWindow = false;
    bool m_showNewPresetWindow = false;
    bool m_showPlayMode = false;
    bool m_showNovelMenu = true;
    char m_projectPath[512] = "";
    int m_editTarget = 0;

    // ИНДЕКС выбранного персонажа на сцене (-1 = никто не выбран)
    int m_selectedCharacterIndex = -1;

public:
    Editor();
    ~Editor();

    void Init(sf::RenderWindow& window);
    void Shutdown();
    void ProcessEvents(sf::RenderWindow& window, const sf::Event& event);
    void Update(sf::RenderWindow& window, sf::Time dt);
    void Draw();
    void Render(sf::RenderWindow& window);

    // Управление выбором персонажа
    int GetSelectedCharacterIndex() const { return m_selectedCharacterIndex; }
    void SetSelectedCharacterIndex(int index) { m_selectedCharacterIndex = index; }
};