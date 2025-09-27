cmake_minimum_required(VERSION 3.10))

project(PrototypeEngine VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

//ソースファイルの追加

add_library(EngineCore
    Engine/Renderer.cpp
    Engine/SceneManager.cpp
    Engine/InputSystem.cpp
    # ...
)

add_executable(Editor
    Editor/EditorMain.cpp
    Engine/EngineWindow.cpp
    Engine/SceneEditorCamera.cpp
    Editor/GameViewPanel.cpp
)
target_link_libraries(Editor EngineCore imgui SDL3 OpenGL::GL)

add_executable(Game
    Game/GameMain.cpp
    Game/GameWinMain.cpp
)
target_link_libraries(Game EngineCore SDL3 OpenGL::GL)