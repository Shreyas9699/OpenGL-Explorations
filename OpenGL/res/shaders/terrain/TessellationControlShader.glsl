// Tessellation Control Shader for procedural terrain rendering
#version 430 core
layout(vertices = 4) out;

in vec3 vs_Pos[];
out vec3 tcs_Pos[];

uniform mat4 model;
uniform mat4 view;

void main() 
{
    tcs_Pos[gl_InvocationID] = vs_Pos[gl_InvocationID];
    
    if (gl_InvocationID == 0) 
    {
        // Transform patch corners to world space
        vec4 worldPos00 = model * vec4(vs_Pos[0], 1.0);
        vec4 worldPos01 = model * vec4(vs_Pos[1], 1.0);
        vec4 worldPos10 = model * vec4(vs_Pos[2], 1.0);
        vec4 worldPos11 = model * vec4(vs_Pos[3], 1.0);
        
        // Get the camera position from the inverse view matrix
        vec3 cameraPos = vec3(inverse(view)[3]);
        
        // Calculate distances from camera to each corner
        float dist00 = distance(cameraPos, worldPos00.xyz);
        float dist01 = distance(cameraPos, worldPos01.xyz);
        float dist10 = distance(cameraPos, worldPos10.xyz);
        float dist11 = distance(cameraPos, worldPos11.xyz);
        
        // Calculate the center of the patch
        vec3 patchCenter = (worldPos00.xyz + worldPos01.xyz + worldPos10.xyz + worldPos11.xyz) * 0.25;
        
        // Calculate distance to patch center
        float distToCenter = distance(cameraPos, patchCenter);
        
        // Dynamic tessellation constants
        const float MIN_DISTANCE = 1.0;
        const float MAX_DISTANCE = 10.0;
        const int MIN_TESS_LEVEL = 1;
        const int MAX_TESS_LEVEL = 32;
        
        // Apply simple distance-based tessellation
        float tessellationFactor = 1.0 - clamp((distToCenter - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
        float tessLevel = mix(float(MIN_TESS_LEVEL), float(MAX_TESS_LEVEL), tessellationFactor);
        
        // Apply distance-based tessellation to each edge
        gl_TessLevelOuter[0] = tessLevel;
        gl_TessLevelOuter[1] = tessLevel;
        gl_TessLevelOuter[2] = tessLevel;
        gl_TessLevelOuter[3] = tessLevel;
        
        // Apply to inner tessellation levels
        gl_TessLevelInner[0] = tessLevel;
        gl_TessLevelInner[1] = tessLevel;
    }
}