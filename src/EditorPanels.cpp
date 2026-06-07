#include "EditorPanels.h"
#include "tinyfiledialogs.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include <cstring>
#include <algorithm>

namespace EditorPanels {

void DrawMainPanel(SceneManager& sceneManager, bool& showNewSceneWindow) {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
    ImGui::Begin("Главное", nullptr, flags);
    ImGui::SetWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(300, 150), ImGuiCond_Always);

    if (ImGui::Button("Создать новую сцену", ImVec2(-1, 40))) {
        showNewSceneWindow = true;
    }

    ImGui::Separator();

    const auto& scenes = sceneManager.GetScenes();
    for (int i = 0; i < static_cast<int>(scenes.size()); ++i) {
        const bool isSelected = (i == sceneManager.GetSelectedSceneIndex());
        if (ImGui::Selectable(scenes[i].GetName(), isSelected)) {
            sceneManager.SelectScene(i);
        }
    }

    ImGui::Text("Список сцен:");
    for (const auto& scene : scenes) {
        ImGui::BulletText("%s", scene.GetName());
    }

    ImGui::End();
}

void DrawPresetPanel(SceneManager& sceneManager, CharacterPresetManager& presetManager, bool& showNewPresetWindow, sf::RenderWindow* window) {
    if (!window)
        return;

    ImGui::Begin("Пресеты персонажей", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    ImGui::SetWindowPos(ImVec2(10, 170), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(300, window->getSize().y - 180), ImGuiCond_Always);

    ImGui::Text("Пресеты персонажей");
    if (ImGui::Button("Создать пресет персонажа", ImVec2(-1, 0))) {
        showNewPresetWindow = true;
    }

    if (presetManager.HasPresets()) {
        std::vector<const char*> presetNames;
        presetNames.reserve(presetManager.GetPresets().size());
        for (const auto& preset : presetManager.GetPresets())
            presetNames.push_back(preset.name.c_str());

        int selectedPresetIndex = presetManager.GetSelectedPresetIndex();
        if (selectedPresetIndex < 0 || selectedPresetIndex >= static_cast<int>(presetNames.size()))
            selectedPresetIndex = 0;

        if (ImGui::Combo("Выбрать пресет", &selectedPresetIndex, presetNames.data(), static_cast<int>(presetNames.size()))) {
            presetManager.SelectPreset(selectedPresetIndex);
        }

        if (const CharacterPreset* preset = presetManager.GetSelectedPreset()) {
            ImGui::TextWrapped("Файл: %s", preset->imagePath.empty() ? "(не задан)" : preset->imagePath.c_str());
            ImGui::Text("Эмоция: %s", preset->emotion.c_str());
            ImGui::TextWrapped("Описание: %s", preset->notes.c_str());
            if (preset->previewTexture) {
                ImGui::Image(*preset->previewTexture, sf::Vector2f(128.f, 128.f));
            }

            if (sceneManager.GetSelectedScene()) {
                if (ImGui::Button("Добавить персонажа из пресета", ImVec2(-1, 0))) {
                    Scene* currentScene = sceneManager.GetSelectedScene();
                    if (currentScene && !preset->imagePath.empty()) {
                        if (currentScene->AddCharacter(preset->imagePath.c_str(), preset->name.c_str(), preset->emotion.c_str(), preset->notes.c_str())) {
                            sceneManager.SelectCharacter(static_cast<int>(currentScene->GetCharacterCount()) - 1);
                        } else {
                            tinyfd_messageBox("Ошибка", "Не удалось загрузить изображение из пресета", "ok", "error", 1);
                        }
                    }
                }
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "Сначала выберите сцену, чтобы добавить персонажа.");
            }
        }
    } else {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Пока нет пресетов. Создайте новый пресет.");
    }

    ImGui::End();
}

