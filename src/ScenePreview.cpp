#include "ScenePreview.h"

bool ScenePreview::EnsureSize(const sf::Vector2u& size) {
    if (size.x == 0 || size.y == 0)
        return false;

    if (m_sceneRenderTexture.getSize() == size)
        return true;

    m_sceneRenderTexture = sf::RenderTexture(size);
    return m_sceneRenderTexture.getSize() == size;
}

bool ScenePreview::RenderScene(Scene& scene, const sf::Vector2u& size) {
    if (!EnsureSize(size))
        return false;

    m_sceneRenderTexture.clear(sf::Color(40, 40, 40));
    scene.Render(m_sceneRenderTexture);
    m_sceneRenderTexture.display();
    return true;
}

const sf::Texture& ScenePreview::GetTexture() const {
    return m_sceneRenderTexture.getTexture();
}
