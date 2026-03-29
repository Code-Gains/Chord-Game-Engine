#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

struct PhysicsBody {
    float mass;
    glm::vec3 velocity;
};

struct Attractor {}; // a tag for imovable objects

