#include "SceneManager.h"
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <memory>

namespace {
std::string Escape(const std::string& value) {
    std::string result;
    for (char character : value) {
        if (character == '\\') result += "\\\\";
        else if (character == '\n') result += "\\n";
        else result += character;
    }
    return result;
}

std::string Unescape(const std::string& value) {
    std::string result;
    bool escaped = false;
    for (char character : value) {
        if (escaped) {
            result += character == 'n' ? '\n' : character;
            escaped = false;
        } else if (character == '\\') {
            escaped = true;
        } else {
            result += character;
        }
    }
    return result;
}
}

bool SceneManager::AddScene(const std::string& name) {
    if (name.empty())
        return false;

    m_scenes.push_back(std::make_unique<Scene>(name.c_str()));
    m_selectedSceneIndex = static_cast<int>(m_scenes.size()) - 1;
    m_selectedCharacterIndex = -1;
    return true;
}

void SceneManager::RemoveSelectedScene() {
    if (m_selectedSceneIndex < 0 || m_selectedSceneIndex >= static_cast<int>(m_scenes.size()))
        return;

    m_scenes.erase(m_scenes.begin() + m_selectedSceneIndex);
    if (m_scenes.empty()) {
        m_selectedSceneIndex = -1;
    } else {
        m_selectedSceneIndex = std::min(m_selectedSceneIndex, static_cast<int>(m_scenes.size()) - 1);
    }
    m_selectedCharacterIndex = -1;
}

void SceneManager::MoveSelectedSceneUp() {
    if (m_selectedSceneIndex <= 0 || m_selectedSceneIndex >= static_cast<int>(m_scenes.size()))
        return;
    std::swap(m_scenes[m_selectedSceneIndex], m_scenes[m_selectedSceneIndex - 1]);
    --m_selectedSceneIndex;
}

void SceneManager::MoveSelectedSceneDown() {
    if (m_selectedSceneIndex < 0 || m_selectedSceneIndex + 1 >= static_cast<int>(m_scenes.size()))
        return;
    std::swap(m_scenes[m_selectedSceneIndex], m_scenes[m_selectedSceneIndex + 1]);
    ++m_selectedSceneIndex;
}

void SceneManager::ResetSelection() {
    m_selectedSceneIndex = -1;
    m_selectedCharacterIndex = -1;
}

Scene* SceneManager::GetSelectedScene() {
    return (m_selectedSceneIndex >= 0 && m_selectedSceneIndex < static_cast<int>(m_scenes.size()))
        ? m_scenes[m_selectedSceneIndex].get()
        : nullptr;
}

