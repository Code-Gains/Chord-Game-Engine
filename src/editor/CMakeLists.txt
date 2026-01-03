add_executable(editor
    Editor.cpp
)

target_include_directories(editor
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..       # points to src/
        ${CMAKE_CURRENT_SOURCE_DIR}/../engine # engine headers
        $ENV{VULKAN_SDK}/Include            # Vulkan headers
)

target_link_libraries(editor
    PRIVATE engine
)
