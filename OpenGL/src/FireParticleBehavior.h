// FireParticleBehavior.h
#pragma once
#include "ParticleBehavior.h"
#include "imgui/imgui.h"


class FireParticleBehavior : public ParticleBehavior 
{
private:
    std::vector<glm::vec4> m_Positions;
    std::vector<glm::vec4> m_Velocities;
    std::vector<float> m_Temperatures;
	std::vector<float> m_SmokeAmounts;
	std::vector<float> m_Lifespans;

	GLuint m_SSBO_Positions = 0;
	GLuint m_SSBO_Velocities = 0;
	GLuint m_SSBO_Temperatures = 0;
	GLuint m_SSBO_SmokeAmounts = 0;
	GLuint m_SSBO_Lifespans = 0;

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

    void CreateParticleBuffers(size_t maxParticles) override 
    {
        m_Positions.resize(maxParticles);
        m_Velocities.resize(maxParticles);
        m_Temperatures.resize(maxParticles);
        m_SmokeAmounts.resize(maxParticles);
        m_Lifespans.resize(maxParticles);

        glGenBuffers(1, &m_SSBO_Positions);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_Positions);
        glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(glm::vec4), m_Positions.data(), GL_DYNAMIC_DRAW);

        glGenBuffers(1, &m_SSBO_Velocities);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_Velocities);
        glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(glm::vec4), m_Velocities.data(), GL_DYNAMIC_DRAW);

        glGenBuffers(1, &m_SSBO_Temperatures);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_Temperatures);
        glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(float), m_Temperatures.data(), GL_DYNAMIC_DRAW);

        glGenBuffers(1, &m_SSBO_SmokeAmounts);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_SmokeAmounts);
        glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(float), m_SmokeAmounts.data(), GL_DYNAMIC_DRAW);

        glGenBuffers(1, &m_SSBO_Lifespans);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_Lifespans);
        glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(float), m_Lifespans.data(), GL_DYNAMIC_DRAW);
    }

    void UpdateParticleBuffers() override 
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_Positions);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_Positions.size() * sizeof(glm::vec4), m_Positions.data());

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_Velocities);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_Velocities.size() * sizeof(glm::vec4), m_Velocities.data());

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_Temperatures);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_Temperatures.size() * sizeof(float), m_Temperatures.data());

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_SmokeAmounts);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_SmokeAmounts.size() * sizeof(float), m_SmokeAmounts.data());

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_Lifespans);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_Lifespans.size() * sizeof(float), m_Lifespans.data());
    }

    void BindParticleBuffers(GLuint baseBinding) override 
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 0, m_SSBO_Positions);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 1, m_SSBO_Velocities);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 2, m_SSBO_Temperatures);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 3, m_SSBO_SmokeAmounts);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 4, m_SSBO_Lifespans);
    }


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
        return "res/shaders/fireParticle/fire_compute.comp";
    }

    std::string GetEmitterShaderPath() const override 
    {
        return "res/shaders/fireParticle/fire_emitter.comp";
    }

    void InitializeParticle(int index)
    {
		// Initialize particle properties
		m_Positions[index]    = glm::vec4(0.0f);
		m_Velocities[index]   = glm::vec4(0.0f);
		m_Temperatures[index] = 0.0f;
		m_SmokeAmounts[index] = 0.0f;
		m_Lifespans[index]    = 0.0f;
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
