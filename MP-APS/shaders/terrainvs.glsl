#version 440 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;

layout (std140, binding = 0) uniform Matrices {
  mat4 projection;
  mat4 view;
};
uniform mat4 modelMatrix;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

void main() {
	gl_Position = projection * view * modelMatrix * vec4(position, 1.0f);
	TexCoords = texCoords;
	Normal = mat3(transpose(inverse(modelMatrix))) * normal;
	FragPos = vec3(modelMatrix * vec4(position, 1.0f));
}