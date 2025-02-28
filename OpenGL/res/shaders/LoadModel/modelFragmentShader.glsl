// Mode loader Fragment shader
#version 430 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

// Lighting uniforms
uniform vec3 lightPos;
uniform vec3 camera_position;
uniform vec3 lightColor;
uniform bool useTexture;
uniform vec3 objectColor;

void main()
{
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(camera_position - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = ambient + diffuse + specular;
    if(useTexture) 
    {
        FragColor = texture(texture_diffuse1, TexCoords) * vec4(result, 1.0);
    }
    else
    {
        FragColor = vec4(objectColor * result, 1.0);
    }
}
