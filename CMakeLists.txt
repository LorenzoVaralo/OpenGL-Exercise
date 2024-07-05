cmake_minimum_required(VERSION 3.0.0)
project(objetosTexturizados VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)

# Determine the root directory of the project
set(PROJECT_ROOT "${CMAKE_SOURCE_DIR}")

# Add this definition so it can be used in the code
add_definitions(-DPROJECT_ROOT="${PROJECT_ROOT}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(OpenGL REQUIRED)

add_executable(objetosTexturizados
    src/main.cpp 
    src/glad.h 
    src/glad.c
    src/glew.h
)

target_link_libraries(objetosTexturizados 
    glfw
    SOIL
    OpenGL::GL
)
