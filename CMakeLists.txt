cmake_minimum_required(VERSION 3.25)

project(Blokk
    VERSION 0.1.6
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# SDL
add_subdirectory(vendored/SDL)

# SDL_image
add_subdirectory(vendored/SDL_image)

# Blokk engine
add_subdirectory(Blokk/Engine/Systems)