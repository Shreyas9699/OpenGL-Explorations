// FireParticleBehavior.h
#pragma once
#include "ParticleBehavior.h"

class FireParticleBehavior : public ParticleBehavior 
{
private:
    float m_Turbulence = 1.0f;
    float m_RiseSpeed = 5.0f;
    float m_HeatDissipation = 0.8f;
    float mass = 1.0e-6f;
    glm::vec3 m_WindDirection = glm::vec3(1.0f, 0.0f, 0.0f);
    float m_WindStrength = 0.5f;
    float m_MinTemperature = 700.0f;
    float m_MaxTemperature = 1200.0f;

public:
    void Initialize() override {}

    void UpdateUniforms(ComputeShader& computeShader) override 
    {
        computeShader.setVec3("windDirection", m_WindDirection);
        computeShader.setFloat("windStrength", m_WindStrength);
    }

    void UpdateEmitterUniforms(ComputeShader& emitterShader) override
    {
        // Set uniforms for fire emitter shader
        emitterShader.setFloat("minTemperature", m_MinTemperature);
        emitterShader.setFloat("maxTemperature", m_MaxTemperature);
		emitterShader.setFloat("mass", mass);
    }

    std::string GetComputeShaderPath() const override 
    {
        return "res/shaders/ParticleSysGPU/fire_compute.comp";
    }

    std::string GetEmitterShaderPath() const override 
    {
        return "res/shaders/ParticleSysGPU/fire_emitter.comp";
    }

    void InitializeParticle(void* particleData, int index)
    {
		FireParticle* particle = static_cast<FireParticle*>(particleData);
		particle->temperature = 0.0f;
		particle->smokeAmount = 0.0f;
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
