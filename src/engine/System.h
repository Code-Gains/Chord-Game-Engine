#pragma once
#include <entt/entt.hpp>

class System {
public:
	System(entt::registry& registry) : _registry(registry) {}
	virtual ~System() = default;
	virtual void Update(float deltaTime) = 0;
	virtual void FixedUpdate(float deltaTime) = 0;
	virtual void DrawUi() = 0;
	virtual void Draw() = 0;

protected:
	entt::registry& _registry;
};
