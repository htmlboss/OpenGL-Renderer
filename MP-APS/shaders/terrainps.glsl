#version 440 core

in vec2 TexCoords;
in vec3 surfaceNormal;
in vec3 toLightVector;
in vec3 toCameraVector;

uniform sampler2D texture_diffuse1;
uniform vec3 lightColor;
uniform float shineDamper;
uniform float reflectivity;

out vec4 out_Color;

void main() {

	vec3 unitNormal = normalize(surfaceNormal);
	vec3 unitLightVector = normalize(toLightVector);
	
	float nDotl = dot(unitNormal, unitLightVector);
	float brightness = max(nDotl, 0.2f);
	vec3 diffuse = brightness * lightColor;
	
	vec3 unitVectorToCamera = normalize(toCameraVector);
	vec3 lightDirection = -unitLightVector;
	vec3 reflectedLightDirection = reflect(lightDirection, unitNormal);
	
	float specularFactor = max(dot(reflectedLightDirection, unitVectorToCamera), 0.0f);
	float dampedFactor = pow(specularFactor,shineDamper);
	vec3 finalSpecular = dampedFactor * reflectivity * lightColor;
	
	out_Color =  vec4(diffuse, 1.0f) * texture(texture_diffuse1, TexCoords) + vec4(finalSpecular, 1.0f);

}