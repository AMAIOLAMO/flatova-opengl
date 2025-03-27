#version 330 core

layout (location = 0) in vec3 i_aPos;
layout (location = 1) in vec2 i_uv;

uniform mat4 model;
uniform mat4 view_proj;

out vec2 uv;

void main() {
    gl_Position = view_proj * model * vec4(i_aPos, 1.0f);
    uv = i_uv;
}

