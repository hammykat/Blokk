add_library(Blokk STATIC
    ../GameObjects/Frames+Animation.cpp
    ../GameObjects/Position.cpp
    ../GameObjects/Velocity.cpp
    ../GameObjects/Visibility.cpp

    IncrementFrames.cpp
    Threading.cpp
    UserUpdates.cpp
    Velocities.cpp
    Visibility_AxisCulling.cpp
    Visibility_BasicCulling.cpp
)

target_include_directories(Blokk
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../Include
        ${CMAKE_CURRENT_SOURCE_DIR}/../GameObjects
        ${CMAKE_CURRENT_SOURCE_DIR}/../Helpers
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Blokk
    PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
)