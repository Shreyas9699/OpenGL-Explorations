// DefaultParticleBehavior.h
#pragma once
#include "ParticleBehavior.h"

class DefaultParticleBehavior : public ParticleBehavior 
{
private:
    glm::vec3 m_GlobalForce = glm::vec3(0.0f);

public:
    void Initialize() override {}
    void Cleanup() override {}

    void UpdateUniforms(ComputeShader& computeShader) override 
    {
        computeShader.setVec3("globalForce", m_GlobalForce);
    }

    void UpdateEmitterUniforms(ComputeShader& emitterShader) override
    {
        // Implement the method to update emitter-specific uniforms
        // For example, you can set a default value or leave it empty if not needed
    }

    std::string GetComputeShaderPath() const override 
    {
        return "res/shaders/ParticleSysGPU/compute.comp";
    }

    std::string GetEmitterShaderPath() const override 
    {
        return "res/shaders/ParticleSysGPU/emitter.comp";
    }

    void InitializeParticle(void* particleData, int index)
    {
        GPUParticle* particle = static_cast<GPUParticle*>(particleData);
        particle->position = glm::vec4(0.0f);
        particle->velocity = glm::vec4(0.0f);
        particle->colorBegin = glm::vec4(0.0f);
        particle->colorEnd = glm::vec4(0.0f);
    }

    void SetGlobalForce(const glm::vec3& force) 
    {
        m_GlobalForce = force;
    }
};