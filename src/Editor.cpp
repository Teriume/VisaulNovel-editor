#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include "Editor.h"
#include "EditorPanels.h"
#include "EditorDialogs.h"
#include <array>

Editor::Editor() = default;
Editor::~Editor() = default;

void Editor::Init(sf::RenderWindow& window) {
    (void)ImGui::SFML::Init(window, false);
    m_window = &window;

    ImGuiIO& io = ImGui::GetIO();
    const std::array<const char*, 4> fontPaths = {
        "/System/Library/Fonts/SFNS.ttf",
        "/System/Library/Fonts/Supplemental/Arial Unicode.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/Library/Fonts/Arial Unicode.ttf"
    };
    io.Fonts->Clear();
    ImFontConfig fontConfig;
    fontConfig.OversampleH = 3;
    fontConfig.OversampleV = 2;
    for (const char* path : fontPaths) {
        ImFont* cyrillicFont = io.Fonts->AddFontFromFileTTF(path, 22.0f, &fontConfig, io.Fonts->GetGlyphRangesCyrillic());
        if (cyrillicFont) {
            io.FontDefault = cyrillicFont;
            (void)ImGui::SFML::UpdateFontTexture();
            break;
        }
    }

    // Автоматическая загрузка сохраненных пресетов при старте редактора
    m_presetManager.LoadFromFile("default_characters.charpresets");
    m_backgroundPresetManager.LoadFromFile("default_backgrounds.bgpresets");
}

void Editor::Shutdown() {
    if (m_window) {
        ImGui::SFML::Shutdown(*m_window);
        m_window = nullptr;
    }
}

void Editor::ProcessEvents(sf::RenderWindow& window, const sf::Event& event) {
    if (m_window)
        ImGui::SFML::ProcessEvent(*m_window, event);
}

void Editor::Update(sf::RenderWindow& window, sf::Time dt) {
    if (m_window)
        ImGui::SFML::Update(*m_window, dt);
}

void Editor::Draw() {
    EditorPanels::DrawMainPanel(m_sceneManager, m_presetManager, m_showNewSceneWindow, m_projectPath, sizeof(m_projectPath));
    if (m_window) {
        EditorPanels::DrawPresetPanel(m_sceneManager, m_presetManager, m_showNewPresetWindow, m_window);
        EditorPanels::DrawBackgroundPresetPanel(m_sceneManager, m_backgroundPresetManager, m_window, m_projectPath);
    }
    if (m_showPlayMode) {
        EditorPanels::DrawPlayPanel(m_sceneManager, m_scenePreview, m_window, m_showPlayMode, m_showNovelMenu);
    } else {
        EditorPanels::DrawInspectorPanel(m_sceneManager, m_presetManager, m_scenePreview, m_window, m_editTarget, m_showPlayMode);
    }
    EditorDialogs::DrawSceneDialog(m_showNewSceneWindow, m_sceneManager);
    EditorDialogs::DrawPresetDialog(m_showNewPresetWindow, m_presetManager);
}

void Editor::Render(sf::RenderWindow& window) {
    ImGui::SFML::Render(window);
}