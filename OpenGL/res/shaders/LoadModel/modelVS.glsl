#version 430 core
layout (location = 0) in vec3 aPos;	 // Attribute data: vertex(s) X, Y, Z position via VBO set up on the CPU side.
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in uint aMeshNum; // Use this for identifying and transforming meshes independently.
layout (location = 4) in uint aSamplerPos; // Pass this to the fragment shader.
 
out vec3 vert_pos_varying; // Vertex position coordinates passed to the fragment shader as interpolated per-vertex.
//out vec3 vert_pos_transformed; // Transformed cube vertex position coordinates also passed as interpolated.
out vec3 vertex_normal;
out vec2 texture_coordinates;
flat out uint mesh_number; // Can be used to identify and transform meshes independently of one another.
flat out uint sampler_array_pos;
 
uniform int rendering_multiple_meshes;

uniform mat4 view;
uniform mat4 projection;
//uniform mat4 animate;
uniform mat4 model;
 
void main()
{
	if (rendering_multiple_meshes == -1)
		mesh_number = aMeshNum; // Receive mesh number via input attribute.
	else
		mesh_number = rendering_multiple_meshes; // For draw method option 0... Receive mesh number via uniform.
 
	vert_pos_varying = aPos; // Send aPos vertex position values to fragment shader, which can be used as colour values instead of using texture images.
	//vert_pos_transformed = vec3(animate * vec4(aPos, 1.0)); // Send transformed position values, which are used for the lighting effects.			
 
	texture_coordinates = aTexCoord;
	//mesh_number = aMeshNum;
	sampler_array_pos = aSamplerPos;
 
	mat3 normal_matrix = transpose(inverse(mat3(model))); //transpose(inverse(mat3(animate)));
	vertex_normal = normal_matrix * aNormal;
	
	if (length(vertex_normal) > 0)
		vertex_normal = normalize(vertex_normal); // Never try to normalise zero vectors (0,0,0)
		
	// https://www.khronos.org/opengl/wiki/Vertex_Post-Processing
	gl_Position = projection * view * model * vec4(aPos, 1.0); // projection * view * model * animate * vec4(aPos, 1.0)
}