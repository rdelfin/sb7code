#include "sb7.h"
#include "sb7ktx.h"

#include <iostream>

GLuint complile_shaders() {
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program;

    static const GLchar* vertex_shader_source = R"""(
#version 450 core

void main(void) {
    const vec4 vertices[6] = vec4[6](vec4( 1.0, -1.0, 0.5, 1.0),
                                     vec4(-1.0, -1.0, 0.5, 1.0),
                                     vec4( 1.0,  1.0, 0.5, 1.0),
                                     vec4( 1.0,  1.0, 0.5, 1.0),
                                     vec4(-1.0,  1.0, 0.5, 1.0),
                                     vec4(-1.0, -1.0, 0.5, 1.0));
    gl_Position = vertices[gl_VertexID];
}
    )""";

    static const GLchar* fragment_shader_source = R"""(
#version 450 core

uniform sampler2D s;

out vec4 color;

void main(void) {
    color = texture(s, gl_FragCoord.xy / textureSize(s, 0));
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
    GLuint texture;
public:
    void startup() {
        glGenTextures(1, &this->texture);
        sb7::ktx::file::load("media/textures/tree.ktx", this->texture);

        rendering_program = complile_shaders();
        glCreateVertexArrays(1, &vertex_array_object);
        glBindVertexArray(vertex_array_object);

    }

    void shutdown() {
        glDeleteVertexArrays(1, &vertex_array_object);
        glDeleteProgram(rendering_program);
        glDeleteTextures(1, &this->texture);
    }

    void render(double currentTime) {
        const GLfloat color[] = { 0.0f, 0.25f, 0.0f, 1.0f };
        glClearBufferfv(GL_COLOR, 0, color);

        glUseProgram(rendering_program);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void generate_texture(float * data, int width, int height)
    {
        int x, y;

        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                data[(y * width + x) * 4 + 0] = (float)((x & y) & 0xFF) / 255.0f;
                data[(y * width + x) * 4 + 1] = (float)((x | y) & 0xFF) / 255.0f;
                data[(y * width + x) * 4 + 2] = (float)((x ^ y) & 0xFF) / 255.0f;
                data[(y * width + x) * 4 + 3] = 1.0f;
            }
        }
    }
};

DECLARE_MAIN(my_application)