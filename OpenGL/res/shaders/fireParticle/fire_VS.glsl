#version 430 core
// We use the instance ID to look up particle data from the SSBO
layout (location = 0) in uint particleID;

// Changed to Structure of Arrays
layout(std430, binding = 1) buffer Positions {
    vec4 positions[];
};

layout(std430, binding = 2) buffer Velocities {
    vec4 velocities[];
};

layout(std430, binding = 3) buffer Temperatures {
    float temperatures[];
};

layout(std430, binding = 5) buffer Lifespans {
    float lifespans[];
};

uniform float radius;

// Matrices
uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;
uniform float scaling_factor;

// Output to fragment shader
out vec4 particleColor;
out float particleAlpha;

// Blackbody color - planck's law
vec3 blackbody(float tempK)
{
    float t = clamp(tempK, 1000.0, 40000.0) / 100.0;
    vec3 c;
    c.r = (t <= 66) ? 1.0 : clamp(1.29293618 * pow(t - 60.0, -0.1332047), 0.0, 1.0);
    c.g = (t <= 66) ? clamp(0.390081578 * log(t) - 0.63184144, 0.0, 1.0) : clamp(1.12989086 * pow(t - 60.0, -0.0755148), 0.0, 1.0);
    c.b = (t >= 66) ? 1.0 : (t <= 19) ? 0.0 : clamp(0.543206789 * log(t - 10.0) - 1.19625489, 0.0, 1.0);
    return c;
}

void main()
{
    // Skip rendering if particle is inactive
    if (velocities[particleID].w <= 0.0) 
        return;

    float lifePercent = velocities[particleID].w / lifespans[particleID];
    
    // Use only the fields you need for rendering
    vec3 pos = positions[particleID].xyz;
    float size = positions[particleID].w;
    
    gl_Position = projection * view * model * vec4(pos, 1.0);
    
    float dist = length((view * model * vec4(pos, 1.0)).xyz);
    float sizefade = mix(0.6, 1.0, lifePercent);
    gl_PointSize = size * sizefade * scaling_factor / dist;

    float yDist = length(pos.xz) / radius;
    //float height = clamp(abs(pos.y) / (particle.velocity.y * lifespan), 0.0, 1.0);

    float t =clamp(temperatures[particleID] / 1200, 0.0, 1.0);
    float kelvin = mix(1000.0, 2200.0, t);
    float emission = 0.30 * (0.25 + 0.75 * t);
    vec3 rgb = blackbody(kelvin) + emission;
    vec4 color = vec4(rgb, 1.0);

    particleColor = color;
    particleAlpha = particleColor.a * lifePercent;
}
