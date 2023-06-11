
#include <winlib/winlib.h>

static void __err_callback(int error, const char *message)
{
	(void)error;
	panic(message);
}

GLFWwindow *winlib_init(unsigned int *width, unsigned int *height)
{
	if (glfwInit() != GLFW_TRUE) {
		panic("glfwInit");
	}

	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode = glfwGetVideoMode(monitor);

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	GLFWwindow *window;

	glfwSetErrorCallback(__err_callback);

	window = glfwCreateWindow(mode->width, mode->height, "window name",
				  monitor, NULL);
	if (window == NULL) {
		glfwTerminate();
		panic("glfw window");
	}

	glfwMakeContextCurrent(window);

	*width = mode->width;
	*height = mode->height;
	return window;

}