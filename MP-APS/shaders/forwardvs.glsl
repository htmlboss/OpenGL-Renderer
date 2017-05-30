#version 440 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 offset;

layout (std140, binding = 0) uniform Matrices {
  mat4 projection;
  mat4 view;
};
uniform mat4 modelMatrix;

out vec3 Color;

void main() {
    const vec4 worldPos = modelMatrix * vec4(position, 1.0f);
    gl_Position = projection * view * worldPos;
    Color = position;
}