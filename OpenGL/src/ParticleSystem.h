#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <unordered_map>
#include <vector>
#include <functional>
#include <queue>

#include "Particle.h"
#include "Shader_t.h"
#include "Camera.h"


class ParticleSystem
{
private:
    std::unordered_map<unsigned int, Particle> m_Particles;
    std::queue<unsigned int> m_ParticlePool;
    unsigned int id = 0;
    std::vector<float> m_Points;

    unsigned int m_VAO, m_VBO;
    std::unique_ptr<Shader> m_Shader;

    bool isBegin = false;
    float lastCreateTime = 0.0f;

    // Default values
    glm::vec3 m_GlobalForce = glm::vec3(0.0f);
    bool m_UseForces = false;
    glm::vec4 m_DefaultColorBegin = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 m_DefaultColorEnd = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    float m_DefaultSizeBegin = 1.0f;
    float m_DefaultSizeEnd = 1.0f;
    float m_DefaultLifespan = 20.0f;

    // Emitter related properties
    EmitterProperties m_Emitter;
    std::function<glm::vec3()> m_PositionGenerator;
    std::function<glm::vec3()> m_VelocityGenerator;

    std::function<glm::vec3()> GeneratePosition();            // Generates positions based on emitter shape
    glm::vec3 GeneratePointPosition();
    glm::vec3 GenerateSpherePosition();
    glm::vec3 GenerateConePosition();
    glm::vec3 GenerateBoxPosition();
    std::function<glm::vec3()> GenerateVelocity();            // Generates velocities based on emitter shape
    glm::vec3 GeneratePointVelocity();
    glm::vec3 GenerateSphereVelocity();
    glm::vec3 GenerateConeVelocity();
    glm::vec3 GenerateBoxVelocity();
    void UpdateGenerators();

    void CreateParticle();              // Create a single particle

public:
    ParticleSystem();
    ~ParticleSystem();

    void Update(float delta);
    void Render(Shader& m_Shader);

    void Destroy(unsigned int id);
    void DeleteInactiveParticles();

    void SetForce(const glm::vec3& force);
    void SetEmitter(const EmitterProperties& emitterProp)
    {
        m_Emitter = emitterProp;
        UpdateGenerators();
    }
    void SetEmitterPosition(const glm::vec3& position) { m_Emitter.position = position; }
    void SetEmitterRotation(const glm::vec3& rotation) { m_Emitter.rotation = rotation; }
    void SetEmitterShape(EmitterShape shape) { m_Emitter.shape = shape; }
    void SetEmitterRadius(float radius) { m_Emitter.radius = radius; }
    void SetEmitterAngle(float angle) { m_Emitter.angle = angle; }
    void SetEmitterDimensions(const glm::vec3& dimensions) { m_Emitter.dimensions = dimensions; }
    void SetEmissionRate(float rate) { m_Emitter.emissionRate = rate; }

    void SetPositionGenerator(const std::function<glm::vec3()>& generator) { m_PositionGenerator = generator; }
    void SetVelocityGenerator(const std::function<glm::vec3()>& generator) { m_VelocityGenerator = generator; }

    size_t GetNumOfParticles() { return m_Particles.size(); }


    glm::vec4 GetarticleColorBegin() { return m_DefaultColorBegin; }
    glm::vec4 GetarticleColorEnd() { return m_DefaultColorEnd; }
    float GetarticleSizeBegin() { return m_DefaultSizeBegin; }
    float GetarticleSizeEnd() { return m_DefaultSizeEnd; }
    float GetarticleLifespan() { return m_DefaultLifespan; }


    void SetParticleColorBegin(const glm::vec4& color) { m_DefaultColorBegin = color; }
    void SetParticleColorEnd(const glm::vec4& color) { m_DefaultColorEnd = color; }
    void SetParticleSizeBegin(float size) { m_DefaultSizeBegin = size; }
    void SetParticleSizeEnd(float size) { m_DefaultSizeEnd = size; }
    void SetParticleLifespan(float lifespan) { m_DefaultLifespan = lifespan; }
};