#pragma once
#include "System.h"
#include <entt/entt.hpp>

class CameraSystem : public System {
public:
    CameraSystem(entt::registry& registry);
    void Update(float deltaTime) override;
    void Draw() override {};
};