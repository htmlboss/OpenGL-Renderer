#version 440 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in vec3 offset;

layout (std140, binding = 0) uniform Matrices {
  mat4 projection;
  mat4 view;
};
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;
out mat3 TBN;

void main() {
    TexCoords = texCoords;
    FragPos = vec3(modelMatrix * vec4(position + offset, 1.0));
    Normal = mat3(modelMatrix) * normal;

    // TBN Matrix
    const vec3 T = normalize(normalMatrix * tangent);
    const vec3 B = normalize(normalMatrix * bitangent);
    const vec3 N = normalize(normalMatrix * normal);
    TBN = transpose(mat3(T, B, N));

    gl_Position = projection * view * vec4(FragPos, 1.0);
}