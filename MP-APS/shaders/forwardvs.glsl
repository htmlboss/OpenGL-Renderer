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
uniform vec3 lightPos;

out vec3 Color;
out vec3 Normal;
out vec3 dirToLight;
out vec2 TexCoords;

void main() {
    const vec4 worldPos = modelMatrix * vec4(position, 1.0f);
    
    gl_Position = projection * view * worldPos;
    Color = normal;
    Normal = (modelMatrix * vec4(normal, 0.0f)).xyz;
    dirToLight = lightPos - worldPos.xyz;
    TexCoords = texCoords;
}