const Scene* SceneManager::GetSelectedScene() const {
    return (m_selectedSceneIndex >= 0 && m_selectedSceneIndex < static_cast<int>(m_scenes.size()))
        ? m_scenes[m_selectedSceneIndex].get()
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

const std::vector<std::unique_ptr<Scene>>& SceneManager::GetScenes() const {
    return m_scenes;
}

size_t SceneManager::GetSceneCount() const {
    return m_scenes.size();
}

Scene* SceneManager::GetScene(const std::string& name) {
    for (auto& scene : m_scenes)
        if (name == scene->GetName()) return scene.get();
    return nullptr;
}

const Scene* SceneManager::GetScene(const std::string& name) const {
    for (const auto& scene : m_scenes)
        if (name == scene->GetName()) return scene.get();
    return nullptr;
}

bool SceneManager::SelectSceneByName(const std::string& name) {
    for (int index = 0; index < static_cast<int>(m_scenes.size()); ++index) {
        if (name == m_scenes[index]->GetName()) {
            SelectScene(index);
            return true;
        }
    }
    return false;
}

bool SceneManager::SaveToFile(const std::string& path) const {
    std::ofstream file(path, std::ios::binary);
    if (!file) return false;

    const std::filesystem::path projectDirectory = std::filesystem::absolute(path).parent_path();
    const auto relativePath = [&projectDirectory](const std::string& resourcePath) {
        if (resourcePath.empty()) return std::string();
        std::error_code error;
        const auto relative = std::filesystem::relative(std::filesystem::absolute(resourcePath), projectDirectory, error);
        return error ? resourcePath : relative.generic_string();
    };

    file << "NANA_NOVEL_1\n" << m_scenes.size() << '\n';
    for (const auto& scenePtr : m_scenes) {
        const Scene& scene = *scenePtr;
        file << "SCENE\n" << std::quoted(scene.GetName()) << '\n';
        file << "BACKGROUND " << std::quoted(relativePath(scene.GetBackgroundPath())) << ' '
             << scene.GetBackgroundPosition().x << ' ' << scene.GetBackgroundPosition().y << ' '
             << scene.GetBackgroundScale().x << ' ' << scene.GetBackgroundScale().y << '\n';
        file << "DIALOGUE " << std::quoted(scene.GetSpeakerName()) << ' '
             << std::quoted(Escape(scene.GetDialogueText())) << '\n';
        file << "CHARACTERS " << scene.GetCharacterCount() << '\n';
        for (const Scene::Character& character : scene.GetCharacters()) {
            const sf::Vector2f position = character.sprite ? character.sprite->getPosition() : sf::Vector2f();
            const sf::Vector2f scale = character.sprite ? character.sprite->getScale() : sf::Vector2f(1.f, 1.f);
            file << std::quoted(relativePath(character.imagePath)) << ' ' << std::quoted(character.name) << ' '
                 << std::quoted(character.emotion) << ' ' << std::quoted(Escape(character.notes)) << ' '
                 << position.x << ' ' << position.y << ' ' << scale.x << ' ' << scale.y << '\n';
        }
        file << "CHOICES " << scene.GetChoices().size() << '\n';
        for (const Scene::Choice& choice : scene.GetChoices())
            file << std::quoted(choice.text) << ' ' << std::quoted(choice.targetScene) << '\n';
    }
    return true;
}

bool SceneManager::LoadFromFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    std::string header;
    size_t sceneCount = 0;
    if (!file || !std::getline(file, header) || header != "NANA_NOVEL_1" || !(file >> sceneCount))
        return false;
    std::string line;
    std::getline(file, line);
    std::vector<std::unique_ptr<Scene>> loadedScenes;
    m_projectDirectory = std::filesystem::absolute(path).parent_path();
    for (size_t i = 0; i < sceneCount; ++i) {
        if (!(file >> line) || line != "SCENE") return false;
        std::string sceneName;
        file >> std::quoted(sceneName);
        loadedScenes.push_back(std::make_unique<Scene>(sceneName.c_str()));
        Scene& scene = *loadedScenes.back();

        std::string backgroundPath;
        sf::Vector2f backgroundPosition, backgroundScale;
        file >> line >> std::quoted(backgroundPath) >> backgroundPosition.x >> backgroundPosition.y
             >> backgroundScale.x >> backgroundScale.y;
        const std::filesystem::path resolvedBackground = m_projectDirectory / backgroundPath;
        if (!backgroundPath.empty() && scene.LoadBackground(resolvedBackground.string().c_str())) {
            scene.SetBackgroundPosition(backgroundPosition);
            scene.SetBackgroundScale(backgroundScale);
        }

        std::string speaker, dialogue;
        file >> line >> std::quoted(speaker) >> std::quoted(dialogue);
        scene.SetSpeakerName(speaker);
        scene.SetDialogueText(Unescape(dialogue));

        size_t characterCount = 0;
        file >> line >> characterCount;
        for (size_t characterIndex = 0; characterIndex < characterCount; ++characterIndex) {
            std::string imagePath, name, emotion, notes;
            sf::Vector2f position, scale;
            file >> std::quoted(imagePath) >> std::quoted(name) >> std::quoted(emotion) >> std::quoted(notes)
                 >> position.x >> position.y >> scale.x >> scale.y;
            const std::filesystem::path resolvedImage = m_projectDirectory / imagePath;
            if (scene.AddCharacter(resolvedImage.string().c_str(), name.c_str(), emotion.c_str(), Unescape(notes).c_str())) {
                Scene::Character* character = scene.GetCharacter(scene.GetCharacterCount() - 1);
                character->sprite->setPosition(position);
                character->sprite->setScale(scale);
            }
        }

        size_t choiceCount = 0;
        file >> line >> choiceCount;
        for (size_t choiceIndex = 0; choiceIndex < choiceCount; ++choiceIndex) {
            std::string text, target;
            file >> std::quoted(text) >> std::quoted(target);
            scene.AddChoice(text, target);
        }
    }
    m_scenes = std::move(loadedScenes);
    m_selectedSceneIndex = m_scenes.empty() ? -1 : 0;
    m_selectedCharacterIndex = -1;
    return true;
}

void SceneManager::SetProjectDirectory(const std::filesystem::path& directory) {
    m_projectDirectory = directory;
}

bool SceneManager::SaveGameState(const std::string& path) const {
    const Scene* scene = GetSelectedScene();
    if (!scene) return false;
    std::ofstream file(path, std::ios::binary);
    if (!file) return false;
    file << "NANA_SAVE_1\n" << std::quoted(scene->GetName()) << '\n';
    return true;
}

bool SceneManager::LoadGameState(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    std::string header, sceneName;
    if (!file || !std::getline(file, header) || header != "NANA_SAVE_1" || !(file >> std::quoted(sceneName)))
        return false;
    return SelectSceneByName(sceneName);
}