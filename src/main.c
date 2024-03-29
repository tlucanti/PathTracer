
#include <winlib/winlib.h>
#include <cllib/cllib.h>
#include <clgl.h>
#include <time.h>

typedef cl_float3 float3;
#define FLOAT3(X, Y, Z)(float3){ .x = X, .y = Y, .z = Z }
#include "source/struct.cl"

#include <linalg.h>

#define MULTIRAY false
#define TRACER_MOVE_STEP 0.1
#define TRACER_LOOK_STEP (PI / 200.0)
#define TRACER_MOUSE_LOOK_STEP (1e-3)

#define SPHERES_NUM 6
#define SUN_DIRECTION normalize(FLOAT3(-1, 0.5, -0.3))

struct Camera {
	float3 position;
	float alpha;
	float theta;
	struct RotateMatrix matrix;
};

struct tracer_state {
	struct Camera camera;
	float3 move_step;
	float look_step[2];
	float mouse_pos[2];
	bool mouse_move;
	cl_int reset_frame;
	bool exit;
};

struct tracer_state g_tracer_state = {
	.camera = { .position = FLOAT3(4, 2.5, -3.5), .alpha = -0.5, .theta = 0.3 },
	.move_step = FLOAT3(0, 0, 0),
	.look_step = { 0, 0 },
	.mouse_move = false,
	.reset_frame = 0,
	.exit = false
};

static void key_callback(GLFWwindow *wind, int key, int scancode, int action,
			 int mods)
{
	(void)wind;
	(void)mods;
	(void)scancode;

	if (action == GLFW_PRESS) {
		g_tracer_state.reset_frame += 1;
		switch (key) {
		case GLFW_KEY_W:
			g_tracer_state.move_step.z += TRACER_MOVE_STEP; break;
		case GLFW_KEY_S:
			g_tracer_state.move_step.z += -TRACER_MOVE_STEP; break;
		case GLFW_KEY_A:
			g_tracer_state.move_step.x += -TRACER_MOVE_STEP; break;
		case GLFW_KEY_D:
			g_tracer_state.move_step.x += TRACER_MOVE_STEP; break;
		case GLFW_KEY_SPACE:
			g_tracer_state.move_step.y += TRACER_MOVE_STEP; break;
		case GLFW_KEY_LEFT_CONTROL:
		case GLFW_KEY_RIGHT_CONTROL:
			g_tracer_state.move_step.y += -TRACER_MOVE_STEP; break;
		case GLFW_KEY_UP:
			g_tracer_state.look_step[0] += -TRACER_LOOK_STEP; break;
		case GLFW_KEY_DOWN:
			g_tracer_state.look_step[0] += TRACER_LOOK_STEP; break;
		case GLFW_KEY_LEFT:
			g_tracer_state.look_step[1] += -TRACER_LOOK_STEP; break;
		case GLFW_KEY_RIGHT:
			g_tracer_state.look_step[1] += TRACER_LOOK_STEP; break;
		case GLFW_KEY_ENTER:
			break;
		case GLFW_KEY_P: {
			float3 p = g_tracer_state.camera.position;
			float alpha = g_tracer_state.camera.alpha;
			float theta = g_tracer_state.camera.theta;
			printf("position {%f, %f, %f}, direction {%f %f}\n",
			       p.x, p.y, p.z, alpha, theta);
			break;
		} case GLFW_KEY_ESCAPE:
			g_tracer_state.exit = true; break;
		}
	} else if (action == GLFW_RELEASE) {
		g_tracer_state.reset_frame -= 1;
		switch (key) {
		case GLFW_KEY_W:
			g_tracer_state.move_step.z -= TRACER_MOVE_STEP; break;
		case GLFW_KEY_S:
			g_tracer_state.move_step.z -= -TRACER_MOVE_STEP; break;
		case GLFW_KEY_A:
			g_tracer_state.move_step.x -= -TRACER_MOVE_STEP; break;
		case GLFW_KEY_D:
			g_tracer_state.move_step.x -= TRACER_MOVE_STEP; break;
		case GLFW_KEY_SPACE:
			g_tracer_state.move_step.y -= TRACER_MOVE_STEP; break;
		case GLFW_KEY_LEFT_CONTROL:
		case GLFW_KEY_RIGHT_CONTROL:
			g_tracer_state.move_step.y -= -TRACER_MOVE_STEP; break;
		case GLFW_KEY_UP:
			g_tracer_state.look_step[0] -= -TRACER_LOOK_STEP; break;
		case GLFW_KEY_DOWN:
			g_tracer_state.look_step[0] -= TRACER_LOOK_STEP; break;
		case GLFW_KEY_LEFT:
			g_tracer_state.look_step[1] -= -TRACER_LOOK_STEP; break;
		case GLFW_KEY_RIGHT:
			g_tracer_state.look_step[1] -= TRACER_LOOK_STEP; break;
		case GLFW_KEY_ESCAPE:
			g_tracer_state.exit = true; break;
		}
	}
}

