#pragma once
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class Scene {
    public:
        struct Character {
            std::string name;
            std::string emotion = "Нормальное";
            std::string notes;
            sf::Texture texture;
            std::unique_ptr<sf::Sprite> sprite;
        };
        
        const char* GetMusicPath() const {
            return m_musicPath.c_str();
        }
        void SetMusicPath(const char* path) {
            m_musicPath = path;
        }
        
        float GetMusicVolume() const {
            return m_musicVolume;
        }
        void SetMusicVolume(float volume) {
            m_musicVolume = volume;
            if (m_hasMusic)
                m_music.setVolume(volume);
        }
        bool IsMusicLoop() const {
            return m_musicLoop;
        }
        void SetMusicLoop(bool loop) {
            m_musicLoop = loop;
            if (m_hasMusic)
                m_music.setLooping(loop);
        }
        bool IsMusicEnabled() const {
            return m_musicEnabled;
        }
        void SetMusicEnabled(bool enabled) {
            m_musicEnabled = enabled;
            if (m_hasMusic) {
                if (enabled)
                    m_music.play();
                else
                    m_music.pause();
            }
        }
    private:
        char m_name[128] = "New Scene";
        sf::Texture m_backgroundTexture;
        std::unique_ptr<sf::Sprite> m_backgroundSprite;
        bool m_hasBackground = false;
        std::string m_backgroundPath;
        std::vector<Character> m_characters;
        sf::Music m_music;
        bool m_hasMusic = false;

    public:
        Scene() = default;
        explicit Scene(const char* name);

        bool LoadBackground(const char* path);
        sf::Sprite* GetBackgroundSprite();
        const sf::Sprite* GetBackgroundSprite() const;
        const std::string& GetBackgroundPath() const;
        void SetBackgroundScale(const sf::Vector2f& scale);
        void SetBackgroundPosition(const sf::Vector2f& position);
        sf::Vector2f GetBackgroundScale() const;
        sf::Vector2f GetBackgroundPosition() const;

        bool AddCharacter(const char* path, const char* name = "Персонаж", const char* emotion = "Нормальное", const char* notes = "");
        bool LoadMusic(const char* path);
        sf::Music* GetMusic();
        const sf::Music* GetMusic() const;
        size_t GetCharacterCount() const;
        Character* GetCharacter(size_t index);
        const std::vector<Character>& GetCharacters() const;

        void Render(sf::RenderTarget& target) const;

        const char* GetName() const {
            return m_name;
        }

        void SetName(const char* name);

        std::string m_musicPath = ""; // Путь к музыке сцены (может быть пустым, если музыка не задана)
        float m_musicVolume = 100.f; // Громкость музыки сцены (0-100)
        bool m_musicLoop = true; // Флаг, указывающий, должна ли музыка сцены зацикливаться
        bool m_musicEnabled = false; // Флаг, указывающий, включена ли музыка для этой сцены
};