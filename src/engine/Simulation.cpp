#include "Simulation.h"
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include "Camera.h"
#include <mutex>
#include <future>
#include <thread>



glm::quat Simulation::RandomQuaternion(std::mt19937& rng) const {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    float u1 = dist(rng);
    float u2 = dist(rng);
    float u3 = dist(rng);

    float sqrt1MinusU1 = sqrt(1.0f - u1);
    float sqrtU1 = sqrt(u1);

    float theta1 = 2.0f * glm::pi<float>() * u2;
    float theta2 = 2.0f * glm::pi<float>() * u3;

    return glm::quat(
        sqrt1MinusU1 * sin(theta1),
        sqrt1MinusU1 * cos(theta1),
        sqrtU1 * sin(theta2),
        sqrtU1 * cos(theta2)
    );
}

// Returns a random float in [0, 1)
float Simulation::RandomFloat(std::mt19937& rng, float x, float y) const {
    std::uniform_real_distribution<float> dist(x, y);
    return dist(rng);
}

Simulation::Simulation(entt::registry &registry) : System(registry)
{
}

void Simulation::Initialize(std::shared_ptr<MeshAsset> centerMesh, std::shared_ptr<MeshAsset> particleMesh)
{
    _centerMesh = centerMesh;
    _particleMesh = particleMesh;
    ResetSimulation();
    // std::mt19937 rng(std::random_device{}());
    // // Cube shape
    // int gap = 5;
    // // particle instantiation
    // for (int x = 0; x < 100; x++) {
    //     for (int y = 0; y < 100; y++) {
    //         for (int z = 0; z < 100; z++) {
    //             auto particleEntity = _registry.create();
    //             _registry.emplace<MeshComponent>(particleEntity, particleMesh);
    //             auto transform = Transform();
    //             transform.position = glm::vec3 {x * gap + 100 + RandomFloat(rng, -2.5, 2.5), y * gap + 100 + RandomFloat(rng, -2.5, 2.5),  z * gap + 100 + RandomFloat(rng, -2.5, 2.5)};
    //             transform.rotation = RandomQuaternion(rng);
    //             _registry.emplace<Transform>(particleEntity, transform);
    //             auto physicsBody = PhysicsBody {1.0f, glm::vec3 {0.0f, 0.0f, -1.0f}};
    //             _registry.emplace<PhysicsBody>(particleEntity, physicsBody);
    //         }
    //     }
    // }

    // // center instantiation
    // auto planetEntity = _registry.create();
    // _registry.emplace<MeshComponent>(planetEntity, centerMesh);
    // auto transform = Transform();
    // transform.position = glm::vec3 {-0, -0,  -0};
    // transform.scale = glm::vec3 {0.1f, 0.1f,  0.1f};
    // auto& newTransformReference =  _registry.emplace<Transform>(planetEntity, transform);
    // _attractorTransform = &newTransformReference;
    // _registry.emplace<SingleRenderTag>(planetEntity);

    // auto physicsBody = PhysicsBody {30000.0f, glm::vec3 {0.0f, 0.0f, 0.0f}};
    // auto& newPhysicsBodyReference = _registry.emplace<PhysicsBody>(planetEntity, physicsBody);
    // _attractorPhysicsBody = &newPhysicsBodyReference;

    // auto attractor = Attractor();
    // _registry.emplace<Attractor>(planetEntity, attractor);


    //-----------------------------
    // Chaos Sphere

    // int numParticles = 1000000;
    // float radius = 500.0f;

    // for (int i = 0; i < numParticles; i++) {
    //     auto particleEntity = _registry.create();
    //     _registry.emplace<MeshComponent>(particleEntity, particleMesh);

    //     // Random point inside unit sphere
    //     glm::vec3 dir = glm::sphericalRand(1.0f); // glm helper
    //     float r = cbrt(RandomFloat(rng, 0.5, 1)) * radius; // cubic root for uniform volume
    //     glm::vec3 position = dir * r;
        
    //     Transform transform;
    //     transform.position = position;
    //     transform.rotation = RandomQuaternion(rng);
    //     _registry.emplace<Transform>(particleEntity, transform);

    //     PhysicsBody physicsBody;
    //     physicsBody.mass = 1.0f;
    //     //physicsBody.velocity = glm::normalize(position) * RandomFloat(rng, 0.5f, 2.0f);
    //     physicsBody.velocity = glm::vec3{
    //     RandomFloat(rng, -10000.0f, 10000.0f),
    //     RandomFloat(rng, -10000.0f, 10000.0f),
    //     RandomFloat(rng, -10000.0f, 10000.0f)
    //      };
    //     _registry.emplace<PhysicsBody>(particleEntity, physicsBody);
    // }


    // // center instantiation
    // auto planetEntity = _registry.create();
    // _registry.emplace<MeshComponent>(planetEntity, centerMesh);
    // auto transform = Transform();
    // transform.position = glm::vec3 {-0, -0,  -0};
    // transform.scale = glm::vec3 {0.1f, 0.1f,  0.1f};
    // _registry.emplace<Transform>(planetEntity, transform);
    // _registry.emplace<SingleRenderTag>(planetEntity);

    // auto physicsBody = PhysicsBody {30000.0f, glm::vec3 {0.0f, 0.0f, 0.0f}};
    // _registry.emplace<PhysicsBody>(planetEntity, physicsBody);

    // auto attractor = Attractor();
    // _registry.emplace<Attractor>(planetEntity, attractor);

    // // triangular cube split
    // int gap = 5;
    // // particle instantiation
    // int mainAxis = 0;
    // for (int x = 0; x < 100; x++) {
    //     for (int y = 0; y < 100; y++) {
    //         for (int z = 0; z < 100; z++) {
    //             auto particleEntity = _registry.create();
    //             _registry.emplace<MeshComponent>(particleEntity, particleMesh);
    //             auto transform = Transform();
    //             transform.position = glm::vec3 {x * gap + 100 + RandomFloat(rng, -2.5, 2.5), y * gap + 100 + RandomFloat(rng, -2.5, 2.5),  z * gap + 100 + RandomFloat(rng, -2.5, 2.5)};
    //             transform.rotation = RandomQuaternion(rng);
    //             _registry.emplace<Transform>(particleEntity, transform);
    //             PhysicsBody physicsBody;
    //             if (mainAxis == 0)
    //                 physicsBody = PhysicsBody {1.0f, glm::vec3 {2.0f  - x * 0.015f, 0.0f, 0.0f}};
    //             else if (mainAxis == 1)
    //                 physicsBody = PhysicsBody {1.0f, glm::vec3 {0.0f, 2.0f - y * 0.015f, 0.0f}};
    //             else if (mainAxis == 2)
    //                 physicsBody = PhysicsBody {1.0f, glm::vec3 {0.0f, 0.0f, 2.0f - z * 0.015f}};
    //             _registry.emplace<PhysicsBody>(particleEntity, physicsBody);
    //         }
    //     }
    //     mainAxis++;
    //     if (mainAxis > 2) {
    //         mainAxis = 0;
    //     }
    // }

    // // center instantiation
    // auto planetEntity = _registry.create();
    // _registry.emplace<MeshComponent>(planetEntity, centerMesh);
    // auto transform = Transform();
    // transform.position = glm::vec3 {-0, -0,  -0};
    // transform.scale = glm::vec3 {0.1f, 0.1f,  0.1f};
    // _registry.emplace<Transform>(planetEntity, transform);
    // _registry.emplace<SingleRenderTag>(planetEntity);

    // auto physicsBody = PhysicsBody {100000.0f, glm::vec3 {0.0f, 0.0f, 0.0f}};
    // _registry.emplace<PhysicsBody>(planetEntity, physicsBody);

    // auto attractor = Attractor();
    // _registry.emplace<Attractor>(planetEntity, attractor);
    // ------------------------------------------
    // int gap = 5;
    // // particle instantiation
    // for (int x = 0; x < 100; x++) {
    //     for (int y = 0; y < 100; y++) {
    //         for (int z = 0; z < 100; z++) {
    //             auto particleEntity = _registry.create();
    //             _registry.emplace<MeshComponent>(particleEntity, particleMesh);
    //             auto transform = Transform();
    //             transform.position = glm::vec3 {x * gap + 100 + RandomFloat(rng, -2.5, 2.5), y * gap + 100 + RandomFloat(rng, -2.5, 2.5),  z * gap + 100 + RandomFloat(rng, -2.5, 2.5)};
    //             transform.rotation = RandomQuaternion(rng);
    //             _registry.emplace<Transform>(particleEntity, transform);
    //             auto physicsBody = PhysicsBody {1.0f, glm::vec3 {0.0f, 0.0f, -1.0f}};
    //             _registry.emplace<PhysicsBody>(particleEntity, physicsBody);
    //         }
    //     }
    // }

    // int count = 1000000; // same amount as 100x100x100

    // float innerRadius = 200.0f;
    // float outerRadius = 250.0f;
    // float thickness   = 5.0f;

    // glm::vec3 center = {100.0f, 100.0f, 100.0f};

    // for (int i = 0; i < count; i++) {
    //     auto particleEntity = _registry.create();

    //     _registry.emplace<MeshComponent>(particleEntity, particleMesh);

    //     Transform transform;

    //     // --- ring sampling ---
    //     float angle = RandomFloat(rng, 0.0f, 2.0f * glm::pi<float>());

    //     // uniform distribution across ring area
    //     float r = RandomFloat(rng,
    //        innerRadius,
    //         outerRadius);

    //     float height = RandomFloat(rng, -thickness * 0.5f, thickness * 0.5f);

    //     transform.position = center + glm::vec3{
    //         cos(angle) * r,
    //         height,
    //         sin(angle) * r
    //     };

    //     transform.rotation = RandomQuaternion(rng);

    //     _registry.emplace<Transform>(particleEntity, transform);

    //     // --- velocity (example: outward from center) ---
    //     glm::vec3 radial = glm::normalize(transform.position - center);

    //     // tangent direction (orbit around Y axis)
    //     glm::vec3 tangent = glm::vec3(-radial.z, 0.0f, radial.x);

    //     // optional: randomize direction (clockwise / counterclockwise)
    //     if (RandomFloat(rng, 0.0f, 1.0f) > 0.5f)
    //         tangent = -tangent;

    //     PhysicsBody physicsBody{
    //         1.0f,
    //         tangent * 1.5f//RandomFloat(rng, 5.0f, 5.0f)
    //     };

    //     _registry.emplace<PhysicsBody>(particleEntity, physicsBody);
    // }

    // // center instantiation
    // auto planetEntity = _registry.create();
    // _registry.emplace<MeshComponent>(planetEntity, centerMesh);
    // auto transform = Transform();
    // transform.position = glm::vec3 {0, 0,  0};
    // transform.scale = glm::vec3 {0.1f, 0.1f,  0.1f};
    // _registry.emplace<Transform>(planetEntity, transform);
    // _registry.emplace<SingleRenderTag>(planetEntity);

    // auto physicsBody = PhysicsBody {30000.0f, glm::vec3 {0.0f, 0.0f, 0.0f}};
    // _registry.emplace<PhysicsBody>(planetEntity, physicsBody);

    // auto attractor = Attractor();
    // _registry.emplace<Attractor>(planetEntity, attractor);

}

