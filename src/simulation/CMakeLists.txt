add_executable(simulation
    Simulation.cpp
)

target_compile_definitions(simulation PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(simulation
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..       # points to src/
        ${CMAKE_CURRENT_SOURCE_DIR}/../engine # engine headers
        $ENV{VULKAN_SDK}/Include            # Vulkan headers
)

target_link_libraries(simulation
    PRIVATE 
        engine
        imgui
)


# -----------------------------
# Run target
# -----------------------------
add_custom_target(run-simulation
    COMMAND $<TARGET_FILE:simulation>
    DEPENDS simulation
    WORKING_DIRECTORY $<TARGET_FILE_DIR:simulation>
)