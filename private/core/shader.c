#include <glad/glad.h>

#include <stdio.h>

#include <core/utils.h>
#include <core/shader.h>


b8 try_compile_shader_with_err(int type, const char **p_src, GLuint *out_shader, char *compile_log) {
    *out_shader = glCreateShader(type);
    glShaderSource(*out_shader, 1, p_src, NULL);
    glCompileShader(*out_shader);

    int success;
    glGetShaderiv(*out_shader, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(*out_shader, 512, NULL, compile_log);
        return false;
    }

    return true;
}

b8 shader_load(const char **p_vert_src, const char **p_frag_src, Shader *out_shader) {
    char log[512];

    if(!try_compile_shader_with_err(GL_VERTEX_SHADER, p_vert_src, &out_shader->vert, log)) {
        printf("Error: failed to compile vertex shaders -> %s\n", log);
        return false;
    }

    if(!try_compile_shader_with_err(GL_FRAGMENT_SHADER, p_frag_src, &out_shader->frag, log)) {
        printf("Error: failed to compile fragment shaders -> %s\n", log);
        return false;
    }


    out_shader->program = glCreateProgram();
    glAttachShader(out_shader->program, out_shader->vert);
    glAttachShader(out_shader->program, out_shader->frag);
    glLinkProgram(out_shader->program);

    int success;
    glGetProgramiv(out_shader->program, GL_LINK_STATUS, &success);

    if(!success) {
        glGetProgramInfoLog(out_shader->program, 512, NULL, log);
        printf("Error: cannot link shader program -> %s\n", log);
        return false;
    }

    return true;
}

void shader_free(const Shader shader) {
    glDeleteShader(shader.vert);
    glDeleteShader(shader.frag);
}

void shader_use(const Shader shader) {
    glUseProgram(shader.program);
}

GLuint shader_get_uniform_loc(const Shader shader, const char *name) {
    return glGetUniformLocation(shader.program, name);
}

void shader_set_uniform_1i(const Shader shader, const char *name, int value) {
    glUseProgram(shader.program);
    GLuint loc = glGetUniformLocation(shader.program, name);
    glUniform1i(loc, value);
}

void shader_set_uniform_1f(const Shader shader, const char *name, float value) {
    glUseProgram(shader.program);
    GLuint loc = glGetUniformLocation(shader.program, name);
    glUniform1f(loc, value);
}

void shader_set_uniform_3f(const Shader shader, const char *name, vec3 value) {
    glUseProgram(shader.program);
    GLuint loc = glGetUniformLocation(shader.program, name);
    glUniform3f(loc, value[0], value[1], value[2]);
}

void shader_set_uniform_mat4fv(const Shader shader, const char *name, mat4 value) {
    glUseProgram(shader.program);
    GLuint loc = glGetUniformLocation(shader.program, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)value);
}

b8 try_load_shader_from_files(Shader *p_shader, const char *vert_path, const char *frag_path) {
    // load shaders for pipeline
    char *vert_shader_src = NULL;
    char *frag_shader_src = NULL;
    size_t load_file_size = 0;

    if(try_load_file_text(vert_path, &vert_shader_src, &load_file_size) == false) {
        printf("Failed to load vertex shader!\n");
        return false;
    }

    if(try_load_file_text(frag_path, &frag_shader_src, &load_file_size) == false) {
        printf("Failed to load fragment shader!\n");
        return false;
    }

    if(shader_load((const char**)&vert_shader_src, (const char**)&frag_shader_src, p_shader) == false) {
        printf("Error: failed to load shader!\n");
        return false;
    }

    file_text_free(vert_shader_src);
    file_text_free(frag_shader_src);

    return true;
}


