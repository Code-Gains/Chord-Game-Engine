#pragma once
#include "System.h"
#include "MeshComponent.h"
#include <random>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class Simulation : public System {
    glm::quat RandomQuaternion(std::mt19937& rng) const;
public:
    Simulation(entt::registry& registry);
    void Initialize(std::shared_ptr<MeshAsset> centerMesh, std::shared_ptr<MeshAsset> particleMesh);
    void Update(float deltaTime) override;
    virtual void DrawUi() override {};
    virtual void Draw() override;
};