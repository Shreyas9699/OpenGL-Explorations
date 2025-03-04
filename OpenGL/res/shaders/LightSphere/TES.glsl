//Tessellation Evaluation Shader (TES.shader)
#version 410 core
layout(triangles, fractional_odd_spacing) in;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() 
{
    // Interpolate the vertex position based on barycentric coordinates
    vec3 pos = (gl_TessCoord.x * gl_in[0].gl_Position.xyz) +
               (gl_TessCoord.y * gl_in[1].gl_Position.xyz) +
               (gl_TessCoord.z * gl_in[2].gl_Position.xyz);

    // Convert to a sphere by normalizing and scaling
    pos = normalize(pos) * 1.0; // Adjust sphere radius if needed

    // Apply transformations
    gl_Position = projection * view * model * vec4(pos, 1.0);
}
