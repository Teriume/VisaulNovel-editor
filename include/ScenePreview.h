#pragma once
#include <SFML/Graphics.hpp>
#include "Scene.h"

class ScenePreview {
public:
    bool EnsureSize(const sf::Vector2u& size);
    bool RenderScene(Scene& scene, const sf::Vector2u& size);
    const sf::Texture& GetTexture() const;

private:
    sf::RenderTexture m_sceneRenderTexture;
};
