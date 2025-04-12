#version 330 core

layout (location = 0) in vec3 i_pos;
layout (location = 1) in vec2 i_uv;
layout (location = 2) in vec3 i_norm;

uniform mat4 norm_mat;
uniform mat4 model;
uniform mat4 view_proj;

out vec2 uv;
out vec3 norm;
out vec3 frag_pos;

void main() {
    gl_Position = view_proj * model * vec4(i_pos, 1.0f);
    norm = vec3(norm_mat * vec4(i_norm, 0.0f));
    frag_pos = vec3(model * vec4(i_pos, 1.0f));
    uv = i_uv;
}

