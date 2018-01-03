#version 440 core

in vec3 WorldPos;

uniform samplerCube environmentMap;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

void main() {
    vec3 envColor = textureLod(environmentMap, WorldPos, 0.0).rgb;

    //envColor = envColor / (envColor + vec3(1.0));
    //envColor = pow(envColor, vec3(1.0/2.2));

    // Apply bloom threshold (very high so the whole thing doesnt glow, just the sun)
    const float brightness = dot(envColor, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 4.5) {
        BrightColor = vec4(envColor, 1.0);
    }
    else {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }

    FragColor = vec4(envColor, 1.0);
}
