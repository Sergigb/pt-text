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
#include <cassert>

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

// text headers
#include "../src/FontAtlas.h"
#include "../src/common.h"
#include "../src/Text2D.h"

// example headers
#include "window.h"
#include "graphics.h"


GLFWwindow* window = nullptr;
GLuint text_shader;

Text2D* my_text_1 = nullptr;  // just used by the framebuffer callback


void on_fb_resize_callback(GLFWwindow* window, int width, int height){
    UNUSED(window);
    #ifdef DEBUG
    assert(my_text_1);
    #endif //DEBUG

    update_ortho_proj(width, 0.0f, height, 0.0f, 1.0f, -1.0f, text_shader);
    glViewport(0, 0, width, height);

    my_text_1->onFramebufferSizeUpdate(width, height);
}


void on_kb_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    UNUSED(scancode);
    UNUSED(mods);

    if(key == GLFW_KEY_UNKNOWN) return;
    if(key == GLFW_KEY_ESCAPE && action)
        glfwSetWindowShouldClose(window, true);
}


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

    // init GLFW window
    window = init_window();
    if(window == nullptr){
        std::cerr << "Failed to open window" << std::endl;
        return EXIT_FAILURE;
    }
    glfwSetFramebufferSizeCallback(window, on_fb_resize_callback);
    glfwSetKeyCallback(window, on_kb_callback);

    // init GL/GLEW
    if(init_gl(window) == EXIT_FAILURE){
        std::cerr << "Failed to init GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    // create the atlas
    FontAtlas atlas(512); 

    if(atlas.loadFont(path, 32)){
        std::cerr << "Failed to load font" << std::endl;
        return EXIT_FAILURE;
    }

    // init atlas
    uint failed_chars = 0;
    failed_chars += atlas.loadCharacterRange(32, 255); // ascii
    failed_chars += atlas.loadCharacterRange(913, 1023); // greek and coptic
    failed_chars += atlas.loadCharacter(128513); // greek and coptic
    std::cerr << "Failed to load " << failed_chars << " characters" << std::endl;
    if(atlas.createAtlas(false))
        std::cerr << "Failed to create the complete atlas (out of space?)" << std::endl;

    // load shader
    if(get_program(text_shader) == EXIT_FAILURE){
        std::cerr << "Failed to create text shaders" << std::endl;
        return EXIT_FAILURE;
    }

    // create ortho projection
    int vp_data[4];
    glGetIntegerv(GL_VIEWPORT, vp_data);
    update_ortho_proj(vp_data[2], 0.0f, vp_data[3], 0.0f, 1.0f, -1.0f, text_shader);

    // create the 2D text object
    Text2D text(vp_data[2], vp_data[3], &atlas, text_shader);
    my_text_1 = &text; // set this pointer for the callback

    float nice_white[3] = {.75f, .75f, .75f};
    float red[3] = {1.f, 0.f, 0.f};

    text.addString(L"Hello World! (Left-aligned)", 50., 50., 1, 
                   STRING_DRAW_ABSOLUTE_TR, STRING_ALIGN_LEFT, nice_white);
    text.addString(L"Relative text (0.5, 0.5)", 0.5, 0.5, 1,
                   STRING_ALIGN_CENTER_XY, nice_white);
    text.addString(L"Hello World! (Right-aligned)", 50., 50., 1,
                   STRING_DRAW_ABSOLUTE_BL, STRING_ALIGN_RIGHT, nice_white);
    text.addString(L"big as shit text", 50., 250., 2,
                   STRING_DRAW_ABSOLUTE_BL, STRING_ALIGN_RIGHT, nice_white);
    text.addString(L"small text", 50., 230., 0.25,
                   STRING_DRAW_ABSOLUTE_BL, STRING_ALIGN_RIGHT, nice_white);
    text.addString(L"We can also have strings of different color within the same object!\n"
                    "And breaklines!", 0.5, 0.75, 0.5,
                    STRING_ALIGN_CENTER_XY, red);

    // main loop
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();

        glClearColor(.1f, 0.1f, 0.1f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        text.render();

        check_gl_errors(true);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}