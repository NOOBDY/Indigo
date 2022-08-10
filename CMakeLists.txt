cmake_minimum_required(VERSION 3.0)
project(Indigo VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)

add_subdirectory(lib)

set(LIB
    ${OPENGL_LIBRARY}
    glfw
    glew_220
)

add_definitions(
    -DGLEW_STATIC
)

add_executable(Indigo
    src/main.cpp
)
target_link_libraries(Indigo ${LIB})
target_include_directories(Indigo PRIVATE
    lib/glew-2.2.0/include/
    lib/glfw-3.3.8/include/
    include
)