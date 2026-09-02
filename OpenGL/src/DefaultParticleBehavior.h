// DefaultParticleBehavior.h
#pragma once
#include "ParticleBehavior.h"

class DefaultParticleBehavior : public ParticleBehavior 
{
private:
    std::vector<glm::vec4> m_Positions;
    std::vector<glm::vec4> m_Velocities;
    std::vector<glm::vec4> m_ColorBegins;
    std::vector<glm::vec4> m_ColorEnds;

    VertexBuffer m_SSBO_Positions;
    VertexBuffer m_SSBO_Velocities;
    VertexBuffer m_SSBO_ColorBegins;
    VertexBuffer m_SSBO_ColorEnds;

    glm::vec3 m_GlobalForce = glm::vec3(0.0f);

public:
    void Initialize() override {}
    void CustomGUI() override {}

    void CreateParticleBuffers(size_t maxParticles) override 
    {
        m_Positions.resize(maxParticles);
        m_Velocities.resize(maxParticles);
        m_ColorBegins.resize(maxParticles);
        m_ColorEnds.resize(maxParticles);

        unsigned int bytes = static_cast<unsigned int>(maxParticles * sizeof(glm::vec4));

        m_SSBO_Positions = VertexBuffer(m_Positions.data(), bytes, GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW);
        m_SSBO_Velocities = VertexBuffer(m_Velocities.data(), bytes, GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW);
        m_SSBO_ColorBegins = VertexBuffer(m_ColorBegins.data(), bytes, GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW);
        m_SSBO_ColorEnds = VertexBuffer(m_ColorEnds.data(), bytes, GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW);
    }

    void UpdateParticleBuffers() override 
    {
        m_SSBO_Positions.UpdateData(m_Positions.data(), static_cast<unsigned int>(m_Positions.size() * sizeof(glm::vec4)));
        m_SSBO_Velocities.UpdateData(m_Velocities.data(), static_cast<unsigned int>(m_Velocities.size() * sizeof(glm::vec4)));
        m_SSBO_ColorBegins.UpdateData(m_ColorBegins.data(), static_cast<unsigned int>(m_ColorBegins.size() * sizeof(glm::vec4)));
        m_SSBO_ColorEnds.UpdateData(m_ColorEnds.data(), static_cast<unsigned int>(m_ColorEnds.size() * sizeof(glm::vec4)));
    }

    void BindParticleBuffers(GLuint baseBinding) override 
    {
        m_SSBO_Positions.BindBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 0);
        m_SSBO_Velocities.BindBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 1);
        m_SSBO_ColorBegins.BindBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 2);
        m_SSBO_ColorEnds.BindBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 3);
    }

    void UpdateUniforms(ComputeShader& computeShader) override 
    {
        computeShader.setVec3("globalForce", m_GlobalForce);
    }

    void UpdateEmitterUniforms(ComputeShader& emitterShader) override
    {
        // Implement the method to update emitter-specific uniforms
        // For example, you can set a default value or leave it empty if not needed
    }

    std::string GetComputeShaderPath() const override 
    {
        return "res/shaders/ParticleSysGPU/compute.comp";
    }

    std::string GetEmitterShaderPath() const override 
    {
        return "res/shaders/ParticleSysGPU/emitter.comp";
    }

    void InitializeParticle(int index)
    {
		m_Positions[index]   = glm::vec4(0.0f);
        m_Velocities[index]  = glm::vec4(0.0f);
        m_ColorBegins[index] = glm::vec4(0.0f);
        m_ColorEnds[index]   = glm::vec4(0.0f);
    }

    void SetGlobalForce(const glm::vec3& force) 
    {
        m_GlobalForce = force;
    }

};