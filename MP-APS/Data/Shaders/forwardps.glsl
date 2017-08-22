#version 440 core

struct Light {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 FragPos;
in vec3 Color;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

uniform Light light;
uniform vec3 viewPos;

out vec4 FinalColor;

void main() {
    // Ambient
	const vec3 ambient = light.ambient * texture(texture_diffuse1, TexCoords).rgb;
	
	// Diffuse
	const vec3 norm = normalize(Normal);
	const vec3 lightDir = normalize(-light.direction);
	const float diff = max(dot(norm, lightDir), 0.0);
	const vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = light.specular * spec * texture(texture_specular1, TexCoords).rgb;  

    FinalColor = vec4( ambient + diffuse + specular, 1.0);
}