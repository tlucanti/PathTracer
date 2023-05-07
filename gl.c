/*
 * GL01Hello.cpp: Test OpenGL/GLUT C/C++ Setup
 * Tested under Eclipse CDT with MinGW/Cygwin and CodeBlocks with MinGW
 * To compile with -lfreeglut -lglu32 -lopengl32
 */
#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include <GL/glext.h>
#include <GL/glcorearb.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

void *glXGetProcAddressARB(char *);
void *glXGetProcAddress(char *);
typedef void (*FGL_GEN_BUFFERS) (GLsizei, GLuint *);
void (*FglGenRenderBuffers) (GLsizei, GLuint *) = NULL;
typedef GLenum (*FGL_CHECK_FRAMEBUFFER_STATUS) (GLenum);
typedef void (*FGL_BIND_RENDER_BUFFER) (GLenum, GLuint);
typedef void (*FGL_RENDER_BUFFER_STORAGE) (GLenum, GLenum, GLsizei, GLsizei);
typedef void (*FGL_FRAME_BUFFER_RENDER_BUFFER) (GLenum, GLenum, GLenum, GLuint);
typedef void (*FGL_BIND_FRAME_BUFFER) (GLenum, GLuint);
FGL_GEN_BUFFERS FglGenBuffers = NULL;
FGL_CHECK_FRAMEBUFFER_STATUS FglCheckFramebufferStatus = NULL;
FGL_BIND_RENDER_BUFFER FglBindRenderbuffer = NULL;
FGL_RENDER_BUFFER_STORAGE FglRenderbufferStorage = NULL;
FGL_FRAME_BUFFER_RENDER_BUFFER FglFramebufferRenderbuffer = NULL;
FGL_BIND_FRAME_BUFFER FglBindFramebuffer = NULL;
#define LOAD_ADDRESS glXGetProcAddress

void load_gl(void)
{
	FglGenBuffers = LOAD_ADDRESS("glGenBuffers");
	FglGenRenderBuffers = LOAD_ADDRESS("glGenRenderBuffers");
	FglCheckFramebufferStatus = LOAD_ADDRESS("glCheckFramebufferStatus");
	FglBindRenderbuffer = LOAD_ADDRESS("glBindRenderbuffer");
	FglRenderbufferStorage = LOAD_ADDRESS("glRenderbufferStorage");
	FglFramebufferRenderbuffer = LOAD_ADDRESS("glFramebufferRenderbuffer");
	FglBindFramebuffer = LOAD_ADDRESS("glBindFramebuffer");
}

/* Handler for window-repaint event. Call back when the window first appears and
   whenever the window needs to be re-painted. */
void _triangle() {
   glClearColor(1.0f, 0.5f, 0.8f, 1.0f); // Set background color to black and opaque
   glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer (background)

   // Draw a Red 1x1 Square centered at origin
   glBegin(GL_QUADS);              // Each set of 4 vertices form a quad
      glColor3f(1.0f, 0.0f, 0.0f); // Red
      glVertex2f(-0.5f, -0.5f);    // x, y
      glVertex2f( 0.5f, -0.5f);
      glVertex2f( 0.5f,  0.5f);
      glVertex2f(-0.5f,  0.5f);
   glEnd();

   glFlush();  // Render now
}

/* Main function: GLUT runs as a console application starting at main()  */
int triangle() {
   int ac = 0;
   glutInit(&ac, NULL);                 // Initialize GLUT
   glutCreateWindow("OpenGL Setup Test"); // Create a window with the given title
   glutInitWindowSize(320, 320);   // Set the window's initial width & height
   glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
   glutDisplayFunc(_triangle); // Register display callback handler for window re-paint
   glutMainLoop();           // Enter the event-processing loop
   return 0;
}

void check_error()
{
	GLenum status = glGetError();

	if (status != GL_NO_ERROR) {
		printf(">>> ERROR: %d\n", status);
	}
}

const char *__glstrerror(GLenum status)
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

void check_buffer()
{
	GLenum status = FglCheckFramebufferStatus(GL_FRAMEBUFFER);
	printf("> BUFFER CHECK: %s: %d\n", __glstrerror(status), status);
	check_error();
}

GLuint create_rbo()
{
	GLuint rb_id;
	glGenRenderbuffers(1, &rb_id);
	check_error();
	printf("generated\n");

	FglBindRenderbuffer(GL_RENDERBUFFER, rb_id);
	check_error();
	printf("binded\n");

	FglRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA32F, 256, 256);
	check_error();
	printf("render buffer storage\n");

        FglFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rb_id);
	check_error();
	printf("framebuffer render buffer\n");

	check_buffer();

	glClearColor(0.5f, 0.1f, 0.6f, 1.0f);
	check_error();
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
	check_error();
        glClear(GL_COLOR_BUFFER_BIT);
	check_error();
	printf("cleared\n");

	FglBindFramebuffer(GL_FRAMEBUFFER, 0);
	check_error();
        FglBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	check_error();
	printf("unbinded\n");

	return 0;
}

int main()
{
	load_gl();
	int fps = 0;

	//Инициализация GLFW
	glfwInit();
	//Настройка GLFW
	//Задается минимальная требуемая версия OpenGL.
	//Мажорная
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//Минорная
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//Установка профайла для которого создается контекст
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Выключение возможности изменения размера окна
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	glfwWindowHint(GLFW_RED_BITS, 8);
        glfwWindowHint(GLFW_GREEN_BITS, 8);
        glfwWindowHint(GLFW_BLUE_BITS, 8);
        glfwWindowHint(GLFW_REFRESH_RATE, 1);
        glfwWindowHint(GLFW_ALPHA_BITS, 8);
        glfwWindowHint(GLFW_STENCIL_BITS, 8);
        glfwWindowHint(GLFW_DEPTH_BITS, 24);

	GLFWwindow* window = glfwCreateWindow(800, 600, "window name", NULL, NULL);
	if (window == NULL)
	{
		printf("error\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	create_rbo();
	while(!glfwWindowShouldClose(window))
	{
	    glfwPollEvents();
	    glfwSwapBuffers(window);
	    ++fps;
	    if (fps % 100 == 0) {
		    printf("%d\n", fps);
	    }
	}
	glfwTerminate();
	return 0;
}
