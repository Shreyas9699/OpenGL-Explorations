// FireParticleBehavior.h
#pragma once
#include "ParticleBehavior.h"
#include "imgui/imgui.h"


class FireParticleBehavior : public ParticleBehavior 
{
private:
    glm::vec3 m_GlobalForce = glm::vec3(0.0f, -9.8f, 0.0f);
    float m_RiseSpeed = 5.0f;
    float m_HeatDissipation = 0.8f;
    float m_FlickerSpeed = 2.0f;
    float m_Turbulence = 0.0f;
    float m_UpwardForce = 3.0f;
    glm::vec3 m_WindDirection = glm::vec3(0.2f, 0.0f, 0.1f);
    float m_WindStrength = 0.0f;
    float m_mass = 1.0e-8f;
    float m_MinTemperature = 700.0f;
    float m_MaxTemperature = 1200.0f;

public:
    void Initialize() override {}
	void Cleanup() override {}

    void UpdateUniforms(ComputeShader& computeShader) override 
    {
        computeShader.setVec3("globalForce", m_GlobalForce);
        computeShader.setVec3("windDirection", m_WindDirection);
        computeShader.setFloat("windStrength", m_WindStrength);
        computeShader.setFloat("turbulenceScale", m_Turbulence);
        computeShader.setFloat("flickerSpeed", m_FlickerSpeed);
    }

    void UpdateEmitterUniforms(ComputeShader& emitterShader) override
    {
        // Set uniforms for fire emitter shader
        emitterShader.setFloat("minTemperature", m_MinTemperature);
        emitterShader.setFloat("maxTemperature", m_MaxTemperature);
		emitterShader.setFloat("mass", m_mass);
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
        particle->position = glm::vec4(0.0f);
        particle->velocity = glm::vec4(0.0f);
		particle->lifespan = 0.0f;
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

    void CustomGUI() override
    {
        if (ImGui::CollapsingHeader("Fire Sim Properties", ImGuiTreeNodeFlags_DefaultOpen))
        {
            /*ImGui::SliderFloat("Rise Speed", &m_RiseSpeed, 0.0f, 10.0f);
        ImGui::SliderFloat("Heat Dissipation", &m_HeatDissipation, 0.0f, 1.0f);*/
            ImGui::SliderFloat("Turbulence", &m_Turbulence, 0.0f, 1.0f, "%.1f");
            ImGui::SliderFloat("Upward Force", &m_UpwardForce, 0.0f, 10.0f, "%.1f");
            ImGui::SliderFloat("Wind Strength", &m_WindStrength, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFloat("Wind Direction X", &m_WindDirection.x, -1.0f, 1.0f, "%.1f");
            ImGui::SliderFloat("Wind Direction Y", &m_WindDirection.y, -1.0f, 1.0f, "%.1f");
            ImGui::SliderFloat("Wind Direction Z", &m_WindDirection.z, -1.0f, 1.0f, "%.1f");
            //ImGui::SliderFloat("Mass", &m_mass, 1.0e-8f, 1.0e-6f);
            ImGui::SliderFloat("Min Temperature", &m_MinTemperature, 0.0f, 2000.0f);
            ImGui::SliderFloat("Max Temperature", &m_MaxTemperature, 0.0f, 2000.0f);
        }
    }
};
