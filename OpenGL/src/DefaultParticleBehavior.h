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

    GLuint m_SSBO_Positions = 0;
    GLuint m_SSBO_Velocities = 0;
    GLuint m_SSBO_ColorBegins = 0;
    GLuint m_SSBO_ColorEnds = 0;

    glm::vec3 m_GlobalForce = glm::vec3(0.0f);

public:
    ~DefaultParticleBehavior() override
    {
        if (m_SSBO_Positions) glDeleteBuffers(1, &m_SSBO_Positions);
        if (m_SSBO_Velocities) glDeleteBuffers(1, &m_SSBO_Velocities);
        if (m_SSBO_ColorBegins) glDeleteBuffers(1, &m_SSBO_ColorBegins);
        if (m_SSBO_ColorEnds) glDeleteBuffers(1, &m_SSBO_ColorEnds);
    }

    void Initialize() override {}
    void CustomGUI() override {}

    void CreateParticleBuffers(size_t maxParticles) override 
    {
        m_Positions.resize(maxParticles);
        m_Velocities.resize(maxParticles);
        m_ColorBegins.resize(maxParticles);
        m_ColorEnds.resize(maxParticles);

        glGenBuffers(1, &m_SSBO_Positions);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_Positions);
        glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(glm::vec4), m_Positions.data(), GL_DYNAMIC_DRAW);

        glGenBuffers(1, &m_SSBO_Velocities);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_Velocities);
        glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(glm::vec4), m_Velocities.data(), GL_DYNAMIC_DRAW);

        glGenBuffers(1, &m_SSBO_ColorBegins);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_ColorBegins);
        glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(glm::vec4), m_ColorBegins.data(), GL_DYNAMIC_DRAW);

        glGenBuffers(1, &m_SSBO_ColorEnds);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_ColorEnds);
        glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(glm::vec4), m_ColorEnds.data(), GL_DYNAMIC_DRAW);
    }

    void UpdateParticleBuffers() override 
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_Positions);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_Positions.size() * sizeof(glm::vec4), m_Positions.data());

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_Velocities);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_Velocities.size() * sizeof(glm::vec4), m_Velocities.data());

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_ColorBegins);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_ColorBegins.size() * sizeof(glm::vec4), m_ColorBegins.data());

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO_ColorEnds);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_ColorEnds.size() * sizeof(glm::vec4), m_ColorEnds.data());
    }

    void BindParticleBuffers(GLuint baseBinding) override 
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 0, m_SSBO_Positions);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 1, m_SSBO_Velocities);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 2, m_SSBO_ColorBegins);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, baseBinding + 3, m_SSBO_ColorEnds);
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