/*
 * Copyright (C) 2023 Sergi Garcia Bordils
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation, either version 3 of the 
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program. If not,
 * see <https://www.gnu.org/licenses/>. 
 *
 */

#include <iostream>

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

