// FireParticleBehavior.h
#pragma once
#include "ParticleBehavior.h"

class FireParticleBehavior : public ParticleBehavior 
{
private:
    float m_Turbulence = 1.0f;
    float m_RiseSpeed = 5.0f;
    float m_HeatDissipation = 0.8f;

public:
    void Initialize() override {}

    void UpdateUniforms(ComputeShader& computeShader, float deltaTime) override 
    {
        computeShader.setFloat("deltaTime", deltaTime);
        computeShader.setFloat("turbulence", m_Turbulence);
        computeShader.setFloat("riseSpeed", m_RiseSpeed);
        computeShader.setFloat("heatDissipation", m_HeatDissipation);
    }

    std::string GetComputeShaderPath() const override 
    {
        return "res/shaders/ParticleSysGPU/fire_compute.comp";
    }

    std::string GetEmitterShaderPath() const override 
    {
        return "res/shaders/ParticleSysGPU/fire_emitter.comp";
    }

    void SetTurbulence(float turbulence) 
    {
        m_Turbulence = turbulence;
    }

    void SetRiseSpeed(float speed) 
    {
        m_RiseSpeed = speed;
    }

    void SetHeatDissipation(float dissipation) 
    {
        m_HeatDissipation = dissipation;
    }
};
