/* common.vert pre-pended @ runtime:
GLSL version
Vertex attributes
model uniform (mat4)
*/

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0f);
}
