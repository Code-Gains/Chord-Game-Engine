#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

struct Transform {  
    glm::vec3 position { 0.0f, 0.0f, 0.0f };
    glm::quat rotation { 1.0f, 0.0f, 0.0f, 0.0f };
    glm::vec3 scale { 1.0f, 1.0f, 1.0f };

    // Returns the 4x4 model matrix for rendering
    glm::mat4 GetModelMatrix() const {
        glm::mat4 t = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 r = glm::toMat4(rotation);
        glm::mat4 s = glm::scale(glm::mat4(1.0f), scale);
        return t * r * s; // Note: TRS order
    }

    // Helper: rotate around axis
    void Rotate(const glm::vec3& axis, float angleRadians) {
        rotation = glm::normalize(glm::angleAxis(angleRadians, glm::normalize(axis)) * rotation);
    }

    // Helper: move in local space
    void TranslateLocal(const glm::vec3& delta) {
        position += rotation * delta;
    }
};