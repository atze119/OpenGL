#version 460 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    sampler2D emission;
    float shininess;
}; 

struct Light {
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    float distance = length(light.position - FragPos); // retrieves the distance between lightsource and fragment
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  
    
      // emission
    vec3 emission = vec3(0.0f);
    // check the specular box (steel) if its black -> middle box is black and then render the texture inside this box.
    if (texture(material.specular, TexCoords).r == 0.0)
    {
        emission = texture(material.emission, TexCoords).rgb;
//        vec3 emissionMask = step(vec3(1.0f), vec3(1.0f)-texture(material.specular, TexCoords).rgb);
//        emission *= emissionMask;
    }

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular + emission;
    FragColor = vec4(result, 1.0);
} 