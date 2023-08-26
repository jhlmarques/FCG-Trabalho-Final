cmake_minimum_required(VERSION 3.0.0)
project(TRAB_FCG VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/bin/Debug")
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/bin/Release")

include_directories(BEFORE SYSTEM ${PROJECT_SOURCE_DIR}/include)

set(SOURCES
    src/camera.cpp
    src/gameobject.cpp
    src/glad.c
    src/globals.cpp
    src/labfunctions.cpp
    src/labstructures.cpp
    src/light_source.cpp
    src/room.cpp
    src/main.cpp
    src/material.cpp
    src/matrices.cpp
    src/puzzle.cpp
    src/stb_image.cpp
    src/texture.cpp
    src/tiny_obj_loader.cpp
)

set(EXECUTABLE_NAME main)

add_executable(${EXECUTABLE_NAME} ${SOURCES})

find_package(OpenGL REQUIRED)

file(GLOB LIBS ${PROJECT_SOURCE_DIR}/lib/*)
target_link_libraries(${EXECUTABLE_NAME} ${LIBS})

