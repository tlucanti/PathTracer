
#ifndef _CL_GL_H
#define _CL_GL_H

#include <CL/cl_gl.h>
#include <GL/glx.h>

#include <cllib/cllib.h>
#include <cllib/common.h>
#include <gllib/gllib.h>

context_t create_gl_context(device_t device)
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

#endif /* _CL_GL_H */
