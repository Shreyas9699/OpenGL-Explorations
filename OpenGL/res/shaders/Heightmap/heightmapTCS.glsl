// heightmapTCS.glsl
#version 410 core
layout(vertices=4) out;
in vec2 TexCoord[];

uniform mat4 model;
uniform mat4 view;
uniform bool isDynamicTess;
uniform float fovCos;

out vec2 TextureCoord[];

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];
    if (gl_InvocationID == 0)
    {
        if (!isDynamicTess)
        {
            gl_TessLevelOuter[0] = 32;
            gl_TessLevelOuter[1] = 32;
            gl_TessLevelOuter[2] = 32;
            gl_TessLevelOuter[3] = 32;
            
            gl_TessLevelInner[0] = 32;
            gl_TessLevelInner[1] = 32;
        }
        else
        {
            // Dynamic tesstllation levels based on distance.
            const int MIN_TESS_LEVEL = 4;
            const int MAX_TESS_LEVEL = 64;
            const float MIN_DISTANCE = 20;
            const float MAX_DISTANCE = 800;
            
            vec4 eyeSpacePos00 = view * model * gl_in[0].gl_Position;
            vec4 eyeSpacePos01 = view * model * gl_in[1].gl_Position;
            vec4 eyeSpacePos10 = view * model * gl_in[2].gl_Position;
            vec4 eyeSpacePos11 = view * model * gl_in[3].gl_Position;
            
            // "distance" from camera scaled between 0 and 1
            float distance00 = clamp( (length(eyeSpacePos00.xyz) - MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0 );
            float distance01 = clamp( (length(eyeSpacePos01.xyz) - MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0 );
            float distance10 = clamp( (length(eyeSpacePos10.xyz) - MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0 );
            float distance11 = clamp( (length(eyeSpacePos11.xyz) - MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0 );
            
            float cosAngle00 = dot(normalize(eyeSpacePos00.xyz), vec3(0.0, 0.0, -1.0));
            float cosAngle01 = dot(normalize(eyeSpacePos01.xyz), vec3(0.0, 0.0, -1.0));
            float cosAngle10 = dot(normalize(eyeSpacePos10.xyz), vec3(0.0, 0.0, -1.0));
            float cosAngle11 = dot(normalize(eyeSpacePos11.xyz), vec3(0.0, 0.0, -1.0));

            float tessLevel0 = 0.0;
            float tessLevel1 = 0.0;
            float tessLevel2 = 0.0;
            float tessLevel3 = 0.0;

            if (cosAngle00 >= fovCos || cosAngle01 >= fovCos || cosAngle10 >= fovCos || cosAngle11 >= fovCos)
            {
                // Inside the view frustum
                tessLevel0 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance10, distance00) );
                tessLevel1 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance00, distance01) );
                tessLevel2 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance01, distance11) );
                tessLevel3 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance11, distance10) );
            }
            
            gl_TessLevelOuter[0] = tessLevel0;
            gl_TessLevelOuter[1] = tessLevel1;
            gl_TessLevelOuter[2] = tessLevel2;
            gl_TessLevelOuter[3] = tessLevel3;
            
            gl_TessLevelInner[0] = max(tessLevel1, tessLevel3);
            gl_TessLevelInner[1] = max(tessLevel0, tessLevel2);
        }
    }
}