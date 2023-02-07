#include <iostream>

#include <GL/glew.h>

#include "window.h"
#include "graphics.h"
#include "src/common.h"


void glfw_error_callback(int error, const char* description){
    std::cerr << "glfw_error_callback: GLFW error " << error << "(" 
              << description << ")" << std::endl;
}


void on_fb_resize(GLFWwindow *win, int width, int height){
    UNUSED(win);
    std::cout << "fb resize to " << width << " " << height << std::endl;
}


GLFWwindow* init_window(){
    GLFWwindow* window;
    glfwSetErrorCallback(glfw_error_callback);
    if(!glfwInit()){
        std::cerr << "Could not start GLFW3" << std::endl;
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(512, 512, "pt-text", NULL, NULL);
    if(!window){
        std::cerr << "Could not open window with GLFW3" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwSetFramebufferSizeCallback(window, on_fb_resize);
    glfwMakeContextCurrent(window);

    return window;
}


int init_gl(GLFWwindow* window){
    glewExperimental = GL_TRUE;

    if(GLEW_OK != glewInit())
        return EXIT_FAILURE;

    // general gl setup
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glfwSwapInterval(0);

    glfwMakeContextCurrent(window);

    check_gl_errors(true);

    return EXIT_SUCCESS;
}
