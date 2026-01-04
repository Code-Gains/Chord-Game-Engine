#include "WindowGLFW.h"
#include <stdexcept>
#include <iostream>

namespace Engine {
    WindowGLFW::WindowGLFW(int width, int height, const char* title) {
        InitGLFW();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!window)
            throw std::runtime_error("Failed to create GLFW window");
        std::cout << "window created\n";
    }

    WindowGLFW::~WindowGLFW() {
        ShutdownGLFW();
    }

    void WindowGLFW::InitGLFW() {
        if (!glfwInit())
            throw std::runtime_error("Failed to initialize GLFW");
    }

    void WindowGLFW::ShutdownGLFW() {
        if (window) {
            glfwDestroyWindow(window);
            window = nullptr;
        }
        glfwTerminate();
    }

    void WindowGLFW::PollEvents() {
        glfwPollEvents();
    }

    bool WindowGLFW::ShouldClose() const {
        return glfwWindowShouldClose(window);
    }

    std::vector<const char*> WindowGLFW::GetRequiredVulkanExtensions() const {
        uint32_t count = 0;
        const char** extensions = glfwGetRequiredInstanceExtensions(&count);
        return std::vector<const char*>(extensions, extensions + count);
    }
} // namespace engine