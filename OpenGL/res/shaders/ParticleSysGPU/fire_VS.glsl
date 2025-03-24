#version 430 core
// We use the instance ID to look up particle data from the SSBO
layout (location = 0) in uint particleID;

// Updated structure for fire simulation
struct FireParticle {
    vec4 position;    // xyz = position, w = size
    vec4 velocity;    // xyz = velocity, w = lifeRemaining
    vec4 colorBegin;
    vec4 colorEnd;    // w = total lifespan
    float temperature;
    float smokeAmount;
    vec2 padding;
};

// Bind the same SSBO that the compute shader uses
layout(std430, binding = 0) readonly buffer ParticleBuffer {
    FireParticle particles[];
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
    FireParticle particle = particles[particleID];

    // Skip rendering if particle is inactive
    if (particle.velocity.w <= 0.0) 
        return;
    
    float lifespan = particle.colorEnd.w;
    float lifeRemaining = particle.velocity.w;
    float lifePercent = lifeRemaining / lifespan;
    
    // Use only the fields you need for rendering
    vec3 pos = particle.position.xyz;
    float size = particle.position.w;
    
    gl_Position = projection * view * model * vec4(pos, 1.0);
    
    float dist = length((view * model * vec4(pos, 1.0)).xyz);
    gl_PointSize = size * lifePercent * scaling_factor / dist;
    
    particleColor = mix(particle.colorEnd, particle.colorBegin, lifePercent);
    particleAlpha = particleColor.a * lifePercent;
}
