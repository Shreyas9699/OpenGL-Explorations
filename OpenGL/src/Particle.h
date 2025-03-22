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

struct Particle
{
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 velocity = glm::vec3(1.0f);
    glm::vec4 colorBegin = glm::vec4(glm::vec3(1.0f), 1.0f);
    glm::vec4 colorEnd = glm::vec4(glm::vec3(0.0f), 1.0f);
    float initialLife = 1.5f;
    float lifeRemaining = 0.0f;
    float sizeBegin = 1.0f;
    float sizeEnd = 1.0f;

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
          sizeBegin(sizeBegin),
          sizeEnd(sizeEnd) { }
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
