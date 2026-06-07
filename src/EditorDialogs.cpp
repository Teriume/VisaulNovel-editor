#include "EditorDialogs.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include "tinyfiledialogs.h"
#include <cstring>
#include <memory>
#include <SFML/Graphics.hpp>

namespace EditorDialogs {

void DrawSceneDialog(bool& showNewSceneWindow, SceneManager& sceneManager) {
    static char sceneName[128] = "";

    if (!showNewSceneWindow)
        return;

    ImGui::Begin("Создать сцену", &showNewSceneWindow, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::InputText("Название сцены", sceneName, sizeof(sceneName));
    if (ImGui::Button("Создать")) {
        if (std::strlen(sceneName) > 0) {
            sceneManager.AddScene(sceneName);
            sceneName[0] = '\0';
            showNewSceneWindow = false;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Отмена")) {
        showNewSceneWindow = false;
        sceneName[0] = '\0';
    }
    ImGui::End();
}

void DrawPresetDialog(bool& showNewPresetWindow, CharacterPresetManager& presetManager) {
    static char presetName[128] = "";
    static char presetNotes[256] = "";
    static char presetImagePath[256] = "";
    static int presetEmotion = 0;
    static sf::Texture presetPreviewTexture;
    static bool previewLoaded = false;

    if (!showNewPresetWindow)
        return;

    ImGui::SetNextWindowSizeConstraints(ImVec2(300, -1), ImVec2(500, -1));
    ImGui::Begin("Создать пресет персонажа", &showNewPresetWindow, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::InputText("Имя пресета", presetName, sizeof(presetName));
    const char* emotions[] = {"Нормальное", "Счастливое", "Грустное", "Злое", "Удивленное"};
    ImGui::Combo("Эмоция", &presetEmotion, emotions, IM_ARRAYSIZE(emotions));
    ImGui::InputTextMultiline("Описание", presetNotes, sizeof(presetNotes), ImVec2(-1, 80));

    if (ImGui::Button("Выбрать файл изображения")) {
        const char* lFilterPatterns[3] = { "*.png", "*.jpg", "*.jpeg" };
        const char* lSelectedPath = tinyfd_openFileDialog(
            "Выберите изображение персонажа",
            "",
            3,
            lFilterPatterns,
            "Изображения (png, jpg)",
            0
        );
        if (lSelectedPath) {
            std::strncpy(presetImagePath, lSelectedPath, sizeof(presetImagePath) - 1);
            presetImagePath[sizeof(presetImagePath) - 1] = '\0';
            previewLoaded = presetPreviewTexture.loadFromFile(presetImagePath);
        }
    }

    ImGui::TextWrapped("Файл: %s", presetImagePath[0] ? presetImagePath : "не выбран");
    if (previewLoaded) {
        ImGui::Image(presetPreviewTexture, sf::Vector2f(128.f, 128.f));
    }

    if (ImGui::Button("Сохранить пресет")) {
        if (presetImagePath[0] != '\0' && presetName[0] != '\0') {
            auto previewTexture = std::make_unique<sf::Texture>();
            if (previewTexture->loadFromFile(presetImagePath)) {
                presetManager.AddPreset(presetName, emotions[presetEmotion], presetNotes, presetImagePath, std::move(previewTexture));
            } else {
                presetManager.AddPreset(presetName, emotions[presetEmotion], presetNotes, presetImagePath, nullptr);
            }
            presetName[0] = '\0';
            presetNotes[0] = '\0';
            presetImagePath[0] = '\0';
            presetEmotion = 0;
            previewLoaded = false;
            showNewPresetWindow = false;
        } else {
            tinyfd_messageBox("Ошибка", "Введите имя пресета и выберите изображение.", "ok", "error", 1);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Отмена")) {
        showNewPresetWindow = false;
        previewLoaded = false;
        presetEmotion = 0;
    }
    ImGui::End();
}

} // namespace EditorDialogs
