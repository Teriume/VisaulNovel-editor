#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include "Editor.h"
#include "EditorPanels.h"
#include "EditorDialogs.h"

Editor::Editor() = default;
Editor::~Editor() = default;

void Editor::Init(sf::RenderWindow& window) {
    ImGui::SFML::Init(window);
    m_window = &window;

    ImGuiIO& io = ImGui::GetIO();
    const char* fontPath = "/System/Library/Fonts/Supplemental/Arial.ttf";
    io.Fonts->Clear();
    io.Fonts->AddFontFromFileTTF(fontPath, 18.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    ImGui::SFML::UpdateFontTexture();
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
    EditorPanels::DrawMainPanel(m_sceneManager, m_showNewSceneWindow);
    if (m_window) {
        EditorPanels::DrawPresetPanel(m_sceneManager, m_presetManager, m_showNewPresetWindow, m_window);
    }
    EditorPanels::DrawInspectorPanel(m_sceneManager, m_presetManager, m_scenePreview, m_window, m_editTarget);
    EditorDialogs::DrawSceneDialog(m_showNewSceneWindow, m_sceneManager);
    EditorDialogs::DrawPresetDialog(m_showNewPresetWindow, m_presetManager);
}

void Editor::Render(sf::RenderWindow& window) {
    ImGui::SFML::Render(window);
}
