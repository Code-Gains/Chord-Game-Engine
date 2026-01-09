# commands to run
# cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DVulkanSdkDir="C:/VulkanSDK/1.4.335.0"    <-- Win64
# cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DVulkanSdkDir="$HOME/vulkan-sdk/x86_64"   <-- Linux
# cmake --build build --target editor

cmake_minimum_required(VERSION 3.26)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # generates compile_commands.json and allows text editor to see files
project(MyVulkanEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include(FetchContent)

# ---------------------
# Vulkan
# ---------------------
if(NOT DEFINED VulkanSdkDir)
    message(FATAL_ERROR "You must set -DVulkanSdkDir=<path to VulkanSDK>")
endif()

# ---------------------
# GLFW
# ---------------------
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG latest
)
FetchContent_MakeAvailable(glfw)

# ---------------------
# GLM
# ---------------------
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.8
)
FetchContent_MakeAvailable(glm)

# ---------------------
# VkBootstrap
# ---------------------
FetchContent_Declare(
    vkbootstrap
    GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap.git
    GIT_TAG main
)
FetchContent_MakeAvailable(vkbootstrap)

# ---------------------
# Add engine + editor
# ---------------------
add_subdirectory(src/engine)
add_subdirectory(src/editor)
