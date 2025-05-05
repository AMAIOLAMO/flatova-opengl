#version 330 core

layout (location = 0) in vec3 i_pos;

uniform mat4 model;
uniform mat4 view_proj;

void main() {
    gl_Position = view_proj * model * vec4(i_pos, 1.0f);
}

