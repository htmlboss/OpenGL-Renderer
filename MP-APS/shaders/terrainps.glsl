#version 440 core

in vec2 TexCoords;
in vec3 surfaceNormal;
in vec3 toLightVector;
in vec3 toCameraVector;

uniform sampler2D texture_diffuse1;     // Grass bg
uniform sampler2D texture_diffuse2;     // Flowers
uniform sampler2D texture_diffuse3;     // Dirt
uniform sampler2D texture_diffuse4;     // Gravel
uniform sampler2D texture_diffuse5;     // Blend map

uniform vec3 lightColor;
uniform float shineDamper;
uniform float reflectivity;

out vec4 out_Color;

void main() {
    
    const vec4 blendMapColor = texture(texture_diffuse5, TexCoords);
    const float bgTex = 1.0f - (blendMapColor.r + blendMapColor.g + blendMapColor.b);
    const vec2 tiledTexCoords = TexCoords * 50.0f; // Scale tex coords

    const vec4 bgGrassAmount = texture(texture_diffuse1, tiledTexCoords) * bgTex;           // How much grass is showing
    const vec4 rDirtAmount = texture(texture_diffuse3, tiledTexCoords) * blendMapColor.r;   // Red component on blend map
    const vec4 gFlowerAmount = texture(texture_diffuse2, tiledTexCoords) * blendMapColor.g; // Green component
    const vec4 bGravelAmount = texture(texture_diffuse4, tiledTexCoords) * blendMapColor.b; // Blue component

    const vec4 totalTextureColor = bgGrassAmount + rDirtAmount + gFlowerAmount + bGravelAmount;

	vec3 unitNormal = normalize(surfaceNormal);
	vec3 unitLightVector = normalize(toLightVector);
	
	float brightness = max(dot(unitNormal, unitLightVector), 0.2f);
	vec3 diffuse = lightColor * brightness;
	
	vec3 unitVectorToCamera = normalize(toCameraVector);
	vec3 lightDirection = -unitLightVector;
	vec3 reflectedLightDirection = reflect(lightDirection, unitNormal);
	
	float specularFactor = max(dot(reflectedLightDirection, unitVectorToCamera), 0.0f);
	float dampedFactor = pow(specularFactor, shineDamper);
	vec3 finalSpecular = dampedFactor * reflectivity * lightColor;
	
	out_Color =  vec4(diffuse, 1.0f) * totalTextureColor + vec4(finalSpecular, 1.0f);

}