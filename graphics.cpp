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
#include <fstream>
#include <sstream>

#include <GL/glew.h>

#include "graphics.h"


const GLchar frag_shader[] = "#version 410\n"
                             "in vec2 st;\n"
                             "out vec4 frag_colour;\n"

                             "uniform sampler2D texture_sampler;\n"
                             "uniform vec3 text_color;\n"

                             "void main(){\n"
                                 "vec4 sampled = vec4(1.0, 1.0, 1.0, texture(texture_sampler, st).r);\n"
                                 "frag_colour = vec4(text_color, 1.0) * sampled;\n"
                             "}\n";


const GLchar vert_shader[] = "#version 410\n"
                             "layout(location = 0) in vec2 vertex;\n"
                             "layout(location = 1) in vec2 tex_coord;\n"
                             "out vec2 st;\n"

                             "uniform mat4 projection;\n"
                             "uniform vec2 disp;\n"

                             "void main(){\n"
                                 "st = tex_coord;\n"
                                 "gl_Position = projection * vec4(vertex + disp, 0.0, 1.0);\n"
                             "}\n";


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


int create_shader(const GLchar* shader_str, GLuint &shader, GLenum type){
    int params = -1;

    shader = glCreateShader(type);
    glShaderSource(shader, 1, &shader_str, NULL);
    glCompileShader(shader);

    // check for compile errors
    glGetShaderiv(shader, GL_COMPILE_STATUS, &params);
    if(params != GL_TRUE){
        int len = 0;
        char message[LOG_MAX_LENGTH];

        glGetShaderInfoLog(shader, LOG_MAX_LENGTH, &len, message);
        std::cerr << "failed to create shader. " << message << std::endl;

        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


int create_program(GLuint vert, GLuint frag, GLuint &program_index){
    GLint params = -1;

    program_index = glCreateProgram();
    glAttachShader(program_index, vert);
    glAttachShader(program_index, frag);
    glLinkProgram(program_index);
    
    glGetProgramiv(program_index, GL_LINK_STATUS, &params);
    if(GL_TRUE != params){
        int len = 0;
        char message[LOG_MAX_LENGTH];

        glGetProgramInfoLog(program_index, LOG_MAX_LENGTH, &len, message);
        std::cerr << "create_program: ERROR: GL program index " << program_index
                  << " did not compile: " << message << std::endl;

        return EXIT_FAILURE;
    }
    glDeleteShader(vert);
    glDeleteShader(frag);

    return EXIT_SUCCESS;
}


GLuint get_program(){
    GLuint vert, frag, program;
    create_shader(vert_shader, vert, GL_VERTEX_SHADER);
    create_shader(frag_shader, frag, GL_FRAGMENT_SHADER);
    create_program(vert, frag, program);
    return program;
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
