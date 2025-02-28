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
    m_PositionGenerator = [this]() { return this->GeneratePosition(); };
    m_VelocityGenerator = [this]() { return this->GenerateVelocity(); };

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
            Destory(pID);
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

glm::vec3 ParticleSystem::GeneratePosition()
{
    glm::vec3 position = m_Emitter.position;

    switch (m_Emitter.shape)
    {
    case EmitterShape::POINT:
        // Point emitter just returns the emitter position
        break;

    case EmitterShape::SPHERE:
    {
        // Generate random direction
        float theta = Random::Float() * 2.0f * glm::pi<float>();
        float phi = Random::Float() * glm::pi<float>();

        float x = sin(phi) * cos(theta);
        float y = sin(phi) * sin(theta);
        float z = cos(phi);

        // Scale by random radius within emitter radius
        float r = m_Emitter.radius * std::pow(Random::Float(), 1.0f / 3.0f); // cube root for uniform distribution
        position += glm::vec3(x, y, z) * r;
        break;
    }

    case EmitterShape::CONE:
    {
        // For cone, we'll start at the tip and direct particles in the cone direction
        // Cone will be along the local Z axis by default
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

        position += rotatedOffset;
        break;
    }

    case EmitterShape::BOX:
    {
        // Random position within box dimensions
        float x = (Random::Float() * 2.0f - 1.0f) * m_Emitter.dimensions.x * 0.5f;
        float y = (Random::Float() * 2.0f - 1.0f) * m_Emitter.dimensions.y * 0.5f;
        float z = (Random::Float() * 2.0f - 1.0f) * m_Emitter.dimensions.z * 0.5f;

        // Apply emitter rotation
        glm::quat rotation = glm::quat(glm::radians(m_Emitter.rotation));
        glm::vec3 rotatedOffset = rotation * glm::vec3(x, y, z);

        position += rotatedOffset;
        break;
    }
    }

    return position;
}

glm::vec3 ParticleSystem::GenerateVelocity()
{
    glm::vec3 velocity = glm::vec3(0.0f);

    switch (m_Emitter.shape)
    {
    case EmitterShape::POINT:
        {
            // Random direction from point
            float theta = Random::Float() * 2.0f * glm::pi<float>();
            float phi = Random::Float() * glm::pi<float>();

            float x = sin(phi) * cos(theta);
            float y = sin(phi) * sin(theta);
            float z = cos(phi);

            velocity = glm::vec3(x, y, z);
            break;
        }

    case EmitterShape::SPHERE:
        {
            // For sphere, particle can go in any direction (but we'll direct it outward)
            glm::vec3 particleOffset = GeneratePosition() - m_Emitter.position;
            if (glm::length(particleOffset) > 0.0001f) {
                velocity = glm::normalize(particleOffset);
            }
            else {
                // Fallback if at center
                velocity = glm::vec3(
                    Random::Float() * 2.0f - 1.0f,
                    Random::Float() * 2.0f - 1.0f,
                    Random::Float() * 2.0f - 1.0f
                );
                velocity = glm::normalize(velocity);
            }
            break;
        }

    case EmitterShape::CONE:
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
            velocity = rotation * glm::vec3(x, y, z);
            break;
        }

    case EmitterShape::BOX:
        {
            // For box, we'll direct particles outward from center
            glm::vec3 particleOffset = GeneratePosition() - m_Emitter.position;
            if (glm::length(particleOffset) > 0.0001f) {
                velocity = glm::normalize(particleOffset);
            }
            else {
                // Fallback if at center
                velocity = glm::vec3(
                    Random::Float() * 2.0f - 1.0f,
                    Random::Float() * 2.0f - 1.0f,
                    Random::Float() * 2.0f - 1.0f
                );
                velocity = glm::normalize(velocity);
            }
            break;
        }
    }

    return velocity;
}

void ParticleSystem::CreateParticle()
{
    if (m_Particles.find(id) == m_Particles.end())
    {
        glm::vec3 position = m_PositionGenerator();
        glm::vec3 velocity = m_VelocityGenerator();

        // Use the custom colors and sizes if they're enabled
        /*glm::vec4 colorBegin = m_UseCustomColors ? m_DefaultColorBegin : glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        glm::vec4 colorEnd = m_UseCustomColors ? m_DefaultColorEnd : glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
        float sizeBegin = m_UseSizeOverLifetime ? m_DefaultSizeBegin : 1.0f;
        float sizeEnd = m_UseSizeOverLifetime ? m_DefaultSizeEnd : 1.0f;*/

        m_Particles.insert({ id,
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
    id++;
}

void ParticleSystem::CreateParticles(float delta)
{
    // This method is kept for backward compatibility
    // but now it just triggers the CreateParticle method
    for (unsigned int i = 0; i < 100; i++)
    {
        CreateParticle();
    }
}

void ParticleSystem::Destory(unsigned int id)
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
            it++;
            m_Particles.erase(ID);
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
