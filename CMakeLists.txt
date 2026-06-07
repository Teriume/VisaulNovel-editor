cmake_minimum_required(VERSION 3.15)
project(VisualNovelEditor)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Поиск SFML (он должен быть установлен в системе)
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};/opt/homebrew/opt/sfml")
find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED PATHS /opt/homebrew/opt/sfml/lib/cmake/SFML)

# 2. Пути к исходникам ImGui
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui")
set(IMGUI_SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui_sfml")

# Список файлов ImGui, которые нужно скомпилировать
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_SFML_DIR}/imgui-SFML.cpp
)

# Команда GLOB автоматически находит все файлы с расширением .cpp в папке src
file(GLOB PROJECT_SOURCES "src/*.cpp")
set(TINYFILEDIALOGS_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/external/libtinyfiledialogs/tinyfiledialogs.c")

# 3. Создание исполняемого файла (теперь используем переменную PROJECT_SOURCES)
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${IMGUI_SOURCES} ${TINYFILEDIALOGS_SOURCE})

# ==========================================
# ОБНОВЛЕННЫЙ БЛОК: Подключение заголовков
# ==========================================
# 4. Подключаем заголовки (include)
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include  # <-- ДОБАВЛЕНО: папка для твоих .h файлов
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${IMGUI_DIR}
    ${IMGUI_SFML_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/external/libtinyfiledialogs
)

# 5. Линковка библиотек
# Для работы с OpenGL (нужен для ImGui)
find_package(OpenGL REQUIRED)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
    OpenGL::GL
)