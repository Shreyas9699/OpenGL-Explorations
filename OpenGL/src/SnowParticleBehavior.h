// SnowParticleBehavior.h
#pragma once
#include "ParticleBehavior.h"

class SnowParticleBehavior : public ParticleBehavior {
private:
    glm::vec3 m_WindDirection = glm::vec3(1.0f, 0.0f, 0.0f);
    float m_WindStrength = 1.0f;
    float m_Turbulence = 2.0f;
    float m_FallSpeed = 2.0f;

public:
    void Initialize() override {}

    void UpdateUniforms(ComputeShader& computeShader, float deltaTime) override 
    {
        computeShader.setFloat("deltaTime", deltaTime);
        computeShader.setVec3("windDirection", m_WindDirection);
        computeShader.setFloat("windStrength", m_WindStrength);
        computeShader.setFloat("turbulence", m_Turbulence);
        computeShader.setFloat("fallSpeed", m_FallSpeed);
    }

    std::string GetComputeShaderPath() const override 
    {
        return "res/shaders/ParticleSysGPU/snow_compute.comp";
    }

    std::string GetEmitterShaderPath() const override 
    {
        return "res/shaders/ParticleSysGPU/snow_emitter.comp";
    }

    void SetWindProperties(const glm::vec3& direction, float strength) 
    {
        m_WindDirection = glm::normalize(direction);
        m_WindStrength = strength;
    }

    void SetTurbulence(float turbulence) {
        m_Turbulence = turbulence;
    }

    void SetFallSpeed(float speed) {
        m_FallSpeed = speed;
    }
};