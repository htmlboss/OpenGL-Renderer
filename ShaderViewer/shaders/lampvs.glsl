#version 430

layout (location = 0) in vec3 position;

// Uniform buffer struct (shares matrix data to all shaders)
layout (std140, binding = 0) uniform Matrices {
  mat4 projection;
  mat4 view;
};
uniform mat4 model;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0f);
}
