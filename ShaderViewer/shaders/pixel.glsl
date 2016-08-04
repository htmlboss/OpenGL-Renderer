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

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec3 Normal;
in vec2 TexCoords;
  
out vec4 color;

uniform vec3 viewPos;
uniform Material material;
//Lights
#define NUM_POINT_LIGHTS 4
uniform DirectionalLight dirLight;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform SpotLight spotLight;

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
        float spec = pow(max(dot(halfDir, normal), 0.0), material.shininess);
        specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));
    }
    // Combine
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    
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
        float spec = pow(max(dot(halfDir, normal), 0.0), material.shininess);
        specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));
    }
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // Combine
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return vec3(ambient + diffuse + specular);
}

/*********************************************************************/
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float lambertian = max(dot(lightDir, normal), 0.0);
    // Ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    // Specular shading
    vec3 specular = vec3(0.0);
    if (lambertian > 0.0) {
        vec3 halfDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(halfDir, normal), 0.0), material.shininess);
        specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));
    }
    // Spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    diffuse *= attenuation;
    ambient *= attenuation;
    specular *= attenuation;

    return vec3(ambient + diffuse + specular);
}

/*********************************************************************/
void main() {
    // Properties
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Directional Lighting
    vec3 result = CalcDirLight(dirLight, normal, viewDir);
    
    // Point Lights
    for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
        result += CalcPointLight(pointLights[i], normal, FragPos, viewDir);
    }

    // Spotlight
    result += CalcSpotLight(spotLight, normal, FragPos, viewDir);
    
    color = vec4(result, 1.0);
} 