void Simulation::FixedUpdate(float deltaTime)
{
    //deltaTime = std::min(deltaTime, 0.05f); // TODO ADD FIXED UPDATE TO ENGINE
    deltaTime *= _timeScale;
    // all attractors
    auto attractorRegistryView = _registry.view<Transform, PhysicsBody, Attractor>();
    // all particles
    auto particleRegistryView = _registry.view<Transform, PhysicsBody>(entt::exclude<Attractor>);
    // size_t numThreads = std::thread::hardware_concurrency();
    // size_t numParticles = 1000000;
    // size_t chunkSize = (numParticles + numThreads - 1) / numThreads;

    // auto& attractorTransform = attractorRegistryView.get<Transform>(*attractorRegistryView.begin());
    // auto& attractorPhysicsBody = attractorRegistryView.get<PhysicsBody>(*attractorRegistryView.begin());

    // std::vector<std::thread> threads;
    // auto itBegin = particleRegistryView.begin();

    // for (size_t t = 0; t < numThreads; ++t) {
    //     threads.emplace_back([&, t]() {
    //         size_t start = t * chunkSize;
    //         size_t end = std::min(start + chunkSize, numParticles);

    //         auto it = itBegin;
    //         std::advance(it, start);

    //         for (size_t i = start; i < end; ++i, ++it) {
    //             auto entity = *it;
    //             auto& particleTransform = particleRegistryView.get<Transform>(entity);
    //             auto& particlePhysicsBody = particleRegistryView.get<PhysicsBody>(entity);

    //             glm::vec3 distance = attractorTransform.position - particleTransform.position;
    //             float epsilon = 0.0001f;
    //             float distanceSquared = glm::dot(distance, distance) + epsilon;
    //             glm::vec3 forceDirection = glm::normalize(distance);

    //             float forceStrength = attractorPhysicsBody.mass / distanceSquared;

    //             particlePhysicsBody.velocity += forceDirection * forceStrength * deltaTime;
    //         }
    //     });
    // }

    //for (auto& t : threads) t.join();


    for (auto attractorEntity : attractorRegistryView) {
        auto& attractorTransform = attractorRegistryView.get<Transform>(attractorEntity);
        auto& attractorPhysicsBody = attractorRegistryView.get<PhysicsBody>(attractorEntity);

        for (auto particleEntity : particleRegistryView) {
            auto& particleTransform = particleRegistryView.get<Transform>(particleEntity);
            auto& particlePhysicsBody = particleRegistryView.get<PhysicsBody>(particleEntity);

            glm::vec3 distanceVec = attractorTransform.position - particleTransform.position;
            // F = G * m1 * m2 / R^2 simplified -> M/R^2
            float epsilon = 0.001f;
            float distanceSquared = glm::dot(distanceVec, distanceVec) + epsilon; // add a little to avoid div by 0
            glm::vec3 forceDirection = glm::normalize(distanceVec);

            float forceStrength = attractorPhysicsBody.mass / distanceSquared;

            if (_pulseEnabled) {
                float distance = glm::length(distanceVec) + 0.001f;
                float pulseForce = _pulseStrength / distance;
                auto pulseDirection = -forceDirection;
                particlePhysicsBody.velocity += pulseDirection * pulseForce;
            }

            particlePhysicsBody.velocity += forceDirection * forceStrength * deltaTime;
        }
    }
    _pulseEnabled = false;
    for (auto entity : particleRegistryView) {
        auto& transform = particleRegistryView.get<Transform>(entity);
        auto& physicsBody = particleRegistryView.get<PhysicsBody>(entity);

        transform.position += physicsBody.velocity * deltaTime;// {0.1f, 0.1f, 0.1f};//physicsBody.velocity * deltaTime;
    }


}

