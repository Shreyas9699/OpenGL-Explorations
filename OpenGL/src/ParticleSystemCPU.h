// Particle System with CPU Implementation
#pragma once
#include "ParticleSystemBase.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class ParticleSystemCPU : public ParticleSystemBase
{
private:
    std::unordered_map<unsigned int, Particle> m_Particles;
    std::queue<unsigned int> m_ParticlePool;
    std::vector<float> m_Points;

    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VBO;
    std::unique_ptr<Shader> m_Shader;

    std::function<glm::vec3()> m_PositionGenerator;
    std::function<glm::vec3()> m_VelocityGenerator;

    std::function<glm::vec3()> GeneratePosition();            // Generates positions based on emitter shape
    glm::vec3 GeneratePointPosition();
    glm::vec3 GenerateSpherePosition();
    glm::vec3 GenerateConePosition();
    glm::vec3 GenerateBoxPosition();
    std::function<glm::vec3()> GenerateVelocity();            // Generates velocities based on emitter shape
    glm::vec3 GeneratePointVelocity();
    glm::vec3 GenerateSphereVelocity();
    glm::vec3 GenerateConeVelocity();
    glm::vec3 GenerateBoxVelocity();
    void UpdateGenerators();

    void CreateParticle();              // Create a single particle

public:
    ParticleSystemCPU();
    ~ParticleSystemCPU();


    void Update(float delta) override;
    void Render(Shader& m_Shader) override;

    void Destroy(unsigned int id);
    void DeleteInactiveParticles();
    size_t GetNumOfParticles() { return m_Particles.size(); }

    void SetEmitter(const EmitterProperties& emitterProp) override;
    void SetEmissionRate(int rate) override;
};