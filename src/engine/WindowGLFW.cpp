#include "WindowGLFW.h"
#include <stdexcept>
#include <iostream>
#include <GLFW/glfw3.h>

#include "Log.h"

namespace Engine {
    WindowGLFW::WindowGLFW(int width, int height, const char* title) {
        _width = width;
        _height = height;
        InitGLFW();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        _window = glfwCreateWindow(_width, _height, title, nullptr, nullptr);
        if (!_window)
            throw std::runtime_error("Failed to create GLFW window");
        
        // hook for our window to track internal GLFW window size info
        glfwSetFramebufferSizeCallback(_window, [](GLFWwindow* window, int width, int height) {
            // Store the new size in the user pointer
            auto win = reinterpret_cast<Engine::WindowGLFW*>(glfwGetWindowUserPointer(window));
            if (win) {
                win->_width = width;
                win->_height = height;
                win->_resized = true; // flag for Core to handle
            }
        });
        glfwSetWindowUserPointer(_window, this);
        ENGINE_LOG_INFO("Window created");
    }

    WindowGLFW::~WindowGLFW() {
        ShutdownGLFW();
    }

    void WindowGLFW::InitGLFW() {
        if (!glfwInit())
            throw std::runtime_error("Failed to initialize GLFW");
    }

    void WindowGLFW::ShutdownGLFW() {
        if (_window) {
            glfwDestroyWindow(_window);
            _window = nullptr;
        }
        glfwTerminate();
    }

    void WindowGLFW::PollEvents() {
        glfwPollEvents();
    }

    bool WindowGLFW::ShouldClose() const {
        return glfwWindowShouldClose(_window);
    }

    VkSurfaceKHR WindowGLFW::CreateAndGetWindowSurface(VkInstance instance) {
        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface(instance, _window, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Vulkan surface");
        }
        return surface;
    }

    std::vector<const char*> WindowGLFW::GetRequiredVulkanExtensions() const {
        uint32_t count = 0;
        const char** extensions = glfwGetRequiredInstanceExtensions(&count);
        return std::vector<const char*>(extensions, extensions + count);
    }

    int Engine::WindowGLFW::GetWidth() {
        return _width;
    }

    int Engine::WindowGLFW::GetHeight() {
        return _height;
    }

    bool Engine::WindowGLFW::WasResized() {
        return _resized;
    }
    void Engine::WindowGLFW::ResetResizedFlag() {
        _resized = false;
    }
} // namespace engine