static void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	(void)mods;

	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		g_tracer_state.mouse_pos[0] = (float)x;
		g_tracer_state.mouse_pos[1] = (float)y;
		g_tracer_state.mouse_move = true;
		g_tracer_state.reset_frame += 1;
	} else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
		g_tracer_state.mouse_move = false;
		g_tracer_state.reset_frame -= 1;
	}
}

void move_camera(GLFWwindow *window)
{
	float3 move_step = g_tracer_state.move_step;

	compute_rotation_matrix(&g_tracer_state.camera.matrix,
				g_tracer_state.camera.alpha,
				g_tracer_state.camera.theta);
	rotate_vector(&move_step, &g_tracer_state.camera.matrix);
	vec_iadd(&g_tracer_state.camera.position, move_step);

	g_tracer_state.camera.theta += g_tracer_state.look_step[0];
	g_tracer_state.camera.alpha += g_tracer_state.look_step[1];

	if (g_tracer_state.mouse_move) {
		double x, y;
		glfwGetCursorPos(window, &x, &y);

		float dx = (float)x - g_tracer_state.mouse_pos[0];
		float dy = (float)y - g_tracer_state.mouse_pos[1];

		g_tracer_state.camera.alpha += dx * TRACER_MOUSE_LOOK_STEP;
		g_tracer_state.camera.theta += dy * TRACER_MOUSE_LOOK_STEP;

		g_tracer_state.mouse_pos[0] = x;
		g_tracer_state.mouse_pos[1] = y;
	}
}

static bool update_tracer_state(GLFWwindow *window)
{
	if (g_tracer_state.exit) {
		return true;
	}
	move_camera(window);

	return false;
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

	// color, position, emission radius, reflective
	spheres[0] = (struct Sphere){ WHITE, FLOAT3(3, -0.1, 7), 0, 1.5, 0.95, 0.0 };
	spheres[1] = (struct Sphere){ LRED, FLOAT3(1.4, -0.2, 4.5), 0.0, 1, 0.0, 0.00 };
	spheres[2] = (struct Sphere){ LGREEN, FLOAT3(0, -0.3, 3), 0.0, 0.8, 0.0, 0.0 };
	spheres[3] = (struct Sphere){ LBLUE, FLOAT3(-1, -0.55, 2), 0.0, 0.5, 0.0, 0.0 };
	spheres[4] = (struct Sphere){ GREY, FLOAT3(-1.8, -0.75, 1.3), 0, 0.3, 0.0, 0.0 };
	spheres[5] = (struct Sphere){ LPURPLE, FLOAT3(0, -50, 0), 0.0, 49, 0.0, 0.0 };

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
	srandom(time(NULL));

	char compile_flags[255];
	size_t printed;
	unsigned int width, height;
	GLFWwindow *window = winlib_init(&width, &height);

	device_t device = create_device(gpu_type);
	context_t context = create_gl_context(device, window);
	queue_t queue = create_queue(context, device);

	float3 sun_dir = SUN_DIRECTION;
	printed = sprintf(compile_flags,
			  "-I . -I source "
			  "-D SCREEN_WIDTH=%d -D SCREEN_HEIGHT=%d "
			  "-D SPHERES_NUM=%d -D RAYS_PER_PIXEL=%d "
			  "-D SUN_DIRECTION=FLOAT3(%f,%f,%f)",
			  width, height, SPHERES_NUM, RAYS_PER_PIXEL,
			  sun_dir.x, sun_dir.y, sun_dir.z);
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
	set_kernel_arg_at(kernel, image, 5);
#if MULTIRAY
	set_kernel_size_3d(kernel, width, height, RAYS_PER_PIXEL);
	set_kernel_local_size_3d(kernel, 1, 1, RAYS_PER_PIXEL);
#else
	set_kernel_size_2d(kernel, width, height);
#endif

	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	unsigned int frameNumber = 0;

	while (!glfwWindowShouldClose(window)) {

		if (update_tracer_state(window)) {
			break;
		}
		if (g_tracer_state.reset_frame) {
			frameNumber = 1;
		}

		set_kernel_arg_at(kernel, g_tracer_state.camera.position, 2);
		set_kernel_arg_at(kernel, g_tracer_state.camera.matrix, 3);
		set_kernel_arg_at(kernel, g_tracer_state.reset_frame, 4);
		set_kernel_arg_at(kernel, frameNumber, 6);
		// process call
		compute(queue, image, kernel);
		// render call
		render(shader);
		// swap front and back buffers
		glfwSwapBuffers(window);
		// poll for events
		glfwPollEvents();

		announce_fps();
		++frameNumber;
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
