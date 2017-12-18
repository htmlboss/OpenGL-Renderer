#version 440 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

layout (std140, binding = 0) uniform Matrices {
  mat4 projection;
  mat4 view;
};
uniform mat4 modelMatrix;

out vec2 TexCoord;

void main() {
    gl_Position = projection * view * modelMatrix * vec4(position, 1.0);
    TexCoord = texCoord;
}