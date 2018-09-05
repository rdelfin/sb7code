#include "sb7.h"

#include <iostream>

GLuint complile_shaders() {
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program;

    static const GLchar* vertex_shader_source = R"""(
#version 450 core

out vec4 vs_color;

void main(void) {
    const vec4 vertices[3] = vec4[3](vec4( 0.25, -0.25, 0.5, 1.0),
                                     vec4(-0.25, -0.25, 0.5, 1.0),
                                     vec4( 0.25,  0.25, 0.5, 1.0));
    const vec4 colors[] = vec4[3](vec4( 1.0, 0.0, 0.0, 1.0),
                                  vec4( 0.0, 1.0, 0.0, 1.0),
                                  vec4( 0.0, 0.0, 1.0, 1.0));
    gl_Position = vertices[gl_VertexID];
    vs_color = colors[gl_VertexID];
}
    )""";

    static const GLchar* fragment_shader_source = R"""(
#version 450 core

in vec4 vs_color;
out vec4 color;

void main(void) {
    color = vs_color;
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
        const GLfloat color[] = { 0.0f, 0.25f, 0.0f, 1.0f };
        glClearBufferfv(GL_COLOR, 0, color);

        glUseProgram(rendering_program);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
};

DECLARE_MAIN(my_application)