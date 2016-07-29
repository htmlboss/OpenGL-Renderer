#version 440 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec3 Normal;  
  
out vec4 color;
  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main() {

    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float lambertian = max(dot(lightDir, normal), 0.0);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 specular = vec3(0.0);

    // Ambient
    vec3 ambient = light.ambient * material.ambient;
  	
    // Diffuse 
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    // Specular
    if (lambertian > 0.0) {
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfDir, normal), 0.0), material.shininess);
    //vec3 reflectDir = reflect(-lightDir, normal);  
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    specular = light.specular * (spec * material.specular);
    }  
        
    vec3 result = ambient + lambertian * diffuse + specular;
    color = vec4(result, 1.0f);
} 