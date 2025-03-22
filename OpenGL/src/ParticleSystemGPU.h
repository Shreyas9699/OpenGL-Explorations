// ParticleSystemGPU.h
#pragma once
#include "ParticleSystemBase.h"
#include "ComputeShader.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "ParticleBehavior.h"
#include <unordered_map>

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
    std::shared_ptr<ParticleBehavior> m_CurrentBehavior;
    std::unordered_map<std::string, std::shared_ptr<ParticleBehavior>> m_BehaviorRegistry;

    bool m_UseGPU = true;
    bool m_NeedsReinitialization = false;

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
    void initializeShaders();

public:
    ParticleSystemGPU();
    ~ParticleSystemGPU();

    // ParticleSystemGPU.h
    void Update(float delta, const glm::mat4& viewProj = glm::mat4(0.0f)) override;
    void Render(Shader& shader) override;
    void SetEmitter(const EmitterProperties& emitterProp) override;
    void SetEmissionRate(int rate) override;

    // New methods for behavior management
    void RegisterBehavior(const std::string& name, std::shared_ptr<ParticleBehavior> behavior);
    bool SetBehavior(const std::string& name);
    std::shared_ptr<ParticleBehavior> GetCurrentBehavior() { return m_CurrentBehavior; }

    // Helper method to create and register standard behaviors
    void RegisterStandardBehaviors();
};