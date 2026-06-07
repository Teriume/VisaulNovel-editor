#pragma once
#include <SFML/Graphics.hpp>
#include "SceneManager.h"
#include "CharacterPresetManager.h"
#include "ScenePreview.h"

namespace EditorPanels {
    void DrawMainPanel(SceneManager& sceneManager, bool& showNewSceneWindow);
    void DrawPresetPanel(SceneManager& sceneManager, CharacterPresetManager& presetManager, bool& showNewPresetWindow, sf::RenderWindow* window);
    void DrawInspectorPanel(SceneManager& sceneManager, CharacterPresetManager& presetManager, ScenePreview& preview, sf::RenderWindow* window, int& editTarget);
}
