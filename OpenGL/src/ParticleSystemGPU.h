// ParticleSystemGPU.h
#pragma once
#include "ParticleSystemBase.h"
#include "ComputeShader.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Random.h"
#include "DefaultParticleBehavior.h"
#include "RainParticleBehavior.h"
#include "FireParticleBehavior.h"
#include "SnowParticleBehavior.h"
#include <unordered_map>

class ParticleSystemGPU : public ParticleSystemBase
{
private:
    unsigned int m_MaxParticles = ParticleConfig::MAX_ABSOLUTE_PARTICLES * ParticleConfig::MAX_ABSOLUTE_LIFESPAN;
    std::unique_ptr<VertexArray> m_ParticleVAO;
    std::unique_ptr<VertexBuffer> m_InstanceVBO;
    std::unique_ptr<VertexBuffer> m_ParticleSSBO;
    std::unique_ptr<VertexBuffer> m_AtomicBuffer;
    std::unique_ptr<VertexBuffer> m_ParticleCountBuffer;
    std::unique_ptr<VertexBuffer> m_FreeListBuffer;
    std::unique_ptr<ComputeShader> m_ComputeShader, m_EmitterShader;
    std::shared_ptr<ParticleBehavior> m_CurrentBehavior;

    float m_ElapsedTime = 0.0f;
    bool m_NeedsReinitialization = false;

    void initParticles();
    void initializeShaders();
    std::shared_ptr<ParticleBehavior> CreateBehaviorFromName(const std::string& name);

public:
    ParticleSystemGPU(const std::string& name = nullptr);
    ~ParticleSystemGPU();

    // ParticleSystemGPU.h
    void Update(float delta, const glm::mat4& viewProj = glm::mat4(0.0f)) override;
    void Render(Shader& shader) override;
	void GuiRender();
    void SetEmitter(const EmitterProperties& emitterProp) override;
    void SetEmissionRate(int rate) override;

    bool SetBehavior(const std::string& name);
    std::shared_ptr<ParticleBehavior> GetCurrentBehavior() { return m_CurrentBehavior; }
};