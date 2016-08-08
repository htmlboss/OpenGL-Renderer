#version 450

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TexCoords;
in vec3 FragPos;  
in vec3 Normal;

out vec4 color;

uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

/*********************************************************************/
vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    float lambertian = max(dot(lightDir, normal), 0.0);
    vec3 specular = vec3(0.0);
    
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular
   if (lambertian > 0.0) {
        vec3 halfDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(halfDir, normal), 0.0), 64.0);
        specular = light.specular * (spec * vec3(texture(texture_specular1, TexCoords)));
    }
    // Combine
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    
    return vec3(ambient + diffuse + specular);
}

/*********************************************************************/
void main() {
    // Properties
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);

    DirectionalLight dirLight;
    dirLight.direction = vec3(1.0, 0.3, 0.5);
    dirLight.ambient = vec3(0.1, 0.1, 0.1);
    dirLight.diffuse = vec3(0.9, 0.9, 0.9);
    dirLight.specular = vec3(1.0, 1.0, 1.0);
    result += CalcDirLight(dirLight, normal, viewDir);

    DirectionalLight dirLight2;
    dirLight2.direction = vec3(-1.0, 0.3, 0.5);
    dirLight2.ambient = vec3(0.1, 0.1, 0.1);
    dirLight2.diffuse = vec3(0.9, 0.9, 0.9);
    dirLight2.specular = vec3(1.0, 1.0, 1.0);
    result += CalcDirLight(dirLight2, normal, viewDir);

    color = vec4(result, 1.0);
}