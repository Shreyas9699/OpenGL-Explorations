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

    // GPU Buffers
    GLuint m_SSBO_Positions = 0;
    GLuint m_SSBO_Velocities = 0;
    GLuint m_SSBO_Temperatures = 0;
    GLuint m_SSBO_SmokeAmounts = 0;
    GLuint m_SSBO_Lifespans = 0;

    // Physics parameters (optimized defaults)
    glm::vec3 m_GlobalForce = glm::vec3(0.0f, -9.8f, 0.0f);
    float m_RiseSpeed = 5.0f;
    float m_HeatDissipation = 0.8f;
    float m_FlickerSpeed = 2.0f;
    float m_Turbulence = 0.0f;
    float m_UpwardForce = 3.0f;
    glm::vec3 m_WindDirection = glm::vec3(0.2f, 0.0f, 0.1f);
    float m_WindStrength = 0.0f;
    float m_MinTemperature = 700.0f;
    float m_MaxTemperature = 1200.0f;
    float m_Buoyancy = 4.0f;
    float m_AirResistance = 0.8f;
    float m_TemperatureDecay = 180.0f;
	float m_BurnTemperature = 1200.0f;
    float m_TurbulenceScale = 0.25f;
    float m_BaseVorticity = 0.5f;

public:
    void Initialize() override {}

    ~FireParticleBehavior() override
    {
        if (m_SSBO_Positions) glDeleteBuffers(1, &m_SSBO_Positions);
        if (m_SSBO_Velocities) glDeleteBuffers(1, &m_SSBO_Velocities);
        if (m_SSBO_Temperatures) glDeleteBuffers(1, &m_SSBO_Temperatures);
        if (m_SSBO_SmokeAmounts) glDeleteBuffers(1, &m_SSBO_SmokeAmounts);
        if (m_SSBO_Lifespans) glDeleteBuffers(1, &m_SSBO_Lifespans);
    }

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

        // Update other buffers less frequently if they don't change much
        static int updateCounter = 0;
        if (++updateCounter % 4 == 0)
        {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_SmokeAmounts);
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_SmokeAmounts.size() * sizeof(float), m_SmokeAmounts.data());

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_Lifespans);
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_Lifespans.size() * sizeof(float), m_Lifespans.data());
        }
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
        computeShader.setFloat("turbulenceScale", m_TurbulenceScale);
        computeShader.setFloat("flickerSpeed", m_FlickerSpeed);
        computeShader.setFloat("buoyancy", m_Buoyancy);
        computeShader.setFloat("airResistance", m_AirResistance);
        computeShader.setFloat("temperatureDecay", m_TemperatureDecay);
		computeShader.setFloat("burnTemperature", m_BurnTemperature);
		computeShader.setFloat("baseVorticity", m_BaseVorticity);
    }

    void UpdateEmitterUniforms(ComputeShader& emitterShader) override
    {
        // Set uniforms for fire emitter shader
        emitterShader.setFloat("minTemperature", m_MinTemperature);
        emitterShader.setFloat("maxTemperature", m_MaxTemperature);

    }

    std::string GetComputeShaderPath() const override 
    {
        return "res/shaders/fireParticle/fire_compute.comp";
    }

    std::string GetEmitterShaderPath() const override 
    {
        return "res/shaders/fireParticle/fire_emitter.comp";
    }

    RenderState GetRenderState() const override
    {
        return { GL_SRC_ALPHA, GL_ONE };
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
            ImGui::SliderFloat("Turbulence", &m_TurbulenceScale, 0.0f, 2.0f, "%.1f");
            ImGui::SliderFloat("Wind Strength", &m_WindStrength, 0.0f, 2.0f, "%.2f");
            ImGui::SliderFloat3("Wind Direction", &m_WindDirection.x, -1.0f, 1.0f, "%.1f");
            ImGui::SliderFloat("Min Temperature", &m_MinTemperature, 0.0f, 2000.0f);
            ImGui::SliderFloat("Max Temperature", &m_MaxTemperature, 0.0f, 2000.0f);

            if (ImGui::CollapsingHeader("Physics Parameters"))
            {
                ImGui::SliderFloat("Air Resistance", &m_AirResistance, 0.0f, 1.0f, "%.2f");
                ImGui::SliderFloat("Temperature Decay", &m_TemperatureDecay, 50.0f, 500.0f, "%.0f");
				ImGui::SliderFloat("Burn Temperature", &m_BurnTemperature, 600.0f, 2000.0f, "%.0f");
                ImGui::SliderFloat("Buoyancy", &m_Buoyancy, 0.0f, 10.0f, "%.1f");
            }
        }
    }
};
