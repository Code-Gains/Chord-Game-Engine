#pragma once

#include <string>
//#include "ECS.hpp"

/* Debugger class for Core ECS that provides a UI
   to visually diagnose problems, should be completely disabled in release config
*/
class EcsDebugger //: public System
{
    //ECS* _ecs;
    bool _enabled = true; // TODO SET FALSE

public:
    EcsDebugger();
    //ECSDebugger(ECS* ecs);
    ~EcsDebugger();

    //void Update(float deltaTime) override;
    //void PeriodicUpdate(float deltaTime) override;
    void Draw();

    void Toggle();

};