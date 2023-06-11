
#ifndef WINLIB_H
# define WINLIB_H

# define GLFW_EXPOSE_NATIVE_X11
# define GLFW_EXPOSE_NATIVE_GLX

# include <glad/glad.h>
# include <GLFW/glfw3.h>
# include <GLFW/glfw3native.h>

# include <common.h>

GLFWwindow *winlib_init(unsigned int *width, unsigned int *height);

#endif /* WINLIB_H */
