#version 330 core

layout (location = 0) in vec3 i_pos;
layout (location = 2) in vec3 i_norm;

uniform mat4 model;
uniform mat4 view_proj;
uniform mat4 norm_mat;
uniform float width;

void main() {
    vec4 world_pos = model * vec4(i_pos, 1.0f);
    vec3 norm = vec3(norm_mat * vec4(i_norm, 0.0f));
    gl_Position = view_proj * vec4(world_pos.xyz + norm * width, 1.0f);
}