void Simulation::DrawUi()
{
    ImGui::Begin("Simulation");
	ImGui::Text("Controls:");
    ImGui::Text("Movement: WASD");
    ImGui::Text("Shortcuts:");
    ImGui::Text("Maximize: Alt + Enter");
    ImGui::Separator();
    ImGui::Text("Live Simulation Parameters:");
    ImGui::Text("Time Scale");
    ImGui::SliderFloat("##TimeScale", &_timeScale, 1.0f, 100.0f);
    if (_attractorPhysicsBody) {
        
        ImGui::Text("Attractor Mass");
        ImGui::SliderFloat("##AttractorMass", &_attractorPhysicsBody->mass, 1000.f, 200000.0f);
    }
    if (_attractorTransform) {
        ImGui::Text("Attractor Position");
        ImGui::SliderFloat3("##AttractorPosition", glm::value_ptr(_attractorTransform->position), -10.f, 10.f);
    }

    // Camera clear color
    auto registryView = _registry.view<Camera>();
    
    if (!registryView.empty()) {
        auto cameraEntity = *registryView.begin();
        auto& camera = registryView.get<Camera>(cameraEntity);
        ImGui::Text("Clear Color");
        ImGui::ColorEdit4("##ClearColor", glm::value_ptr(camera.clearColor));
    }
    ImGui::Separator();

    ImGui::Text("Simulation Type");
    const char* simTypes[] = { "Cube", "Trilateral Cube", "Rings", "Displaced Rings" }; // add more later
    int currentTypeIndex = static_cast<int>(_currentSimulationType);
    if (ImGui::Combo("##SimulationType", &currentTypeIndex, simTypes, IM_ARRAYSIZE(simTypes))) {
        _currentSimulationType = static_cast<SimulationType>(currentTypeIndex);
    }

    ImGui::Separator();
    ImGui::Text("Pulse Settings");

    // Enable/disable pulse
    ImGui::Checkbox("Enable Pulse", &_pulseEnabled);

    // Pulse strength (repulsion magnitude)
    ImGui::SliderFloat("Pulse Strength", &_pulseStrength, 0.0f, 10000.0f);
    // Reset button
    if (ImGui::Button("Reset Simulation")) {
        ResetSimulation();  // call your reset function
    }

	ImGui::End();
}

