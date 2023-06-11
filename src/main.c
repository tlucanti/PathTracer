
#include <winlib/winlib.h>
#include <cllib/cllib.h>
#include <clgl.h>

static void key_callback(GLFWwindow *wind, int key, int scancode, int action,
			 int mods)
{
	(void)wind;
	(void)scancode;
	(void)mods;
	printf("action %d pressed %d\b", action, key);
}

static void framebuffer_size_callback(GLFWwindow *wind, int width, int height)
{
	(void)wind;
	glViewport(0, 0, width, height);
}

int main()
{
	unsigned int width, height;
	GLFWwindow *window = winlib_init(&width, &height);

	device_t device = create_device(gpu_type);
	context_t context = create_gl_context(device, window);

	GLuint texture = create_texture(width, height);
	buffer_t image = create_image(context, texture, read_write);

	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	while (!glfwWindowShouldClose(window)) {
		// process call
		//processTimeStep();
		// render call
		//renderFrame();
		// swap front and back buffers
		glfwSwapBuffers(window);
		// poll for events
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;

	(void)image;
}