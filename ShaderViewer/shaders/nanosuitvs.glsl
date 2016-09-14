/* common.vert pre-pended @ runtime:
GLSL version
Vertex attributes
model uniform (mat4)
*/

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;
out vec4 viewSpace;

void main() {
    viewSpace = view * model * vec4(position, 1.0f);
    gl_Position = projection * viewSpace;
    FragPos = vec3(model * vec4(position, 1.0f));
    Normal = mat3(transpose(inverse(model))) * normal;
    TexCoords = texCoords;

}
