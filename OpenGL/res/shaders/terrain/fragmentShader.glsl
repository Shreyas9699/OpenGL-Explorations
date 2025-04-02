// Fragment Shader for terrain rendering
#version 430 core
out vec4 FragColor;


uniform vec2 u_resolution;
uniform float scale = 0.0001;
uniform int seed;
uniform int octaves;
uniform float persistence;
uniform float lacunarity;
uniform vec2 offset = vec2(0.0);
//uniform float frequency = 4.0;
//uniform float amplitude = 1.0;

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
    vec3 rockColor = vec3(0.50, 0.45, 0.40);
    vec3 darkRockColor = vec3(0.35, 0.30, 0.25);
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
    else if (nHeight < 0.55) 
    {
        v_Color = grassColor;
    }
    else if (nHeight < 0.65) 
    {
        v_Color = forestColor;
    }
    else if (nHeight < 0.75) 
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
    // Map frag coordinates to [-1,1]
    vec2 st = gl_FragCoord.xy / u_resolution.xy;
    st = st * 2.0 - 1.0;

    // FBM parameters
    float amplitude = 1.0;
    float frequency = 1.0;
    float nHeight = 0.0;
    float totalAmplitude = 0.0;
    
    // Fractal Brownian Motion loop
    for (int i = 0; i < octaves; i++) {
        vec2 octaveOffsets = getOctaveOffsets(i, seed);
        // Scale coordinates: note the order of operations
        float noise = snoise((st + octaveOffsets) * frequency / scale);
        // Remap from [-1, 1] to [0, 1]
        noise = noise * 0.5 + 0.5;
        
        nHeight += noise * amplitude;
        totalAmplitude += amplitude;
        
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    // Normalize final noise value to [0,1]
    nHeight /= totalAmplitude;
    
    // Choose terrain color based on normalized height
    vec3 v_Color = GenerateColor(nHeight);
    FragColor = vec4(v_Color, 1.0);
}