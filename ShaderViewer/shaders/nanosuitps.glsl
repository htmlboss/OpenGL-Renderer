#version 430

// http://www.learnopengl.com/#!Lighting/Light-casters
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

in vec2 TexCoords;
in vec3 FragPos;  
in vec3 Normal;

out vec4 color;

uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_reflectance1;
uniform samplerCube skybox;

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
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float lambertian = max(dot(lightDir, normal), 0.0);
    vec3 specular = vec3(0.0);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    if (lambertian > 0.0) {
        vec3 halfDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(halfDir, normal), 0.0), 64.0);
        specular = light.specular * (spec * vec3(texture(texture_specular1, TexCoords)));
    }
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // Combine
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return vec3(ambient + diffuse + specular);
}

/*********************************************************************/
// http://www.learnopengl.com/img/advanced/cubemaps_reflection_theory.png
vec4 CalcReflectance(vec3 Position, vec3 CameraPos, vec3 normal) {

    vec3 I = normalize(Position - CameraPos);
    vec3 R = reflect(I, normal);
    float reflect_intensity = texture(texture_reflectance1, TexCoords).r;
    
    vec4 reflect_color;
    if(reflect_intensity > 0.1) { // Only sample reflections when above a certain treshold
        reflect_color = texture(skybox, R) * reflect_intensity;
    }

    return reflect_color;
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

    PointLight pLight;
    pLight.position = vec3(1.0, 2.0, 0.5);
    pLight.ambient = vec3(0.1, 0.1, 0.1);
    pLight.diffuse = vec3(1.0, 0.8, 0.5);
    pLight.specular = vec3(1.0, 0.8, 0.5);
    pLight.linear = 0.15;
    pLight.quadratic = 0.07;
    pLight.constant = 1.0;
    result += CalcPointLight(pLight, normal, FragPos, viewDir);

    vec4 resultCasted = vec4(result, 1.0);
    
    // Need to allow passing of defines from C++ so this shader can be shared
    // between the nanosuit and the floor (not physically correct, but whatever)
    #define REFLECT
    #ifdef REFLECT
    vec4 reflectance = CalcReflectance(FragPos, viewPos, normal);
    resultCasted += reflectance;
    #endif

    color = resultCasted;
}