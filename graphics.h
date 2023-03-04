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

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <GLFW/glfw3.h>

#define LOG_MAX_LENGTH 2048

/* Checks if there are OpenGL errors, it basically calls glGetError.
 * If print is true it prints the error.*/
bool check_gl_errors(bool print);

/* Compiles and creates a shader from a string */
int create_shader(const GLchar* shader_str, GLuint &shader, GLenum type);

/* Creates a shader program from a vertex and fragment shader */
int create_program(GLuint vert, GLuint frag, GLuint &program_index);

/* Returns the shader program that renders the text */
int get_program(GLuint& program);

/* Initializes the GL/GLEW. Returns EXIT_FAILURE on error */
int init_gl(GLFWwindow* window);

#endif