// normalGS.glsl
#version 430 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
    vec3 position;
} gs_in[];

uniform mat4 view;
uniform mat4 projection;
uniform float normal_length = 0.1;

void GenerateLine(int index)
{
    // Start of the normal line (vertex position)
    gl_Position = projection * view * vec4(gs_in[index].position, 1.0);
    EmitVertex();
    
    // End of the normal line (vertex position + normal direction)
    vec3 end_pos = gs_in[index].position + (gs_in[index].normal * normal_length);
    gl_Position = projection * view * vec4(end_pos, 1.0);
    EmitVertex();
    
    EndPrimitive();
}

void main()
{
    GenerateLine(0);
    GenerateLine(1);
    GenerateLine(2);
}