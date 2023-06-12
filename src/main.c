
#include <winlib/winlib.h>
#include <cllib/cllib.h>
#include <clgl.h>
#include <time.h>

typedef cl_float3 float3;
#define FLOAT3(X, Y, Z)                \
	(float3)                       \
	{                              \
		.x = X, .y = Y, .z = Z \
	}
#include "source/struct.cl"

#define SPHERES_NUM 5

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

void compute(queue_t queue, buffer_t image, kernel_t kernel)
{
	cl_command_queue qe = queue.__queue;
	cl_mem img = image.__buffer;

	cl_event event;
	cl_int err;

	glFinish();

	err = clEnqueueAcquireGLObjects(qe, 1, &img, 0, NULL, &event);
	cl_panic_on(err, "clEnqueueAcquireGLObjects", err);

	err = clWaitForEvents(1, &event);
	cl_panic_on(err, "clWaitForEvents", err);

	run_kernel(queue, kernel);

	err = clEnqueueReleaseGLObjects(qe, 1, &img, 0, NULL, &event);
	cl_panic_on(err, "clWaitForEvents", err);

	err = clWaitForEvents(1, &event);
	cl_panic_on(err, "clWaitForEvents", err);
}

void render(shader_t shader)
{
	const float matrix[] = {
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	GLuint program = shader.__program;
	GLuint texture = shader.__texture;
	GLuint vao = shader.__vao;
	GLint mat_loc, tex_loc;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.2, 0.2, 0.2, 1.0);
	glEnable(GL_DEPTH_TEST);
	glUseProgram(program);

	mat_loc = glGetUniformLocation(program, "matrix");
	tex_loc = glGetUniformLocation(program, "tex");

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(tex_loc, 0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glGenerateMipmap(GL_TEXTURE_2D);
	glUniformMatrix4fv(mat_loc, 1, GL_FALSE, matrix);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

static buffer_t create_scene(context_t context, queue_t queue)
{
	struct Sphere spheres[SPHERES_NUM];
	buffer_t scene;


	spheres[0] = (struct Sphere){ .color = RED,
				      .position = FLOAT3(-0.3, -0.8, 9),
				      .emissionStrength = 0.0,
				      .radius = 1 };
	spheres[1] = (struct Sphere){ .color = PURPLE,
				      .position = FLOAT3(0, -100, 0),
				      .emissionStrength = 0.0,
				      .radius = 99 };
	spheres[2] = (struct Sphere){ .color = BLUE,
				      .position = FLOAT3(0, 0, 7),
				      .emissionStrength = 0.05,
				      .radius = 0.8 };
	spheres[3] = (struct Sphere){ .color = WHITE,
				      .position = FLOAT3(-8, 8, 10),
				      .emissionStrength = 1,
				      .radius = 10 };
	spheres[4] = (struct Sphere){ .color = CYAN,
				      .position = FLOAT3(1.3, -0.3, 7),
				      .emissionStrength = 0.0,
				      .radius = 0.7 };

	scene = create_buffer(context, read_only, sizeof(spheres));
	fill_buffer(queue, scene, sizeof(spheres), spheres, true);
	return scene;
}

static void announce_fps()
{
	static struct timespec prev;
	static int frame = 0;
	struct timespec cur;

	clock_gettime(CLOCK_REALTIME, &cur);
	double start = prev.tv_sec + (double)prev.tv_nsec * 1e-9;
	double end = cur.tv_sec + (double)cur.tv_nsec * 1e-9;
	if (frame % 100 == 0) {
		printf("frame: %d, fps: %f\n", frame, 1 / (end - start));
	}
	++frame;
	prev = cur;
}

int main()
{
	char compile_flags[255];
	size_t printed;
	unsigned int width, height;
	GLFWwindow *window = winlib_init(&width, &height);

	device_t device = create_device(gpu_type);
	context_t context = create_gl_context(device, window);
	queue_t queue = create_queue(context, device);

	printed = sprintf(
		compile_flags,
		"-I . -I source "
		"-D SPHERES_NUM=%d -D SCREEN_WIDTH=%d -D SCREEN_HEIGHT=%d",
		SPHERES_NUM, width, height);
	panic_on(printed == 0 || printed > sizeof(compile_flags),
		 "buffer overflow");
	kernel_t kernel = create_kernel(device, context,
					"#include <source/path_tracer.cl>",
					"runKernel", compile_flags);

	shader_t shader = create_shader(width, height);
	buffer_t image = create_image(context, shader, read_write);

	buffer_t scene = create_scene(context, queue);

	set_kernel_arg(kernel, image);
	set_kernel_arg(kernel, scene);
	set_kernel_size(kernel, width, height);

	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	while (!glfwWindowShouldClose(window)) {
		// process call
		compute(queue, image, kernel);
		// render call
		render(shader);
		// swap front and back buffers
		glfwSwapBuffers(window);
		// poll for events
		glfwPollEvents();

		announce_fps();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}

#if 0
int _main()
{
	char compile_flags[255];
	size_t printed;
	unsigned int width, height;
	GLFWwindow *window = winlib_init(&width, &height);

	device_t device = create_device(gpu_type);
	context_t context = create_gl_context(device, window);
	queue_t queue = create_queue(context, device);

	shader_t shader = create_shader(width, height);
	buffer_t image = create_image(context, shader, read_write);

	printed = sprintf(
		compile_flags,
		"-I . -I source "
		"-D SPHERES_NUM=%d -D SCREEN_WIDTH=%d -D SCREEN_HEIGHT=%d",
		SPHERES_NUM, width, height);
	panic_on(printed == 0 || printed > sizeof(compile_flags),
		 "buffer overflow");

	kernel_t kernel = create_kernel(device, context,
					"#include <source/path_tracer.cl>",
					"runKernel", compile_flags);

	buffer_t scene = create_scene(context, queue);

	set_kernel_arg(kernel, image);
	set_kernel_arg(kernel, scene);
	set_kernel_size(kernel, width, height);

	run_kernel(queue, kernel);

	flush_queue(queue);
	printf("done\n");

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
#endif
