#include "ParticleSystemGPU.h"
#include "Random.h"
#include "DefaultParticleBehavior.h"
#include "FireParticleBehavior.h"

#include <cmath>
#include <glm/gtx/quaternion.hpp>

ParticleSystemGPU::ParticleSystemGPU(const std::string& name)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    if (!name.empty())
    {
        SetBehavior(name);
    }
    else
    {
        std::cout << "WARN::ParticleSystemGPU::ParticleSystemGPU    No behavior name provided, using default." << std::endl;
        // Set default behavior
        SetBehavior("default");
    }

    initParticles();
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
}

std::shared_ptr<ParticleBehavior> ParticleSystemGPU::CreateBehaviorFromName(const std::string& name)
{
    if (name == "default")
    {
        return std::make_shared<DefaultParticleBehavior>();
    }
    else if (name == "fire")
    {
        return std::make_shared<FireParticleBehavior>();
    }

        // Return nullptr or throw exception if behavior not found
    std::cerr << "ERROR::ParticleSystemGPU::CreateBehaviorFromName    Unknown behavior: " << name << std::endl;
    return nullptr;
}

bool ParticleSystemGPU::SetBehavior(const std::string& name)
{
    auto behavior = CreateBehaviorFromName(name);
    if (!behavior)
    {
        return false;
    }

    m_CurrentBehavior = behavior;
    m_CurrentBehavior->Initialize();
    m_NeedsReinitialization = true;
    return true;
}

void ParticleSystemGPU::initializeShaders()
{
    if (!m_CurrentBehavior)
    {
        std::cerr << "ERROR::ParticleSystemGPU::initializeShaders    No behavior set" << std::endl;
        return;
    }

    // Create compute shader
    m_ComputeShader = std::make_unique<ComputeShader>(m_CurrentBehavior->GetComputeShaderPath().c_str());

    // Create emitter shader
    m_EmitterShader = std::make_unique<ComputeShader>(m_CurrentBehavior->GetEmitterShaderPath().c_str());
}

void ParticleSystemGPU::initParticles()
{
    if (!m_CurrentBehavior)
    {
        std::cerr << "ERROR::ParticleSystemGPU::initParticles    No behavior set" << std::endl;
        return;
    }

    // Initialize shaders
    initializeShaders();

    m_CurrentBehavior->CreateParticleBuffers(m_MaxParticles);

    // Initialize buffers
    // Initialize particle count buffer
    GLuint initialZero = 0;
    m_ParticleCountBuffer = std::make_unique<VertexBuffer>(
        &initialZero, static_cast<unsigned int>(sizeof(GLuint)), GL_ATOMIC_COUNTER_BUFFER, GL_DYNAMIC_DRAW
    );

    // Initialize atomic buffer
    m_AtomicBuffer = std::make_unique<VertexBuffer>(
        &initialZero, static_cast<unsigned int>(sizeof(GLuint)), GL_ATOMIC_COUNTER_BUFFER, GL_DYNAMIC_DRAW
    );

    // Allocate free list + particle index buffer in one loop
    std::vector<GLint> freeIndices(m_MaxParticles + 1);  // +1 for count at index 0
    std::vector<GLuint> particleIndices(m_MaxParticles);

    freeIndices[0] = m_MaxParticles; // First element stores the count

    for (GLuint i = 0; i < m_MaxParticles; i++)
    {
        freeIndices[i + 1] = m_MaxParticles - 1 - i;  // LIFO Free List
        particleIndices[i] = i;  // Particle indices
        m_CurrentBehavior->InitializeParticle(i);
    }

    // Create free list buffer
    m_FreeListBuffer = std::make_unique<VertexBuffer>(
        freeIndices.data(),
        static_cast<unsigned int>((m_MaxParticles + 1) * sizeof(GLint)),
        GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW
    );

    // Create particle SSBO
    m_CurrentBehavior->UpdateParticleBuffers();

    m_ParticleVAO = std::make_unique<VertexArray>();
    m_InstanceVBO = std::make_unique<VertexBuffer>(particleIndices.data(), static_cast<unsigned int>(m_MaxParticles * sizeof(GLuint)));
    VertexBufferLayout layout;
    layout.Push<int>(1); // For the particle index
    m_ParticleVAO->AddBuffer(*m_InstanceVBO, layout);

    // Let behavior set up any additional resources
    m_CurrentBehavior->SetupAdditionalResources();

    // Reset the reinitialization flag
    m_NeedsReinitialization = false;
}

