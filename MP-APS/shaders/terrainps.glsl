#version 440 core

struct Light {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec2 TexCoords;
in vec3 FragPos;
in vec4 LightSpaceFragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;     // Grass bg
uniform sampler2D texture_diffuse2;     // Flowers
uniform sampler2D texture_diffuse3;     // Dirt
uniform sampler2D texture_diffuse4;     // Gravel
uniform sampler2D texture_diffuse5;     // Blend map
uniform sampler2D shadowMap;

uniform vec3 viewPos;
uniform Light light;

out vec4 out_Color;

float CalculateShadow(const vec4 lightSpaceFragPos) {
	
	vec3 projCoords = lightSpaceFragPos.xyz / lightSpaceFragPos.w; // For perspective shadow map
	projCoords = projCoords * 0.5 + 0.5; // Transform to [0, 1] range;

	const float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;

	float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
	return shadow;
}

void main() {
    
    const vec4 blendMapColor = texture(texture_diffuse5, TexCoords);
    const float bgTex = 1.0f - (blendMapColor.r + blendMapColor.g + blendMapColor.b);
    const vec2 tiledTexCoords = TexCoords * 100.0f; // Scale tex coords

    const vec4 bgGrassAmount = texture(texture_diffuse1, tiledTexCoords) * bgTex;           // How much grass is showing
    const vec4 rDirtAmount = texture(texture_diffuse3, tiledTexCoords) * blendMapColor.r;   // Red component on blend map
    const vec4 gFlowerAmount = texture(texture_diffuse2, tiledTexCoords) * blendMapColor.g; // Green component
    const vec4 bGravelAmount = texture(texture_diffuse4, tiledTexCoords) * blendMapColor.b; // Blue component
	
	// Determine texture of pixel
    const vec4 totalTextureColor = bgGrassAmount + rDirtAmount + gFlowerAmount + bGravelAmount;
	
	// Ambient
	const vec3 ambient = light.ambient * totalTextureColor.xyz;
	
	// Diffuse
	const vec3 norm = normalize(Normal);
	const vec3 lightDir = normalize(-light.direction);
	const float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = light.diffuse * diff * totalTextureColor.xyz;

	// Specular
	const vec3 viewDir = normalize(viewPos - FragPos);
	const vec3 reflectDir = reflect(-lightDir, norm);
	const float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 64.0f);
	const vec3 specular = light.specular * spec; // Need specular map

	// Shadow
	const float shadow = CalculateShadow(LightSpaceFragPos);

	out_Color =  vec4(ambient + (1.0 - shadow) * diffuse, 1.0f);
}