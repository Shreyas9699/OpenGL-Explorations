#include "ParticleSystem.h"
#include "Random.h"

#include <cmath>
#include <glm/gtx/quaternion.hpp>

ParticleSystem::ParticleSystem()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    // glEnable(GL_POINT_SMOOTH); //is deprecated in core profile as its from legacy OpenGL 
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize the emitter position generators
    m_PositionGenerator = [this]() { return this->GeneratePointPosition(); };
    m_VelocityGenerator = [this]() { return this->GeneratePointVelocity(); };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
}

ParticleSystem::~ParticleSystem()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    m_Shader.reset();
}

void ParticleSystem::Update(float delta)
{
    // Update emitter and create new particles
    m_Emitter.accumulatedTime += delta;
    float particlesThisFrame = m_Emitter.emissionRate * delta;
    int numParticles = static_cast<int>(particlesThisFrame);

    // Handle fractional particles using accumulated time
    m_Emitter.accumulatedTime += particlesThisFrame - numParticles;
    if (m_Emitter.accumulatedTime >= 1.0f)
    {
        numParticles += 1;
        m_Emitter.accumulatedTime -= 1.0f;
    }

    // Create particles
    for (int i = 0; i < numParticles; i++)
    {
        CreateParticle();
    }

    DeleteInactiveParticles();
    for (auto& [pID, particle] : m_Particles)
    {
        if (particle.LifeRemaining > 0)
        {
            // Calculate life percentage for interpolation
            float life = particle.LifeRemaining / particle.lifeSpan;


            if (m_UseForces)
            {
                particle.Velocity += m_GlobalForce * delta; // vel2 = vel1 + acc * t
            }

            particle.LifeRemaining -= delta;
            particle.Position += particle.Velocity * delta;

            particle.currentSize = glm::mix(particle.sizeEnd, particle.sizeBegin, life);
        }
        else
        {
            Destroy(pID);
        }
    }

    // create render data
    m_Points.clear();
    for (auto& particle : m_Particles)
    {
        m_Points.push_back(particle.second.Position.x);
        m_Points.push_back(particle.second.Position.y);
        m_Points.push_back(particle.second.Position.z);

        float life = particle.second.LifeRemaining / particle.second.lifeSpan;
        glm::vec4 color = glm::mix(particle.second.colorEnd, particle.second.colorBegin, life);

        m_Points.push_back(color.r);
        m_Points.push_back(color.g);
        m_Points.push_back(color.b);
        m_Points.push_back(color.a * life); // fade effect

        m_Points.push_back(particle.second.currentSize);
    }

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Points.size() * sizeof(float), m_Points.data(), GL_DYNAMIC_DRAW);

    // why 7* => 
    //  We now have 8 values per vertex: 3 for position, 4 for color, 1 for size
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ParticleSystem::Render(Shader& m_Shader)
{
    // draw particles
    m_Shader.Bind();
    m_Shader.setVec4("color", 1.0f, 1.0f, 1.0f, 1.0f);
    glBindVertexArray(m_VAO);
    //glPointSize(3.0f);
    glDrawArrays(GL_POINTS, 0, (GLsizei)m_Points.size() / 8); // Changed from /3 to /7 to account for position + color
    glBindVertexArray(0);
    m_Shader.Unbind();
}

std::function<glm::vec3()> ParticleSystem::GeneratePosition()
{
    // Assign the appropriate generator based on shape
    switch (m_Emitter.shape)
    {
    case EmitterShape::POINT:
        return[this]() { return this->GeneratePointPosition(); };
    case EmitterShape::SPHERE:
        return [this]() { return this->GenerateSpherePosition(); };
    case EmitterShape::CONE:
        return [this]() { return this->GenerateConePosition(); };
    case EmitterShape::BOX:
        return [this]() { return this->GenerateBoxPosition(); };
    default:
        std::cerr << "ParticleSystem::GeneratePosition:: Invalid EmitterShpape, default EmitterShape::POINT is applied" << std::endl;
        return[this]() { return this->GeneratePointPosition(); };
    }
}

