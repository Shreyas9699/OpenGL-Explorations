#version 430 core

// Input from vertex shader
in vec4 fragColor;
in float fragLifeRemaining;

// Output fragment color
layout(location = 0) out vec4 outColor;

// Uniform for soft particle blending
uniform float uSoftParticleDepthScale = 0.1;

void main() 
{
    // Create circular particle shape
    vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
    float distFromCenter = length(circCoord);
    
    // Soft circular falloff
    float alpha = 1.0 - smoothstep(0.0, 1.0, distFromCenter);
    
    // Final color with circular falloff and life-based alpha
    outColor = vec4(fragColor.rgb, fragColor.a * alpha);
    
    // Optional: Fade out particles as they near end of life
    outColor.a *= clamp(fragLifeRemaining, 0.0, 1.0);
    
    // Discard fully transparent fragments
    if (outColor.a < 0.01) {
        discard;
    }
}