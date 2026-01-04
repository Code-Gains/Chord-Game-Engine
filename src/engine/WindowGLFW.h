#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <vector>

namespace Engine {
    class WindowGLFW {
        GLFWwindow* window = nullptr;
        void InitGLFW();
        void ShutdownGLFW();

    public:
        WindowGLFW(int width, int height, const char* title);
        ~WindowGLFW();

        // non-copyable
        WindowGLFW(const WindowGLFW&) = delete;
        WindowGLFW& operator=(const WindowGLFW&) = delete;

        void PollEvents();
        bool ShouldClose() const;

        GLFWwindow* GetNativeHandle() const { return window; };

        // Vulkan
        std::vector<const char*> GetRequiredVulkanExtensions() const;
    };
} // namespace engine