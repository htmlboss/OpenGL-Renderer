#version 440 core

layout (location = 0) in vec3 position;

layout (std140, binding = 0) uniform Matrices {
  mat4 projection;
  mat4 view;
};

out vec3 WorldPos;

void main() {
	WorldPos = vec3(position.x, -position.y, position.z); // Flip the y-axis otherwise the reflection probe will be upside down.
    
    const vec4 pos = projection * mat4(mat3(view)) * vec4(position, 1.0);

    gl_Position = pos.xyww;
}
