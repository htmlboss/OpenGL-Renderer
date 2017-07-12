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

out vec3 FragPos;
out vec3 Color;
out vec3 Normal;
out vec2 TexCoords;

void main() {
    const vec3 FragPos = vec3(modelMatrix * vec4(position, 1.0));
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
    
    Color = normal;
    Normal = mat3(modelMatrix) * normal;
    TexCoords = texCoords;
}