#version 430

uniform vec3 lightColor;
out vec4 color;

void main() {
    color = vec4(lightColor, 1.0);
}