void Simulation::Draw()
{
}

void Simulation::CreateBalancedParticle(std::mt19937 &rng, const glm::vec3 &position, float mass)
{
    auto particleEntity = _registry.create();
    _registry.emplace<MeshComponent>(particleEntity, _particleMesh);
    auto transform = Transform();
    transform.position = position;
    transform.rotation = RandomQuaternion(rng);
    _registry.emplace<Transform>(particleEntity, transform);

    glm::vec3 radiusVector = transform.position - glm::vec3{0.0f,0.0f,0.0f};
    float r = glm::length(radiusVector);
    float v = std::sqrt(mass / r);

    glm::vec3 up = glm::abs(radiusVector.y) < 0.99f ? glm::vec3(0,1,0) : glm::vec3(1,0,0);
    glm::vec3 tangentialDir = glm::normalize(glm::cross(radiusVector, up));
    auto physicsBody = PhysicsBody {1.0f, tangentialDir * v};
    _registry.emplace<PhysicsBody>(particleEntity, physicsBody);
}

void Simulation::CreateOrbitalDisk(std::mt19937 &rng, int count, float innerRadius, float outerRadius, float thickness, float attractorMass, float verticalOffset)
{
    std::uniform_real_distribution<float> angleDist(0.0f, glm::two_pi<float>());
    std::uniform_real_distribution<float> dist01(0.0f, 1.0f);

    for (int i = 0; i < count; i++) {
        float angle = angleDist(rng);

        // sample radius PER particle
        float t = dist01(rng);
        float radius = sqrt(innerRadius*innerRadius + t * (outerRadius*outerRadius - innerRadius*innerRadius));

        glm::vec3 pos;
        pos.x = cos(angle) * radius;
        pos.z = sin(angle) * radius;

        pos.y = verticalOffset;

        CreateBalancedParticle(rng, pos, attractorMass);
    }
}

