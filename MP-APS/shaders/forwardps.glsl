#version 440 core

in vec3 Color;
in vec3 Normal;
in vec3 dirToLight;
in vec2 TexCoords;

uniform vec3 lightColor;

out vec4 FinalColor;

void main() {

    const vec3 unitNormal = normalize(Normal);
    const vec3 unitLightDir = normalize(dirToLight);
    
    const float nDotl = dot(unitNormal, unitLightDir);
    const float brightness = max(nDotl, 0.0f);

    const vec3 diffuse = brightness * lightColor;

    FinalColor = vec4(Color, 1.0f);
}