#version 440 core

in vec3 WorldPos;

uniform samplerCube environmentMap;

out vec4 FragColor;

void main() {
    vec3 envColor = textureLod(environmentMap, WorldPos, 0.0).rgb;

    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 

    FragColor = vec4(envColor, 1.0);
}
