// heightmapGS.glsl
#version 410 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in vec2 TexCoord_TES[];

uniform sampler2D heightMap;
uniform vec2 uTexelSize;
uniform float normalLength = 2.0; // Length of normal vectors

vec3 calculateNormal(vec2 texCoord) 
{
    float left  = textureOffset(heightMap, texCoord, ivec2(-1, 0)).r;
    float right = textureOffset(heightMap, texCoord, ivec2(1, 0)).r;
    float up    = textureOffset(heightMap, texCoord, ivec2(0, 1)).r;
    float down  = textureOffset(heightMap, texCoord, ivec2(0, -1)).r;
    
    return normalize(vec3(down - up, 2.0, left - right));
}

void emitNormal(vec4 position, vec3 normal)
{
    gl_Position = position;
    EmitVertex();
    
    gl_Position = position + vec4(normal * normalLength, 0.0);
    EmitVertex();
    
    EndPrimitive();
}

void main()
{
    for(int i = 0; i < gl_in.length(); i++)
    {
        vec3 normal = calculateNormal(TexCoord_TES[i]);
        emitNormal(gl_in[i].gl_Position, normal);
    }
}