// heightmapGS.glsl (Geometry Shader)
#version 410 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out; // 2 vertices per normal line

in vec2 TexCoord_TES[];
in float Height;

uniform sampler2D heightMap;
uniform bool showNormals;
uniform vec2 uTexelSize; 
uniform float normalLength = 0.5; // Length of normal lines

out float Height_gs;
out vec2 TexCoord_gs[];

vec3 calculateNormal(vec2 texCoord) 
{
    // Your existing normal calculation
    float left  = textureOffset(heightMap, texCoord, ivec2(-uTexelSize.x, 0)).r;
    float right = textureOffset(heightMap, texCoord, ivec2(uTexelSize.x, 0)).r;
    float up    = textureOffset(heightMap, texCoord, ivec2(0, uTexelSize.y)).r;
    float down  = textureOffset(heightMap, texCoord, ivec2(0, -uTexelSize.y)).r;
    
    return normalize(vec3(down - up, 2.0, left - right));
}

void EmitNormalLine(vec4 position, vec3 normal)
{
    gl_Position = position;
    EmitVertex();
    
    gl_Position = position + vec4(normal * normalLength, 0.0);
    EmitVertex();
    
    EndPrimitive();
}

void main()
{
    Height_gs = Height;
    TexCoord_gs = TexCoord_TES;
    // First emit the original triangle
    for(int i = 0; i < 3; i++)
    {
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
    
    // Then emit normal lines if enabled
    if(showNormals)
    {
        for(int i = 0; i < 3; i++)
        {
            vec3 normal = calculateNormal(TexCoord_TES[i]);
            EmitNormalLine(gl_in[i].gl_Position, normal);
        }
    }
}