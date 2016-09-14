out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

void main() {
  gl_Position = projection * view * model * vec4(position + offset, 1.0);
  FragPos = vec3(model * vec4(position, 1.0f));
  Normal = mat3(transpose(inverse(model))) * normal;
  TexCoords = texCoords;
}
