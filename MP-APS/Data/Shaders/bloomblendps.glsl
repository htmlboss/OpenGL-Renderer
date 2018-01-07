#version 440 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;

uniform vec4 vibranceCoefficient;
uniform float vibranceAmount;

// ----------------------------------------------------------------------------
vec3 vibrance(const vec3 rgb, const float amount) {
    const vec4 rgba = vec4(rgb, 1.0f);
    const vec4 luminance = vec4(dot(rgba, vibranceCoefficient));

    const vec4 mask = clamp(rgba - luminance, 0.0f, 1.0f);
    
    const float luminanceMask = 1.0f - dot(vibranceCoefficient, mask);

    return mix(luminance, rgba, 1.0f + amount * luminanceMask).rgb;
}

// ----------------------------------------------------------------------------
void main() {             
    const float gamma = 2.2; 
    const vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    
    vec3 hdrColor = texture(scene, TexCoords).rgb;
    hdrColor += bloomColor; // apply bloom

    hdrColor = vibrance(hdrColor, vibranceAmount);

    // Tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor);
    // Also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    
    FragColor = vec4(result, 1.0);
}
