#version 440 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;

layout (std140, binding = 0) uniform Matrices {
  mat4 projection;
  mat4 view;
};
uniform mat4 modelMatrix;
uniform mat4 lightSpaceMatrix;

out VertexData {
	out vec2 TexCoords;
	out vec3 FragPos;
	out mat3 TBN;
    out vec4 FragPosLightSpace;
} vertexData;

void main() {
    vertexData.TexCoords = texCoords;
    vertexData.FragPos = vec3(modelMatrix * vec4(position, 1.0));

    // Construct TBN matrix
    vec3 T = normalize(vec3(modelMatrix * vec4(tangent, 0.0)));
    const vec3 N = normalize(vec3(modelMatrix * vec4(normal, 0.0)));
    
    // Gram-schmidt process (produces higher-quality normal mapping on large meshes)
    // Re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    // Then calculate Bitangent
    const vec3 B = cross(N, T);

    vertexData.TBN = mat3(T, B, N);

    vertexData.FragPosLightSpace = lightSpaceMatrix * vec4(vertexData.FragPos, 1.0);

    gl_Position = projection * view * vec4(vertexData.FragPos, 1.0);
}