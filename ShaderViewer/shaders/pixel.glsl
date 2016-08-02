#version 440 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
}; 

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // Attenuation variables
    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;  
in vec3 Normal;
in vec2 TexCoords;
  
out vec4 color;
  
uniform vec3 viewPos;
uniform Material material;
uniform PointLight light;

void main() {

    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float lambertian = max(dot(lightDir, normal), 0.0);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 specular = vec3(0.0);

    //Attenuation
    float dist = length(light.position - FragPos);
    float attenuation = 1.0f / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    // Ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    ambient *= attenuation;
  	
    // Diffuse 
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    diffuse *= attenuation;
    
    // Specular
    if (lambertian > 0.0) {
        vec3 halfDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(halfDir, normal), 0.0), material.shininess);
        specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));
        specular *= attenuation;
    }
        
    vec3 result = ambient + lambertian * diffuse + specular;
    color = vec4(result, 1.0f);
} 