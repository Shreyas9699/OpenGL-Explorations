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

layout(std430, binding = 3) buffer ColorBegin {
    vec4 colorBegins[];
};

layout(std430, binding = 4) buffer ColorEnds {
    vec4 colorEnds[];
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
    // Skip rendering if particle is inactive
    if (velocities[particleID].w <= 0.0) 
        return;
    
    // Calculate life percentage for interpolation
    float lifespan = colorEnds[particleID].w;
    float lifeRemaining = velocities[particleID].w;
    float lifePercent = lifeRemaining / lifespan;
    
    // Calculate position and size
    vec3 position = positions[particleID].xyz;
    float size = positions[particleID].w;
    
    // Transform position to clip space
    gl_Position = projection * view * model * vec4(position, 1.0);
    
    // Calculate point size with depth attenuation
    // This makes particles that are further away appear smaller
    float dist = length((view * model * vec4(position, 1.0)).xyz);
    gl_PointSize = size * lifePercent * scaling_factor / dist;
    
    // Calculate color based on life
    particleColor = mix(colorEnds[particleID], colorBegins[particleID], lifePercent);
    particleAlpha = particleColor.a * lifePercent; // Fade out as particles die
}