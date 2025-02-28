#version 430 core

uniform mat4 projection;
uniform mat4 view;

out vec3 nearPoint;
out vec3 farPoint;

// Grid position are in xy clipped space
const vec3 gridPlane[4] = vec3[4] (
    vec3(1, 1, 0), vec3(-1, -1, 0), 
	vec3(-1, 1, 0), vec3(1, -1, 0)
);

const int Indices[6] = int[6](0, 1, 2, 1, 0, 3);

// reverse projection, takes point and depth to reconstruct the world-space position
vec3 unProjectPoint(float x, float y, float z, mat4 view, mat4 projection)
{
	mat4 viewInv = inverse(view);
	mat4 projectionInv = inverse(projection);
	vec4 unProjectPoint = viewInv * projectionInv * vec4(x, y, z, 1.0);
	return unProjectPoint.xyz / unProjectPoint.w;
}

void main()
{
	vec3 p = gridPlane[Indices[gl_VertexID]].xyz;
	nearPoint = unProjectPoint(p.x, p.y, 0.0, view, projection).xyz; // unprojecting on the near plane center
	farPoint = unProjectPoint(p.x, p.y, 1.0, view, projection).xyz; // unprojecting on the far plane center
	gl_Position = vec4(p, 1.0);
}