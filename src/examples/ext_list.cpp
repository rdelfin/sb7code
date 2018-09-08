#include "sb7.h"

#include <iostream>

class my_application : public sb7::application {
public:
    void startup() {
        int num_ext;
        glGetIntegerv(GL_NUM_EXTENSIONS, &num_ext);
        std::cout << "There are " << num_ext << " extensions." << std::endl;
        std::cout << "Extensions:" << std::endl;
        for(int i = 0; i < num_ext; i++)
            std::cout << glGetStringi(GL_EXTENSIONS, i) << std::endl;
        exit(0);
    }
};

DECLARE_MAIN(my_application)