void Simulation::ResetSimulation()
{
    // Clear all particles
    auto particleRegistryView = _registry.view<Transform, PhysicsBody>(entt::exclude<Attractor>);
    for (auto entity : particleRegistryView) {
        _registry.destroy(entity);
    }

    // Clear all attractors
    auto attractorRegistryView = _registry.view<Transform, PhysicsBody, Attractor>();
    for (auto entity : attractorRegistryView) {
        _registry.destroy(entity);
    }

    // Reset cached pointers
    _attractorPhysicsBody = nullptr;
    _attractorTransform = nullptr;


    switch (_currentSimulationType) {
    case SimulationType::Cube:
        InitializeCubeSimulation();
        break;
    case SimulationType::TrilateralCube:
        InitializeTrilateralCubeSimulation();
        break;
    case SimulationType::Rings:
        InitializeRingsSimulation();
        break;
    case SimulationType::DisplacedRings:
        InitializeDisplacedRingsSimulation();
        break;
}

}

void Simulation::InitializeCubeSimulation()
{
    std::mt19937 rng(std::random_device{}());
    // Cube shape
    int gap = 5;
    float particleSpeed = 5.0f;
    // particle instantiation
    for (int x = 0; x < 100; x++) {
        for (int y = 0; y < 100; y++) {
            for (int z = 0; z < 100; z++) {
                auto particleEntity = _registry.create();
                _registry.emplace<MeshComponent>(particleEntity, _particleMesh);
                auto transform = Transform();
                transform.position = glm::vec3 {x * gap + 100 + RandomFloat(rng, -2.5, 2.5), y * gap + 100 + RandomFloat(rng, -2.5, 2.5),  z * gap + 100 + RandomFloat(rng, -2.5, 2.5)};
                transform.rotation = RandomQuaternion(rng);
                _registry.emplace<Transform>(particleEntity, transform);
                auto physicsBody = PhysicsBody {1.0f, glm::vec3 {0.0f, 0.0f, -particleSpeed}};
                _registry.emplace<PhysicsBody>(particleEntity, physicsBody);
            }
        }
    }

    // center instantiation
    auto planetEntity = _registry.create();
    _registry.emplace<MeshComponent>(planetEntity, _centerMesh);
    auto transform = Transform();
    transform.position = glm::vec3 {-0, -0,  -0};
    transform.scale = glm::vec3 {0.1f, 0.1f,  0.1f};
    auto& newTransformReference =  _registry.emplace<Transform>(planetEntity, transform);
    _attractorTransform = &newTransformReference;
    _registry.emplace<SingleRenderTag>(planetEntity);

    auto physicsBody = PhysicsBody {30000.0f, glm::vec3 {0.0f, 0.0f, 0.0f}};
    auto& newPhysicsBodyReference = _registry.emplace<PhysicsBody>(planetEntity, physicsBody);
    _attractorPhysicsBody = &newPhysicsBodyReference;

    auto attractor = Attractor();
    _registry.emplace<Attractor>(planetEntity, attractor);
}

