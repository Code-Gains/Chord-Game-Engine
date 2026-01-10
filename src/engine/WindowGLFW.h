#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VkBootstrap.h>
#include <vector>

namespace Engine {
    class WindowGLFW {
        GLFWwindow* _window = nullptr;

        uint32_t _width;
        uint32_t _height;
        bool _resized = false;
        void InitGLFW();

    public:
        // Control
        WindowGLFW(uint32_t width, uint32_t height, const char* title);
        ~WindowGLFW();
        void ShutdownGLFW();

        // non-copyable
        WindowGLFW(const WindowGLFW&) = delete;
        WindowGLFW& operator=(const WindowGLFW&) = delete;

        void PollEvents();
        bool ShouldClose() const;

        GLFWwindow* GetNativeHandle() const { return _window; };
        VkSurfaceKHR CreateAndGetWindowSurface(VkInstance instance);

        // Vulkan
        std::vector<const char*> GetRequiredVulkanExtensions() const;

        // Getters
        uint32_t GetWidth();
        uint32_t GetHeight();

        bool WasResized();
        void ResetResizedFlag();
    };
} // namespace engine