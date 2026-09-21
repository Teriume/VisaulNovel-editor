#include "EditorDialogs.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include "tinyfiledialogs.h"
#include <cstring>
#include <memory>
#include <unordered_map>
#include <vector>
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
    
    // Динамический список создаваемых эмоций
    static std::unordered_map<std::string, std::string> emotionsMap;
    static std::string defaultEmotion = "Нормальное";

    // Поля ввода для добавления новой эмоции
    static char newEmotionName[64] = "Нормальное";
    static char newEmotionPath[256] = "";

    if (!showNewPresetWindow)
        return;

    ImGui::SetNextWindowSizeConstraints(ImVec2(350, -1), ImVec2(550, -1));
    ImGui::Begin("Создать пресет персонажа", &showNewPresetWindow, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::InputText("Имя пресета", presetName, sizeof(presetName));
    ImGui::InputTextMultiline("Описание", presetNotes, sizeof(presetNotes), ImVec2(-1, 60));

    ImGui::SeparatorText("Добавление эмоций / спрайтов");

    ImGui::InputText("Название эмоции", newEmotionName, sizeof(newEmotionName));
    ImGui::TextWrapped("Файл: %s", newEmotionPath[0] ? newEmotionPath : "не выбран");

    if (ImGui::Button("Выбрать файл для эмоции")) {
        const char* lFilterPatterns[3] = { "*.png", "*.jpg", "*.jpeg" };
        const char* lSelectedPath = tinyfd_openFileDialog(
            "Выберите изображение эмоции",
            "",
            3,
            lFilterPatterns,
            "Изображения (png, jpg)",
            0
        );
        if (lSelectedPath) {
            std::strncpy(newEmotionPath, lSelectedPath, sizeof(newEmotionPath) - 1);
            newEmotionPath[sizeof(newEmotionPath) - 1] = '\0';
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Добавить эмоцию")) {
        if (newEmotionName[0] != '\0' && newEmotionPath[0] != '\0') {
            emotionsMap[newEmotionName] = newEmotionPath;
            if (emotionsMap.size() == 1) {
                defaultEmotion = newEmotionName;
            }
            newEmotionName[0] = '\0';
            newEmotionPath[0] = '\0';
        } else {
            tinyfd_messageBox("Ошибка", "Введите название эмоции и выберите файл изображения.", "ok", "error", 1);
        }
    }

    ImGui::Spacing();
    ImGui::Text("Список добавленных эмоций (%zu):", emotionsMap.size());

    std::string emotionToDelete = "";
    ImGui::BeginChild("EmotionsListChild", ImVec2(0, 120), true);
    for (const auto& [emoName, emoPath] : emotionsMap) {
        ImGui::PushID(emoName.c_str());
        ImGui::Text("%s -> %s", emoName.c_str(), emoPath.c_str());
        ImGui::SameLine();
        if (ImGui::Button("X")) {
            emotionToDelete = emoName;
        }
        ImGui::PopID();
    }
    ImGui::EndChild();

    if (!emotionToDelete.empty()) {
        emotionsMap.erase(emotionToDelete);
        if (defaultEmotion == emotionToDelete) {
            defaultEmotion = emotionsMap.empty() ? "" : emotionsMap.begin()->first;
        }
    }

    if (!emotionsMap.empty()) {
        std::vector<const char*> emotionNames;
        int currentDefaultIdx = 0;
        int idx = 0;
        for (const auto& [emoName, _] : emotionsMap) {
            emotionNames.push_back(emoName.c_str());
            if (emoName == defaultEmotion) currentDefaultIdx = idx;
            idx++;
        }
        if (ImGui::Combo("Эмоция по умолчанию", &currentDefaultIdx, emotionNames.data(), static_cast<int>(emotionNames.size()))) {
            defaultEmotion = emotionNames[currentDefaultIdx];
        }
    }

    ImGui::Separator();

  if (ImGui::Button("Сохранить пресет")) {
        if (presetName[0] != '\0' && !emotionsMap.empty()) {
            auto previewTexture = std::make_unique<sf::Texture>();
            std::string previewPath = emotionsMap[defaultEmotion];
            if (!previewPath.empty() && previewTexture->loadFromFile(previewPath)) {
                presetManager.AddPreset(presetName, emotionsMap, defaultEmotion, presetNotes, std::move(previewTexture));
            } else {
                presetManager.AddPreset(presetName, emotionsMap, defaultEmotion, presetNotes, nullptr);
            }

            // Вызываем системный диалог выборa файла для сохранения
            const char* filterPatterns[1] = { "*.charpresets" };
            const char* savePath = tinyfd_saveFileDialog(
                "Сохранить пресеты персонажей",
                "default_characters.charpresets",
                1,
                filterPatterns,
                "Файлы пресетов (*.charpresets)"
            );

            if (savePath) {
                if (presetManager.SaveToFile(savePath)) {
                    tinyfd_messageBox("Успех", "Файл пресета успешно сохранен на диск!", "ok", "info", 1);
                } else {
                    tinyfd_messageBox("Ошибка", "Не удалось записать файл на диск. Проверьте права доступа.", "ok", "error", 1);
                }
            }

            // Очистка состояния окна
            presetName[0] = '\0';
            presetNotes[0] = '\0';
            newEmotionName[0] = '\0';
            newEmotionPath[0] = '\0';
            emotionsMap.clear();
            defaultEmotion = "Нормальное";
            showNewPresetWindow = false;
        } else {
            tinyfd_messageBox("Ошибка", "Введите имя пресета и добавьте хотя бы одну эмоцию со спрайтом.", "ok", "error", 1);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Отмена")) {
        showNewPresetWindow = false;
        presetName[0] = '\0';
        presetNotes[0] = '\0';
        newEmotionName[0] = '\0';
        newEmotionPath[0] = '\0';
        emotionsMap.clear();
        defaultEmotion = "Нормальное";
    }

    ImGui::End();
}

} // namespace EditorDialogs