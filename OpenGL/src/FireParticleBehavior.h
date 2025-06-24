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
    std::vector<float> m_Densities;
    std::vector<float> m_Pressures;
    std::vector<float> m_SmoothingLengths;
    std::vector<glm::uvec2> m_GridHash;
    std::vector<GLuint> m_SortedIndices;
    std::vector<int> m_CellStarts;
    std::vector<int> m_CellEnds;

    // GPU Buffers
    GLuint m_SSBO_Positions = 0;
    GLuint m_SSBO_Velocities = 0;
    GLuint m_SSBO_Temperatures = 0;
    GLuint m_SSBO_SmokeAmounts = 0;
    GLuint m_SSBO_Lifespans = 0;
    GLuint m_SSBO_Densities = 0;
    GLuint m_SSBO_Pressures = 0;
    GLuint m_SSBO_SmoothingLengths = 0;
    GLuint m_SSBO_GridHash = 0;
    GLuint m_SSBO_SortedIndices = 0;
    GLuint m_SSBO_CellStarts = 0;
    GLuint m_SSBO_CellEnds = 0;

    // Physics parameters (optimized defaults)
    glm::vec3 m_GlobalForce = glm::vec3(0.0f, -9.8f, 0.0f);
    float m_RiseSpeed = 5.0f;
    float m_HeatDissipation = 0.8f;
    float m_FlickerSpeed = 2.0f;
    float m_Turbulence = 0.0f;
    float m_UpwardForce = 3.0f;
    glm::vec3 m_WindDirection = glm::vec3(0.2f, 0.0f, 0.1f);
    float m_WindStrength = 0.0f;
    float m_Mass = 1.0e-8f;
    float m_MinTemperature = 700.0f;
    float m_MaxTemperature = 1200.0f;
    float m_SmoothingLength = 0.08f; // Reduced for better performance
    float m_RestDensity = 0.5f;
    float m_GasConstant = 0.5f; // Reduced for stability
    float m_Buoyancy = 4.0f;
    float m_AirResistance = 0.12f;
    float m_TemperatureDecay = 180.0f;
    float m_TurbulenceScale = 0.6f; // Reduced for performance
    float m_BaseVorticity = 2.5f;

    // Grid parameters for optimized spatial hashing
    float m_CellSize = 0.25f; // Optimized cell size
    int m_GridResolution = 256; // Increased resolution

