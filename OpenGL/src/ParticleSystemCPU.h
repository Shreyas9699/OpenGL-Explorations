// Particle System with CPU Implementation
#pragma once
#include "ParticleSystemBase.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

struct ParticleCPU
{
	// few extra properties for CPU particles as calculation is done in CPU
    // only rendering is done in shader
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

    ParticleCPU() = default;

    // Constructor with all parameters
    ParticleCPU(glm::vec3 position, glm::vec3 velocity, glm::vec4 colorBegin, glm::vec4 colorEnd, float lifeSpan,
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
        size(sizeBegin) {
    }
};

class ParticleSystemCPU : public ParticleSystemBase
{
private:
    std::unordered_map<unsigned int, ParticleCPU> m_Particles;
    std::queue<unsigned int> m_ParticlePool;
    std::vector<float> m_Points;
    size_t m_MaxBufferSize = 0;  // Track allocated capacity for pre-allocated vector optimization

    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VBO;
    std::unique_ptr<Shader> m_Shader;

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
    ParticleSystemCPU();
    ~ParticleSystemCPU();


    void Update(float delta, const glm::mat4& viewProj = glm::mat4(0.0f)) override;
    void Render(Shader& m_Shader) override;

    void Destroy(unsigned int id);
    void DeleteInactiveParticles();
    size_t GetNumOfParticles() { return m_Particles.size(); }

    void SetEmitter(const EmitterProperties& emitterProp) override;
    void SetEmissionRate(int rate) override;
};