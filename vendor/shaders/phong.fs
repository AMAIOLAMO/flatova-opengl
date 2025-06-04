#version 330 core

struct Material {
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
    float specular_factor;
};

struct GlobalLight {
    vec3 dir;
    vec3 color;
};

struct PointLight {
    vec3 pos;
    vec3 color;
    float strength;
};

uniform Material material;
uniform GlobalLight global_light;
uniform vec3 cam_pos;

in vec2 uv;
in vec3 norm;
in vec3 frag_pos;

out vec4 FragColor;

float light_strength = 1.0f;

float ambient_strength = 0.3f;
float specular_strength = 1.0f;

vec3 calc_global_light(vec3 light_dir, vec3 light_color) {
    vec3 n = normalize(norm);
    vec3 cam_dir = normalize(cam_pos - frag_pos);

    // AMBIENT
    vec3 ambient = light_color * material.ambient * ambient_strength;

    // DIFFUSE
    float diff = max(dot(n, light_dir), 0.1f);
    vec3 diffuse = light_color * diff * texture(material.diffuse, uv).rgb;

    // SPECULAR
    vec3 light_to_frag = -light_dir;
    float spec = max(dot(reflect(light_to_frag, n), cam_dir), 0.0f);
    spec = pow(spec, material.specular_factor) * specular_strength;
    vec3 specular = light_color * texture(material.specular, uv).rgb * spec;

    return ambient + diffuse + specular;
}

void main() {
    // RESULT
    vec3 result = calc_global_light(global_light.dir, global_light.color);

    FragColor = vec4(result, 1.0f);
}

