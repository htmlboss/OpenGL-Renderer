#version 440 core

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
//uniform float exposure;

out vec4 color;

vec3 saturation(const vec3 rgb, const float adjustment) {
    // Algorithm from Chapter 16 of OpenGL Shading Language
    const vec3 W = vec3(0.2125, 0.7154, 0.0721);
    vec3 intensity = vec3(dot(rgb, W));

    return mix(intensity, rgb, adjustment);
}

void main() {             
    const float gamma = 2.2f;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;

    // reinhard
    vec3 result = hdrColor / (hdrColor + vec3(1.0f));
    // exposure
    //vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0f / gamma));
    
    color = vec4(saturation(result, 1.25f), 1.0f);
}