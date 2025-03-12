// Particle System with GPU Implementation
#pragma once
#include "ParticleSystemBase.h"
#include "ComputeShader.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class ParticleSystemGPU : public ParticleSystemBase
{
private:
    std::unique_ptr<Shader> m_Shader;
    std::unique_ptr<VertexArray> m_ParticleVAO;
    std::unique_ptr<VertexBuffer> m_InstanceVBO;
    std::unique_ptr<VertexBuffer> m_ParticleSSBO;
    std::unique_ptr<VertexBuffer> m_AtomicBuffer;
    std::unique_ptr<VertexBuffer> m_ParticleCountBuffer;
    std::unique_ptr<VertexBuffer> m_FreeListBuffer;
    std::unique_ptr<ComputeShader> m_ComputeShader, m_EmitterShader;
    bool m_UseGPU = true;

    // Struct for GPU particles (keep aligned to vec4 for better performance)
    struct GPUParticle
    {
        glm::vec4 position;    // xyz = position, w = size
        glm::vec4 velocity;    // xyz = velocity, w = lifeRemaining
        glm::vec4 colorBegin;
        glm::vec4 colorEnd;    // w = lifespan
    };

    std::vector<GPUParticle> m_GPUParticles;
    unsigned int m_MaxParticles = ParticleConfig::MAX_ABSOLUTE_PARTICLES * ParticleConfig::MAX_ABSOLUTE_LIFESPAN;

    void initParticles();

public:
    ParticleSystemGPU();
    ~ParticleSystemGPU();

    void Update(float delta) override;
    void Render(Shader& shader) override;
    void SetEmitter(const EmitterProperties& emitterProp) override;
    void SetEmissionRate(int rate) override;
};