public:
    void Initialize() override {}

    void Cleanup() override
    {
        // Clean up all vectors
        m_Positions.clear();
        m_Velocities.clear();
        m_Temperatures.clear();
        m_SmokeAmounts.clear();
        m_Lifespans.clear();
        m_Densities.clear();
        m_Pressures.clear();
        m_SmoothingLengths.clear();
        m_GridHash.clear();
        m_SortedIndices.clear();
        m_CellStarts.clear();
        m_CellEnds.clear();

        // Delete GPU buffers
        if (m_SSBO_Positions) glDeleteBuffers(1, &m_SSBO_Positions);
        if (m_SSBO_Velocities) glDeleteBuffers(1, &m_SSBO_Velocities);
        if (m_SSBO_Temperatures) glDeleteBuffers(1, &m_SSBO_Temperatures);
        if (m_SSBO_SmokeAmounts) glDeleteBuffers(1, &m_SSBO_SmokeAmounts);
        if (m_SSBO_Lifespans) glDeleteBuffers(1, &m_SSBO_Lifespans);
        if (m_SSBO_Densities) glDeleteBuffers(1, &m_SSBO_Densities);
        if (m_SSBO_Pressures) glDeleteBuffers(1, &m_SSBO_Pressures);
        if (m_SSBO_SmoothingLengths) glDeleteBuffers(1, &m_SSBO_SmoothingLengths);
        if (m_SSBO_GridHash) glDeleteBuffers(1, &m_SSBO_GridHash);
        if (m_SSBO_SortedIndices) glDeleteBuffers(1, &m_SSBO_SortedIndices);
        if (m_SSBO_CellStarts) glDeleteBuffers(1, &m_SSBO_CellStarts);
        if (m_SSBO_CellEnds) glDeleteBuffers(1, &m_SSBO_CellEnds);

        // Reset all IDs
        m_SSBO_Positions = 0;
        m_SSBO_Velocities = 0;
        m_SSBO_Temperatures = 0;
        m_SSBO_SmokeAmounts = 0;
        m_SSBO_Lifespans = 0;
        m_SSBO_Densities = 0;
        m_SSBO_Pressures = 0;
        m_SSBO_SmoothingLengths = 0;
        m_SSBO_GridHash = 0;
        m_SSBO_SortedIndices = 0;
        m_SSBO_CellStarts = 0;
        m_SSBO_CellEnds = 0;
    }

    void CreateParticleBuffers(size_t maxParticles) override 
    {
        m_Positions.resize(maxParticles);
        m_Velocities.resize(maxParticles);
        m_Temperatures.resize(maxParticles);
        m_SmokeAmounts.resize(maxParticles);
        m_Lifespans.resize(maxParticles);
        m_Densities.resize(maxParticles);
        m_Pressures.resize(maxParticles);
        m_SmoothingLengths.resize(maxParticles);
        m_GridHash.resize(maxParticles);
        m_SortedIndices.resize(maxParticles);
        m_CellStarts.resize(maxParticles);
        m_CellEnds.resize(maxParticles);

        // Optimize grid size based on expected particle distribution
        size_t gridSize = std::max(size_t(m_GridResolution * m_GridResolution), maxParticles / 4);
        m_CellStarts.resize(gridSize, -1); // Initialize to -1
        m_CellEnds.resize(gridSize, -1);

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

        glGenBuffers(1, &m_SSBO_Densities);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_Densities);
        glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(float), m_Densities.data(), GL_DYNAMIC_DRAW);

        glGenBuffers(1, &m_SSBO_Pressures);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_Pressures);
        glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(float), m_Pressures.data(), GL_DYNAMIC_DRAW);

        glGenBuffers(1, &m_SSBO_SmoothingLengths);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_SmoothingLengths);
        glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(float), m_SmoothingLengths.data(), GL_DYNAMIC_DRAW);

        glGenBuffers(1, &m_SSBO_GridHash);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_GridHash);
        glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(glm::uvec2), m_GridHash.data(), GL_DYNAMIC_DRAW);

        glGenBuffers(1, &m_SSBO_SortedIndices);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_SortedIndices);
        glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(GLuint), m_SortedIndices.data(), GL_DYNAMIC_DRAW);

        glGenBuffers(1, &m_SSBO_CellStarts);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_CellStarts);
        glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(int), m_CellStarts.data(), GL_DYNAMIC_DRAW);

        glGenBuffers(1, &m_SSBO_CellEnds);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_CellEnds);
        glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(int), m_CellEnds.data(), GL_DYNAMIC_DRAW);
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

        /*glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_Densities);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_Densities.size() * sizeof(float), m_Densities.data());

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_Pressures);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_Pressures.size() * sizeof(float), m_Pressures.data());

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_SmoothingLengths);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_SmoothingLengths.size() * sizeof(float), m_SmoothingLengths.data());

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_GridHash);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_GridHash.size() * sizeof(glm::vec4), m_GridHash.data());

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_SortedIndices);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_SortedIndices.size() * sizeof(GLuint), m_SortedIndices.data());

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_CellStarts);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_CellStarts.size() * sizeof(int), m_CellStarts.data());

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_CellEnds);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_CellEnds.size() * sizeof(int), m_CellEnds.data());*/
    }

    void BindParticleBuffers(GLuint baseBinding) override 
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 0, m_SSBO_Positions);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 1, m_SSBO_Velocities);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 2, m_SSBO_Temperatures);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 3, m_SSBO_SmokeAmounts);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 4, m_SSBO_Lifespans);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 5, m_SSBO_Densities);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 6, m_SSBO_Pressures);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 7, m_SSBO_SmoothingLengths);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 8, m_SSBO_GridHash);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 9, m_SSBO_SortedIndices);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 10, m_SSBO_CellStarts);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 11, m_SSBO_CellEnds);
    }

    void UpdateUniforms(ComputeShader& computeShader) override
    {
        computeShader.setVec3("globalForce", m_GlobalForce);
        computeShader.setVec3("windDirection", m_WindDirection);
        computeShader.setFloat("windStrength", m_WindStrength);
        computeShader.setFloat("turbulenceScale", m_TurbulenceScale);
        computeShader.setFloat("flickerSpeed", m_FlickerSpeed);
        computeShader.setFloat("gasConstant", m_GasConstant);
        computeShader.setFloat("mass", m_Mass);
        computeShader.setFloat("buoyancy", m_Buoyancy);
        computeShader.setFloat("airResistance", m_AirResistance);
        computeShader.setFloat("temperatureDecay", m_TemperatureDecay);
        computeShader.setFloat("restDensity", m_RestDensity);
    }

    void UpdateEmitterUniforms(ComputeShader& emitterShader) override
    {
        // Set uniforms for fire emitter shader
        emitterShader.setFloat("minTemperature", m_MinTemperature);
        emitterShader.setFloat("maxTemperature", m_MaxTemperature);
		emitterShader.setFloat("mass", m_Mass);
        emitterShader.setFloat("h", m_SmoothingLength);
        emitterShader.setFloat("restDensity", m_RestDensity);

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
            ImGui::SliderFloat("Turbulence", &m_TurbulenceScale, 0.0f, 2.0f, "%.1f");
            ImGui::SliderFloat("Wind Strength", &m_WindStrength, 0.0f, 2.0f, "%.2f");
            ImGui::SliderFloat3("Wind Direction", &m_WindDirection.x, -1.0f, 1.0f, "%.1f");
            ImGui::SliderFloat("Min Temperature", &m_MinTemperature, 0.0f, 2000.0f);
            ImGui::SliderFloat("Max Temperature", &m_MaxTemperature, 0.0f, 2000.0f);

            if (ImGui::CollapsingHeader("SPH Parameters"))
            {
                ImGui::SliderFloat("Gas Constant", &m_GasConstant, 0.1f, 10.0f, "%.1f");
                ImGui::SliderFloat("Rest Density", &m_RestDensity, 0.5f, 2.0f, "%.1f");
                ImGui::SliderFloat("Smoothing Length", &m_SmoothingLength, 0.05f, 0.3f, "%.2f");
                ImGui::SliderFloat("Buoyancy", &m_Buoyancy, 0.0f, 10.0f, "%.1f");
            }

            if (ImGui::CollapsingHeader("Physics Parameters"))
            {
                ImGui::SliderFloat("Air Resistance", &m_AirResistance, 0.0f, 1.0f, "%.2f");
                ImGui::SliderFloat("Temperature Decay", &m_TemperatureDecay, 50.0f, 500.0f, "%.0f");
            }
        }
    }
};
