#version 410 core
in float Height;
in vec2 TexCoord_TES;

out vec4 FragColor;

uniform sampler2D heightMap;
uniform bool showNormals;
uniform vec2 uTexelSize; 

vec3 calculateNormal(vec2 texCoord) 
{
    float left  = textureOffset(heightMap, texCoord, ivec2(-uTexelSize.x, 0)).r;
    float right = textureOffset(heightMap, texCoord, ivec2( uTexelSize.x, 0)).r;
    float up    = textureOffset(heightMap, texCoord, ivec2(0,  uTexelSize.y)).r;
    float down  = textureOffset(heightMap, texCoord, ivec2(0, -uTexelSize.y)).r;
    
    return normalize(vec3(down - up, 2.0, left - right));
}

void main()
{
    vec4 baseColor = vec4(1.0, 1.0, 0.0, 1.0);
    vec3 normal = calculateNormal(TexCoord_TES);
    vec4 normalColor = vec4(1.0, 1.0, 0.0, 1.0);
    float intensity =  0.0;
    if(abs(dot(normal, vec3(0.0, 1.0, 0.0))) < 0.99)
        intensity = 1.0;
    FragColor =  mix(baseColor, normalColor, intensity);
}
