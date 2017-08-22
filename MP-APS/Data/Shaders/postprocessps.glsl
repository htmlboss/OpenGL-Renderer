#version 440 core

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform float saturationFactor = 1.25f;
uniform vec4 vibranceCoefficient;
uniform float vibranceAmount;

out vec4 color;

vec3 saturation(const vec3 rgb, const float adjustment) {
    // Algorithm from Chapter 16 of OpenGL Shading Language
    const vec3 W = vec3(0.2125, 0.7154, 0.0721);
    vec3 intensity = vec3(dot(rgb, W));

    return mix(intensity, rgb, adjustment);
}

vec3 vibrance(const vec3 rgb, const float amount) {
    const vec4 rgba = vec4(rgb, 1.0f);
    const vec4 luminance = vec4(dot(rgba, vibranceCoefficient));

    const vec4 mask = clamp(rgba - luminance, 0.0f, 1.0f);
    
    const float luminanceMask = 1.0f - dot(vibranceCoefficient, mask);

    return mix(luminance, rgba, 1.0f + amount * luminanceMask).rgb;
}

void main() {             
    const vec3 sourceColor = texture(hdrBuffer, TexCoords).rgb;

    // reinhard tone mapping
    //vec3 result = sourceColor / (sourceColor + vec3(1.0f));
    //const float gamma = 2.2f;
    //result = pow(result, vec3(1.0f / gamma));

    color = vec4(vibrance(sourceColor, vibranceAmount), 1.0f);
}