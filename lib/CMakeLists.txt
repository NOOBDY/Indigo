add_definitions(
    -DGLEW_STATIC
)

# GLFW

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(glfw)

# GLEW

set(GLEW_SOURCE
    glew-2.2.0/src/glew.c
)

include_directories(
    glew-2.2.0/include
)

add_library(glew_220 STATIC
    ${GLEW_SOURCE}
)
target_link_libraries(glew_220
    ${OPENGL_LIBRARY}
    ${EXTRA_LIBS}
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
set(ASSIMP_INSTALL OFF)
set(ASSIMP_WARNINGS_AS_ERRORS OFF)
set(ASSIMP_BUILD_TESTS OFF)

add_subdirectory(assimp)