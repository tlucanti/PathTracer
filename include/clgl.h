
#ifndef _CL_GL_H
#define _CL_GL_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <CL/cl_gl.h>

#include <cllib/cllib.h>
#include <cllib/common.h>

const char *frag_text = "#version 330\n"
			"uniform sampler2D tex;\n"
			"in vec2 texcoord;\n"
			"out vec4 fragColor;\n"
			"void main() {\n"
			"    fragColor = texture2D(tex,texcoord);\n"
			"}\n";

const char *vert_text = "#version 330\n"
			"layout(location = 0) in vec3 pos;\n"
			"layout(location = 1) in vec2 tex;\n"
			"uniform mat4 matrix;\n"
			"out vec2 texcoord;\n"
			"void main() {\n"
			"	texcoord = tex;\n"
			"	gl_Position = matrix * vec4(pos,1.0);\n"
			"}\n";

const float vertices[] = { -1.0f, -1.0f, 0.0, 1.0f,  -1.0f, 0.0,
			   1.0f,  1.0f,	 0.0, -1.0f, 1.0f,  0.0 };
const float texcords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
static const uint indices[6] = { 0, 1, 2, 0, 2, 3 };

static void __compile_log(GLint shader, const char *message)
{
	GLint len;
	char *log;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	log = malloc(len);
	panic_on(log == NULL, "malloc");

	glGetShaderInfoLog(shader, len, NULL, log);
	printf("shader %s compile log:\n%s\n", message, log);
	free(log);
}

static void __link_log(GLint program, const char *message)
{
	GLint len;
	char *log;

	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
	log = malloc(len);
	panic_on(log == NULL, "malloc");

	glGetProgramInfoLog(program, len, NULL, log);
	printf("program %s compile log:\n%s\n", message, log);
	free(log);
}

static GLuint __init_shaders()
{
	GLuint vertex, fragment;
	GLint vlen, flen;
	GLint compiled, linked;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);

	vlen = strlen(vert_text);
	flen = strlen(frag_text);

	printf("%p\n", glShaderSource);
	glShaderSource(vertex, 1, &vert_text, &vlen);
	glShaderSource(fragment, 1, &frag_text, &flen);

	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		__compile_log(vertex, "vertex");
		panic("vertex: glCompileShader");
	}

	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		__compile_log(fragment, "fragment");
		panic("fragment: glCompileShader");
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked) {
		__link_log(program, "program");
		panic("program: glLinkProgram");
	}

	return program;
}

static GLuint __create_texture(GLuint width, GLuint height)
{
	GLuint texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
		     GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

static GLuint __create_buffer(size_t size, const float *data)
{
	GLuint buffer;

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return buffer;
}

static GLuint __create_ibo()
{
	GLuint ibo;

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
		     GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return ibo;
}


static __inline GLuint create_texture(unsigned int width, unsigned int height)
{
	if (!gladLoadGL()) {
		panic("gladLoadGL");
	}
	printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);

	// opengl
	GLuint program = __init_shaders();
	GLuint texture = __create_texture(width, height);
	GLuint vbo = __create_buffer(sizeof(vertices), vertices);
	GLuint tbo = __create_buffer(sizeof(texcords), texcords);
	GLuint ibo = __create_ibo();
	GLuint vao;

	// bind vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// attach vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	// attach tbo
	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	// attach ibo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBindVertexArray(0);

	(void)program;
	return texture;
}

static inline buffer_t create_image(context_t context, GLuint texture,
				      enum buffer_type type)
{
	cl_context ctx = context.__context;
	cl_mem buffer;
	cl_int err;

	buffer = clCreateFromGLTexture(ctx, type, GL_TEXTURE_2D, 0, texture,
				       &err);
	cl_panic_on(err, "GL_TEXTURE_2D", err);

	return (buffer_t){ .__buffer = buffer };
}

static inline context_t create_gl_context(device_t device, GLFWwindow *window)
{
	const context_props props[] = {
		CL_GL_CONTEXT_KHR,
		(cl_context_properties)glfwGetGLXContext(window),
		CL_GLX_DISPLAY_KHR,
		(cl_context_properties)glfwGetX11Display(),
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)__create_platform(),
		0
	};
	return create_context_with_props(device, props);
}

#if 0
static inline context_t create_gl_context(device_t device)
{
	cl_platform_id platform;
	cl_uint num_platforms;

	cl_context context;
	static cl_context_properties properties[7] = {};

	cl_device_id dev;
	cl_int err;

	err = clGetPlatformIDs(0, NULL, &num_platforms);
	cl_panic_on(err, "clGetPlatformIDs", err);
	if (num_platforms == 0) {
		panic("no platforms avaliable");
	} else if (num_platforms > 1) {
		warn("muliple platforms avaliable, chosing first one");
	}
	err = clGetPlatformIDs(1, &platform, NULL);
	cl_panic_on(err, "clGetPlatformIDs", err);

	properties[0] = CL_GL_CONTEXT_KHR;
	properties[1] = (cl_context_properties)glXGetCurrentContext();

	properties[2] = CL_GLX_DISPLAY_KHR;
	properties[3] = (cl_context_properties)glXGetCurrentDisplay();

	properties[4] = CL_CONTEXT_PLATFORM;
	properties[5] = (cl_context_properties)platform;

	properties[6] = 0;

	dev = device.__device;
	context = clCreateContext(properties, 1, &dev, NULL, NULL, &err);
	cl_panic_on(err, "clCreateContext", err);

	return (context_t){ .__context = context };
}
#endif

#endif /* _CL_GL_H */
