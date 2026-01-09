# vk-bootstrap library
add_library(vkbootstrap STATIC
    ${vkbootstrap_SOURCE_DIR}/src/VkBootstrap.cpp
)

target_include_directories(vkbootstrap
    PUBLIC
        ${vkbootstrap_SOURCE_DIR}/src
        ${VulkanSdkDir}/include
)


# Engine library
add_library(engine STATIC
    Core.cpp
    Renderer.cpp
    VulkanDevice.cpp
    WindowGLFW.cpp
)

target_compile_definitions(engine PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

# Include engine headers + dependencies
target_include_directories(engine
    PUBLIC
        ${VulkanSdkDir}/include         # Vulkan headers
        ${CMAKE_CURRENT_SOURCE_DIR}     # engine headers
        ${glfw_SOURCE_DIR}/include      # GLFW headers
        ${glm_SOURCE_DIR}               # GLM headers
)

# Vulkan library (cross-platform)
if(WIN32)
    set(VulkanLib "${VulkanSdkDir}/Lib/vulkan-1.lib")
elseif(UNIX)
    set(VulkanLib "${VulkanSdkDir}/lib/libvulkan.so")
endif()

# Link libraries
target_link_libraries(engine
    PUBLIC
        vkbootstrap
        glfw
        fmt::fmt
        ${VulkanLib}   # cross-platform Vulkan library
)
