#version 440 core

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
//uniform float exposure;

out vec4 color;

void main() {             
    const float gamma = 2.2f;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;

    // reinhard
    vec3 result = hdrColor / (hdrColor + vec3(1.0f));
    // exposure
    //vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0f / gamma));
    
    color = vec4(result, 1.0f);
}