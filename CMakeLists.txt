cmake_minimum_required(VERSION 3.0.0)
project(objetosTexturizados VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)

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
