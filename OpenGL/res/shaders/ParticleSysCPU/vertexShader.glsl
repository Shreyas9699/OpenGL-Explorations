#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in float aSize;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

out vec4 VfragColor;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	float dist = length((view * model * vec4(aPos, 1.0)));
	VfragColor = aColor;
	gl_PointSize = aSize * 40.0 / dist;
}