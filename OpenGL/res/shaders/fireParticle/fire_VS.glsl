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
    gl_PointSize = size * lifePercent * scaling_factor / dist;

    float yDist = length(pos.xz) / radius;
    //float height = clamp(abs(pos.y) / (particle.velocity.y * lifespan), 0.0, 1.0);

    vec4 color = vec4(0.0);
    if (lifePercent > 0.6)
        {
            float factor = (lifePercent - 0.6) / 0.3;
            if (yDist < 0.85 && lifePercent > 0.9)
            {
                color = mix(vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 0.46, 0.0, 1.0), 1.0 - factor); // #FFFFFF to #FF7500
            }
            else
            {
                color = mix(vec4(0.98, 0.75, 0.0, 1.0), vec4(1.0, 0.46, 0.0, 1.0), 1.0 - factor); // #FAC000 to #FF7500
            }
        }
        else if (lifePercent > 0.3)
        {
            float factor = (lifePercent - 0.3f) / 0.4f;
            color = mix(vec4(1.0, 0.46, 0.0, 1.0), vec4(0.84, 0.21, 0.01, 1.0), 1.0 - factor); // #FF7500 to #D73502
        }
        else
        {
            float factor = lifePercent / 0.3;
            color = mix(vec4(0.84, 0.21, 0.01, 1.0), vec4(0.50, 0.07, 0.0, 0.0), 1.0f - factor);
        }

    particleColor = color;
    particleAlpha = particleColor.a * lifePercent;
}
