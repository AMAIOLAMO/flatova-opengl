#version 330 core

uniform sampler2D skymap;

in vec2 uv;
in vec3 norm;

out vec4 FragColor;

void main() {
    FragColor = texture(skymap, uv);
}

