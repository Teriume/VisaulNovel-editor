#pragma once
#include <SFML/Graphics.hpp>
#include "Editor.h"

class Application {
public:
    Application();
    ~Application() = default;

    int Run();

private:
    void ProcessEvents(); // <-- Пустые скобки!
    void Update(sf::Time dt);
    void Render();

private:
    sf::RenderWindow m_window;
    Editor           m_editor;
    sf::Clock        m_clock;
};