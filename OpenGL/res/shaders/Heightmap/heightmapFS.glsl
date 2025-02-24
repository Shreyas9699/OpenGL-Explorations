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
    vec2 uv = TexCoord_TES * numGrids / 32.0;
    vec2 grid = mod(uv, 1.0);
    float h = (Height + 16.0)/64.0;

    vec4 baseColor = vec4(h, h, h, 1.0);

    if (enableGrid && (grid.x < edgeThreshold || grid.y < edgeThreshold)) 
    {
        baseColor = vec4(1.0, 0.0, 0.0, 1.0);
    } 

    if (showNormals)
    {
        // normal color is not coming properly
        vec3 normal = calculateNormal(TexCoord_TES);
        vec4 normalColor = vec4(1.0, 1.0, 0.0, 1.0);
        float intensity =  0.0;
        if(abs(dot(normal, vec3(0.0, 1.0, 0.0))) < 0.99)
            intensity = 1.0;
        baseColor = mix(baseColor, normalColor, intensity);
    }

    FragColor = baseColor;
}