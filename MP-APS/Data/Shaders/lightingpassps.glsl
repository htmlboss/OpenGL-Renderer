#version 440 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D ssao;

uniform vec3 viewPos;

struct DirLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

/***********************************************************************************/
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(-light.direction);
  // Diffuse shading
  float diff = max(dot(normal, lightDir), 0.0);
  // Specular shading
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
  // Combine results
  vec3 ambient = light.ambient * vec3(texture(gAlbedoSpec, TexCoords).rgb);
  vec3 diffuse = light.diffuse * diff * vec3(texture(gAlbedoSpec, TexCoords).rgb);
  vec3 specular = light.specular * spec * vec3(texture(gAlbedoSpec, TexCoords).a);
  
  return (ambient + diffuse + specular);
}

/***********************************************************************************/
void main() {
    // Retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    // Then calculate lighting as usual
    vec3 lighting  = Diffuse * 0.0f;
    vec3 viewDir  = normalize(viewPos - FragPos);

    // Create sun
    DirLight sun;
    sun.direction = vec3(-0.2f, -1.0f, -0.3f);
    sun.ambient = vec3(0.1f, 0.1f, 0.1f);
    sun.diffuse = vec3(0.5f, 0.5f, 0.5f);
    sun.specular = vec3(1.0f);
    
    FragColor = vec4(CalcDirLight(sun, Normal, viewDir), 1.0);
}