void ParticleSystemGPU::Update(float delta, const glm::mat4& viewProj)
{
    m_ElapsedTime += delta;
    // Check if behavior has changed and reinitialize if needed
    if (m_NeedsReinitialization)
    {
        initParticles();
    }

    if (!m_ComputeShader || !m_EmitterShader)
    {
        std::cerr << "ERROR::ParticleSystemGPU::Update    Invalid Compute shader or Emitter shader" << std::endl;
        return;
    }

    // Reset atomic counter for new particles
    GLuint zero = 0;
    m_AtomicBuffer->UpdateData(&zero, sizeof(GLuint), 0);

    // Reset particle counter to zero
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
        m_AtomicBuffer->BindBase(GL_ATOMIC_COUNTER_BUFFER, 1);
        m_FreeListBuffer->BindBase(GL_SHADER_STORAGE_BUFFER, 0);
        m_CurrentBehavior->BindParticleBuffers(1);

        // Set emitter shader uniforms
        m_EmitterShader->Bind();
        m_EmitterShader->setInt("newParticleCount", newParticlesCount);
        m_EmitterShader->setVec3("emitterPosition", m_Emitter.position);
        m_EmitterShader->setVec3("emitterRotation", m_Emitter.rotation);
        m_EmitterShader->setVec3("emitterDimensions", m_Emitter.dimensions);
        m_EmitterShader->setFloat("emitterRadius", m_Emitter.radius);
        m_EmitterShader->setFloat("emitterAngle", m_Emitter.angle);
        m_EmitterShader->setInt("emitterShape", static_cast<int>(m_Emitter.shape));
        m_EmitterShader->setFloat("torusInnerRadius", m_Emitter.torusInnerRadius);
        m_EmitterShader->setFloat("torusOuterRadius", m_Emitter.torusOuterRadius);
        m_EmitterShader->setFloat("particleLifespan", m_DefaultLifespan);
        m_EmitterShader->setFloat("particleSizeBegin", m_DefaultSizeBegin);
        m_EmitterShader->setVec4("particleColorBegin", m_DefaultColorBegin);
        m_EmitterShader->setVec4("particleColorEnd", m_DefaultColorEnd);

        m_CurrentBehavior->UpdateEmitterUniforms(*m_EmitterShader);

        // Dispatch a reasonable number of threads
        // We don't need many threads, just enough to handle emission
        GLCall(glDispatchCompute((newParticlesCount + 255) / 256, 1, 1));
        m_EmitterShader->Unbind();

        // Wait for emission shader to complete
        GLCall(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT));
    }

    // ---- UPDATE PARTICLES ON GPU ----
    // Bind the SSBO for the update shader
    m_ParticleCountBuffer->BindBase(GL_ATOMIC_COUNTER_BUFFER, 1);
    m_FreeListBuffer->BindBase(GL_SHADER_STORAGE_BUFFER, 0);
    m_CurrentBehavior->BindParticleBuffers(1);

    // Set compute shader uniforms
    m_ComputeShader->Bind();

    // Set standard uniforms
    m_ComputeShader->setFloat("deltaTime", delta);
    m_ComputeShader->setFloat("time", m_ElapsedTime);
    m_ComputeShader->setFloat("sizeBegin", m_DefaultSizeBegin);
    m_ComputeShader->setFloat("sizeEnd", m_DefaultSizeEnd);

    // Let behavior update its specific uniforms
    m_CurrentBehavior->UpdateUniforms(*m_ComputeShader);

    // Dispatch compute shader
    // Each workgroup processes 256 particles
    int numWorkGroups = (m_MaxParticles + 255) / 256;
    glDispatchCompute(numWorkGroups, 1, 1);
    m_ComputeShader->Unbind();

    /*GLint freeListCount;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_FreeListBuffer->GetRendererID());
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLint), &freeListCount);
    std::cout << "Free list count: " << freeListCount << std::endl;*/

    // Ensure compute shader finishes before rendering
    GLCall(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT));
    GLCall(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_ParticleCountBuffer->GetRendererID()));
    GLCall(glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &m_ActiveParticleCount));
}

void ParticleSystemGPU::Render(Shader& shader)
{
    shader.Bind();
    m_ParticleVAO->Bind();

	const ParticleBehavior::RenderState rs = m_CurrentBehavior->GetRenderState();
    glEnable(GL_BLEND);
	glBlendFunc(rs.blendSrc, rs.blendDst);
	glDepthMask(GL_FALSE);

    // Draw m_MaxParticles instances - the shader will discard inactive ones
    GLCall(glDrawArrays(GL_POINTS, 0, m_MaxParticles));

	glDepthMask(GL_TRUE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_ParticleVAO->Unbind();
    shader.Unbind();
}

void ParticleSystemGPU::GuiRender()
{
    m_CurrentBehavior->CustomGUI();
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