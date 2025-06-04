#version 330 core

uniform sampler2D albedo;
uniform sampler2D secondary;

in vec2 uv;

out vec4 FragColor;

void main() {
    vec2 new_uv = vec2(uv.x, -uv.y);
    FragColor = texture(albedo, new_uv);
}

