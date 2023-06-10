
#ifndef _GLLIB_GLLIB_H
#define _GLLIB_GLLIB_H

#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/glcorearb.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include <common.h>

void glGenRenderbuffers(GLsizei, GLuint *);
void glBindRenderbuffer(GLenum, GLuint);
void glRenderbufferStorage(GLenum, GLenum, GLsizei, GLsizei);
void glFramebufferRenderbuffer(GLenum, GLenum, GLenum, GLuint);
void glBindFramebuffer(GLenum, GLuint);
GLenum glCheckFramebufferStatus(GLenum);

typedef struct {
	GLFWwindow *__window;
} window_t;

typedef struct {
	GLuint __rbo_id;
} render_buffer_t;

window_t create_window(unsigned int width, unsigned int height);
bool window_opened(window_t window);
render_buffer_t create_rbo();

#endif /* _GLLIB_GLLIB_H */
