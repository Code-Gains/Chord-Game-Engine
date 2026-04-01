#pragma once
#include "System.h"
#include "MeshComponent.h"
#include <random>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "PhysicsBody.h"
#include "Transform.h"

enum class SimulationType {
    Cube,
    TrilateralCube,
    Rings,
    DisplacedRings
};

class Simulation : public System {
    glm::quat RandomQuaternion(std::mt19937& rng) const;
    float RandomFloat(std::mt19937& rng, float x, float y) const;
    // exposed parameters
    PhysicsBody* _attractorPhysicsBody = nullptr;
    Transform* _attractorTransform = nullptr;
    std::shared_ptr<MeshAsset> _centerMesh;
    std::shared_ptr<MeshAsset> _particleMesh;
    SimulationType _currentSimulationType = SimulationType::Cube;
    float _timeScale = 1.0f;
    bool _pulseEnabled = false;
    float _pulseStrength = 10000.0f;

public:
    Simulation(entt::registry& registry);
    void Initialize(std::shared_ptr<MeshAsset> centerMesh, std::shared_ptr<MeshAsset> particleMesh);
    void Update(float deltaTime) override {};
    void FixedUpdate(float deltaTime) override;
    virtual void DrawUi() override;
    virtual void Draw() override;

    void CreateBalancedParticle(std::mt19937& rng, const glm::vec3& position, float mass);
    void CreateOrbitalDisk(std::mt19937& rng, int count, float innerRadius, float outerRadius, float thickness, float attractorMass, float verticalOffset);

    //void StartSimulation();
    void ResetSimulation();
    void InitializeCubeSimulation();
    void InitializeTrilateralCubeSimulation();
    void InitializeDisplacedRingsSimulation();
    void InitializeRingsSimulation();
};