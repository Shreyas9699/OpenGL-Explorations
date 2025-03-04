#version 420 core
 
layout (location = 0) in vec3 aPos;	 // Attribute data: vertex(s) X, Y, Z position via VBO set up on the CPU side.
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
 
out vec3 vert_pos_varying; // Vertex position coordinates passed to the fragment shader as interpolated per-vertex.
out vec3 vert_pos_transformed; // Transformed cube vertex position coordinates also passed as interpolated.
out vec3 vertex_normal;
out vec2 texture_coordinates;
 
uniform mat4 view;
uniform mat4 projection;
uniform mat4 animate;
 
void main()
{
	vert_pos_varying = aPos; // Send aPos vertex position values to fragment shader, which can be used as colour values instead of using texture images.
	vert_pos_transformed = vec3(animate * vec4(aPos, 1.0)); // Send transformed position values, which are used for the lighting effects.
 
	texture_coordinates = aTexCoord;
 
	mat3 normal_matrix = transpose(inverse(mat3(animate)));
	vertex_normal = normal_matrix * aNormal;
	
	if (length(vertex_normal) > 0)
		vertex_normal = normalize(vertex_normal); //Never try to normalise zero vectors (0,0,0)
		
	// https://www.khronos.org/opengl/wiki/Vertex_Post-Processing
	gl_Position = projection * view * animate * vec4(aPos, 1.0); // Output to vertex stream for the "Vertex Post-Processing" stage.

}
