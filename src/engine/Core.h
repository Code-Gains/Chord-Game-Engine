#pragma once
#include "Log.h"
#include "WindowGLFW.h"

namespace Engine {
    class Core {
        std::unique_ptr<WindowGLFW> window;
        void InitWindow();
    public:
        Core() = default;
        void Init();
        void Run(); // main loop
        void Shutdown();
    };
} // namespace Engine