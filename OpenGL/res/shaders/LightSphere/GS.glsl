//Geometry Shader (GS.shader)
#version 410 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 128) out;

out vec4 fragPos;

void main() 
{
    for(int i = 0; i < 3; i++) 
    {
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}
