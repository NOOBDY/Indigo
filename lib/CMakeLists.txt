# GLFW

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(glfw)

# GLEW
set(GLEW_SOURCE
    glew-2.2.0/src/glew.c
)

add_library(glew_220 STATIC
    ${GLEW_SOURCE}
)
target_include_directories(glew_220 PUBLIC
    glew-2.2.0/include
)
target_link_libraries(glew_220
    ${OPENGL_LIBRARY}
    ${EXTRA_LIBS}
)
add_definitions(
    -DGLEW_STATIC
)
# spdlog

add_subdirectory(spdlog)

# glm

add_subdirectory(glm)

# assimp

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(BUILD_SHARED_LIBS OFF)
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_OBJ_IMPORTER ON)
set(ASSIMP_BUILD_GLTF_IMPORTER ON)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_WARNINGS_AS_ERRORS OFF)
set(ASSIMP_BUILD_TESTS OFF)

add_subdirectory(assimp)

# imgui

set(IMGUI_SOURCE
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
)

set(IMGUI_INCLUDE_DIR
    lib/imgui/
    lib/imgui/backends/
    PARENT_SCOPE
)

add_library(ImGui STATIC
    ${IMGUI_SOURCE}
)
target_include_directories(ImGui PUBLIC
    imgui/
    imgui/backends/
    glfw/include/
)
