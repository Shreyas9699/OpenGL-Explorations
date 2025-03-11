#include "ParticleSystemGPU.h"
#include "Random.h"

#include <cmath>
#include <glm/gtx/quaternion.hpp>

ParticleSystemGPU::ParticleSystemGPU()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_particleSSBO);
    glGenBuffers(1, &m_atomicBuffer);
    glGenBuffers(1, &m_instanceVBO);
    glGenBuffers(1, &m_particleCountBuffer);
    glGenBuffers(1, &m_freeListBuffer);

    // Compile emitter compute shader
    m_EmitterShader = std::make_unique<ComputeShader>("res/shaders/ParticleSysGPU/emitter.comp");

    initParticles();
}

void ParticleSystemGPU::initParticles()
{
    // Create compute shader
    m_ComputeShader = std::make_unique<ComputeShader>("res/shaders/ParticleSysGPU/compute.comp");

    // Resize GPU particles buffer
    m_GPUParticles.resize(m_MaxParticles);

    // Initialize buffers
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_particleCountBuffer);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_atomicBuffer);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

    // Allocate free list + particle index buffer in one loop
    std::vector<GLint> freeIndices(m_MaxParticles + 1);  // +1 for count at index 0
    std::vector<GLuint> particleIndices(m_MaxParticles);

    freeIndices[0] = m_MaxParticles; // First element stores the count

    for (GLuint i = 0; i < m_MaxParticles; i++) 
    {
        freeIndices[i + 1] = m_MaxParticles - 1 - i;  // LIFO Free List
        particleIndices[i] = i;  // Particle indices

        // Initialize GPU particles
        m_GPUParticles[i].position = glm::vec4(0.0f, 0.0f, 0.0f, m_DefaultSizeBegin);
        m_GPUParticles[i].velocity = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); // w = lifeRemaining (0 = inactive)
        m_GPUParticles[i].colorBegin = m_DefaultColorBegin;
        m_GPUParticles[i].colorEnd = glm::vec4(m_DefaultColorEnd.r, m_DefaultColorEnd.g, m_DefaultColorEnd.b, m_DefaultLifespan);
    }

    // Upload Free List Buffer
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_freeListBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, (m_MaxParticles + 1) * sizeof(GLint), freeIndices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Upload Particle SSBO
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particleSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, m_MaxParticles * sizeof(GPUParticle), m_GPUParticles.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Set up VAO & VBO for rendering particles
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, m_MaxParticles * sizeof(GLuint), particleIndices.data(), GL_STATIC_DRAW);
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

ParticleSystemGPU::~ParticleSystemGPU()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    m_Shader.reset();
}

void ParticleSystemGPU::Update(float delta)
{
    if (!m_ComputeShader || !m_EmitterShader)
    {
        std::cerr << "ERROR::ParticleSystemGPU::Update    Invalid Compute shader or Emitter shader" << std::endl;
        return;
    }

    // Reset atomic counter for new particles
    GLuint zero = 0;
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_atomicBuffer);
    glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);

    // Reset particle counter to zero
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_particleCountBuffer);
    glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);

    // Prepare to emit new particles
    int newParticlesCount = static_cast<int>(m_Emitter.emissionRate * delta);

    /*std::cout << "m_Emitter.emissionRate: " << m_Emitter.emissionRate << std::endl
        << "newParticlesCount for the frame: " << newParticlesCount << std::endl;*/

    // Calculate accumulated fractional particles
    m_Emitter.accumulatedTime += m_Emitter.emissionRate * delta - newParticlesCount;
    if (m_Emitter.accumulatedTime >= 1.0f)
    {
        newParticlesCount += 1;
        m_Emitter.accumulatedTime -= 1.0f;
    }

    // ---- EMIT PARTICLES ON GPU ----
    if (newParticlesCount > 0) 
    {
        // Bind atomic counter for emitter shader to use
        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, m_atomicBuffer);

        // Bind particle SSBO
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_particleSSBO);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_freeListBuffer);

        // Set emitter shader uniforms
        m_EmitterShader->Bind();
        m_EmitterShader->setInt("newParticleCount", newParticlesCount);
        m_EmitterShader->setVec3("emitterPosition", m_Emitter.position);
        m_EmitterShader->setVec3("emitterRotation", m_Emitter.rotation);
        m_EmitterShader->setVec3("emitterDimensions", m_Emitter.dimensions);
        m_EmitterShader->setFloat("emitterRadius", m_Emitter.radius);
        m_EmitterShader->setFloat("emitterAngle", m_Emitter.angle);
        m_EmitterShader->setInt("emitterShape", static_cast<int>(m_Emitter.shape));
        m_EmitterShader->setFloat("particleLifespan", m_DefaultLifespan);
        m_EmitterShader->setFloat("particleSizeBegin", m_DefaultSizeBegin);
        m_EmitterShader->setVec4("particleColorBegin", m_DefaultColorBegin);
        m_EmitterShader->setVec4("particleColorEnd", m_DefaultColorEnd);

        // Dispatch a reasonable number of threads
        // We don't need many threads, just enough to handle emission
        glDispatchCompute((newParticlesCount + 255) / 256, 1, 1);

        // Wait for emission shader to complete
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        m_EmitterShader->Unbind();
    }

    // ---- UPDATE PARTICLES ON GPU ----
    // Bind the SSBO for the update shader
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_particleSSBO);

    // Bind particle counter atomic buffer to binding point 1
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, m_particleCountBuffer);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_freeListBuffer);

    // Set compute shader uniforms
    m_ComputeShader->Bind();
    m_ComputeShader->setFloat("deltaTime", delta);
    m_ComputeShader->setVec3("globalForce", m_GlobalForce);
    m_ComputeShader->setFloat("sizeBegin", m_DefaultSizeBegin);
    m_ComputeShader->setFloat("sizeEnd", m_DefaultSizeEnd);

    // Dispatch compute shader
    // Each workgroup processes 256 particles
    int numWorkGroups = (m_MaxParticles + 255) / 256;
    glDispatchCompute(numWorkGroups, 1, 1);

    // Ensure compute shader finishes before rendering
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    m_ComputeShader->Unbind();

    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_particleCountBuffer);
    glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &m_ActiveParticleCount);

    // Note: No more readback to CPU - we keep all data on the GPU
}

void ParticleSystemGPU::Render(Shader& shader)
{
    shader.Bind();

    // Bind particle buffer as SSBO for shader to read from
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_particleSSBO);

    // Draw particles using instanced rendering
    glBindVertexArray(m_VAO);
    // Draw m_MaxParticles instances - the shader will discard inactive ones
    glDrawArrays(GL_POINTS, 0, m_MaxParticles);
    glBindVertexArray(0);

    shader.Unbind();
}

void ParticleSystemGPU::SetEmitter(const EmitterProperties& emitterProp)
{
    m_Emitter = emitterProp;
}

void ParticleSystemGPU::SetEmissionRate(int rate)
{
    m_Emitter.emissionRate = rate;
    m_Emitter.accumulatedTime = 0.0f;
}
