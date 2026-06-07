#pragma once
#include <SFML/Graphics.hpp>
#include "SceneManager.h"
#include "CharacterPresetManager.h"
#include "ScenePreview.h"

class Editor {
private:
    sf::RenderWindow* m_window = nullptr;
    SceneManager m_sceneManager;
    CharacterPresetManager m_presetManager;
    ScenePreview m_scenePreview;

    bool m_showPresetWindow = true;
    bool m_showNewSceneWindow = false;
    bool m_showNewPresetWindow = false;
    int m_editTarget = 0;

public:
    Editor();
    ~Editor();

    void Init(sf::RenderWindow& window);
    void ProcessEvents(sf::RenderWindow& window, const sf::Event& event);
    void Update(sf::RenderWindow& window, sf::Time dt);
    void Draw();
    void Render(sf::RenderWindow& window);
};