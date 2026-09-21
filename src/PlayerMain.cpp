#include "SceneManager.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <array>
#include <filesystem>
#include <iostream>

namespace {
const char* FindFont() {
    const std::array<const char*, 4> paths = {
        "/System/Library/Fonts/SFNS.ttf",
        "/System/Library/Fonts/Supplemental/Arial Unicode.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/Library/Fonts/Arial Unicode.ttf"
    };
    for (const char* path : paths)
        if (std::filesystem::exists(path)) return path;
    return nullptr;
}

void DrawText(sf::RenderWindow& window, const sf::Font& font, const std::string& value,
              unsigned int size, sf::Vector2f position, sf::Color color = sf::Color::White) {
    sf::Text text(font, value, size);
    text.setPosition(position);
    text.setFillColor(color);
    window.draw(text);
}
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: VisualNovelPlayer path/to/novel.nana\n";
        return 1;
    }

    SceneManager sceneManager;
    if (!sceneManager.LoadFromFile(argv[1]) || sceneManager.GetSceneCount() == 0) {
        std::cerr << "Could not load novel: " << argv[1] << '\n';
        return 1;
    }

    sf::RenderWindow window(sf::VideoMode({1280, 720}), "Na-Na Novel");
    window.setFramerateLimit(60);
    sf::Font font;
    const char* fontPath = FindFont();
    const bool hasFont = fontPath && font.openFromFile(fontPath);
    const sf::Color panelColor(15, 18, 28, 235);
    const sf::Color accentColor(255, 205, 105);
    bool showMenu = true;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouse->button != sf::Mouse::Button::Left)
                    continue;
                if (showMenu) {
                    if (sf::FloatRect({460.0f, 330.0f}, {360.0f, 54.0f}).contains(sf::Vector2f(mouse->position)))
                        showMenu = false;
                    continue;
                }
                Scene* scene = sceneManager.GetSelectedScene();
                if (!scene) continue;
                const auto& choices = scene->GetChoices();
                bool choiceClicked = false;
                const float choiceTop = 570.0f;
                for (size_t index = 0; index < choices.size(); ++index) {
                    const sf::FloatRect bounds({40.0f, choiceTop + static_cast<float>(index) * 42.0f}, {1200.0f, 36.0f});
                    if (bounds.contains(sf::Vector2f(mouse->position))) {
                        if (!choices[index].targetScene.empty())
                            sceneManager.SelectSceneByName(choices[index].targetScene);
                        choiceClicked = true;
                        break;
                    }
                }
                if (!choiceClicked && choices.empty()) {
                    const int next = sceneManager.GetSelectedSceneIndex() + 1;
                    if (next < static_cast<int>(sceneManager.GetSceneCount()))
                        sceneManager.SelectScene(next);
                }
            }
        }

        window.clear(sf::Color(20, 22, 32));
        if (showMenu) {
            sf::RectangleShape menuPanel({620.0f, 360.0f});
            menuPanel.setPosition({330.0f, 180.0f});
            menuPanel.setFillColor(panelColor);
            menuPanel.setOutlineColor(accentColor);
            menuPanel.setOutlineThickness(2.0f);
            window.draw(menuPanel);
            if (hasFont) {
                DrawText(window, font, "Моя визуальная новелла", 34, {405.0f, 230.0f}, accentColor);
                DrawText(window, font, "Новая игра", 24, {535.0f, 343.0f});
                DrawText(window, font, "Нажмите, чтобы начать", 16, {505.0f, 410.0f}, sf::Color(190, 195, 210));
            }
            window.display();
            continue;
        }
        Scene* scene = sceneManager.GetSelectedScene();
        if (scene) {
            scene->Render(window);
            const sf::Vector2u size = window.getSize();
            sf::RectangleShape dialoguePanel({static_cast<float>(size.x) - 48.0f, 145.0f});
            dialoguePanel.setPosition({24.0f, static_cast<float>(size.y) - 169.0f});
            dialoguePanel.setFillColor(panelColor);
            dialoguePanel.setOutlineColor(sf::Color(255, 205, 105, 180));
            dialoguePanel.setOutlineThickness(2.0f);
            window.draw(dialoguePanel);

            if (hasFont) {
                if (!scene->GetSpeakerName().empty())
                    DrawText(window, font, scene->GetSpeakerName(), 24, {48.0f, static_cast<float>(size.y) - 155.0f}, accentColor);
                DrawText(window, font, scene->GetDialogueText(), 20, {48.0f, static_cast<float>(size.y) - 116.0f});
                const auto& choices = scene->GetChoices();
                for (size_t index = 0; index < choices.size(); ++index) {
                    sf::RectangleShape choiceBox({static_cast<float>(size.x) - 80.0f, 36.0f});
                    choiceBox.setPosition({40.0f, 570.0f + static_cast<float>(index) * 42.0f});
                    choiceBox.setFillColor(sf::Color(40, 45, 60, 245));
                    choiceBox.setOutlineColor(accentColor);
                    choiceBox.setOutlineThickness(1.0f);
                    window.draw(choiceBox);
                    DrawText(window, font, choices[index].text, 18, choiceBox.getPosition() + sf::Vector2f(14.0f, 5.0f));
                }
            }
        }
        window.display();
    }
    return 0;
}
