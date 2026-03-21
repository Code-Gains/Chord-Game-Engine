#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include "EcsDebugger.h"
#include "Transform.h"
#include "InputSystem.h"

EcsDebugger::EcsDebugger(entt::registry& registry) : System(registry)
{
}

EcsDebugger::~EcsDebugger()
{
}

void EcsDebugger::Update(float deltaTime)
{
	_fps = 1.0f / deltaTime;
}

void EcsDebugger::Draw()
{
	if (!_enabled)
		return;

    ImGui::Begin("ECS Debugger");
	ImGui::Text("Performance:");
	ImGui::Text("FPS: %.1f", _fps);
	ImGui::Separator();
    ImGui::Text("Transform entity count: %lld", _registry.view<Transform>().size()); //_ecs->GetEntityCount());
	 // Get input state (singleton example)
    auto view = _registry.view<InputState>();
    if (!view.empty()) {
        auto entity = *view.begin();
        auto& input = view.get<InputState>(entity);

        ImGui::Separator();
        ImGui::Text("Input:");

		ImGui::Text("Mouse Pos: (%.2f, %.2f)", input.mouseX, input.mouseY);
    	ImGui::Text("Mouse Delta: (%.2f, %.2f)", input.deltaX, input.deltaY);

        for (const auto& [key, state] : input.keys) {
            if (state.held || state.pressed || state.released) {
                ImGui::Text("Key %d | H:%d P:%d R:%d",
                    key,
                    state.held,
                    state.pressed,
                    state.released
                );
            }
        }
    }
	ImGui::End();
}

void EcsDebugger::Toggle()
{
	_enabled = !_enabled;
}