void Simulation::InitializeTrilateralCubeSimulation()
{
    std::mt19937 rng(std::random_device{}());
    // triangular cube split
    int gap = 5;
    float particleSpeed = 5.0f;
    // particle instantiation
    int mainAxis = 0;
    for (int x = 50; x < 100; x++) {
        for (int y = 50; y < 100; y++) {
            for (int z = 50; z < 100; z++) {
                auto particleEntity = _registry.create();
                _registry.emplace<MeshComponent>(particleEntity, _particleMesh);
                auto transform = Transform();
                transform.position = glm::vec3 {x * gap + 100 + RandomFloat(rng, -2.5, 2.5), y * gap + 100 + RandomFloat(rng, -2.5, 2.5),  z * gap + 100 + RandomFloat(rng, -2.5, 2.5)};
                transform.rotation = RandomQuaternion(rng);
                _registry.emplace<Transform>(particleEntity, transform);
                PhysicsBody physicsBody;
                if (mainAxis == 0)
                    physicsBody = PhysicsBody {1.0f, glm::vec3 {particleSpeed  - x * 0.015f, 0.0f, 0.0f}};
                else if (mainAxis == 1)
                    physicsBody = PhysicsBody {1.0f, glm::vec3 {0.0f, particleSpeed - y * 0.015f, 0.0f}};
                else if (mainAxis == 2)
                    physicsBody = PhysicsBody {1.0f, glm::vec3 {0.0f, 0.0f, particleSpeed - z * 0.015f}};
                _registry.emplace<PhysicsBody>(particleEntity, physicsBody);
            }
        }
        mainAxis++;
        if (mainAxis > 2) {
            mainAxis = 0;
        }
    }

    // center instantiation
    auto planetEntity = _registry.create();
    _registry.emplace<MeshComponent>(planetEntity, _centerMesh);
    auto transform = Transform();
    transform.position = glm::vec3 {-0, -0,  -0};
    transform.scale = glm::vec3 {0.1f, 0.1f,  0.1f};
    auto& newTransformReference = _registry.emplace<Transform>(planetEntity, transform);
    _attractorTransform = &newTransformReference;
    _registry.emplace<SingleRenderTag>(planetEntity);

    auto physicsBody = PhysicsBody {30000.0f, glm::vec3 {0.0f, 0.0f, 0.0f}};
    auto& newPhysicsBodyReference = _registry.emplace<PhysicsBody>(planetEntity, physicsBody);
    _attractorPhysicsBody = &newPhysicsBodyReference;

    auto attractor = Attractor();
    _registry.emplace<Attractor>(planetEntity, attractor);
}