void DrawInspectorPanel(SceneManager& sceneManager, CharacterPresetManager& presetManager, ScenePreview& preview, sf::RenderWindow* window, int& editTarget) {
    ImGui::Begin("Анализ Сцены", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    if (window) {
        ImGui::SetWindowPos(ImVec2(310, 0), ImGuiCond_Always);
        ImGui::SetWindowSize(ImVec2(window->getSize().x - 250, window->getSize().y), ImGuiCond_Always);
    }

    Scene* currentScene = sceneManager.GetSelectedScene();
    if (!currentScene) {
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Выберите сцену в списке слева, чтобы начать работу.");
        ImGui::End();
        return;
    }

    ImGui::Text("Редактирование: %s", currentScene->GetName());

    ImGui::SameLine();
    if (ImGui::RadioButton("Персонаж", &editTarget, 1)) {}
    ImGui::Separator();

    ImVec2 availableSize = ImGui::GetContentRegionAvail();
    const float previewHeight = std::clamp(availableSize.y * 0.35f, 240.0f, 420.0f);
    ImVec2 previewSize(availableSize.x, previewHeight);
    if (previewSize.x < 200.0f) previewSize.x = 200.0f;
    if (previewSize.y < 180.0f) previewSize.y = 180.0f;

    if (preview.RenderScene(*currentScene, sf::Vector2u(static_cast<unsigned>(previewSize.x), static_cast<unsigned>(previewSize.y)))) {
        ImGui::BeginChild("ScenePreviewChild", previewSize, true, ImGuiChildFlags_AlwaysUseWindowPadding);
        ImGui::Image(preview.GetTexture(), sf::Vector2f(previewSize.x, previewSize.y));

        bool previewHovered = ImGui::IsItemHovered();
        if (previewHovered) {
            ImGui::Text("ЛКМ: передвинуть %s", editTarget == 1 ? "персонажа" : "фон");
            ImGui::Text("Колесо: масштабировать %s", editTarget == 1 ? "персонажа" : "фон");
        }

        if (previewHovered && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            ImVec2 dragDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
            if (dragDelta.x != 0.0f || dragDelta.y != 0.0f) {
                sf::Vector2f delta(static_cast<float>(dragDelta.x), static_cast<float>(dragDelta.y));
                if (editTarget == 1) {
                    if (Scene::Character* character = currentScene->GetCharacter(sceneManager.GetSelectedCharacterIndex()); character && character->sprite) {
                        character->sprite->move(delta);
                    }
                } else if (auto* background = currentScene->GetBackgroundSprite()) {
                    currentScene->SetBackgroundPosition(currentScene->GetBackgroundPosition() + delta);
                }
                ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
            }
        }

        const float wheel = ImGui::GetIO().MouseWheel;
        if (previewHovered && wheel != 0.0f) {
            const float factor = 1.0f + wheel * 0.10f;
            if (editTarget == 1) {
                if (Scene::Character* character = currentScene->GetCharacter(sceneManager.GetSelectedCharacterIndex()); character && character->sprite) {
                    sf::Vector2f scale = character->sprite->getScale();
                    scale *= factor;
                    scale.x = std::clamp(scale.x, 0.05f, 10.0f);
                    scale.y = std::clamp(scale.y, 0.05f, 10.0f);
                    character->sprite->setScale(scale);
                }
            } else if (currentScene->GetBackgroundSprite()) {
                sf::Vector2f scale = currentScene->GetBackgroundScale();
                scale *= factor;
                scale.x = std::clamp(scale.x, 0.05f, 10.0f);
                scale.y = std::clamp(scale.y, 0.05f, 10.0f);
                currentScene->SetBackgroundScale(scale);
            }
        }

        ImGui::EndChild();
    }

    ImGui::SeparatorText("Общие настройки");
    char buf[128];
    std::strncpy(buf, currentScene->GetName(), sizeof(buf));
    if (ImGui::InputText("Название сцены", buf, sizeof(buf))) {
        currentScene->SetName(buf);
    }

    ImGui::Spacing();
    ImGui::SeparatorText("Персонажи на сцене");
    if (ImGui::Button("Добавить персонажа", ImVec2(-1, 0))) {
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
            if (currentScene->AddCharacter(lSelectedPath, "Персонаж")) {
                sceneManager.SelectCharacter(static_cast<int>(currentScene->GetCharacterCount()) - 1);
            } else {
                tinyfd_messageBox("Ошибка", "Не удалось загрузить изображение персонажа", "ok", "error", 1);
            }
        }
    }

    if (presetManager.HasPresets()) {
        std::vector<const char*> presetNames;
        presetNames.reserve(presetManager.GetPresets().size());
        for (const auto& preset : presetManager.GetPresets())
            presetNames.push_back(preset.name.c_str());

        int selectedPresetIndex = presetManager.GetSelectedPresetIndex();
        if (selectedPresetIndex < 0 || selectedPresetIndex >= static_cast<int>(presetNames.size()))
            selectedPresetIndex = 0;

        if (ImGui::Combo("Пресет персонажа", &selectedPresetIndex, presetNames.data(), static_cast<int>(presetNames.size()))) {
            presetManager.SelectPreset(selectedPresetIndex);
        }
        if (ImGui::Button("Добавить персонажа из пресета")) {
            const CharacterPreset* preset = presetManager.GetSelectedPreset();
            if (preset && currentScene) {
                if (currentScene->AddCharacter(preset->imagePath.c_str(), preset->name.c_str(), preset->emotion.c_str(), preset->notes.c_str())) {
                    sceneManager.SelectCharacter(static_cast<int>(currentScene->GetCharacterCount()) - 1);
                } else {
                    tinyfd_messageBox("Ошибка", "Не удалось загрузить изображение из пресета", "ok", "error", 1);
                }
            }
        }
    } else {
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Нет пресетов. Создайте их в главном меню.");
    }

    const auto& characters = currentScene->GetCharacters();
    for (int i = 0; i < static_cast<int>(characters.size()); ++i) {
        const bool isSelected = (i == sceneManager.GetSelectedCharacterIndex());
        if (ImGui::Selectable(characters[i].name.c_str(), isSelected)) {
            sceneManager.SelectCharacter(i);
        }
    }

    if (sceneManager.GetSelectedCharacterIndex() >= 0 && sceneManager.GetSelectedCharacterIndex() < static_cast<int>(characters.size())) {
        Scene::Character* character = currentScene->GetCharacter(sceneManager.GetSelectedCharacterIndex());
        if (character && character->sprite) {
            ImGui::SeparatorText("Параметры персонажа");
            char nameBuf[128];
            std::strncpy(nameBuf, character->name.c_str(), sizeof(nameBuf));
            if (ImGui::InputText("Имя персонажа", nameBuf, sizeof(nameBuf))) {
                character->name = nameBuf;
            }

            const char* emotions[] = {"Нормальное", "Счастливое", "Грустное", "Злое", "Удивленное"};
            int emotionIndex = 0;
            for (int j = 0; j < IM_ARRAYSIZE(emotions); ++j) {
                if (character->emotion == emotions[j]) {
                    emotionIndex = j;
                    break;
                }
            }
            if (ImGui::Combo("Эмоция", &emotionIndex, emotions, IM_ARRAYSIZE(emotions))) {
                character->emotion = emotions[emotionIndex];
            }

            char notesBuf[256];
            std::strncpy(notesBuf, character->notes.c_str(), sizeof(notesBuf));
            if (ImGui::InputTextMultiline("Описание", notesBuf, sizeof(notesBuf), ImVec2(-1, 80))) {
                character->notes = notesBuf;
            }

            sf::Vector2f charPosition = character->sprite->getPosition();
            if (ImGui::DragFloat2("Позиция персонажа", &charPosition.x, 1.0f)) {
                character->sprite->setPosition(charPosition);
            }
            sf::Vector2f charScale = character->sprite->getScale();
            if (ImGui::DragFloat2("Масштаб персонажа", &charScale.x, 0.01f, 0.05f, 10.0f)) {
                character->sprite->setScale(charScale);
            }
        }
    }

    ImGui::SeparatorText("Фон сцены");
    if (auto* background = currentScene->GetBackgroundSprite()) {
        const sf::Vector2f bgScale = background->getScale();
        const sf::Vector2f bgPosition = background->getPosition();
        const auto& texture = background->getTexture();
        ImGui::Text("Фон загружен: %ux%u", static_cast<unsigned>(texture.getSize().x), static_cast<unsigned>(texture.getSize().y));
        ImGui::TextWrapped("Последний путь: %s", currentScene->GetBackgroundPath().empty() ? "(не указан)" : currentScene->GetBackgroundPath().c_str());
        ImGui::Spacing();
        const sf::FloatRect bounds = background->getGlobalBounds();
        const float maxPreviewWidth = 420.0f;
        const float maxPreviewHeight = 220.0f;
        float displayScale = 1.0f;
        if (bounds.size.x > 0 && bounds.size.y > 0) {
            displayScale = std::min(maxPreviewWidth / bounds.size.x, maxPreviewHeight / bounds.size.y);
        }
        displayScale = std::max(displayScale, 0.1f);
        ImGui::Image(background->getTexture(), sf::Vector2f(bounds.size.x * displayScale, bounds.size.y * displayScale));

        sf::Vector2f newBgPosition = bgPosition;
        if (ImGui::DragFloat2("Позиция фона", &newBgPosition.x, 1.0f)) {
            currentScene->SetBackgroundPosition(newBgPosition);
        }

        sf::Vector2f newBgScale = bgScale;
        if (ImGui::DragFloat2("Масштаб фона", &newBgScale.x, 0.01f, 0.1f, 10.0f)) {
            currentScene->SetBackgroundScale(newBgScale);
        }
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Фон не задан");
    }

    if (ImGui::Button("Выбрать изображение фона...", ImVec2(-1, 30))) {
        const char* lFilterPatterns[3] = { "*.png", "*.jpg", "*.jpeg" };
        const char* lSelectedPath = tinyfd_openFileDialog(
            "Выберите картинку для фона",
            "",
            3,
            lFilterPatterns,
            "Изображения (png, jpg)",
            0
        );
        if (lSelectedPath) {
            if (!currentScene->LoadBackground(lSelectedPath)) {
                tinyfd_messageBox("Ошибка", "Не удалось загрузить изображение фона", "ok", "error", 1);
            }
        }
    }

    ImGui::SeparatorText("Музыка сцены");
    if (currentScene->GetMusic()) {
        ImGui::Text("Музыка загружена");
        ImGui::TextWrapped("Последний путь: %s", currentScene->GetMusicPath()[0] == '\0' ? "(не указан)" : currentScene->GetMusicPath());
        float volume = currentScene->GetMusicVolume();
        if (ImGui::SliderFloat("Громкость", &volume, 0.0f, 100.0f)) {
            currentScene->SetMusicVolume(volume);
        }
        bool loop = currentScene->IsMusicLoop();
        if (ImGui::Checkbox("Зацикливание", &loop)) {
            currentScene->SetMusicLoop(loop);
        }
        bool enabled = currentScene->IsMusicEnabled();
        if (ImGui::Checkbox("Включить музыку", &enabled)) {
            currentScene->SetMusicEnabled(enabled);
        }
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Музыка не задана");
    }

    if (ImGui::Button("Выбрать музыкальный файл...", ImVec2(-1, 30))) {
        const char* lFilterPatterns[3] = { "*.ogg", "*.wav", "*.flac" };
        const char* lSelectedPath = tinyfd_openFileDialog(
            "Выберите музыкальный файл",
            "",
            3,
            lFilterPatterns,
            "Аудио (ogg, wav, flac)",
            0
        );
        if (lSelectedPath) {
            if (!currentScene->LoadMusic(lSelectedPath)) {
                tinyfd_messageBox("Ошибка", "Не удалось загрузить музыкальный файл", "ok", "error", 1);
            }
        }
    }

    if (window) {
        ImGui::SetCursorPosY(window->getSize().y - 60);
        if (ImGui::Button("Удалить эту сцену", ImVec2(150, 0))) {
            sceneManager.RemoveSelectedScene();
            ImGui::End();
            return;
        }
    }

    ImGui::End();
}

} // namespace EditorPanels
