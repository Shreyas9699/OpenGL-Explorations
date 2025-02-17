// heightmapTES.glsl (Tessellation Evaluation Shader)
#version 410 core
layout(quads, fractional_odd_spacing, ccw) in;

uniform sampler2D heightMap;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec2 TextureCoord[];

out vec2 TexCoord_TES;
out float Height;

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec2 t00 = TextureCoord[0];
    vec2 t01 = TextureCoord[1];
    vec2 t10 = TextureCoord[2];
    vec2 t11 = TextureCoord[3];

    vec2 t0 = mix(t00, t10, u);;
    vec2 t1 = mix(t01, t11, u);
    vec2 texCoord = mix(t0, t1, v);

    TexCoord_TES = texCoord;
    Height = texture(heightMap, texCoord).y * 64.0 - 16.0;

    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    vec4 uVec = p01 - p00;
    vec4 vVec = p10 - p00;
    vec4 normal = normalize( vec4(cross(vVec.xyz, uVec.xyz), 0) );

    vec4 p0 = mix(p00, p10, u);
    vec4 p1 = mix(p01, p11, u);
    vec4 p = mix(p0, p1, v) + normal * Height;

    gl_Position = projection * view * model * p;
}