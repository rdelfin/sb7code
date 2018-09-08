#include "sb7.h"
#include "sb7color.h"
#include "vmath.h"

#include <iostream>
#include <vector>

GLuint complile_shaders() {
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program;

    static const GLchar* vertex_shader_source = R"""(
#version 410 core

in vec3 position;

out VS_OUT
{
    vec4 color;
} vs_out;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

void main(void)
{
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
    vs_out.color = vec4(1.0, 0.0, 0.0, 1.0);
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
    std::vector<vmath::vec3> mesh_points;
public:
    void startup() {
        mesh_points.resize(20*20);
        for(int x = 0; x < 20; x++) {
            for(int y = 0; y < 20; y++) {
                vmath::vec3 point = vmath::vec3((x-10.0f) * 0.2f, (y-10.f) * 0.2f, 0.0f);
                mesh_points[y + x*20] = point;
                std::cout << "(" << point[0] << ", " << point[1] << ", " << point[2] << ")" << std::endl;
            }
        }

        this->rendering_program = complile_shaders();
        glCreateVertexArrays(1, &this->vao);
        glBindVertexArray(this->vao);

        glGenBuffers(1, &this->buffer);
        glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
        glBufferData(GL_ARRAY_BUFFER, mesh_points.size()*3, mesh_points.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);

        this->mv_location = glGetUniformLocation(this->rendering_program, "mv_matrix");
        this->proj_location = glGetUniformLocation(this->rendering_program, "proj_matrix");

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CW);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glPointSize(10.0f);
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

        float f = (float)currentTime * (float)M_PI * 0.1f;
        vmath::mat4 mv_matrix = vmath::translate(0.0f, 0.0f, -2.0f);

        glUseProgram(this->rendering_program);

        glUniformMatrix4fv(this->mv_location, 1, GL_FALSE, mv_matrix);
        glUniformMatrix4fv(this->proj_location, 1, GL_FALSE, proj_matrix);
        glDrawArrays(GL_POINTS, 0, this->mesh_points.size());
    }
};

DECLARE_MAIN(my_application)