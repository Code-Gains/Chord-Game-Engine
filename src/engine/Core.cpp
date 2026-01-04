#include <chrono>
#include <thread>
#include "Core.h"

namespace Engine {

    void Core::InitWindow() {
        window = std::make_unique<WindowGLFW>(1280, 720, "Engine");
    }
    void Core::Init() {
         InitWindow();
    }

    void Core::Run() {
        ENGINE_LOG_INFO("Starting Engine Main Loop.");
        bool running = true;
        while (!window->ShouldClose()) {
            window->PollEvents();
        }
        ENGINE_LOG_INFO("Engine Shutting Down.");
    }

    void Core::Shutdown() {
        window.reset();
    }
}