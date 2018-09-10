#include "sb7.h"
#include "sb7color.h"
#include "vmath.h"

#include <iostream>

GLuint complile_shaders() {
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program;

    static const GLchar* vertex_shader_source = R"""(
#version 410 core

in vec4 position;

out VS_OUT
{
    vec4 color;
} vs_out;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

void main(void)
{
    gl_Position = proj_matrix * mv_matrix * position;
    vs_out.color = position * 2.0 + vec4(0.5, 0.5, 0.5, 0.0);
}
    )""";

    static const GLchar* fragment_shader_source = R"""(
#version 450 core

out vec4 color;

in VS_OUT {
    vec4 color;
} fs_in;

void main(void) {
    color = fs_in.color;
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
    GLint rendering_program, mv_location, proj_location;
    GLuint vao, buffer;
    float aspect;
    vmath::mat4 proj_matrix;
public:
    void startup() {
        static const GLfloat vertex_positions[] = {
            -0.25f,  0.25f, -0.25f,
            -0.25f, -0.25f, -0.25f,
             0.25f, -0.25f, -0.25f,

             0.25f, -0.25f, -0.25f,
             0.25f,  0.25f, -0.25f,
            -0.25f,  0.25f, -0.25f,

             0.25f, -0.25f, -0.25f,
             0.25f, -0.25f,  0.25f,
             0.25f,  0.25f, -0.25f,

             0.25f, -0.25f,  0.25f,
             0.25f,  0.25f,  0.25f,
             0.25f,  0.25f, -0.25f,

             0.25f, -0.25f,  0.25f,
            -0.25f, -0.25f,  0.25f,
             0.25f,  0.25f,  0.25f,

            -0.25f, -0.25f,  0.25f,
            -0.25f,  0.25f,  0.25f,
             0.25f,  0.25f,  0.25f,

            -0.25f, -0.25f,  0.25f,
            -0.25f, -0.25f, -0.25f,
            -0.25f,  0.25f,  0.25f,

            -0.25f, -0.25f, -0.25f,
            -0.25f,  0.25f, -0.25f,
            -0.25f,  0.25f,  0.25f,

            -0.25f, -0.25f,  0.25f,
             0.25f, -0.25f,  0.25f,
             0.25f, -0.25f, -0.25f,

             0.25f, -0.25f, -0.25f,
            -0.25f, -0.25f, -0.25f,
            -0.25f, -0.25f,  0.25f,

            -0.25f,  0.25f, -0.25f,
             0.25f,  0.25f, -0.25f,
             0.25f,  0.25f,  0.25f,

             0.25f,  0.25f,  0.25f,
            -0.25f,  0.25f,  0.25f,
            -0.25f,  0.25f, -0.25f
        };

        this->rendering_program = complile_shaders();
        glCreateVertexArrays(1, &this->vao);
        glBindVertexArray(this->vao);

        glGenBuffers(1, &this->buffer);
        glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);

        this->mv_location = glGetUniformLocation(this->rendering_program, "mv_matrix");
        this->proj_location = glGetUniformLocation(this->rendering_program, "proj_matrix");

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CW);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    void shutdown() {
        glDeleteVertexArrays(1, &this->vao);
        glDeleteProgram(this->rendering_program);
        glDeleteVertexArrays(1, &this->vao);
    }

    void onResize(int w, int h) {
        sb7::application::onResize(w, h);
        this->aspect = (float)info.windowWidth / (float)info.windowHeight;
        this->proj_matrix = vmath::perspective(50.0f,
                                               this->aspect,
                                               0.1f,
                                               1000.0f);
    }

    void render(double currentTime) {
        static const GLfloat one = 1.0f;

        glViewport(0, 0, info.windowWidth, info.windowHeight);
        glClearBufferfv(GL_COLOR, 0, sb7::color::Green);
        glClearBufferfv(GL_DEPTH, 0, &one);
        glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

        glUseProgram(this->rendering_program);

        glUniformMatrix4fv(this->proj_location, 1, GL_FALSE, proj_matrix);

        for(int i = 0; i < 24; i++) {
            float f = (float)i + (float)currentTime * 0.3f;
            vmath::mat4 mv_matrix =
                vmath::translate(0.0f, 0.0f, -20.0f) *
                vmath::rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f) *
                vmath::rotate((float)currentTime * 21.0f, 1.0f, 0.0f, 0.0f) *
                vmath::translate(sinf(2.1f * f) * 2.0f,
                                cosf(1.7f * f) * 2.0f,
                                sinf(1.3f * f) * cosf(1.5f * f) * 2.0f);


            glUniformMatrix4fv(this->mv_location, 1, GL_FALSE, mv_matrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
};

DECLARE_MAIN(my_application)