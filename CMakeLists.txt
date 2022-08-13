cmake_minimum_required(VERSION 3.20)
project(Indigo VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)

add_subdirectory(lib)

set(LIB
    ${OPENGL_LIBRARY}
    glfw
    GLEW
    spdlog::spdlog
)

link_directories(
    lib/glew/lib/
)

set(GLFW_USE_WAYLAND ON)

add_definitions(
    -DGLEW_STATIC
)

add_executable(Indigo
    src/main.cpp
    src/log.cpp

    include/log.hpp
)
target_link_libraries(Indigo ${LIB})
target_include_directories(Indigo SYSTEM PRIVATE
    lib/glew-2.2.0/include/
    lib/glfw/include/
    lib/spdlog/include/
)
target_include_directories(Indigo PRIVATE
    include/
)
target_compile_definitions(Indigo PRIVATE
    -DGLFW_USE_WAYLAND=ON
)