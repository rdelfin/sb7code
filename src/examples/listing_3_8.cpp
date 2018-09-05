#include "sb7.h"

#include <iostream>

GLuint complile_shaders() {
    GLuint vertex_shader, fragment_shader, tess_control_shader, tess_evaluation_shader;
    GLuint program;

    static const GLchar* vertex_shader_source = R"""(
#version 450 core

void main(void) {
    const vec4 vertices[3] = vec4[3](vec4( 0.25, -0.25, 0.5, 1.0),
                                     vec4(-0.25, -0.25, 0.5, 1.0),
                                     vec4( 0.25,  0.25, 0.5, 1.0));
    gl_Position = vertices[gl_VertexID];
}
    )""";

    static const GLchar* tess_control_shader_source = R"""(
#version 450 core

layout (vertices = 3) out;

void main(void) {
    if(gl_InvocationID == 0) {
        gl_TessLevelInner[0] = 5.0;
        gl_TessLevelOuter[0] = 5.0;
        gl_TessLevelOuter[1] = 5.0;
        gl_TessLevelOuter[2] = 5.0;
    }

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
    )""";

    static const GLchar* tess_evaluation_shader_source = R"""(
#version 450 core

layout (triangles, equal_spacing, cw) in;

void main(void) {
    gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position) +
                  (gl_TessCoord.y * gl_in[1].gl_Position) +
                  (gl_TessCoord.z * gl_in[2].gl_Position);
}
    )""";

    static const GLchar* fragment_shader_source = R"""(
#version 450 core

out vec4 color;

void main(void) {
    color = vec4(0.0, 0.8, 1.0, 1.0);
}
    )""";

    program = glCreateProgram();

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    tess_control_shader = glCreateShader(GL_TESS_CONTROL_SHADER);
    glShaderSource(tess_control_shader, 1, &tess_control_shader_source, NULL);
    glCompileShader(tess_control_shader);

    tess_evaluation_shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
    glShaderSource(tess_evaluation_shader, 1, &tess_evaluation_shader_source, NULL);
    glCompileShader(tess_evaluation_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    glAttachShader(program, vertex_shader);
    glAttachShader(program, tess_control_shader);
    glAttachShader(program, tess_evaluation_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glDeleteShader(vertex_shader);
    glDeleteShader(tess_control_shader);
    glDeleteShader(tess_evaluation_shader);
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
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    void shutdown() {
        glDeleteVertexArrays(1, &vertex_array_object);
        glDeleteProgram(rendering_program);
    }

    void render(double currentTime) {
        const GLfloat color[] = { 0.0f, 0.25f, 0.0f, 1.0f };
        glClearBufferfv(GL_COLOR, 0, color);

        glUseProgram(rendering_program);

        glDrawArrays(GL_PATCHES, 0, 3);
    }
};

DECLARE_MAIN(my_application)