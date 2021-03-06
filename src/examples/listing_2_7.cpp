#include "sb7.h"

#include <iostream>

GLuint complile_shaders() {
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program;

    static const GLchar* vertex_shader_source = R"""(
#version 450 core

void main(void) {
    gl_Position = vec4(0.0, 0.0, 0.5, 1.0);
}
    )""";

    static const GLchar* fragment_shader_source = R"""(
#version 450 core

out vec4 color;

void main(void) {
    color = vec4(0.0, 0.8, 1.0, 1.0);
}
    )""";

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}

class my_application : public sb7::application {
private:
    GLint rendering_program;
    GLuint vertex_array_object;
public:
    void startup() {
        rendering_program = complile_shaders();
        glCreateVertexArrays(1, &vertex_array_object);
        glBindVertexArray(vertex_array_object);
        glPointSize(40.0f);
    }

    void shutdown() {
        glDeleteVertexArrays(1, &vertex_array_object);
        glDeleteProgram(rendering_program);
        glDeleteVertexArrays(1, &vertex_array_object);
    }

    void render(double currentTime) {
        const GLfloat color[] = { (float)sin(currentTime) * 0.5f + 0.5f,
                                  (float)cos(currentTime) * 0.5f + 0.5f,
                                  0.0f, 1.0f };
        glClearBufferfv(GL_COLOR, 0, color);

        glUseProgram(rendering_program);
        glDrawArrays(GL_POINTS, 0, 1);
    }
};

DECLARE_MAIN(my_application)