//Tessellation Control Shader (TCS.shader)
#version 410 core
layout(vertices = 3) out;

void main() 
{
    // Set the tessellation levels dynamically (adjust these for more/less detail)
    gl_TessLevelInner[0] = 5.0;  // Inner tessellation level
    gl_TessLevelOuter[0] = 5.0;  // Outer tessellation levels
    gl_TessLevelOuter[1] = 5.0;
    gl_TessLevelOuter[2] = 5.0;

    // Pass vertex positions to the next shader stage
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
