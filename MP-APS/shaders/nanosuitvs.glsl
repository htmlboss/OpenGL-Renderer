#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 offset; // For 3D instancing

// Uniform buffer struct (shares matrix data to all shaders)
layout (std140, binding = 0) uniform Matrices {
  mat4 projection;
  mat4 view;
};
// All objects have a model matrix
uniform mat4 model;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;
out vec4 viewSpace;

void main() {
    viewSpace = view * model * vec4(position + offset, 1.0f);
    gl_Position = projection * viewSpace;

    FragPos = vec3(model * vec4(position, 1.0f));
    Normal = mat3(transpose(inverse(model))) * normal;
    TexCoords = texCoords;

}
