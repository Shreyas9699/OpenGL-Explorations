// Tessellation Evaluation Shader for procedural terrain rendering
#version 450 core
layout(quads, fractional_odd_spacing, ccw) in;
in vec3 tcs_Pos[];

out float v_Height;
out vec3 v_Normal;
out float v_LandMask;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float scale;
uniform int seed;
uniform int octaves;
uniform float persistence;
uniform float lacunarity;
uniform vec2 offset;
uniform float heightMultiplier;

uniform int noiseType = 0; // 0 = Simplex, 1 = Perlin
uniform float seaLevel; // Controls water coverage (0.3-0.7 is good range)
uniform float islandDensity; // Controls number of small islands (0.0-1.0)
uniform bool enableIslands; // Toggle island features

vec3 mod289(vec3 x)  { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2 mod289(vec2 x)  { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 permute(vec3 x) { return mod289(((x*34.0)+1.0)*x); }

vec2 getOctaveOffsets(int octave, int seed)
{
    float xOffset = float(((seed * 127) % 100000) + octave * 1000) / 100000.0 + offset.x;
    float yOffset = float(((seed * 347) % 100000) + octave * 1000) / 100000.0 + offset.y;
    return vec2(xOffset, yOffset);
}

float getAnimationCurve(float height)
{
    // to remove height effect on water
    if(height < 0.4)
        return 0.0;

    return smoothstep(0.4, 1.0, height);
}

// -------------------------- Simplex Noise range [-1, 1] --------------------------
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

// -------------------------- Perlin Noise --------------------------
float fade(float t) 
{
  return t*t*t*(t*(t*6.0 - 15.0) + 10.0);
}

vec2 grad(vec2 p) 
{
    // Hash function to generate pseudo-random vector
    float angle = fract(sin(dot(p , vec2(127.1, 311.7))) * 43758.5453) * 6.2831853;
    return vec2(cos(angle), sin(angle)); // unit vector
}

float pnoise(vec2 p) 
{
  /* Calculate lattice points. */
  vec2 p0 = floor(p);
  vec2 p1 = p0 + vec2(1.0, 0.0);
  vec2 p2 = p0 + vec2(0.0, 1.0);
  vec2 p3 = p0 + vec2(1.0, 1.0);
  
  /* Look up gradients at lattice points. */
  vec2 g0 = grad(p0);
  vec2 g1 = grad(p1);
  vec2 g2 = grad(p2);
  vec2 g3 = grad(p3);
    
  float t0 = p.x - p0.x;
  float fade_t0 = fade(t0); /* Used for interpolation in horizontal direction */

  float t1 = p.y - p0.y;
  float fade_t1 = fade(t1); /* Used for interpolation in vertical direction. */

  /* Calculate dot products and interpolate.*/
  float p0p1 = (1.0 - fade_t0) * dot(g0, (p - p0)) + fade_t0 * dot(g1, (p - p1)); /* between upper two lattice points */
  float p2p3 = (1.0 - fade_t0) * dot(g2, (p - p2)) + fade_t0 * dot(g3, (p - p3)); /* between lower two lattice points */
  
  /* Calculate final result */
  return (1.0 - fade_t1) * p0p1 + fade_t1 * p2p3;
}

// -------------------------- Domain warping for more natural coastlines --------------------------
vec2 domainWarp(vec2 coord, float strength) 
{
    float noiseX = snoise(coord) * strength;
    float noiseY = snoise(coord + vec2(5.2, 1.3)) * strength;
    return coord + vec2(noiseX, noiseY);
}

// -------------------------- Creates continent/island masks --------------------------
float landmassDistribution(vec2 coord, float seaLevel) 
{
    // Large-scale gradient for continent shapes
    float continentShape = snoise(coord * 0.03) * 0.5 + 0.5;
    
    // Apply domain warping for natural coastlines
    vec2 warped = domainWarp(coord * 0.1, 2.0);
    float coastalDetail = snoise(warped * 0.2) * 0.5 + 0.5;
    
    // Combine and apply sea level threshold
    float combined = continentShape * coastalDetail;
    return smoothstep(seaLevel - 0.1, seaLevel + 0.1, combined);
}

void main() 
{
    float u = gl_TessCoord.x, v = gl_TessCoord.y;

    // Bilinear interpolation of model-space position
    vec3 top = mix(tcs_Pos[0], tcs_Pos[1], u);
    vec3 bottom = mix(tcs_Pos[3], tcs_Pos[2], u);
    vec3 pos = mix(top, bottom, v);

    vec4 worldPos = model * vec4(pos, 1.0);

    // Generate height using FBM
    vec2 st = worldPos.xz;
    float height = 0.0;
    float amplitude = 1.0;
    float frequency = 1.0;
    float totalAmplitude = 0.0;

    // Fractal Brownian Motion loop (keep your existing loop)
    for (int i = 0; i < octaves; i++) 
    {
        vec2 octaveOffsets = getOctaveOffsets(i, seed);
        float noise;
        if (noiseType == 0)
        {
            noise = snoise((st + octaveOffsets) * frequency / scale);
        }
        else if (noiseType == 1)
        {
            noise = pnoise((st + octaveOffsets) * frequency / scale);
        }
        
        noise = noise * 0.5 + 0.5;
        
        height += noise * amplitude;
        totalAmplitude += amplitude;
        
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    
    // Normalize final noise value
    height /= totalAmplitude;
    
    // Apply landmass distribution mask
    float landMask = 1.0;
    if (enableIslands) 
    {
        landMask = landmassDistribution(st, seaLevel);
        
        // Add small islands based on a different noise scale
        if (islandDensity > 0.0) 
        {
            float islands = snoise(st * 0.2) * 0.5 + 0.5;
            float islandMask = smoothstep(1.0 - islandDensity * 0.3, 1.0, islands);
            landMask = max(landMask, islandMask * 0.8); // Blend islands with main landmass
        }
        
        // Apply the mask to height
        float oceanDepth = 0.25; // Deep ocean baseline
        height = mix(oceanDepth, height, landMask);
    }
    
    // Store raw height before applying multiplier (for coloring)
    float rawHeight = height;
    
    // Apply heightMultiplier with animation curve
    pos.y = getAnimationCurve(height) * heightMultiplier;

    // Calculate normal (keep your existing normal calculation)
    float delta = 0.5;
    float hL = snoise(st + vec2(-delta, 0.0));
    float hR = snoise(st + vec2(delta, 0.0));
    float hD = snoise(st + vec2(0.0, -delta));
    float hU = snoise(st + vec2(0.0, delta));

    vec3 normal = normalize(vec3(hL - hR, 2.0, hD - hU));
    v_Normal = normal;

    gl_Position = projection * view * vec4(pos, 1.0);
    v_Height = rawHeight; // Pass the raw height for coloring
    
    // Add an output to fragment shader if you want to use landMask there
    v_LandMask = landMask;
}