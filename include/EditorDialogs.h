#pragma once
#include "SceneManager.h"
#include "CharacterPresetManager.h"

namespace EditorDialogs {
    void DrawSceneDialog(bool& showNewSceneWindow, SceneManager& sceneManager);
    void DrawPresetDialog(bool& showNewPresetWindow, CharacterPresetManager& presetManager);
}
