#include <iostream>

#include <GL/glew.h>

#include "graphics.h"


bool check_gl_errors(bool print){
    bool error = false;
    GLenum e = glGetError();

    while(e != GL_NO_ERROR){
        error = true;
        if(print)
            std::cerr << "check_gl_errors: GL error with value " << e << std::endl;

        e = glGetError();
    }

    return error;
}
