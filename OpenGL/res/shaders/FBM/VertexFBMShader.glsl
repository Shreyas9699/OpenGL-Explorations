// VertexFBMShader.glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform float time;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

float random(vec2 st) 
{
    // This function generates a pseudo-random number based on a 2D input vector st.
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

float noise(vec2 st) 
{
    vec2 i = floor(st);
    vec2 f = fract(st);

    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Hermite smoothing function
    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) + (c - a)* u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

#define OCTAVE 4
float fbm(vec2 st) 
{
    float value = 0.0;          // accumulates the final noise value.
    float amplitude = 0.8;      // controls the contribution of each octave.
    float frequency = 2.0;      // determines how "stretched" the noise is in each layer.

    for (int i = 0; i < OCTAVE; i++) 
    {
        value += amplitude * noise(st * frequency);
        st *= frequency;
        amplitude *= 0.6;
    }
    return value;
}

void main()
{
    Normal = mat3(transpose(inverse(model))) * aNormal;
    vec3 modifiedPosition = aPos;
    vec2 gridCoords = aPos.xz * 5.0;
    float displacement = fbm(gridCoords + time * 0.2) * 0.5; // Adjust multiplier for height variation
    modifiedPosition.y += displacement;
    modifiedPosition.y = max(modifiedPosition.y, 0.0);
    FragPos = vec3(model * vec4(modifiedPosition, 1.0));
    gl_Position = projection * view * model * vec4(modifiedPosition, 1.0);
}