#version 330 core

layout (location = 0) in vec3 i_aPos;
layout (location = 1) in vec2 i_uv;
layout (location = 2) in vec3 i_norm;

uniform mat4 model;
uniform mat4 view_proj;

out vec2 pos_xz;
out vec2 uv;

void main() {
    vec3 new_pos = vec3(i_aPos.x, 0.0f, i_aPos.z) * 50.0f;

    gl_Position = view_proj * model * vec4(new_pos, 1.0f);
    
    pos_xz = (model * vec4(new_pos, 1.0f)).xz;

    uv = i_uv;
}

