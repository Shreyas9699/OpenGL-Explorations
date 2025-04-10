// Fragment Shader for terrain rendering
#version 450 core
in float v_Height;
in vec3 v_Normal;
in float v_LandMask;

out vec4 FragColor;

uniform bool wireframe;
uniform bool enableLandmassColoring;
uniform vec3 u_lightDir = vec3(0.5, 1.0, 0.3);

vec3 GenerateColor(float nHeight, float landMask)
{
    // Enhanced terrain thresholds with more detailed transitions
    float DEEP_OCEAN = 0.25;
    float OCEAN = 0.30;
    float SHALLOW_WATER = 0.40;
    float SHORE = 0.45;
    float BEACH = 0.47;
    float LOWLAND = 0.52;
    float GRASS = 0.58;
    float FOREST = 0.65;
    float ROCK = 0.75;
    float HIGH_ROCK = 0.82;
    float SNOW = 0.85;
    
    // Enhanced color palette
    vec3 deepOceanColor = vec3(0.03, 0.10, 0.30);
    vec3 oceanColor = vec3(0.05, 0.13, 0.35);
    vec3 shallowWaterColor = vec3(0.15, 0.30, 0.60);
    vec3 shoreColor = vec3(0.25, 0.40, 0.60);
    vec3 beachColor = vec3(0.85, 0.75, 0.55);
    vec3 lowlandColor = vec3(0.45, 0.60, 0.30);
    vec3 grassColor = vec3(0.35, 0.50, 0.20);
    vec3 forestColor = vec3(0.20, 0.30, 0.15);
    vec3 rockColor = vec3(0.36, 0.26, 0.24);
    vec3 darkRockColor = vec3(0.24, 0.17, 0.16);
    vec3 snowColor = vec3(0.95, 0.95, 0.95);
    
    // Apply terrain types with smoother transitions
    vec3 terrainColor;
    
    if (nHeight < DEEP_OCEAN) 
    {
        terrainColor = deepOceanColor;
    }
    else if (nHeight < OCEAN) 
    {
        float t = (nHeight - DEEP_OCEAN) / (OCEAN - DEEP_OCEAN);
        terrainColor = mix(deepOceanColor, oceanColor, t);
    }
    else if (nHeight < SHALLOW_WATER) 
    {
        float t = (nHeight - OCEAN) / (SHALLOW_WATER - OCEAN);
        terrainColor = mix(oceanColor, shallowWaterColor, t);
    } 
    else if (nHeight < SHORE) 
    {
        float t = (nHeight - SHALLOW_WATER) / (SHORE - SHALLOW_WATER);
        terrainColor = mix(shallowWaterColor, shoreColor, t);
    }
    else if (nHeight < BEACH) 
    {
        float t = (nHeight - SHORE) / (BEACH - SHORE);
        terrainColor = mix(shoreColor, beachColor, t);
    }
    else if (nHeight < LOWLAND) 
    {
        float t = (nHeight - BEACH) / (LOWLAND - BEACH);
        terrainColor = mix(beachColor, lowlandColor, t);
    }
    else if (nHeight < GRASS) 
    {
        float t = (nHeight - LOWLAND) / (GRASS - LOWLAND);
        terrainColor = mix(lowlandColor, grassColor, t);
    }
    else if (nHeight < FOREST) 
    {
        float t = (nHeight - GRASS) / (FOREST - GRASS);
        terrainColor = mix(grassColor, forestColor, t);
    }
    else if (nHeight < ROCK) 
    {
        float t = (nHeight - FOREST) / (ROCK - FOREST);
        terrainColor = mix(forestColor, rockColor, t);
    }
    else if (nHeight < HIGH_ROCK) 
    {
        float t = (nHeight - ROCK) / (HIGH_ROCK - ROCK);
        terrainColor = mix(rockColor, darkRockColor, t);
    }
    else if (nHeight < SNOW) 
    {
        float t = (nHeight - HIGH_ROCK) / (SNOW - HIGH_ROCK);
        terrainColor = mix(darkRockColor, snowColor, t);
    }
    else 
    {
        terrainColor = snowColor;
    }
    
    // Optional: Apply landmass-specific coloring for archipelagos
    if (enableLandmassColoring && landMask < 1.0 && landMask > 0.0) 
    {
        // Make beaches more prominent on islands
        if (nHeight >= SHORE && nHeight < LOWLAND) 
        {
            float coastalInfluence = 1.0 - smoothstep(0.0, 0.2, landMask);
            terrainColor = mix(terrainColor, beachColor, coastalInfluence * 0.5);
        }
    }
    
    return terrainColor;
}

void main() 
{
    vec3 lightDir = normalize(u_lightDir);
    float diff = max(dot(v_Normal, lightDir), 0.2);
    
    if (!wireframe)
    {
        vec3 v_Color = GenerateColor(v_Height, v_LandMask);
        FragColor = vec4(v_Color, 1.0);
    }
    else
    {
        FragColor = vec4(vec3(1.0), 1.0);
    }
}