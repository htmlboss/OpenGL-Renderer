#version 430
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 offset; // For 3D instancing

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

layout (std140, binding = 0) uniform Matrices {
  mat4 projection;
  mat4 view;
};
uniform mat4 model;

void main() {
    vec4 worldPos = model * vec4(position + offset, 1.0f);
    FragPos = worldPos.xyz;
    gl_Position = projection * view * worldPos;
    TexCoords = texCoords;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalMatrix * normal;
}
