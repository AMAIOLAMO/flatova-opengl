#version 330 core

in vec2 pos_xz;
in vec2 uv;

out vec4 FragColor;

void main() {
    float thickness = 0.01f;
    vec2 grid = step(abs(fract(pos_xz) - 0.5f), vec2(thickness));
    vec2 new_uv = uv - 0.5f;
    float fade_off = 1.0f - sqrt(new_uv.x * new_uv.x + new_uv.y * new_uv.y);
    fade_off = pow(fade_off, 2.0f);
    float color = min(grid.x + grid.y, 1.0f) * fade_off;
    FragColor = vec4(color);
}

