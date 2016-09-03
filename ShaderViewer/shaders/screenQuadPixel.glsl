#version 430

in vec2 TexCoords;
out vec4 color;

uniform sampler2D screenTexture;

void main() {
  color = texture(screenTexture, TexCoords);
}
