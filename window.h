#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>

/* Initializes the GLFW window. Returns a pointer of type GLFWwindow to the window. Returns nullptr
 * on error*/
GLFWwindow* init_window();

/* Initializes the GL/GLEW. Returns EXIT_FAILURE on error */
int init_gl(GLFWwindow* window);

#endif