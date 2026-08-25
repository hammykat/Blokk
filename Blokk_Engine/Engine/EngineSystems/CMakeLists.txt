add_library(Blokk
    EngineClassData.hpp

    # Engine systems
    IncrementFrames.cpp
    Velocities.cpp
    Visibility_BasicCulling.cpp
    Visibility_AxisCulling.cpp
    UserUpdates.cpp
    Animations.cpp
    Threading.hpp
    Threading.cpp

    # GameObjects
    ../GameObjects/Reg/Frames+Animation.cpp
    ../GameObjects/Reg/Position.cpp
    ../GameObjects/Reg/Velocity.cpp
    ../GameObjects/Reg/Visibility.cpp
)

target_include_directories(Blokk PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Helpers
    ${CMAKE_CURRENT_SOURCE_DIR}/../GameObjects/Reg
    ${CMAKE_CURRENT_SOURCE_DIR}/../Errors

    # Public Blokk headers
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Include

    # Raylib
    ${CMAKE_SOURCE_DIR}/External/RayLibInclude
)