void Simulation::InitializeDisplacedRingsSimulation()
{
    std::mt19937 rng(std::random_device{}());
    float attractorMass = 1000.0f;
    // CreateOrbitalDisk(rng, 10000, 100.0f, 150.0f, 10.0f, attractorMass);
    // CreateOrbitalDisk(rng, 10000, 200.0f, 250.0f, 10.0f, attractorMass);
    // CreateOrbitalDisk(rng, 10000, 300.0f, 350.0f, 10.0f, attractorMass);
    // CreateOrbitalDisk(rng, 10000, 400.0f, 450.0f, 10.0f, attractorMass);
    // CreateOrbitalDisk(rng, 10000, 500.0f, 550.0f, 10.0f, attractorMass);

    CreateOrbitalDisk(rng, 10000, 50.0f, 70.0f, 10.0f, attractorMass, 20.0f);
    CreateOrbitalDisk(rng, 10000, 80.0f, 100.0f, 10.0f, attractorMass, 20.0f);
    CreateOrbitalDisk(rng, 10000, 110.0f, 130.0f, 10.0f, attractorMass, 20.0f);
    CreateOrbitalDisk(rng, 10000, 140.0f, 160.0f, 10.0f, attractorMass, 20.0f);
    CreateOrbitalDisk(rng, 10000, 170.0f, 190.0f, 10.0f, attractorMass, 20.0f);
    //CreateBalancedParticle(rng, glm::vec3 { 20.0f, 20.0f, 20.0f }, attractorMass);
    // auto particleEntity = _registry.create();
    // _registry.emplace<MeshComponent>(particleEntity, _particleMesh);
    // auto t = Transform();
    // t.position = glm::vec3 { 20.0f, 20.0f, 20.0f };
    // t.rotation = RandomQuaternion(rng);
    // _registry.emplace<Transform>(particleEntity, t);

    // glm::vec3 radiusVector = t.position - glm::vec3{0.0f,0.0f,0.0f};
    // float r = glm::length(radiusVector);
    // float v = std::sqrt(1000.0f / r);

    // glm::vec3 up = glm::abs(radiusVector.y) < 0.99f ? glm::vec3(0,1,0) : glm::vec3(1,0,0);
    // glm::vec3 tangentialDir = glm::normalize(glm::cross(radiusVector, up));
    // auto pb = PhysicsBody {1.0f, tangentialDir * v};
    // _registry.emplace<PhysicsBody>(particleEntity, pb);

     // center instantiation
    auto planetEntity = _registry.create();
    _registry.emplace<MeshComponent>(planetEntity, _centerMesh);
    auto transform = Transform();
    transform.position = glm::vec3 {0.0f, 0.0f, 0.0f};
    transform.scale = glm::vec3 {1.0f, 1.0f,  1.0f};
    auto& newTransformReference =  _registry.emplace<Transform>(planetEntity, transform);
    _attractorTransform = &newTransformReference;
    _registry.emplace<SingleRenderTag>(planetEntity);

    auto physicsBody = PhysicsBody {attractorMass, glm::vec3 {0.0f, 0.0f, 0.0f}};
    auto& newPhysicsBodyReference = _registry.emplace<PhysicsBody>(planetEntity, physicsBody);
    _attractorPhysicsBody = &newPhysicsBodyReference;

    auto attractor = Attractor();
    _registry.emplace<Attractor>(planetEntity, attractor);
}

