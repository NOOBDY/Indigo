cmake_minimum_required(VERSION 3.20)
project(Indigo VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_POLICY_DEFAULT_CMP0072 NEW)

find_package(OpenGL REQUIRED)

add_subdirectory(lib)

set(LIB
    ${OPENGL_LIBRARY}
    glfw
    glew_220
    spdlog::spdlog
    assimp
)

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
    lib/assimp/include/
)
target_include_directories(Indigo PRIVATE
    include/
)
target_compile_options(Indigo PRIVATE
    -Wall -pedantic
)