#version 440 core

in vec3 Color;

out vec4 FinalColor;

void main() {

    FinalColor = vec4(Color, 1.0);
}