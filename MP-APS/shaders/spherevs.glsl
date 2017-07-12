#version 430

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoords;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    vec4 pos = projection * view * model * vec4(position, 1.0f);
    gl_Position = pos.xyww;
    TexCoords = texCoords;
}
