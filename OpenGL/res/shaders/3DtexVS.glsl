// 3DtexVS.glsl 
#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

out vec2 TexCoord;

uniform mat4 u_MVP;

void main()
{
   gl_Position = u_MVP * vec4(position, 1.0);
   TexCoord = texCoord;
};