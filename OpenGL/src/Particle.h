#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

static constexpr unsigned int MAX_ABSOLUTE_PARTICLES = 1000000;
static constexpr unsigned int MAX_ABSOLUTE_LIFESPAN = 30;

enum class EmitterShape
{
    POINT,
    SPHERE,
    CONE,
    BOX
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

    int emissionRate = 1000;                // Particles per second
    float accumulatedTime = 0.0f;

    /*float MINLife;
    float MAXLife;*/

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

struct Particle
{
    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 Velocity = glm::vec3(1.0f);
    glm::vec4 colorBegin = glm::vec4(glm::vec3(1.0f), 1.0f);
    glm::vec4 colorEnd = glm::vec4(glm::vec3(0.0f), 1.0f);
    glm::vec4 color = glm::vec4(glm::vec3(1.0f), 1.0f);
    float lifeSpan = 1.5f;
    float LifeRemaining = 0.0f;
    bool Active = true;
    float sizeBegin = 1.0f;
    float sizeEnd = 1.0f;
    float currentSize = 1.0f;

    Particle() = default;

    // Constructor with all parameters
    Particle(glm::vec3 position, glm::vec3 velocity, glm::vec4 colorBegin, glm::vec4 colorEnd, float lifeSpan,
        float lifeRemaining, bool active, float sizeBegin = 1.0f, float sizeEnd = 1.0f)
        : Position(position),
          Velocity(velocity),
          colorBegin(colorBegin),
          colorEnd(colorEnd),
          lifeSpan(lifeSpan),
          LifeRemaining(lifeRemaining),
          Active(active),
          sizeBegin(sizeBegin),
          sizeEnd(sizeEnd),
          currentSize(sizeBegin) { }
};