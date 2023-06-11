
#include <cllib/cllib.h>
#include <cllib/common.h>

cl_platform_id __create_platform(void)
{
	static cl_platform_id platform;
	cl_uint num_platforms;

	static bool platform_ready = false;
	cl_int err;

	if (platform_ready) {
		return platform;
	}
	err = clGetPlatformIDs(0, NULL, &num_platforms);
	cl_panic_on(err, "clGetPlatformIDs", err);
	if (num_platforms == 0) {
		panic("no platforms avaliable");
	} else if (num_platforms > 1) {
		warn("muliple platforms avaliable, chosing first one");
	}
	err = clGetPlatformIDs(1, &platform, NULL);
	cl_panic_on(err, "clGetPlatformIDs", err);
	return platform;
}

device_t create_device(enum device_type type)
{
	cl_platform_id platform;

	cl_device_id dev;
	cl_uint num_devices;

	cl_int err;

	platform = __create_platform();
	err = clGetDeviceIDs(platform, type, 0, NULL, &num_devices);
	cl_panic_on(err, "clGetDeviceIDs", err);
	if (num_devices == 0) {
		panic("no devices avaliable");
	} else if (num_devices > 1) {
		warn("multiple devices avaliable, chosing first one");
	}
	err = clGetDeviceIDs(platform, type, 1, &dev, NULL);
	cl_panic_on(err, "clGetDeviceIDs", err);

	return (device_t){ .__device = dev };
}

__always_inline context_t create_context(device_t device)
{
	return create_context_with_props(device, NULL);
}

inline context_t create_context_with_props(device_t device,
					   const context_props *properties)
{
	cl_context context;
	cl_device_id dev;
	cl_int err;

	dev = device.__device;
	context = clCreateContext(properties, 1, &dev, NULL, NULL, &err);
	cl_panic_on(err, "clCreateContext", err);

	return (context_t){ .__context = context };
}

kernel_t create_kernel(device_t device, context_t context, const char *source,
		       const char *kernel_name, const char *options)
{
	cl_program program;
	cl_device_id dev;
	cl_context ctx;
	cl_kernel kernel;
	cl_int err;
	size_t log_size;

	dev = device.__device;
	ctx = context.__context;
	program = clCreateProgramWithSource(ctx, 1, &source, NULL, &err);
	cl_panic_on(err, "clCreateProgramWithSource", err);
	err = clBuildProgram(program, 1, &dev, options, NULL, NULL);
	if (err == CL_BUILD_PROGRAM_FAILURE && CLLIB_PRINT_PROGRAM_LOG) {
		err = clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
					    0, NULL, &log_size);
		if (err != CL_SUCCESS) {
			cl_panic("clGetProgramInfo", err);
		}
		char log_data[log_size];
		err = clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
					    log_size, log_data, NULL);
		if (err != CL_SUCCESS) {
			cl_panic("clGetProgramInfo", err);
		}
		printf("%s\n", log_data);
		panic("build failure");
	}
	cl_panic_on(err, "clBuildProgram", err);

	kernel = clCreateKernel(program, kernel_name, &err);
	cl_panic_on(err, "clCreateKernel", err);
	return (kernel_t){ .__kernel = kernel, .__arg = 0 };
};

inline queue_t create_queue(context_t context, device_t device)
{
	cl_context ctx;
	cl_device_id dev;
	cl_command_queue queue;
	cl_int err;

	ctx = context.__context;
	dev = device.__device;
	queue = clCreateCommandQueueWithProperties(ctx, dev, NULL, &err);
	cl_panic_on(err, "clCreateCommandQueueWithProperties", err);

	return (queue_t){ .__queue = queue };
}

inline buffer_t create_buffer(context_t context, enum buffer_type type,
			      size_t size)
{
	cl_context ctx;
	cl_mem buffer;
	cl_int err;

	ctx = context.__context;
	buffer = clCreateBuffer(ctx, type, size, NULL, &err);
	cl_panic_on(err, "clCreateBuffer", err);

	return (buffer_t){ .__buffer = buffer };
}

cl_mem clCreateFromGLRenderbuffer(cl_context context, cl_mem_flags flags,
				  unsigned int renderbuffer,
				  cl_int *errcode_ret);
inline buffer_t create_buffer_from_rbo(context_t context, enum buffer_type type,
				       unsigned int rbo)
{
	cl_context ctx;
	cl_mem buffer;
	cl_int err;

	ctx = context.__context;
	buffer = clCreateFromGLRenderbuffer(ctx, type, rbo, &err);
	cl_panic_on(err, "clCreateFromGLRenderbuffer", err);

	return (buffer_t){ .__buffer = buffer };
}

inline void fill_buffer(queue_t queue, buffer_t buffer, size_t size, void *data,
			bool blocking_write)
{
	cl_command_queue qw;
	cl_mem buff;
	cl_int err;

	qw = queue.__queue;
	buff = buffer.__buffer;
	err = clEnqueueWriteBuffer(qw, buff, blocking_write, 0, size, data, 0,
				   NULL, NULL);
	cl_panic_on(err, "clEnqueueWriteBuffer", err);
}

