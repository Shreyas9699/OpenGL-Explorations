#version 430 core

// We use the instance ID to look up particle data from the SSBO
layout (location = 0) in uint particleID;

// Particle data structure matches the one in compute shader
struct Particle 
{
    vec4 position;    // xyz = position, w = size
    vec4 velocity;    // xyz = velocity, w = lifeRemaining
    vec4 colorBegin;
    vec4 colorEnd;    // w = lifespan
};

// Bind the same SSBO that the compute shader uses
layout(std430, binding = 0) readonly buffer ParticleBuffer {
    Particle particles[];
};

// Matrices
uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;
uniform float scaling_factor;

// Output to fragment shader
out vec4 particleColor;
out float particleAlpha;

void main()
{
    // Get the particle data from the SSBO using the instance ID
    Particle particle = particles[particleID];
    
    // Skip rendering if particle is inactive
    if (particle.velocity.w <= 0.0) {
        // Move the vertex far away so it's not visible
        gl_Position = vec4(0, 0, -1000, 1);
        gl_PointSize = 0;
        particleColor = vec4(0);
        return;
    }
    
    // Calculate life percentage for interpolation
    float lifespan = particle.colorEnd.w;
    float lifeRemaining = particle.velocity.w;
    float lifePercent = lifeRemaining / lifespan;
    
    // Calculate position and size
    vec3 position = particle.position.xyz;
    float size = particle.position.w;
    
    // Transform position to clip space
    gl_Position = projection * view * model * vec4(position, 1.0);
    
    // Calculate point size with depth attenuation
    // This makes particles that are further away appear smaller
    float dist = length((view * model * vec4(position, 1.0)).xyz);
    gl_PointSize = size * lifePercent * scaling_factor / dist;
    
    // Calculate color based on life
    particleColor = mix(particle.colorEnd, particle.colorBegin, lifePercent);
    particleAlpha = particleColor.a * lifePercent; // Fade out as particles die
}