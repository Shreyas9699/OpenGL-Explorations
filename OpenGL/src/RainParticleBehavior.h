// RainParticleBehavior.h
#pragma once
#include "ParticleBehavior.h"

class RainParticleBehavior : public ParticleBehavior 
{
private:
    glm::vec3 m_WindDirection = glm::vec3(1.0f, 0.0f, 0.0f);
    float m_WindStrength = 2.0f;
    float m_RainSpeed = 15.0f;

public:
    void Initialize() override {}

    void UpdateUniforms(ComputeShader& computeShader, float deltaTime) override 
    {
        computeShader.setFloat("deltaTime", deltaTime);
        computeShader.setVec3("windDirection", m_WindDirection);
        computeShader.setFloat("windStrength", m_WindStrength);
        computeShader.setFloat("rainSpeed", m_RainSpeed);
    }

    std::string GetComputeShaderPath() const override 
    {
        return "res/shaders/ParticleSysGPU/rain_compute.comp";
    }

    std::string GetEmitterShaderPath() const override
    {
        return "res/shaders/ParticleSysGPU/rain_emitter.comp";
    }

    void SetWindProperties(const glm::vec3& direction, float strength) 
    {
        m_WindDirection = glm::normalize(direction);
        m_WindStrength = strength;
    }

    void SetRainSpeed(float speed) 
    {
        m_RainSpeed = speed;
    }
};