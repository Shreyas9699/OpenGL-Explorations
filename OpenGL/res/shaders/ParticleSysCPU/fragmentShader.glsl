#version 410 core
out vec4 FragmenOut;

in vec4 VfragColor;

uniform vec4 color = vec4(1.0, 0.0, 0.0, 1.0);

void main()
{
	vec2 cirCoord = gl_PointCoord * 2.0 - 1.0;
	float dist = dot(cirCoord, cirCoord);

	if (dist > 1.0)
	{
		discard;
	}

	//float alpha = 1.0 - smoothstep(0.8, 1.0, dist);
	FragmenOut = VfragColor;
	//FragmenOut.a *= alpha;
}