#include "sb7.h"

#include <iostream>

class my_application : public sb7::application {
public:
    void render(double currentTime) {
        const GLfloat color[] = { (float)sin(currentTime) * 0.5f + 0.5f,
                                  (float)cos(currentTime) * 0.5f + 0.5f,
                                  0.0f, 1.0f };
        glClearBufferfv(GL_COLOR, 0, color);
    }
};

DECLARE_MAIN(my_application)