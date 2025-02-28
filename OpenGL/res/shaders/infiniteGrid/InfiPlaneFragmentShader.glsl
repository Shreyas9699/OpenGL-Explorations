#version 430 core
layout(location = 0) out vec4 outColor;

in vec3 nearPoint;
in vec3 farPoint;

uniform float near = 0.01;
uniform float far = 1000.0;
uniform float gridSize = -1.0;  // New uniform to control grid size
uniform mat4 view;
uniform mat4 projection;

vec4 gridLines(vec3 fragPos3D, float scale) 
{
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    
    float lineThreshold = 1.0;
    float alpha = 1.0 - min(line, 1.0);
    alpha = (alpha > (1.0 - lineThreshold)) ? alpha : 0.0;
    
    return vec4(0.2, 0.2, 0.2, alpha);
}

vec4 drawAxes(vec3 fragPos3D) 
{
    vec4 color = vec4(0.0);
    vec2 derivatives = fwidth(fragPos3D.xz);
    float miniX = min(derivatives.x, 1.0);
    float miniZ = min(derivatives.y, 1.0);
    
    if (abs(fragPos3D.x) < 0.5 * miniX) 
    {
        color = vec4(0.0, 0.0, 1.0, 1.0);
    }
    if (abs(fragPos3D.z) < 0.5 * miniZ) 
    {
        color = vec4(1.0, 0.0, 0.0, 1.0);
    }
    return color;
}

float computeDepth(vec3 pos) 
{
    vec4 clipSpacePos = projection * view * vec4(pos.xyz, 1.0);
    return (clipSpacePos.z / clipSpacePos.w) * 0.5 + 0.5;
}

float computeLinearDepth(vec3 pos) 
{
    vec4 clipSpacePos = projection * view * vec4(pos.xyz, 1.0);
    float clipSpaceDepth = (clipSpacePos.z / clipSpacePos.w) * 2.0 - 1.0;
    float linearDepth = (2.0 * near * far) / (far + near - clipSpaceDepth * (far - near));
    return linearDepth / far;
}

void main() 
{
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    if (t <= 0.0) discard;
    
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

    // Clamp grid if gridSize is positive
    if (gridSize > 0.0) 
    {
        if (abs(fragPos3D.x) > gridSize * 0.5 || abs(fragPos3D.z) > gridSize * 0.5) discard;
    }

    gl_FragDepth = computeDepth(fragPos3D);
    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0.0, (0.5 - linearDepth));

    vec4 gridColor = vec4(0.0);

    // Disable LOD if grid size is small
    if (gridSize <= 30.0 && gridSize > 0.0)
    {
        gridColor = gridLines(fragPos3D, 1.0);
    }
    else
    {
        // LOD Configuration
        const int NUM_LODS = 3;
        float lodScales[NUM_LODS] = float[](100.0, 10.0, 1.0);
        float lodStarts[NUM_LODS] = float[](0.7, 0.3, 0.0);
        float lodEnds[NUM_LODS] = float[](1.0, 0.7, 0.3);
        float transition = 0.1;

        for (int i = 0; i < NUM_LODS; i++) 
        {
            float fadeIn = smoothstep(lodStarts[i] - transition, lodStarts[i], linearDepth);
            float fadeOut = 1.0 - smoothstep(lodEnds[i], lodEnds[i] + transition, linearDepth);
            float weight = fadeIn * fadeOut;
            gridColor += gridLines(fragPos3D, lodScales[i]) * weight;
        }
    }

    vec4 axisColor = drawAxes(fragPos3D);
    outColor = gridColor + axisColor;
    outColor.a *= fading;
    outColor *= float(t > 0.0);
}