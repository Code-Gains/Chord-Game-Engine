#pragma once

#include <string>
#include <entt/entt.hpp>
#include "System.h"
//#include "ECS.hpp"

/* Debugger class for Core ECS that provides a UI
   to visually diagnose problems, should be completely disabled in release config
*/
class EcsDebugger : public System {
    //ECS* _ecs;
    bool _enabled = true; // TODO SET FALSE
    float _fps = 0;

public:
    EcsDebugger(entt::registry& registry);
    //ECSDebugger(ECS* ecs);
    ~EcsDebugger();

    void Update(float deltaTime) override;
    void Draw() override;

    void Toggle();

};