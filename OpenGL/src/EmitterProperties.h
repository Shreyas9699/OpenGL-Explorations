#pragma once
#include <glm/glm.hpp>

enum class EmitterShape
{
    POINT = 0,
    SPHERE = 1,
    CONE = 2,
    BOX = 3,
    CIRCLE = 4,
    HEMISPHERE = 5,
    TORUS = 6
};

struct EmitterProperties
{
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);       // Euler angles in degrees
    glm::vec3 scale = glm::vec3(1.0f);
    EmitterShape shape = EmitterShape::POINT;
    float radius = 1.0f;                        // For SPHERE and CONE
    float angle = 30.0f;                        // For CONE (in degrees)
    glm::vec3 dimensions = glm::vec3(1.0f);     // For BOX
    float torusInnerRadius = 1.0f;
    float torusOuterRadius = 3.0f;
    int emissionRate = 1000;
    float accumulatedTime = 0.0f;

    EmitterProperties() = default;

    EmitterProperties(glm::vec3 pos, glm::vec3 rotation, glm::vec3 scale, EmitterShape shape, float radius, float angle,
        glm::vec3 dia, int emissionRate)
        : position(pos),
        rotation(rotation),
        scale(scale),
        shape(shape),
        radius(radius),
        angle(angle),
        dimensions(dia),
        emissionRate(emissionRate) {}
};