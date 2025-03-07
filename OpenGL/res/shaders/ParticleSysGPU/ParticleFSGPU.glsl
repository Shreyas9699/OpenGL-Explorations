#version 430 core

in vec4 particleColor;
in float particleAlpha;

out vec4 FragColor;

void main()
{
    // Calculate distance from center of point for circular particles
    vec2 coord = gl_PointCoord * 2.0 - 1.0;
    float dist = dot(coord, coord);
    
    // Discard fragments outside the circle
    if (dist > 1.0) 
    {
        discard;
    }
    
    // Apply a soft edge to the particle
    float alpha = 1.0 - smoothstep(0.7, 1.0, dist);
    
    // Set final color with alpha
    FragColor = vec4(particleColor.rgb, particleAlpha * alpha);
}