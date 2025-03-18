#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

// MAX_ABSOLUTE_PARTICLES * MAX_ABSOLUTE_LIFESPAN => max particles alive (current under 0.9 mill for GTX 1050)
namespace ParticleConfig
{
    static constexpr unsigned int MAX_ABSOLUTE_PARTICLES = 60000;
    static constexpr unsigned int MAX_ABSOLUTE_LIFESPAN = 30;
};

enum class EmitterShape 
{
    POINT       = 0,
    SPHERE      = 1,
    CONE        = 2,
    BOX         = 3, 
    CIRCLE      = 4,
    HEMISPHERE  = 5,
    TORUS       = 6
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

    float torusInnerRadius = 0.5f;
    float torusOuterRadius = 1.0f;

    int emissionRate = 1000;
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
        emissionRate(emissionRate) {
    }
};

struct Particle
{
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 velocity = glm::vec3(1.0f);
    glm::vec4 colorBegin = glm::vec4(glm::vec3(1.0f), 1.0f);
    glm::vec4 colorEnd = glm::vec4(glm::vec3(0.0f), 1.0f);
    glm::vec4 color = glm::vec4(glm::vec3(1.0f), 1.0f);
    float initialLife = 1.5f;
    float lifeRemaining = 0.0f;
    bool Active = true;
    float sizeBegin = 1.0f;
    float sizeEnd = 1.0f;
    float size = 1.0f;

    Particle() = default;

    // Constructor with all parameters
    Particle(glm::vec3 position, glm::vec3 velocity, glm::vec4 colorBegin, glm::vec4 colorEnd, float lifeSpan,
        float lifeRemaining, bool active, float sizeBegin = 1.0f, float sizeEnd = 1.0f)
        : position(position),
          velocity(velocity),
          colorBegin(colorBegin),
          colorEnd(colorEnd),
          initialLife(lifeSpan),
          lifeRemaining(lifeRemaining),
          Active(active),
          sizeBegin(sizeBegin),
          sizeEnd(sizeEnd),
          size(sizeBegin) { }
};

// Struct for GPU particles(keep aligned to vec4 for better performance)
struct GPUParticle
{
    glm::vec4 position;    // xyz = position, w = size
    glm::vec4 velocity;    // xyz = velocity, w = lifeRemaining
    glm::vec4 colorBegin;
    glm::vec4 colorEnd;    // w = lifespan
};

// Indirect draw command structure
struct DrawElementsIndirectCommand 
{
    GLuint count;
    GLuint instanceCount;
    GLuint firstIndex;
    GLuint baseVertex;
    GLuint baseInstance;
};
