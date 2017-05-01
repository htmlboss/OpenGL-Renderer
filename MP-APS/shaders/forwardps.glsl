#version 440 core

in vec3 Color;
in vec3 Normal;
in vec3 dirToLight;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform vec3 lightColor;

out vec4 FinalColor;

void main() {

    const vec3 unitNormal = normalize(Normal);
    const vec3 unitLightDir = normalize(dirToLight);
    
    const float nDotl = dot(unitNormal, unitLightDir);
    const float brightness = max(nDotl, 0.0f);

    const vec3 diffuse = brightness * lightColor * Color;

    FinalColor = /*vec4(diffuse, 1.0f)*/ texture(texture_diffuse1, TexCoords);
}