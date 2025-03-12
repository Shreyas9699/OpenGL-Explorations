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
    // Initialize particle count buffer
    GLuint initialZero = 0;
    m_ParticleCountBuffer = std::make_unique<VertexBuffer>(
        &initialZero, sizeof(GLuint), GL_ATOMIC_COUNTER_BUFFER, GL_DYNAMIC_DRAW
    );

    // Initialize atomic buffer
    m_AtomicBuffer = std::make_unique<VertexBuffer>(
        &initialZero, sizeof(GLuint), GL_ATOMIC_COUNTER_BUFFER, GL_DYNAMIC_DRAW
    );

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

    // Create free list buffer
    m_FreeListBuffer = std::make_unique<VertexBuffer>(
        freeIndices.data(), (m_MaxParticles + 1) * sizeof(GLint), GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW
    );

    // Create particle SSBO
    m_ParticleSSBO = std::make_unique<VertexBuffer>(
        m_GPUParticles.data(), m_MaxParticles * sizeof(GPUParticle), GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW
    );

    m_ParticleVAO = std::make_unique<VertexArray>();
    m_InstanceVBO = std::make_unique<VertexBuffer>( particleIndices.data(), m_MaxParticles * sizeof(GLuint));
    VertexBufferLayout layout;
    layout.Push<int>(1); // For the particle index
    m_ParticleVAO->AddBuffer(*m_InstanceVBO, layout);
}

ParticleSystemGPU::~ParticleSystemGPU()
{
    m_ParticleVAO.reset();
    m_InstanceVBO.reset();
    m_ParticleSSBO.reset();
    m_AtomicBuffer.reset();
    m_ParticleCountBuffer.reset();
    m_FreeListBuffer.reset();

    m_ComputeShader.reset();
    m_EmitterShader.reset();
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
    /*GLCall(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_AtomicBuffer->GetRendererID()));
    GLCall(glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero));*/
    m_AtomicBuffer->UpdateData(&zero, sizeof(GLuint), 0);

    // Reset particle counter to zero
   /* GLCall(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_ParticleCountBuffer->GetRendererID()));
    GLCall(glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero));*/
    m_ParticleCountBuffer->UpdateData(&zero, sizeof(GLuint), 0);

    // Prepare to emit new particles
    int newParticlesCount = static_cast<int>(m_Emitter.emissionRate * delta);

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
        m_AtomicBuffer->BindBase(GL_ATOMIC_COUNTER_BUFFER, 0);
        m_ParticleSSBO->BindBase(GL_SHADER_STORAGE_BUFFER, 0);
        m_FreeListBuffer->BindBase(GL_SHADER_STORAGE_BUFFER, 1);

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
        GLCall(glDispatchCompute((newParticlesCount + 255) / 256, 1, 1));
        m_EmitterShader->Unbind();

        // Wait for emission shader to complete
        GLCall(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT));
    }

    // ---- UPDATE PARTICLES ON GPU ----
    // Bind the SSBO for the update shader
    m_ParticleSSBO->BindBase(GL_SHADER_STORAGE_BUFFER, 0);
    m_ParticleCountBuffer->BindBase(GL_ATOMIC_COUNTER_BUFFER, 1);
    m_FreeListBuffer->BindBase(GL_SHADER_STORAGE_BUFFER, 1);

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
    m_ComputeShader->Unbind();

    // Ensure compute shader finishes before rendering
    GLCall(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_ParticleCountBuffer->GetRendererID());
    glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &m_ActiveParticleCount);
}

void ParticleSystemGPU::Render(Shader& shader)
{
    shader.Bind();
    m_ParticleSSBO->BindBase(GL_SHADER_STORAGE_BUFFER, 0);
    m_ParticleVAO->Bind();
    // Draw m_MaxParticles instances - the shader will discard inactive ones
    GLCall(glDrawArrays(GL_POINTS, 0, m_MaxParticles));
    m_ParticleVAO->Unbind();
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
