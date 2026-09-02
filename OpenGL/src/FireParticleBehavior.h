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
    VertexBuffer m_SSBO_Positions;
    VertexBuffer m_SSBO_Velocities;
    VertexBuffer m_SSBO_Temperatures;
    VertexBuffer m_SSBO_SmokeAmounts;
    VertexBuffer m_SSBO_Lifespans;

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

    void CreateParticleBuffers(size_t maxParticles) override 
    {
        m_Positions.resize(maxParticles);
        m_Velocities.resize(maxParticles);
        m_Temperatures.resize(maxParticles);
        m_SmokeAmounts.resize(maxParticles);
        m_Lifespans.resize(maxParticles);

        unsigned int bytes = static_cast<unsigned int>(maxParticles * sizeof(glm::vec4));

        m_SSBO_Positions = VertexBuffer(m_Positions.data(), bytes, GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW);
        m_SSBO_Velocities = VertexBuffer(m_Velocities.data(), bytes, GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW);

        bytes = static_cast<unsigned int>(maxParticles * sizeof(float));

        m_SSBO_Temperatures = VertexBuffer(m_Temperatures.data(), bytes, GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW);
        m_SSBO_SmokeAmounts = VertexBuffer(m_SmokeAmounts.data(), bytes, GL_DYNAMIC_DRAW);
        m_SSBO_Lifespans = VertexBuffer(m_Lifespans.data(), bytes, GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW);
    }

    void UpdateParticleBuffers() override 
    {
        m_SSBO_Positions.UpdateData(m_Positions.data(), static_cast<unsigned int>(m_Positions.size() * sizeof(glm::vec4)));
        m_SSBO_Velocities.UpdateData(m_Velocities.data(), static_cast<unsigned int>(m_Velocities.size() * sizeof(glm::vec4)));
        m_SSBO_Temperatures.UpdateData(m_Temperatures.data(), static_cast<unsigned int>(m_Temperatures.size() * sizeof(float)));

        // Update other buffers less frequently if they don't change much
        static int updateCounter = 0;
        if (++updateCounter % 4 == 0)
        {
            m_SSBO_SmokeAmounts.UpdateData(m_SmokeAmounts.data(), static_cast<unsigned int>(m_SmokeAmounts.size() * sizeof(float)));
            m_SSBO_Lifespans.UpdateData(m_Lifespans.data(), static_cast<unsigned int>(m_Lifespans.size() * sizeof(float)));
        }
    }

    void BindParticleBuffers(GLuint baseBinding) override 
    {
        m_SSBO_Positions.BindBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 0);
        m_SSBO_Velocities.BindBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 1);
        m_SSBO_Temperatures.BindBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 2);
        m_SSBO_SmokeAmounts.BindBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 3);
        m_SSBO_Lifespans.BindBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 4);
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
