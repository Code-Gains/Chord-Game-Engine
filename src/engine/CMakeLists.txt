add_library(engine STATIC
    Core.cpp
    Renderer.cpp
    VulkanDevice.cpp
)

# Include engine headers + dependencies
target_include_directories(engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}          # engine headers
        ${glfw_SOURCE_DIR}/include           # GLFW headers
        ${glm_SOURCE_DIR}                    # GLM headers
        ${VulkanSdkDir}/Include              # Vulkan headers
        ${vkbootstrap_SOURCE_DIR}/src       # VkBootstrap headers
)

message(STATUS "Engine include directories:")
get_target_property(engine_includes engine INCLUDE_DIRECTORIES)
foreach(dir ${engine_includes})
    message(STATUS "  ${dir}")
endforeach()

# Link libraries
target_link_libraries(engine
    PUBLIC
        ${VulkanSdkDir}/Lib/vulkan-1.lib
        glfw
)
