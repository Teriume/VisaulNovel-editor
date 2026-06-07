#include "Application.h"

Application::Application() 
    // В SFML 3 конструктор VideoMode принимает Vector2u, поэтому нужны {{ }}
    : m_window(sf::VideoMode({1280, 720}), "Na-Na Engine") 
{
    m_window.setFramerateLimit(60); // Используем точку, так как это объект
    m_editor.Init(m_window);
}

void Application::ProcessEvents() {
    // В SFML 3 pollEvent возвращает std::optional. Это современно и безопасно.
    while (const std::optional event = m_window.pollEvent()) {
        // Передаем событие в ImGui (разыменовываем optional через *)
        m_editor.ProcessEvents(m_window, *event);

        // Проверяем тип события по-новому
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }
    }
}

void Application::Update(sf::Time dt) {
    m_editor.Update(m_window, dt);
}

void Application::Render() {
    m_window.clear(sf::Color(30, 30, 30));
    
    m_editor.Draw();
    m_editor.Render(m_window);

    m_window.display();
}

int Application::Run() {
    while (m_window.isOpen()) {
        ProcessEvents();
        Update(m_clock.restart());
        Render();
    }
    return 0;
}