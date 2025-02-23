#version 430 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform bool isTex;

uniform vec3 diffuseColor;
uniform sampler2D diffuseTexture;

void main()
{    
    
    vec4 texColor = vec4(1.0);
    if (isTex) 
    {
        texColor = texture(diffuseTexture, TexCoords);
    } 
    else 
    {
        texColor = vec4(diffuseColor, 1.0);
    }
    // Normalize the normal vector
    vec3 norm = normalize(Normal);
    
    // Calculate light direction
    vec3 lightDir = normalize(lightPos - FragPos);
    
    // **Ambient Lighting**
    float ambientStrength = 0.2; // Soft ambient light
    vec3 ambient = ambientStrength * lightColor;

    // **Diffuse Lighting (Lambertian Reflection)**
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // **Specular Lighting (Shiny Effect)**
    float specularStrength = 0.5; 
    vec3 viewDir = normalize(viewPos - FragPos); // Direction to the camera
    vec3 reflectDir = reflect(-lightDir, norm); // Reflection vector
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); // Shininess factor
    vec3 specular = specularStrength * spec * lightColor;

    // **Combine Lighting Components**
    vec3 lighting = ambient + diffuse + specular;
    
    // **Apply Lighting to Texture**
    vec3 finalColor;
    if(isTex)
    {
        vec3 textureColor = texture(texture_diffuse1, TexCoords).rgb;
        finalColor = textureColor * lighting;
    }
    else
    {
        finalColor = lighting;
    }
    
    FragColor = vec4(finalColor, 1.0);
}