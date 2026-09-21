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

void Scene::CenterBackground(float sceneWidth, float sceneHeight) {
    if (!m_hasBackground || !m_backgroundSprite)
        return;
    const sf::FloatRect bounds = m_backgroundSprite->getLocalBounds();
    m_backgroundSprite->setPosition({
        (sceneWidth - bounds.size.x * m_backgroundSprite->getScale().x) * 0.5f,
        (sceneHeight - bounds.size.y * m_backgroundSprite->getScale().y) * 0.5f
    });
}

sf::Vector2f Scene::GetBackgroundScale() const {
    return m_backgroundSprite ? m_backgroundSprite->getScale() : sf::Vector2f(1.f, 1.f);
}

sf::Vector2f Scene::GetBackgroundPosition() const {
    return m_backgroundSprite ? m_backgroundSprite->getPosition() : sf::Vector2f(0.f, 0.f);
}

bool Scene::AddCharacter(const char* imagePath, const char* name, const char* emotion, const char* notes) {
    auto newTexture = std::make_shared<sf::Texture>();
    if (!imagePath || !newTexture->loadFromFile(imagePath)) {
        return false;
    }

    Character character;
    character.name = name ? name : "Персонаж";
    character.emotion = emotion ? emotion : "Нормальное";
    character.notes = notes ? notes : "";
    //character.imagePath = imagePath ? imagePath : "";
    character.texture = newTexture;
    character.sprite = std::make_unique<sf::Sprite>(*character.texture);
    character.sprite->setPosition(sf::Vector2f(100.f, 100.f));
    character.sprite->setScale(sf::Vector2f(1.f, 1.f));

    // Используйте имя вектора персонажей из вашего Scene.h (m_characters или characters)
    m_characters.push_back(std::move(character));
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

std::vector<Scene::Choice>& Scene::GetChoices() {
    return m_choices;
}

const std::vector<Scene::Choice>& Scene::GetChoices() const {
    return m_choices;
}

void Scene::AddChoice(const std::string& text, const std::string& targetScene) {
    m_choices.push_back({text, targetScene});
}

void Scene::RemoveChoice(size_t index) {
    if (index < m_choices.size())
        m_choices.erase(m_choices.begin() + static_cast<std::ptrdiff_t>(index));
}

const std::string& Scene::GetSpeakerName() const {
    return m_speakerName;
}

const std::string& Scene::GetDialogueText() const {
    return m_dialogueText;
}

void Scene::SetSpeakerName(const std::string& name) {
    m_speakerName = name;
}

void Scene::SetDialogueText(const std::string& text) {
    m_dialogueText = text;
}

void Scene::Render(sf::RenderTarget& target) const {
    if (m_hasBackground && m_backgroundSprite)
        target.draw(*m_backgroundSprite);

    for (const auto& character : m_characters)
        if (character.sprite)
            target.draw(*character.sprite);
}
