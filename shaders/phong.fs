#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float specular_factor;
};

uniform Material material;
uniform vec3 cam_pos;

in vec3 norm;
in vec3 frag_pos;

out vec4 FragColor;

uniform vec3 light_pos;
float light_strength = 1.0f;

vec3 light_color = vec3(1.0f, 1.0f, 1.0f);
float ambient_strength = 0.3f;
float specular_strength = 1.0f;

void main() {
    vec3 n = normalize(norm);
    vec3 light_dir = normalize(light_pos - frag_pos);
    vec3 cam_dir = normalize(cam_pos - frag_pos);

    // AMBIENT
    vec3 ambient = light_color * material.ambient * ambient_strength;

    // DIFFUSE
    float diff = max(dot(n, light_dir), 0.0f);
    vec3 diffuse = light_color * diff * material.diffuse;


    // SPECULAR
    vec3 light_to_frag = normalize(frag_pos - light_pos);
    float spec = max(dot(reflect(light_to_frag, n), cam_dir), 0.0f);
    spec = pow(spec, material.specular_factor) * specular_strength;
    vec3 specular = light_color * material.specular * spec;

    // RESULT
    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0f);
}

