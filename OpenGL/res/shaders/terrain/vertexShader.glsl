// Vertex shader for procedural terrain rendering
#version 450 core
layout (location = 0) in vec3 aPos;
out vec3 vs_Pos;

void main() 
{
    vs_Pos = aPos;
    gl_Position = vec4(aPos, 1.0); // Critical for correct TCS input
}