#include "EditorPanels.h"
#include "tinyfiledialogs.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include <cstring>
#include <algorithm>
#include <cstdlib>
#include <string>
#include <filesystem>
#include <vector>

namespace EditorPanels {

void DrawMainPanel(SceneManager& sceneManager, CharacterPresetManager& characterPresetManager, bool& showNewSceneWindow, char* projectPath, size_t projectPathSize) {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
    ImGui::Begin("Главное", nullptr, flags);
    ImGui::SetWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(300, 150), ImGuiCond_Always);

    if (ImGui::Button("Создать новую сцену", ImVec2(-1, 40))) {
        showNewSceneWindow = true;
    }

    if (ImGui::Button("Открыть новеллу", ImVec2(-1, 0))) {
        const char* patterns[] = {"*.nana"};
        const char* selectedPath = tinyfd_openFileDialog("Открыть новеллу", "", 1, patterns, "Проект Na-Na", 0);
        if (selectedPath && sceneManager.LoadFromFile(selectedPath)) {
            std::strncpy(projectPath, selectedPath, projectPathSize - 1);
            projectPath[projectPathSize - 1] = '\0';
            characterPresetManager.LoadFromFile(std::filesystem::path(selectedPath).replace_extension(".charpresets").string());
        }
    }
    if (ImGui::Button("Сохранить новеллу", ImVec2(-1, 0))) {
        const char* selectedPath = projectPath[0] ? projectPath : tinyfd_saveFileDialog(
            "Сохранить новеллу", "novel.nana", 1, (const char*[]){"*.nana"}, "Проект Na-Na");
        if (selectedPath && sceneManager.SaveToFile(selectedPath)) {
            std::strncpy(projectPath, selectedPath, projectPathSize - 1);
            projectPath[projectPathSize - 1] = '\0';
            characterPresetManager.SaveToFile(std::filesystem::path(selectedPath).replace_extension(".charpresets").string());
        }
    }
    if (projectPath[0] && ImGui::Button("Запустить отдельной игрой", ImVec2(-1, 0))) {
        const std::string command = "./VisualNovelPlayer \"" + std::string(projectPath) + "\" >/dev/null 2>&1 &";
        std::system(command.c_str());
    }

    ImGui::Separator();

    const auto& scenes = sceneManager.GetScenes();
    if (ImGui::Button("Вверх"))
        sceneManager.MoveSelectedSceneUp();
    ImGui::SameLine();
    if (ImGui::Button("Вниз"))
        sceneManager.MoveSelectedSceneDown();

    for (int i = 0; i < static_cast<int>(scenes.size()); ++i) {
        const bool isSelected = (i == sceneManager.GetSelectedSceneIndex());
        if (ImGui::Selectable(scenes[i]->GetName(), isSelected)) {
            sceneManager.SelectScene(i);
        }
    }

    ImGui::Text("Список сцен:");
    for (const auto& scene : scenes) {
        ImGui::BulletText("%s", scene->GetName());
    }

    ImGui::End();
}

