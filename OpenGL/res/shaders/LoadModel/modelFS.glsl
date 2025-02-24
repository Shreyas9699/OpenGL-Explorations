#version 430 core
out vec4 fragment_colour;   

in vec3 vert_pos_varying;
in vec3 vertex_normal;
in vec2 texture_coordinates;
flat in uint mesh_number;
flat in uint sampler_array_pos;

uniform bool meshes_combined;
uniform sampler2D images[32];
uniform vec3 camera_position;
uniform vec3 light_position;
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 materialColor = vec3(1.0, 1.0, 1.0);
uniform float ambientStrength = 0.55;
uniform float specularStrength = 0.5;
uniform float shininess = 32.0;

void main() 
{
    vec3 view_direction = normalize(camera_position - vert_pos_varying);
    vec3 light_direction = normalize(vec3(light_position - vert_pos_varying));
    
    vec3 ambient_result = ambientStrength * lightColor;
    
    float diffuse_factor = 0.75;
    float diffuse_angle = max(dot(light_direction, vertex_normal), -0.05);
    vec3 diffuse_result = diffuse_factor * diffuse_angle * lightColor;
    
    vec3 reflect_direction = normalize(reflect(-light_direction, vertex_normal));
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), shininess);
    vec3 specular_result = specularStrength * spec * lightColor;  // Added missing semicolon

    vec3 lighting = ambient_result + diffuse_result + specular_result;
    
    if(sampler_array_pos != 4294967295u)  // Using proper if statement syntax
    {
        vec4 texColor = texture(images[sampler_array_pos], texture_coordinates);
        vec3 result = lighting * texColor.rgb;
        fragment_colour = vec4(result, texColor.a);
    }
    else
    {
        fragment_colour = vec4(lighting * materialColor, 1.0);
    }
}