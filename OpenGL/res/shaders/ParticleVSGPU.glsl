#version 430 core

// Particle structure matching the compute shader
struct Particle {
    vec3 position;
    vec3 velocity;
    vec4 color;
    float lifeRemaining;
    float lifeSpan;
    float size;
};

// Match the buffer binding from compute shader
layout(std430, binding = 1) buffer ParticleBuffer 
{
    Particle particles[];
};

// Uniform matrices for transformation
uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

// Output to fragment shader
out vec4 fragColor;
out float fragLifeRemaining;

// Vertex shader main function
void main() 
{
    // Get particle data for this vertex
    Particle p = particles[gl_VertexID];

    // Only render active particles
    if (p.lifeRemaining > 0.0) 
    {
        // Transform position
        vec4 worldPos = uModelMatrix * vec4(p.position, 1.0);
        vec4 viewPos = uViewMatrix * worldPos;
        gl_Position = uProjectionMatrix * viewPos;

        // Set point size based on particle's size
        gl_PointSize = max(1.0, p.size);

        // Pass color and life to fragment shader
        fragColor = p.color;
        fragLifeRemaining = p.lifeRemaining;
    }
    else 
    {
        // Hide inactive particles
        gl_Position = vec4(-1.0, -1.0, -1.0, 1.0);
        gl_PointSize = 0.0;
        fragColor = vec4(0.0);
        fragLifeRemaining = 0.0;
    }
}