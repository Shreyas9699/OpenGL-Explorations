// heightmapFS.glsl (Fragment Shader)
#version 410 core
in float Height;
in vec2 TexCoord_TES;

uniform sampler2D heightMap;
uniform int numGrids;
uniform bool enableGrid;
uniform bool showNormals;
uniform vec2 uTexelSize; 

out vec4 FragColor;

vec3 calculateNormal(vec2 texCoord) 
{
    float left  = textureOffset(heightMap, texCoord, ivec2(-uTexelSize.x, 0)).r;
    float right = textureOffset(heightMap, texCoord, ivec2(uTexelSize.x, 0)).r;
    float up    = textureOffset(heightMap, texCoord, ivec2(0, uTexelSize.y)).r;
    float down  = textureOffset(heightMap, texCoord, ivec2(0, -uTexelSize.y)).r;
    
    return normalize(vec3(down - up, 2.0, left - right));
}

void main()
{
    float edgeThreshold = 0.01;
    // based on rez, the actual grid would be lot more and doesnt look good visuallly, hence halved
    vec2 uv = TexCoord_TES * numGrids / 32.0f;
    vec2 grid = mod(uv, 1.0);
    float h = (Height + 16)/64.0f;

    // Always set the base height color
    vec4 baseColor = vec4(h, h, h, 1.0);

    if (enableGrid && (grid.x < edgeThreshold || grid.y < edgeThreshold)) 
    {
        baseColor = vec4(1.0, 0.0, 0.0, 1.0);
    } 
    if(showNormals)
    {
        vec3 normal = calculateNormal(TexCoord_TES);
        vec4 normalColor = vec4(normal * 0.5 + 0.5, 1.0);
        baseColor = mix(baseColor, normalColor, 0.5); 
    }
    FragColor = baseColor;
}