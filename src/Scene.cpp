#include "Scene.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

Scene::Scene(const char* name) {
    std::strncpy(m_name, name, sizeof(m_name) - 1);
    m_name[sizeof(m_name) - 1] = '\0';
}

void Scene::SetName(const char* name) {
    std::strncpy(m_name, name, sizeof(m_name) - 1);
    m_name[sizeof(m_name) - 1] = '\0';
}

bool Scene::LoadBackground(const char* path) {
    if (!m_backgroundTexture.loadFromFile(path))
        return false;

    m_backgroundPath = path;
    m_backgroundSprite = std::make_unique<sf::Sprite>(m_backgroundTexture);
    m_backgroundSprite->setPosition(sf::Vector2f(0.f, 0.f));
    m_backgroundSprite->setScale(sf::Vector2f(1.f, 1.f));
    m_hasBackground = true;
    return true;
}

sf::Sprite* Scene::GetBackgroundSprite() {
    return m_hasBackground ? m_backgroundSprite.get() : nullptr;
}

const sf::Sprite* Scene::GetBackgroundSprite() const {
    return m_hasBackground ? m_backgroundSprite.get() : nullptr;
}

const std::string& Scene::GetBackgroundPath() const {
    return m_backgroundPath;
}

void Scene::SetBackgroundScale(const sf::Vector2f& scale) {
    if (m_hasBackground && m_backgroundSprite)
        m_backgroundSprite->setScale(scale);
}

void Scene::SetBackgroundPosition(const sf::Vector2f& position) {
    if (m_hasBackground && m_backgroundSprite)
        m_backgroundSprite->setPosition(position);
}

sf::Vector2f Scene::GetBackgroundScale() const {
    return m_backgroundSprite ? m_backgroundSprite->getScale() : sf::Vector2f(1.f, 1.f);
}

sf::Vector2f Scene::GetBackgroundPosition() const {
    return m_backgroundSprite ? m_backgroundSprite->getPosition() : sf::Vector2f(0.f, 0.f);
}

bool Scene::AddCharacter(const char* path, const char* name, const char* emotion, const char* notes) {
    m_characters.emplace_back();
    Character& character = m_characters.back();

    if (!character.texture.loadFromFile(path)) {
        m_characters.pop_back();
        return false;
    }

    character.name = name ? name : "Персонаж";
    character.emotion = emotion ? emotion : "Нормальное";
    character.notes = notes ? notes : "";
    character.sprite = std::make_unique<sf::Sprite>(character.texture);
    character.sprite->setPosition(sf::Vector2f(100.f, 100.f));
    character.sprite->setScale(sf::Vector2f(1.f, 1.f));
    return true;
}

bool Scene::LoadMusic(const char* path) {
    if (!m_music.openFromFile(path))
        return false;

    m_musicPath = path;
    m_hasMusic = true;
    m_music.setVolume(m_musicVolume);
    m_music.setLooping(m_musicLoop);
    if (m_musicEnabled) {
        m_music.play();
    } else {
        m_music.pause();
    }
    return true;
}

sf::Music* Scene::GetMusic() {
    return m_hasMusic ? &m_music : nullptr;
}

const sf::Music* Scene::GetMusic() const {
    return m_hasMusic ? &m_music : nullptr;
}

size_t Scene::GetCharacterCount() const {
    return m_characters.size();
}

Scene::Character* Scene::GetCharacter(size_t index) {
    return index < m_characters.size() ? &m_characters[index] : nullptr;
}

const std::vector<Scene::Character>& Scene::GetCharacters() const {
    return m_characters;
}

void Scene::Render(sf::RenderTarget& target) const {
    if (m_hasBackground && m_backgroundSprite)
        target.draw(*m_backgroundSprite);

    for (const auto& character : m_characters)
        if (character.sprite)
            target.draw(*character.sprite);
}