void Simulation::InitializeRingsSimulation()
{
    std::mt19937 rng(std::random_device{}());
    float attractorMass = 1000.0f;
    // CreateOrbitalDisk(rng, 10000, 100.0f, 150.0f, 10.0f, attractorMass);
    // CreateOrbitalDisk(rng, 10000, 200.0f, 250.0f, 10.0f, attractorMass);
    // CreateOrbitalDisk(rng, 10000, 300.0f, 350.0f, 10.0f, attractorMass);
    // CreateOrbitalDisk(rng, 10000, 400.0f, 450.0f, 10.0f, attractorMass);
    // CreateOrbitalDisk(rng, 10000, 500.0f, 550.0f, 10.0f, attractorMass);

    CreateOrbitalDisk(rng, 5000, 50.0f, 70.0f, 10.0f, attractorMass, 0.0f);
    CreateOrbitalDisk(rng, 10000, 80.0f, 100.0f, 10.0f, attractorMass, 0.0f);
    CreateOrbitalDisk(rng, 10000, 110.0f, 130.0f, 10.0f, attractorMass, 0.0f);
    CreateOrbitalDisk(rng, 10000, 140.0f, 160.0f, 10.0f, attractorMass, 0.0f);
    CreateOrbitalDisk(rng, 10000, 170.0f, 190.0f, 10.0f, attractorMass, 0.0f);
    //CreateBalancedParticle(rng, glm::vec3 { 20.0f, 20.0f, 20.0f }, attractorMass);
    // auto particleEntity = _registry.create();
    // _registry.emplace<MeshComponent>(particleEntity, _particleMesh);
    // auto t = Transform();
    // t.position = glm::vec3 { 20.0f, 20.0f, 20.0f };
    // t.rotation = RandomQuaternion(rng);
    // _registry.emplace<Transform>(particleEntity, t);

    // glm::vec3 radiusVector = t.position - glm::vec3{0.0f,0.0f,0.0f};
    // float r = glm::length(radiusVector);
    // float v = std::sqrt(1000.0f / r);

    // glm::vec3 up = glm::abs(radiusVector.y) < 0.99f ? glm::vec3(0,1,0) : glm::vec3(1,0,0);
    // glm::vec3 tangentialDir = glm::normalize(glm::cross(radiusVector, up));
    // auto pb = PhysicsBody {1.0f, tangentialDir * v};
    // _registry.emplace<PhysicsBody>(particleEntity, pb);

     // center instantiation
    auto planetEntity = _registry.create();
    _registry.emplace<MeshComponent>(planetEntity, _centerMesh);
    auto transform = Transform();
    transform.position = glm::vec3 {0.0f, 0.0f, 0.0f};
    transform.scale = glm::vec3 {1.0f, 1.0f,  1.0f};
    auto& newTransformReference =  _registry.emplace<Transform>(planetEntity, transform);
    _attractorTransform = &newTransformReference;
    _registry.emplace<SingleRenderTag>(planetEntity);

    auto physicsBody = PhysicsBody {attractorMass, glm::vec3 {0.0f, 0.0f, 0.0f}};
    auto& newPhysicsBodyReference = _registry.emplace<PhysicsBody>(planetEntity, physicsBody);
    _attractorPhysicsBody = &newPhysicsBodyReference;

    auto attractor = Attractor();
    _registry.emplace<Attractor>(planetEntity, attractor);
}
