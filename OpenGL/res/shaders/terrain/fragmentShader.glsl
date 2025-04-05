// Fragment Shader for terrain rendering
#version 450 core
in float v_Height;
in vec3 v_Normal;

out vec4 FragColor;

uniform bool wireframe;
uniform vec3 u_lightDir = vec3(0.5, 1.0, 0.3);

vec3 GenerateColor(float nHeight)
{
    // Define terrain thresholds
    float DEEP_WATER = 0.30;
    float SHALLOW_WATER = 0.40;
    float SHORE = 0.45;
    float GRASS = 0.55;
    float FOREST = 0.65;
    float ROCK = 0.75;
    float SNOW = 0.85;
    
    // Define colors
    vec3 deepWaterColor = vec3(0.05, 0.13, 0.35);
    vec3 shallowWaterColor = vec3(0.15, 0.30, 0.60);
    vec3 sandColor = vec3(0.85, 0.75, 0.55);
    vec3 grassColor = vec3(0.35, 0.50, 0.20);
    vec3 forestColor = vec3(0.20, 0.30, 0.15);
    vec3 rockColor = vec3(0.36, 0.26, 0.24); //vec3(0.50, 0.45, 0.40);
    vec3 darkRockColor = vec3(0.24, 0.17, 0.16); //vec3(0.35, 0.30, 0.25);
    vec3 snowColor = vec3(0.95, 0.95, 0.95);
    
    // Apply terrain types
    vec3 v_Color;
    
    if (nHeight < 0.3) 
    {
        v_Color = deepWaterColor;
    }
    else if (nHeight < 0.4) 
    {
        v_Color = shallowWaterColor;
    } 
    else if (nHeight < 0.45) 
    {
        v_Color = sandColor;
    }
    else if (nHeight < 0.50) 
    {
        v_Color = grassColor;
    }
    else if (nHeight < 0.55)
    {
        v_Color = forestColor;
    }
    else if (nHeight < 0.60) 
    {
        v_Color = rockColor;
    }
    else if (nHeight < 0.85) 
    {
        v_Color = darkRockColor;
    }
    else 
    {
        v_Color = snowColor;
    }
    return v_Color;
}

void main() 
{
    // Choose terrain color based on normalized height
    vec3 v_Color = vec3(1.0);
    vec3 lightDir = normalize(u_lightDir);
    float diff = max(dot(v_Normal, lightDir), 0.2);

    if (!wireframe)
    {
        v_Color = GenerateColor(v_Height);
    }
    FragColor = vec4(v_Color * diff, 1.0);
}