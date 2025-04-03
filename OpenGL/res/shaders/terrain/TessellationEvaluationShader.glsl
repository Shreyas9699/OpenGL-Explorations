// Tessellation Evaluation Shader for procedural terrain rendering
#version 430 core
layout (triangles, equal_spacing, cw) in;
in vec3 tc_Pos[];

out float te_Height;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform vec2 u_resolution;
uniform float scale = 0.0001;
uniform int seed;
uniform int octaves;
uniform float persistence;
uniform float lacunarity;
uniform vec2 offset = vec2(0.0);

vec3 mod289(vec3 x)  { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2 mod289(vec2 x)  { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 permute(vec3 x) { return mod289(((x*34.0)+1.0)*x); }

// Simplex Noise range [-1, 1]
float snoise(vec2 v) 
{
    const vec4 C = vec4( 0.211324865405187, 
                         0.366025403784439, 
                        -0.577350269189626,
                         0.024390243902439);
    
    vec2 i  = floor(v + dot(v, C.yy) );
    vec2 x0 = v -   i + dot(i, C.xx);

    vec2 i1;
    i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;

    i = mod289(i);
    vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
        + i.x + vec3(0.0, i1.x, 1.0 ));

    vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;

    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;

    m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

    vec3 g;
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

vec2 getOctaveOffsets(int octave, int seed)
{
    float xOffset = float(((seed * 127) % 100000) + octave * 1000) / 100000.0 + offset.x;
    float yOffset = float(((seed * 347) % 100000) + octave * 1000) / 100000.0 + offset.y;
    return vec2(xOffset, yOffset);
}

void main()
{
    // Barycentric interpolation
    vec3 pos = gl_TessCoord.x * tc_Pos[0] +
               gl_TessCoord.y * tc_Pos[1] +
               gl_TessCoord.z * tc_Pos[2];

    vec2 st = pos.xz * scale;
    
    // Compute FBM (Fractal Brownian Motion) noise for height displacement
    float amplitude = 1.0;
    float frequency = 1.0;
    float nHeight = 0.0;
    float totalAmplitude = 0.0;
    
    for (int i = 0; i < octaves; i++) 
    {
        vec2 octaveOffsets = getOctaveOffsets(i, seed);
        float noise = snoise((st + octaveOffsets) * frequency / scale);
        noise = noise * 0.5 + 0.5; // Remap from [-1,1] to [0,1]
        
        nHeight += noise * amplitude;
        totalAmplitude += amplitude;
        
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    nHeight /= totalAmplitude;  // Normalize final noise value to [0,1]
    
    // Displace the vertex along the Y-axis by the noise height.
    pos.y = nHeight;
    
    // Pass the height to the fragment shader
    te_Height = nHeight;
    
    // Transform to clip space
    gl_Position = projection * view * model * vec4(pos, 1.0);
}