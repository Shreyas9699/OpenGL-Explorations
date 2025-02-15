// heightmapFS.glsl
#version 410 core
in float Height;
in vec2 TexCoord_TES;

uniform int numGrids;
uniform bool enableGrid;
uniform bool showNormals;

out vec4 FragColor;

void main()
{
    vec4 baseColor;
    float edgeThreshold = 0.01;
    // based on rez, the actual grid would be lot more and doesnt look good visuallly, hence halved
    vec2 uv = TexCoord_TES * numGrids / 32.0f;
    vec2 grid = mod(uv, 1.0);
    float h = (Height + 16)/64.0f;

    if (enableGrid && (grid.x < edgeThreshold || grid.y < edgeThreshold)) 
    {
        baseColor = vec4(1.0, 0.0, 0.0, 1.0);
    } 
    else 
    {
        baseColor = vec4(h, h, h, 1.0);
    }
    
    FragColor = baseColor;
}