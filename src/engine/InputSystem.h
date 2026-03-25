#pragma once
#include <unordered_map>
#include "WindowGLFW.h"
#include "System.h"


struct KeyState {
    bool pressed = false;   // this frame
    bool held = false;      // is being held down
    bool released = false;  // released this frame
};

struct InputState {
    std::unordered_map<int, KeyState> keys;
    double mouseX = 0.0;
    double mouseY = 0.0;

    double deltaX = 0.0;
    double deltaY = 0.0;
};

class InputSystem : public System {
public:
    InputSystem(entt::registry& registry, entt::entity inputEntity, Engine::WindowGLFW* engineWindow);
    void Update(float deltaTime) override;
    void Draw() override {};
    void AddKeyToMonitor(int key) { _monitoredKeys.push_back(key); }

private:
    std::vector<int> _monitoredKeys;
    entt::entity _inputEntity;
    GLFWwindow* _window;
    Engine::WindowGLFW* _engineWindow;
};