glm::vec3 ParticleSystem::GeneratePointPosition()
{
    return m_Emitter.position; // Point emitter just returns the emitter position
}

glm::vec3 ParticleSystem::GenerateSpherePosition()
{
    // Generate random direction
    float theta = Random::Float() * 2.0f * glm::pi<float>();
    float phi = Random::Float() * glm::pi<float>();

    float x = sin(phi) * cos(theta);
    float y = sin(phi) * sin(theta);
    float z = cos(phi);

    // Scale by random radius within emitter radius
    float r = m_Emitter.radius * std::pow(Random::Float(), 1.0f / 3.0f); // cube root for uniform distribution
    return m_Emitter.position + glm::vec3(x, y, z) * r;
}

glm::vec3 ParticleSystem::GenerateConePosition()
{
    // For cone, we'll start at the tip and direct particles in the cone direction
    float theta = Random::Float() * 2.0f * glm::pi<float>();
    float distance = Random::Float() * m_Emitter.radius;

    // Angle spread calculation
    float spread = glm::radians(m_Emitter.angle);
    float phi = Random::Float() * spread;

    // Convert to Cartesian coordinates
    float x = distance * sin(phi) * cos(theta);
    float y = distance * sin(phi) * sin(theta);
    float z = distance * cos(phi);

    // Apply emitter rotation
    glm::quat rotation = glm::quat(glm::radians(m_Emitter.rotation));
    glm::vec3 rotatedOffset = rotation * glm::vec3(x, y, z);

    return m_Emitter.position + rotatedOffset;
}

glm::vec3 ParticleSystem::GenerateBoxPosition()
{
    // Random position within box dimensions
    float x = (Random::Float() * 2.0f - 1.0f) * m_Emitter.dimensions.x * 0.5f;
    float y = (Random::Float() * 2.0f - 1.0f) * m_Emitter.dimensions.y * 0.5f;
    float z = (Random::Float() * 2.0f - 1.0f) * m_Emitter.dimensions.z * 0.5f;

    // Apply emitter rotation
    glm::quat rotation = glm::quat(glm::radians(m_Emitter.rotation));
    glm::vec3 rotatedOffset = rotation * glm::vec3(x, y, z);

    return m_Emitter.position + rotatedOffset;
}

std::function<glm::vec3()> ParticleSystem::GenerateVelocity()
{
    // Assign the appropriate generator based on shape
    switch (m_Emitter.shape)
    {
    case EmitterShape::POINT:
        return [this]() { return this->GeneratePointVelocity(); };
    case EmitterShape::SPHERE:
        return [this]() { return this->GenerateSphereVelocity(); };
    case EmitterShape::CONE:
        return [this]() { return this->GenerateConeVelocity(); };
    case EmitterShape::BOX:
        return [this]() { return this->GenerateBoxVelocity(); };
    default:
        std::cerr << "ParticleSystem::GeneratePosition:: Invalid EmitterShpape, default EmitterShape::POINT is applied" << std::endl;
        return [this]() { return this->GeneratePointVelocity(); };
    }
}

glm::vec3 ParticleSystem::GeneratePointVelocity()
{
    // Random direction from point
    float theta = Random::Float() * 2.0f * glm::pi<float>();
    float phi = Random::Float() * glm::pi<float>();

    float x = sin(phi) * cos(theta);
    float y = sin(phi) * sin(theta);
    float z = cos(phi);

    return glm::vec3(x, y, z);
}

glm::vec3 ParticleSystem::GenerateSphereVelocity()
{
    // For sphere, particle can go in any direction (but we'll direct it outward)
    glm::vec3 particleOffset = GenerateSpherePosition() - m_Emitter.position;
    if (glm::length(particleOffset) > 0.0001f)
    {
        return glm::normalize(particleOffset);
    }
    particleOffset = glm::vec3(
        Random::Float() * 2.0f - 1.0f,
        Random::Float() * 2.0f - 1.0f,
        Random::Float() * 2.0f - 1.0f
    );

    return glm::normalize(particleOffset);
}

