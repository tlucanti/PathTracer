
#include <gllib/gllib.h>

#define gl_panic_on_err(message)                         \
	do {                                             \
		GLenum __status = glGetError();          \
                                                         \
		if (unlikely(__status != GL_NO_ERROR)) { \
			gl_panic(message, status);       \
		}                                        \
	} while (false)

#define gl_panic(message, status) panic(message)

static const char *gl_strerror(GLenum);

static inline void gl_check_buffer()
{
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	printf("> BUFFER CHECK: %s: %d\n", gl_strerror(status), status);
	gl_panic_on_err("glCheckFramebufferStatus");
}

window_t create_window(unsigned int width, unsigned int height)
{
	GLFWwindow *window;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_REFRESH_RATE, 1);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);

	window = glfwCreateWindow(width, height, "window name", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		panic("window creation error");
	}
	glfwMakeContextCurrent(window);
	glViewport(0, 0, width, height);
	return (window_t){ .__window = window };
}

__always_inline bool window_opened(window_t window)
{
	glfwPollEvents();
	if (unlikely(glfwWindowShouldClose(window.__window))) {
		glfwTerminate();
		return false;
	}
	return true;
}

render_buffer_t create_rbo()
{
	GLuint rbo_id;

	glGenRenderbuffers(1, &rbo_id);
	gl_panic_on_err("glRenderBuffers");

	glBindRenderbuffer(GL_RENDERBUFFER, rbo_id);
	gl_panic_on_err("glBindRenderbuffer");

	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA32F, 256, 256);
	gl_panic_on_err("glRenderbufferStorage");

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				  GL_RENDERBUFFER, rbo_id);
	gl_panic_on_err("glFramebufferRenderbuffer");

	gl_check_buffer();

	glClearColor(0.5f, 0.1f, 0.6f, 1.0f);
	gl_panic_on_err("glClearColor");
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	gl_panic_on_err("glDrawBuffer");
	glClear(GL_COLOR_BUFFER_BIT);
	gl_panic_on_err("glClear");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	gl_panic_on_err("glBindFramebuffer");
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	gl_panic_on_err("glBindFramebuffer");

	return (render_buffer_t){ .__rbo_id = rbo_id };
}

static const char *gl_strerror(GLenum status)
{
	switch (status) {
		case GL_FRAMEBUFFER_COMPLETE: return "OK: buffer completed";
		case GL_FRAMEBUFFER_UNDEFINED: return "GL_FRAMEBUFFER_UNDEFINED";
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
		case GL_FRAMEBUFFER_UNSUPPORTED: return "GL_FRAMEBUFFER_UNSUPPORTED";
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
		case 0: return "error status";
		default: return "unknown status";
	}
}