inline void dump_buffer(queue_t queue, buffer_t buffer, size_t size, void *data,
			bool blocking_read)
{
	cl_command_queue qw;
	cl_mem buff;
	cl_int err;

	qw = queue.__queue;
	buff = buffer.__buffer;
	err = clEnqueueReadBuffer(qw, buff, blocking_read, 0, size, data, 0,
				  NULL, NULL);
	cl_panic_on(err, "clEnqueueReadBuffer", err);
}

inline void __set_kernel_arg(cl_kernel kernel, unsigned int arg_index,
			     size_t arg_size, void *arg_value)
{
	cl_int err;

	err = clSetKernelArg(kernel, arg_index, arg_size, arg_value);
	cl_panic_on(err, "clSetKernelArg", err);
}

inline void __run_kernel(cl_command_queue queue, cl_kernel kernel,
			 unsigned int h, unsigned int w)
{
	(void)h;
	(void)w;
	cl_int err;
	size_t local_size[2] = { 8, 1 };
	size_t global_size[2] = { 8, 1 };

	err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_size,
				     local_size, 0, NULL, NULL);
	cl_panic_on(err, "clEnqueueNDRangeKernel", err);
}

inline void flush_queue(queue_t queue)
{
	cl_command_queue qw;
	cl_int err;

	qw = queue.__queue;
	err = clFlush(qw);
	cl_panic_on(err, "clFlush", err);
}

__cold __noreturn void __cl_panic(const char *msg, cl_int error,
				  const char *file, unsigned long line)
{
	printf("%s:%ld\npanic: %s: %s\n", file, line, msg, cl_strerror(error));
	abort();
}

const char *cl_strerror(cl_int error)
{
	switch(error)
	{
	// run-time and JIT compiler errors
	case 0: return "CL_SUCCESS";
	case -1: return "CL_DEVICE_NOT_FOUND";
	case -2: return "CL_DEVICE_NOT_AVAILABLE";
	case -3: return "CL_COMPILER_NOT_AVAILABLE";
	case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
	case -5: return "CL_OUT_OF_RESOURCES";
	case -6: return "CL_OUT_OF_HOST_MEMORY";
	case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
	case -8: return "CL_MEM_COPY_OVERLAP";
	case -9: return "CL_IMAGE_FORMAT_MISMATCH";
	case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
	case -11: return "CL_BUILD_PROGRAM_FAILURE";
	case -12: return "CL_MAP_FAILURE";
	case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
	case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
	case -15: return "CL_COMPILE_PROGRAM_FAILURE";
	case -16: return "CL_LINKER_NOT_AVAILABLE";
	case -17: return "CL_LINK_PROGRAM_FAILURE";
	case -18: return "CL_DEVICE_PARTITION_FAILED";
	case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

	// compile-time errors
	case -30: return "CL_INVALID_VALUE";
	case -31: return "CL_INVALID_DEVICE_TYPE";
	case -32: return "CL_INVALID_PLATFORM";
	case -33: return "CL_INVALID_DEVICE";
	case -34: return "CL_INVALID_CONTEXT";
	case -35: return "CL_INVALID_QUEUE_PROPERTIES";
	case -36: return "CL_INVALID_COMMAND_QUEUE";
	case -37: return "CL_INVALID_HOST_PTR";
	case -38: return "CL_INVALID_MEM_OBJECT";
	case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
	case -40: return "CL_INVALID_IMAGE_SIZE";
	case -41: return "CL_INVALID_SAMPLER";
	case -42: return "CL_INVALID_BINARY";
	case -43: return "CL_INVALID_BUILD_OPTIONS";
	case -44: return "CL_INVALID_PROGRAM";
	case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
	case -46: return "CL_INVALID_KERNEL_NAME";
	case -47: return "CL_INVALID_KERNEL_DEFINITION";
	case -48: return "CL_INVALID_KERNEL";
	case -49: return "CL_INVALID_ARG_INDEX";
	case -50: return "CL_INVALID_ARG_VALUE";
	case -51: return "CL_INVALID_ARG_SIZE";
	case -52: return "CL_INVALID_KERNEL_ARGS";
	case -53: return "CL_INVALID_WORK_DIMENSION";
	case -54: return "CL_INVALID_WORK_GROUP_SIZE";
	case -55: return "CL_INVALID_WORK_ITEM_SIZE";
	case -56: return "CL_INVALID_GLOBAL_OFFSET";
	case -57: return "CL_INVALID_EVENT_WAIT_LIST";
	case -58: return "CL_INVALID_EVENT";
	case -59: return "CL_INVALID_OPERATION";
	case -60: return "CL_INVALID_GL_OBJECT";
	case -61: return "CL_INVALID_BUFFER_SIZE";
	case -62: return "CL_INVALID_MIP_LEVEL";
	case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
	case -64: return "CL_INVALID_PROPERTY";
	case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
	case -66: return "CL_INVALID_COMPILER_OPTIONS";
	case -67: return "CL_INVALID_LINKER_OPTIONS";
	case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

	// extension errors
	case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
	case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
	case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
	case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
	case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
	case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
	default: return "Unknown OpenCL error";
	}
}
