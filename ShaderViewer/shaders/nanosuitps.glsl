#version 440 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
};

in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;

uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

void main() {    
    color = vec4(texture(texture_diffuse1, TexCoords));
}