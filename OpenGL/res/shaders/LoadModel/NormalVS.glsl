// normalVS.glsl
#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 normal;
    vec3 position;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float explosionFactor = 1.0;

void main()
{
    vec3 Position = aPos + aNormal * explosionFactor;
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vs_out.normal = normalize(normalMatrix * aNormal);
    vs_out.position = vec3(model * vec4(Position, 1.0));
    gl_Position = projection * view * model * vec4(Position, 1.0);
}