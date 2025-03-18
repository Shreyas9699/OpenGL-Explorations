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

    std::vector<GPUParticle> m_GPUParticles;
    unsigned int m_MaxParticles = ParticleConfig::MAX_ABSOLUTE_PARTICLES * ParticleConfig::MAX_ABSOLUTE_LIFESPAN;

    void initParticles();

public:
    ParticleSystemGPU();
    ~ParticleSystemGPU();

    void Update(float delta, const glm::mat4& viewProj = glm::mat4(0.0f)) override;
    void Render(Shader& shader) override;
    void SetEmitter(const EmitterProperties& emitterProp) override;
    void SetEmissionRate(int rate) override;
};