glm::vec3 ParticleSystem::GenerateConeVelocity()
{
    // Cone emitter directs particles within the cone angle
    float theta = Random::Float() * 2.0f * glm::pi<float>();

    // Angle spread calculation
    float spread = glm::radians(m_Emitter.angle);
    float phi = Random::Float() * spread;

    // Convert to Cartesian coordinates
    float x = sin(phi) * cos(theta);
    float y = sin(phi) * sin(theta);
    float z = cos(phi);

    // Apply emitter rotation
    glm::quat rotation = glm::quat(glm::radians(m_Emitter.rotation));
    return rotation * glm::vec3(x, y, z);
}

glm::vec3 ParticleSystem::GenerateBoxVelocity()
{
    // For box, we'll direct particles outward from center
    glm::vec3 particleOffset = GenerateBoxPosition() - m_Emitter.position;
    if (glm::length(particleOffset) > 0.0001f)
    {
        return glm::normalize(particleOffset);
    }
    // Fallback if at center
    particleOffset = glm::vec3(
        Random::Float() * 2.0f - 1.0f,
        Random::Float() * 2.0f - 1.0f,
        Random::Float() * 2.0f - 1.0f
    );
    return glm::normalize(particleOffset);
}

void ParticleSystem::UpdateGenerators()
{
    // Update position and velocity generators dynamically
    m_PositionGenerator = GeneratePosition();
    m_VelocityGenerator = GenerateVelocity();
}

void ParticleSystem::CreateParticle()
{
    unsigned int particleID;
    if (!m_ParticlePool.empty())
    {
        particleID = m_ParticlePool.front();
        m_ParticlePool.pop();
        //std::cout << "Used m_ParticlePool to create new particle" << std::endl;
    }
    else
    {
        particleID = id++;
    }

    glm::vec3 position = m_PositionGenerator();
    glm::vec3 velocity = m_VelocityGenerator();

    // Use the custom colors and sizes if they're enabled
    /*glm::vec4 colorBegin = m_UseCustomColors ? m_DefaultColorBegin : glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 colorEnd = m_UseCustomColors ? m_DefaultColorEnd : glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    float sizeBegin = m_UseSizeOverLifetime ? m_DefaultSizeBegin : 1.0f;
    float sizeEnd = m_UseSizeOverLifetime ? m_DefaultSizeEnd : 1.0f;*/
    if (m_Particles.find(particleID) != m_Particles.end())
    {
        m_Particles[particleID] = Particle(
           position,
           velocity,
           m_DefaultColorBegin,
           m_DefaultColorEnd,
           m_DefaultLifespan,                              // Default lifespan
           m_DefaultLifespan,
           true,
           m_DefaultSizeBegin,
           m_DefaultSizeEnd);
    }
    else
    {
        m_Particles.insert({ particleID,
        Particle(
            position,
            velocity,
            m_DefaultColorBegin,
            m_DefaultColorEnd,
            m_DefaultLifespan,                              // Default lifespan
            m_DefaultLifespan,
            true,
            m_DefaultSizeBegin,
            m_DefaultSizeEnd)
            });
    }
}

void ParticleSystem::Destroy(unsigned int id)
{
    m_Particles[id].Active = false;
}

void ParticleSystem::DeleteInactiveParticles()
{
    for (auto it = m_Particles.begin(); it != m_Particles.end(); )
    {
        if (!it->second.Active)
        {
            unsigned int ID = it->first;
            it = m_Particles.erase(it);
            m_ParticlePool.push(ID);
        }
        else
        {
            it++;
        }
    }
}

void ParticleSystem::SetForce(const glm::vec3& force)
{
    m_GlobalForce = force;
    m_UseForces = glm::length(force) > 0.0001f;
}