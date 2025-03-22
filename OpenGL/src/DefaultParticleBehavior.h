// DefaultParticleBehavior.h
#pragma once
#include "ParticleBehavior.h"

class DefaultParticleBehavior : public ParticleBehavior 
{
private:
    glm::vec3 m_GlobalForce = glm::vec3(0.0f);

public:
    void Initialize() override {}

    void UpdateUniforms(ComputeShader& computeShader, float deltaTime) override 
    {
        computeShader.setFloat("deltaTime", deltaTime);
        computeShader.setVec3("globalForce", m_GlobalForce);
    }

    std::string GetComputeShaderPath() const override 
    {
        return "res/shaders/ParticleSysGPU/compute.comp";
    }

    std::string GetEmitterShaderPath() const override 
    {
        return "res/shaders/ParticleSysGPU/emitter.comp";
    }

    void SetGlobalForce(const glm::vec3& force) 
    {
        m_GlobalForce = force;
    }
};