void DrawPresetPanel(SceneManager& sceneManager, CharacterPresetManager& presetManager, bool& showNewPresetWindow, sf::RenderWindow* window) {
    if (!window)
        return;

    ImGui::Begin("Пресеты персонажей", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    ImGui::SetWindowPos(ImVec2(10, 170), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(300, 230), ImGuiCond_Always);

    ImGui::Text("Пресеты персонажей");
    if (ImGui::Button("Создать пресет персонажа", ImVec2(-1, 0))) {
        showNewPresetWindow = true;
    }

    ImGui::BeginChild("CharacterPresetList", ImVec2(0, 120), true);

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

        if (CharacterPreset* preset = presetManager.GetSelectedPreset()) {
            ImGui::TextWrapped("Описание: %s", preset->notes.c_str());
            ImGui::Text("Дефолтная эмоция: %s", preset->defaultEmotion.c_str());

            if (preset->previewTexture) {
                ImGui::Image(*preset->previewTexture, sf::Vector2f(80.f, 80.f));
            }

            ImGui::SeparatorText("Доступные эмоции:");
            for (const auto& [emoName, emoPath] : preset->emotions) {
                ImGui::BulletText("%s: %s", emoName.c_str(), emoPath.c_str());
            }

            Scene* currentScene = sceneManager.GetSelectedScene();
            if (currentScene) {
                if (ImGui::Button("Добавить персонажа из пресета", ImVec2(-1, 0))) {
                    auto it = preset->emotions.find(preset->defaultEmotion);
                    std::string imagePath = (it != preset->emotions.end()) ? it->second : (preset->emotions.empty() ? "" : preset->emotions.begin()->second);

                    if (!imagePath.empty()) {
                        if (currentScene->AddCharacter(imagePath.c_str(), preset->name.c_str(), preset->defaultEmotion.c_str(), preset->notes.c_str())) {
                            sceneManager.SelectCharacter(static_cast<int>(currentScene->GetCharacterCount()) - 1);
                        } else {
                            tinyfd_messageBox("Ошибка", "Не удалось загрузить изображение из пресета", "ok", "error", 1);
                        }
                    }
                }
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "Сначала выберите сцену.");
            }
        }
    } else {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Пока нет пресетов.");
    }

    ImGui::EndChild();

    if (ImGui::Button("Экспорт пресетов...")) {
        const char* patterns[] = { "*.charpresets" };
        const char* savePath = tinyfd_saveFileDialog(
            "Экспорт пресетов персонажей",
            "default_characters.charpresets",
            1,
            patterns,
            "Файлы пресетов (*.charpresets)"
        );
        if (savePath) {
            if (presetManager.SaveToFile(savePath)) {
                tinyfd_messageBox("Успех", "Пресеты сохранены!", "ok", "info", 1);
            } else {
                tinyfd_messageBox("Ошибка", "Не удалось записать файл.", "ok", "error", 1);
            }
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Импорт пресетов...")) {
        const char* patterns[] = { "*.charpresets" };
        const char* openPath = tinyfd_openFileDialog(
            "Импорт пресетов персонажей",
            "",
            1,
            patterns,
            "Файлы пресетов (*.charpresets)",
            0
        );
        if (openPath) {
            if (presetManager.ImportFromFile(openPath)) {
                tinyfd_messageBox("Успех", "Пресеты импортированы!", "ok", "info", 1);
            } else {
                tinyfd_messageBox("Ошибка", "Не удалось прочитать файл.", "ok", "error", 1);
            }
        }
    }
    ImGui::End();
}

