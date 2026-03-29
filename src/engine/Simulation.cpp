#include "Simulation.h"
#include "Transform.h"
#include "PhysicsBody.h"
#include <iostream>



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

Simulation::Simulation(entt::registry &registry) : System(registry)
{
}

void Simulation::Initialize(std::shared_ptr<MeshAsset> centerMesh, std::shared_ptr<MeshAsset> particleMesh)
{
    std::mt19937 rng(std::random_device{}());
    int gap = 5;
    // particle instantiation
    for (int x = 0; x < 100; x++) {
        for (int y = 0; y < 100; y++) {
            for (int z = 0; z < 100; z++) {
                auto particleEntity = _registry.create();
                _registry.emplace<MeshComponent>(particleEntity, particleMesh);
                auto transform = Transform();
                transform.position = glm::vec3 {x * gap + 50, y * gap + 50,  z * gap + 50};
                transform.rotation = RandomQuaternion(rng);
                _registry.emplace<Transform>(particleEntity, transform);
                auto physicsBody = PhysicsBody {1.0f, glm::vec3 {0.0f, 0.0f, -1.0f}};
                _registry.emplace<PhysicsBody>(particleEntity, physicsBody);
            }
        }
    }
    // center instantiation
    auto planetEntity = _registry.create();
    _registry.emplace<MeshComponent>(planetEntity, centerMesh);
    auto transform = Transform();
    transform.position = glm::vec3 {-0, -0,  -0};
    transform.scale = glm::vec3 {0.1f, 0.1f,  0.1f};
    _registry.emplace<Transform>(planetEntity, transform);
    _registry.emplace<SingleRenderTag>(planetEntity);

    auto physicsBody = PhysicsBody {20000.0f, glm::vec3 {0.0f, 0.0f, 0.0f}};
    _registry.emplace<PhysicsBody>(planetEntity, physicsBody);

    auto attractor = Attractor();
    _registry.emplace<Attractor>(planetEntity, attractor);

}

void Simulation::Update(float deltaTime)
{
    // all attractors
    auto attractorRegistryView = _registry.view<Transform, PhysicsBody, Attractor>();
    // all particles
    auto particleRegistryView = _registry.view<Transform, PhysicsBody>(entt::exclude<Attractor>);

    for (auto attractorEntity : attractorRegistryView) {
        auto& attractorTransform = attractorRegistryView.get<Transform>(attractorEntity);
        auto& attractorPhysicsBody = attractorRegistryView.get<PhysicsBody>(attractorEntity);

        for (auto particleEntity : particleRegistryView) {
            auto& particleTransform = particleRegistryView.get<Transform>(particleEntity);
            auto& particlePhysicsBody = attractorRegistryView.get<PhysicsBody>(particleEntity);

            glm::vec3 distance = attractorTransform.position - particleTransform.position;
            glm::vec3 forceDirection = glm::normalize(distance);
            // F = G* m1 * m2 / R^2 TODO  add g
            float epsilon = 0.0001f;
            float distanceSquared = glm::dot(distance, distance) + epsilon; // add a little to avoid div by 0

            float forceStrength = attractorPhysicsBody.mass / distanceSquared;

            particlePhysicsBody.velocity += forceDirection * forceStrength * deltaTime;
            //std::cout << particlePhysicsBody.velocity.x << std::endl;
        }
    }

    for (auto entity : particleRegistryView) {
        auto& transform = particleRegistryView.get<Transform>(entity);
        auto& physicsBody = attractorRegistryView.get<PhysicsBody>(entity);

        transform.position += physicsBody.velocity;// {0.1f, 0.1f, 0.1f};//physicsBody.velocity * deltaTime;
    }


}

void Simulation::Draw()
{
}
