#version 440 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;

layout (std140, binding = 0) uniform Matrices {
  mat4 projection;
  mat4 view;
};
uniform mat4 modelMatrix;
uniform vec3 lightPos;

out vec2 TexCoords;
out vec3 surfaceNormal;
out vec3 toLightVector;
out vec3 toCameraVector;

void main() {

	vec4 worldPosition = modelMatrix * vec4(position, 1.0f);
	gl_Position = projection * view * worldPosition;
	TexCoords = texCoords;
	
	surfaceNormal = (modelMatrix * vec4(normal, 0.0f)).xyz;
	toLightVector = lightPos - worldPosition.xyz;
	toCameraVector = (inverse(view) * vec4(0.0f, 0.0f, 0.0f, 1.0f)).xyz - worldPosition.xyz;
}