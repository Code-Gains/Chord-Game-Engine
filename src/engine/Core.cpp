#include <chrono>
#include <thread>
#include "Core.h"

namespace Engine {
    void Core::Run() {
        ENGINE_LOG_INFO("Starting Engine Main Loop.");
        bool running = true;
        int frame = 0;

        while (running && frame < 100) {
            ENGINE_LOG_INFO("Frame: " + std::to_string(frame));
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            frame++;
        }

        ENGINE_LOG_INFO("Engine Shutting Down.");
    }
}