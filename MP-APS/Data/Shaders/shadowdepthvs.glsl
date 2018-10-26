#version 440 core
layout (location = 0) in vec3 position;

uniform mat4 lightSpaceMatrix;
uniform mat4 modelMatrix;

out float VertexDepth;

void main() {
    gl_Position = lightSpaceMatrix * modelMatrix * vec4(position, 1.0);
    VertexDepth = gl_Position.z;
}