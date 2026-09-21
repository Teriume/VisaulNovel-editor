#pragma once
#include <SFML/Graphics.hpp>
#include "SceneManager.h"
#include "CharacterPresetManager.h"
#include "ScenePreview.h"
#include "BackgroundPresetManager.h"

namespace EditorPanels {
    void DrawMainPanel(SceneManager& sceneManager, CharacterPresetManager& characterPresetManager, bool& showNewSceneWindow, char* projectPath, size_t projectPathSize);
    void DrawPresetPanel(SceneManager& sceneManager, CharacterPresetManager& presetManager, bool& showNewPresetWindow, sf::RenderWindow* window);
    void DrawBackgroundPresetPanel(SceneManager& sceneManager, BackgroundPresetManager& presetManager, sf::RenderWindow* window, const char* projectPath);
    void DrawInspectorPanel(SceneManager& sceneManager, CharacterPresetManager& presetManager, ScenePreview& preview, sf::RenderWindow* window, int& editTarget, bool& showPlayMode);
    void DrawPlayPanel(SceneManager& sceneManager, ScenePreview& preview, sf::RenderWindow* window, bool& showPlayMode, bool& showNovelMenu);
}
