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

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#ifdef SAVE_STB
    #ifdef __GNUC__ // make gcc shut up about missing field initializers on stb
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    #else
        // clang??
    #endif

    #define STB_IMAGE_WRITE_IMPLEMENTATION
    #include <stb_image_write.h>
#endif

#include "src/FontAtlas.h"
#include "src/common.h"

// example headers
#include "window.h"
#include "graphics.h"


GLFWwindow* window;


int main(int argc, char* argv[]){
    /*
     * Usage:
     * ./main PATH_TO_FONT
     */
    const char* path;

    if(argc < 2){
        std::cerr << "Missing argument path to font" << std::endl;
        return EXIT_FAILURE;
    }
    else if(argc > 2){
        std::cerr << "Too many arguments" << std::endl;
        return EXIT_FAILURE;
    }
    else
        path = argv[1];

    FontAtlas atlas(512); // create the atlas

    if(atlas.loadFont(path, 32)){
        std::cerr << "Failed to load font" << std::endl;
        return EXIT_FAILURE;
    }

    // init atlas
    uint failed_chars = 0;
    failed_chars += atlas.loadCharacterRange(32, 255); // ascii
    failed_chars += atlas.loadCharacterRange(913, 1023); // greek and coptic
    std::cerr << "Failed to load " << failed_chars << " characters" << std::endl;
    if(atlas.createAtlas(false))
        std::cerr << "Failed to create the complete atlas (out of space?)" << std::endl;

    // init GLFW window
    window = init_window();
    if(window == nullptr){
        std::cerr << "Failed to open window" << std::endl;
        return EXIT_FAILURE;
    }
    // init GL/GLEW
    if(init_gl(window) == EXIT_FAILURE){
        std::cerr << "Failed to init GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    // load shader
    GLuint shader = get_program();
    std:: cout << shader << std::endl;

    // main loop
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        check_gl_errors(true);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}