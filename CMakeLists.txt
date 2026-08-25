cmake_minimum_required(VERSION 3.25)

project(Blokk
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(Blokk_Engine/Engine/EngineSystems)

add_executable(Game
    main.cpp
)

target_link_libraries(Game PRIVATE Blokk)