void DrawBackgroundPresetPanel(SceneManager& sceneManager, BackgroundPresetManager& presetManager, sf::RenderWindow* window, const char* projectPath) {
    if (!window) return;

    ImGui::Begin("Пресеты фонов", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    ImGui::SetWindowPos(ImVec2(10, 410), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(300, 230), ImGuiCond_Always);

    Scene* scene = sceneManager.GetSelectedScene();
    static std::string loadedProjectPath;
    if (projectPath && projectPath[0] && loadedProjectPath != projectPath) {
        const std::string presetPath = std::filesystem::path(projectPath).replace_extension(".bgpresets").string();
        presetManager.LoadFromFile(presetPath);
        loadedProjectPath = projectPath;
    }

    static char presetName[128] = "Новый фон";
    if (scene && scene->GetBackgroundSprite() && ImGui::Button("Сохранить текущий фон")) {
        ImGui::OpenPopup("Имя пресета фона");
    }

    if (ImGui::BeginPopupModal("Имя пресета фона", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("Название", presetName, sizeof(presetName));
        if (ImGui::Button("Добавить") && scene) {
            presetManager.Add(presetName, scene->GetBackgroundPath());
            presetManager.SaveToFile("default_backgrounds.bgpresets");

            std::strncpy(presetName, "Новый фон", sizeof(presetName) - 1);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    for (const auto& preset : presetManager.GetPresets()) {
        if (ImGui::Button(preset.name.c_str(), ImVec2(-1, 0)) && scene)
            scene->LoadBackground(preset.imagePath.c_str());
    }

    if (projectPath && projectPath[0]) {
        const std::string presetPath = std::filesystem::path(projectPath).replace_extension(".bgpresets").string();
        if (ImGui::Button("Сохранить пресеты")) presetManager.SaveToFile(presetPath);
        ImGui::SameLine();
        if (ImGui::Button("Загрузить пресеты")) presetManager.LoadFromFile(presetPath);
    }

    ImGui::End();
}

void DrawInspectorPanel(SceneManager& sceneManager, CharacterPresetManager& presetManager, ScenePreview& preview, sf::RenderWindow* window, int& editTarget, bool& showPlayMode) {
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
    if (ImGui::Button("Запустить новеллу")) {
        showPlayMode = true;
    }

    ImGui::SameLine();
    if (ImGui::RadioButton("Фон", &editTarget, 0)) {}
    ImGui::SameLine();
    if (ImGui::RadioButton("Персонаж", &editTarget, 1)) {}
    ImGui::Separator();

    ImVec2 availableSize = ImGui::GetContentRegionAvail();
    const float previewHeight = std::clamp(availableSize.y * 0.50f, 320.0f, 520.0f);
    ImVec2 previewSize(availableSize.x, previewHeight);
    if (previewSize.x < 200.0f) previewSize.x = 200.0f;
    if (previewSize.y < 180.0f) previewSize.y = 180.0f;

    if (preview.RenderScene(*currentScene, sf::Vector2u(static_cast<unsigned>(previewSize.x), static_cast<unsigned>(previewSize.y)))) {
        ImGui::BeginChild("ScenePreviewChild", previewSize, true, ImGuiChildFlags_AlwaysUseWindowPadding);
        
        // Запоминаем экранные координаты начала Canvas
        ImVec2 canvasPos = ImGui::GetCursorScreenPos();
        
        // Отрисовываем текстуру предпросмотра с помощью sf::Vector2f
        ImGui::Image(preview.GetRenderTexture(), sf::Vector2f(previewSize.x, previewSize.y));

        // Накладываем поверх невидимую кнопку для корректного захвата ввода мыши
        ImGui::SetCursorScreenPos(canvasPos);
        ImGui::InvisibleButton("##SceneCanvasInteractive", previewSize, ImGuiButtonFlags_MouseButtonLeft);

        bool canvasHovered = ImGui::IsItemHovered();
        bool canvasActive = ImGui::IsItemActive();

        if (canvasHovered) {
            ImGui::SetTooltip("ЛКМ: выбор / перетаскивание (%s)\nКолесо: масштаб", editTarget == 1 ? "персонажа" : "фона");
        }

        // 1. Выбор персонажа по клику
        if (canvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            ImVec2 mousePos = ImGui::GetMousePos();
            float sceneX = (mousePos.x - canvasPos.x) / previewSize.x * 1280.0f;
            float sceneY = (mousePos.y - canvasPos.y) / previewSize.y * 720.0f;
            sf::Vector2f clickPos(sceneX, sceneY);

            const auto& characters = currentScene->GetCharacters();
            for (int i = static_cast<int>(characters.size()) - 1; i >= 0; --i) {
                if (characters[i].sprite && characters[i].sprite->getGlobalBounds().contains(clickPos)) {
                    sceneManager.SelectCharacter(i);
                    editTarget = 1; // Автоматически включаем режим редактирования персонажа
                    break;
                }
            }
        }

        // 2. Перетаскивание объектов мышью (работает при удерживании ЛКМ)
        if (canvasActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f)) {
            ImVec2 mouseDelta = ImGui::GetIO().MouseDelta;
            if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f) {
                const float sceneScaleX = 1280.0f / previewSize.x;
                const float sceneScaleY = 720.0f / previewSize.y;
                sf::Vector2f delta(mouseDelta.x * sceneScaleX, mouseDelta.y * sceneScaleY);

                if (editTarget == 1) {
                    int selCharIdx = sceneManager.GetSelectedCharacterIndex();
                    if (selCharIdx < 0 && currentScene->GetCharacterCount() > 0) {
                        selCharIdx = 0;
                        sceneManager.SelectCharacter(0);
                    }

                    if (Scene::Character* character = currentScene->GetCharacter(selCharIdx)) {
                        if (character->sprite) {
                            character->sprite->move(delta);
                        }
                    }
                } else if (editTarget == 0) {
                    if (currentScene->GetBackgroundSprite()) {
                        currentScene->SetBackgroundPosition(currentScene->GetBackgroundPosition() + delta);
                    }
                }
            }
        }

        // 3. Масштабирование колесом мыши
        const float wheel = ImGui::GetIO().MouseWheel;
        if (canvasHovered && wheel != 0.0f) {
            const float factor = 1.0f + wheel * 0.10f;
            if (editTarget == 1) {
                int selCharIdx = sceneManager.GetSelectedCharacterIndex();
                if (selCharIdx >= 0) {
                    if (Scene::Character* character = currentScene->GetCharacter(selCharIdx)) {
                        if (character->sprite) {
                            sf::Vector2f scale = character->sprite->getScale();
                            scale *= factor;
                            scale.x = std::clamp(scale.x, 0.05f, 10.0f);
                            scale.y = std::clamp(scale.y, 0.05f, 10.0f);
                            character->sprite->setScale(scale);
                        }
                    }
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
    static int bufferedSceneIndex = -1;
    static char sceneNameBuf[128] = "";
    static char speakerBuf[128] = "";
    static char dialogueBuf[2048] = "";
    const int currentSceneIndex = sceneManager.GetSelectedSceneIndex();
    if (bufferedSceneIndex != currentSceneIndex) {
        std::strncpy(sceneNameBuf, currentScene->GetName(), sizeof(sceneNameBuf) - 1);
        std::strncpy(speakerBuf, currentScene->GetSpeakerName().c_str(), sizeof(speakerBuf) - 1);
        std::strncpy(dialogueBuf, currentScene->GetDialogueText().c_str(), sizeof(dialogueBuf) - 1);
        sceneNameBuf[sizeof(sceneNameBuf) - 1] = '\0';
        speakerBuf[sizeof(speakerBuf) - 1] = '\0';
        dialogueBuf[sizeof(dialogueBuf) - 1] = '\0';
        bufferedSceneIndex = currentSceneIndex;
    }
    if (ImGui::InputText("Название сцены", sceneNameBuf, sizeof(sceneNameBuf))) {
        currentScene->SetName(sceneNameBuf);
    }

    if (ImGui::InputText("Имя говорящего", speakerBuf, sizeof(speakerBuf))) {
        currentScene->SetSpeakerName(speakerBuf);
    }

    if (ImGui::InputTextMultiline("Текст реплики", dialogueBuf, sizeof(dialogueBuf), ImVec2(-1, 90))) {
        currentScene->SetDialogueText(dialogueBuf);
    }

    ImGui::SeparatorText("Варианты выбора");
    const auto& scenes = sceneManager.GetScenes();
    for (size_t choiceIndex = 0; choiceIndex < currentScene->GetChoices().size(); ++choiceIndex) {
        Scene::Choice& choice = currentScene->GetChoices()[choiceIndex];
        ImGui::PushID(static_cast<int>(choiceIndex));
        char choiceText[256];
        std::strncpy(choiceText, choice.text.c_str(), sizeof(choiceText) - 1);
        choiceText[sizeof(choiceText) - 1] = '\0';
        if (ImGui::InputText("Текст варианта", choiceText, sizeof(choiceText)))
            choice.text = choiceText;
        std::vector<const char*> sceneNames;
        sceneNames.reserve(scenes.size() + 1);
        sceneNames.push_back("Без перехода");
        for (const auto& scene : scenes) sceneNames.push_back(scene->GetName());
        int targetIndex = 0;
        for (size_t sceneIndex = 0; sceneIndex < scenes.size(); ++sceneIndex) {
            if (choice.targetScene == scenes[sceneIndex]->GetName()) targetIndex = static_cast<int>(sceneIndex + 1);
        }
        if (ImGui::Combo("Перейти в сцену", &targetIndex, sceneNames.data(), static_cast<int>(sceneNames.size())))
            choice.targetScene = targetIndex == 0 ? "" : scenes[static_cast<size_t>(targetIndex - 1)]->GetName();
        if (ImGui::Button("Удалить вариант")) {
            currentScene->RemoveChoice(choiceIndex);
            ImGui::PopID();
            break;
        }
        ImGui::PopID();
    }
    if (ImGui::Button("Добавить вариант"))
        currentScene->AddChoice();

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
                auto it = preset->emotions.find(preset->defaultEmotion);
                std::string imagePath = (it != preset->emotions.end()) ? it->second : (preset->emotions.empty() ? "" : preset->emotions.begin()->second);

                if (!imagePath.empty()) {
                    if (currentScene->AddCharacter(imagePath.c_str(), preset->name.c_str(), preset->defaultEmotion.c_str(), preset->notes.c_str())) {
                        sceneManager.SelectCharacter(static_cast<int>(currentScene->GetCharacterCount()) - 1);
                    } else {
                        tinyfd_messageBox("Ошибка", "Не удалось загрузить изображение из пресета", "ok", "error", 1);
                    }
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

            const CharacterPreset* matchingPreset = nullptr;
            for (const auto& preset : presetManager.GetPresets()) {
                if (preset.name == character->name) {
                    matchingPreset = &preset;
                    break;
                }
            }

            if (matchingPreset && !matchingPreset->emotions.empty()) {
                std::vector<std::string> emoKeys;
                std::vector<const char*> emoCStr;
                int currentEmotionIndex = 0;
                int idx = 0;

                for (const auto& [emoName, emoPath] : matchingPreset->emotions) {
                    emoKeys.push_back(emoName);
                    if (emoName == character->emotion) {
                        currentEmotionIndex = idx;
                    }
                    idx++;
                }

                for (const auto& key : emoKeys) {
                    emoCStr.push_back(key.c_str());
                }

                if (ImGui::Combo("Эмоция", &currentEmotionIndex, emoCStr.data(), static_cast<int>(emoCStr.size()))) {
                    std::string selectedEmotion = emoKeys[currentEmotionIndex];
                    character->emotion = selectedEmotion;

                    auto it = matchingPreset->emotions.find(selectedEmotion);
                    if (it != matchingPreset->emotions.end()) {
                        if (character->texture && character->texture->loadFromFile(it->second)) {
                            character->sprite->setTexture(*character->texture, true);
                        }
                    }
                }
            } else {
                char emoBuf[128];
                std::strncpy(emoBuf, character->emotion.c_str(), sizeof(emoBuf));
                if (ImGui::InputText("Эмоция", emoBuf, sizeof(emoBuf))) {
                    character->emotion = emoBuf;
                }
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
            float uniformScale = character->sprite->getScale().x;
            if (ImGui::SliderFloat("Масштаб персонажа", &uniformScale, 0.05f, 10.0f, "%.2fx"))
                character->sprite->setScale(sf::Vector2f(uniformScale, uniformScale));
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
        if (ImGui::Button("Центрировать фон"))
            currentScene->CenterBackground();

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

    if (ImGui::Button("Удалить эту сцену", ImVec2(-1, 0))) {
        sceneManager.RemoveSelectedScene();
        ImGui::End();
        return;
    }

    ImGui::End();
}

void DrawPlayPanel(SceneManager& sceneManager, ScenePreview& preview, sf::RenderWindow* window, bool& showPlayMode, bool& showNovelMenu) {
    static char gameSavePath[512] = "";
    ImGui::Begin("Запуск новеллы", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    if (window) {
        ImGui::SetWindowPos(ImVec2(310, 0), ImGuiCond_Always);
        ImGui::SetWindowSize(ImVec2(window->getSize().x - 250, window->getSize().y), ImGuiCond_Always);
    }

    Scene* scene = sceneManager.GetSelectedScene();
    if (!scene) {
        ImGui::Text("Сначала выберите сцену.");
        if (ImGui::Button("Вернуться к редактору"))
            showPlayMode = false;
        ImGui::End();
        return;
    }

    if (showNovelMenu) {
        ImGui::Spacing();
        ImGui::Text("Моя визуальная новелла");
        ImGui::TextWrapped("Главное меню проекта");
        if (ImGui::Button("Новая игра", ImVec2(220, 42))) {
            if (sceneManager.GetSceneCount() > 0)
                sceneManager.SelectScene(0);
            showNovelMenu = false;
        }
        if (ImGui::Button("Вернуться к редактору")) {
            showPlayMode = false;
            showNovelMenu = true;
        }
        ImGui::End();
        return;
    }

    ImGui::Text("Сцена: %s", scene->GetName());
    ImGui::SameLine();
    if (ImGui::Button("Редактировать"))
        showPlayMode = false;
    ImGui::SameLine();
    if (ImGui::Button("Сохранить игру")) {
        const char* selectedPath = gameSavePath[0] ? gameSavePath : tinyfd_saveFileDialog(
            "Сохранить игру", "save.nanasave", 1, (const char*[]){"*.nanasave"}, "Сохранение игры");
        if (selectedPath && sceneManager.SaveGameState(selectedPath)) {
            std::strncpy(gameSavePath, selectedPath, sizeof(gameSavePath) - 1);
            gameSavePath[sizeof(gameSavePath) - 1] = '\0';
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Загрузить игру")) {
        const char* patterns[] = {"*.nanasave"};
        const char* selectedPath = tinyfd_openFileDialog("Загрузить игру", "", 1, patterns, "Сохранение игры", 0);
        if (selectedPath)
            sceneManager.LoadGameState(selectedPath);
    }
    ImGui::Separator();

    ImVec2 availableSize = ImGui::GetContentRegionAvail();
    const float previewHeight = std::max(240.0f, availableSize.y - 155.0f);
    ImVec2 previewSize(std::max(200.0f, availableSize.x), previewHeight);
    if (preview.RenderScene(*scene, sf::Vector2u(static_cast<unsigned>(previewSize.x), static_cast<unsigned>(previewSize.y)))) {
        ImGui::Image(preview.GetRenderTexture(), sf::Vector2f(previewSize.x, previewSize.y));
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
            const int nextScene = sceneManager.GetSelectedSceneIndex() + 1;
            if (nextScene < static_cast<int>(sceneManager.GetSceneCount()))
                sceneManager.SelectScene(nextScene);
            else
                showNovelMenu = true;
        }
    }

    ImGui::Spacing();
    ImGui::BeginChild("DialoguePreview", ImVec2(0, 110), true);
    if (!scene->GetSpeakerName().empty())
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.35f, 1.0f), "%s", scene->GetSpeakerName().c_str());
    ImGui::TextWrapped("%s", scene->GetDialogueText().empty() ? "Введите текст реплики в редакторе." : scene->GetDialogueText().c_str());
    ImGui::EndChild();

    if (!scene->GetChoices().empty()) {
        ImGui::SeparatorText("Выбор");
        for (const Scene::Choice& choice : scene->GetChoices()) {
            if (ImGui::Button(choice.text.c_str(), ImVec2(-1, 32)) && !choice.targetScene.empty()) {
                sceneManager.SelectSceneByName(choice.targetScene);
            }
        }
    } else if (ImGui::Button("Показать главное меню")) {
        showNovelMenu = true;
    }
    ImGui::End();
}

} // namespace EditorPanels