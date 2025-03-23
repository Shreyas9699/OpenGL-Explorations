// Base Particle system class
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <unordered_map>
#include <functional>
#include <queue>

#include "EmitterProperties.h"
#include "Particle.h"
#include "Shader_t.h"
#include "Camera.h"

// Base Class for Particle System (Common Functionality for CPU & GPU)
class ParticleSystemBase
{
protected:
    unsigned int id = 0;
    size_t m_ActiveParticleCount = 0;
    bool isBegin = false;
    float lastCreateTime = 0.0f;

    // Particle Properties (Shared Across CPU & GPU)
    glm::vec3 m_GlobalForce = glm::vec3(0.0f);
    bool m_UseForces = false;
    glm::vec4 m_DefaultColorBegin = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 m_DefaultColorEnd = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    float m_DefaultSizeBegin = 1.0f;
    float m_DefaultSizeEnd = 1.0f;
    float m_DefaultLifespan = 5.0f;

    // Emitter Properties
    EmitterProperties m_Emitter;

public:
    ParticleSystemBase() = default;
    virtual ~ParticleSystemBase() = default;

    virtual void Update(float delta, const glm::mat4& viewProj = glm::mat4(0.0f)) = 0;
    virtual void Render(Shader& shader) = 0;

    // Setters
    virtual void SetEmitter(const EmitterProperties& emitterProp) = 0;
    virtual void SetEmissionRate(int rate) = 0;
    void SetForce(const glm::vec3& force) { m_GlobalForce = force; }
    void SetEmitterPosition(const glm::vec3& position) { m_Emitter.position = position; }
    void SetEmitterRotation(const glm::vec3& rotation) { m_Emitter.rotation = rotation; }
    void SetEmitterShape(EmitterShape shape) { m_Emitter.shape = shape; }
    void SetEmitterRadius(float radius) { m_Emitter.radius = radius; }
    void SetEmitterAngle(float angle) { m_Emitter.angle = angle; }
    void SetEmitterDimensions(const glm::vec3& dimensions) { m_Emitter.dimensions = dimensions; }
    void SetEmitterTorusInnerRadius(const float innerRadius) { m_Emitter.torusInnerRadius = innerRadius; }
    void SetEmitterTorusOuterRadius(const float outterRadius) { m_Emitter.torusOuterRadius = outterRadius; }

    void SetParticleColorBegin(const glm::vec4& color) { m_DefaultColorBegin = color; }
    void SetParticleColorEnd(const glm::vec4& color) { m_DefaultColorEnd = color; }
    void SetParticleSizeBegin(float size) { m_DefaultSizeBegin = size; }
    void SetParticleSizeEnd(float size) { m_DefaultSizeEnd = size; }
    void SetParticleLifespan(float lifespan) { m_DefaultLifespan = lifespan; }

    // Getters
    size_t GetActiveParticleCount() const { return m_ActiveParticleCount; }
    glm::vec4 GetParticleColorBegin() { return m_DefaultColorBegin; }
    glm::vec4 GetParticleColorEnd() { return m_DefaultColorEnd; }
    float GetParticleSizeBegin() { return m_DefaultSizeBegin; }
    float GetParticleSizeEnd() { return m_DefaultSizeEnd; }
    float GetParticleLifespan() { return m_DefaultLifespan; }
};