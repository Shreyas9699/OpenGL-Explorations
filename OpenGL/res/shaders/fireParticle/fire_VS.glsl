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

    vec3 cBrown  = vec3(0.25, 0.06, 0.02);
    vec3 cRed    = vec3(0.85, 0.16, 0.02);
    vec3 cOrange = vec3(1.00, 0.45, 0.00);
    vec3 cYellow = vec3(1.00, 0.85, 0.20);
    vec3 cWhite  = vec3(1.00, 1.00, 1.00);

    vec3 rgb;
    if (t < 0.2) {
        rgb = mix(cBrown, cRed, t / 0.2);
    } else if (t < 0.4) {
        rgb = mix(cRed, cOrange, (t - 0.2) / 0.2);
    } else if (t < 0.6) {
        rgb = mix(cOrange, cYellow, (t - 0.4) / 0.2);
    } else {
        rgb = mix(cYellow, cWhite, (t - 0.6) / 0.4);
    }
    vec4 color = vec4(rgb, 1.0);

    particleColor = color;
    particleAlpha = particleColor.a * lifePercent;
}
