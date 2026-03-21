#include "InputSystem.h"
#include <iostream>


InputSystem::InputSystem(entt::registry& registry, entt::entity inputEntity, GLFWwindow* window) : System(registry), _inputEntity(inputEntity), _window(window) {
    _monitoredKeys = {
        GLFW_KEY_W,
        GLFW_KEY_A,
        GLFW_KEY_S,
        GLFW_KEY_D,
        GLFW_KEY_SPACE
    };
}

void InputSystem::Update(float deltaTime)
{
    auto & inputState = _registry.get<InputState>(_inputEntity);

    double x, y;
    glfwGetCursorPos(_window, &x, &y);

    inputState.deltaX = x - inputState.mouseX;
    inputState.deltaY = y - inputState.mouseY;

    inputState.mouseX = x;
    inputState.mouseY = y;
    for (int key : _monitoredKeys) {
        KeyState& state = inputState.keys[key];

        bool currentlyPressed = glfwGetKey(_window, key) == GLFW_PRESS;

        state.pressed  = currentlyPressed && !state.held;
        state.released = !currentlyPressed && state.held;
        state.held     = currentlyPressed;
    }
}

// void InputSystem::Update(InputState &input, GLFWwindow *window)
// {
//     for (int key : _monitoredKeys) {
//         KeyState& state = input.keys[key];

//         bool currentlyPressed = glfwGetKey(window, key) == GLFW_PRESS;

//         state.pressed  = currentlyPressed && !state.held;
//         state.released = !currentlyPressed && state.held;
//         state.held     = currentlyPressed;
//     }
// }