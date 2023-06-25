
#ifndef _CLLIB_CLLIB_H
#define _CLLIB_CLLIB_H

#include <CL/cl.h>
#include <common.h>

#ifdef CONFIG_DONT_PRINT_PROGRAM_LOG
#define CLLIB_PRINT_PROGRAM_LOG false
#else
#define CLLIB_PRINT_PROGRAM_LOG true
#endif

typedef struct {
	cl_device_id __device;
} device_t;

typedef struct {
	cl_context __context;
} context_t;

typedef struct {
	cl_kernel __kernel;
	unsigned short __arg;
	unsigned short __dimentions;
	bool __set_local;
	size_t __global_size[3];
	size_t __local_size[3];
} kernel_t;

typedef struct {
	cl_command_queue __queue;
} queue_t;

typedef struct {
	cl_mem __buffer;
} buffer_t;

enum buffer_type {
	/**
	 * created memory buffer can be read and wrote when used inside a kernel
	 */
	read_write = CL_MEM_READ_WRITE,

	/**
	 * created memory buffer is a write-only memory object when used inside
	 * a kernel
	 */
	write_only = CL_MEM_WRITE_ONLY,

	/**
	 * created memory buffer object is a read-only memory object when used
	 * inside a kernel
	 */
	read_only = CL_MEM_READ_ONLY,

	/**
	 * created memory buffer can only be filled from host to kernel
	 */
	fill_only = CL_MEM_HOST_WRITE_ONLY,

	/**
	 * created memory buffer can only be dumped from kernel to host
	 */
	dump_only = CL_MEM_HOST_READ_ONLY,

	/**
	 * created memory buffer cannot be accessed to fill or dump from host
	 */
	no_access = CL_MEM_HOST_NO_ACCESS,
};

enum device_type {
	cpu_type = CL_DEVICE_TYPE_CPU,
	gpu_type = CL_DEVICE_TYPE_GPU
};

typedef cl_context_properties context_props;

device_t create_device(enum device_type type);
context_t create_context(device_t device);
context_t create_context_with_props(device_t device,
				    const context_props *properties);
kernel_t create_kernel(device_t device, context_t context, const char *source,
		       const char *kernel_name, const char *options);
queue_t create_queue(context_t context, device_t device);
buffer_t create_buffer(context_t context, enum buffer_type, size_t size);
buffer_t create_buffer_from(context_t context, enum buffer_type, void *ptr,
			    size_t size);
buffer_t create_buffer_from_rbo(context_t context, enum buffer_type type,
				unsigned int rbo);
void fill_buffer(queue_t queue, buffer_t buffer, size_t size, void *data,
		 bool blocking_write);
void dump_buffer(queue_t queue, buffer_t buffer, size_t size, void *data,
		 bool blocking_read);
void flush_queue(queue_t queue);

#define set_kernel_size_1d(kernel, size) \
	__set_kernel_size(&(kernel), 1, size, 0, 0)
#define set_kernel_size_2d(kernel, width, height) \
	__set_kernel_size(&(kernel), 2, width, height, 0)
#define set_kernel_size_3d(kernel, width, height, depth) \
	__set_kernel_size(&(kernel), 3, width, height, depth)

#define set_kernel_local_size_1d(kernel, size) \
	__set_kernel_local_size(&(kernel), 1, size, 0, 0)
#define set_kernel_local_size_2d(kernel, width, height) \
	__set_kernel_local_size(&(kernel), 2, width, height, 0)
#define set_kernel_local_size_3d(kernel, width, height, depth) \
	__set_kernel_local_size(&(kernel), 3, width, height, depth)

#define set_kernel_arg(kernel, arg) \
	__set_kernel_arg((kernel).__kernel, (kernel).__arg++, sizeof(arg), &arg)

#define set_kernel_arg_at(kernel, arg, pos) \
	__set_kernel_arg(kernel.__kernel, pos, sizeof(arg), &arg)

#define run_kernel(queue, kernel) __run_kernel(queue, &(kernel))

void __set_kernel_arg(cl_kernel kernel, unsigned int arg_index, size_t arg_size,
		      void *arg_value);
void __set_kernel_size(kernel_t *kernel, unsigned short dimentions,
		       size_t width, size_t height, size_t depth);
void __set_kernel_local_size(kernel_t *kernel, unsigned short dimentions,
			     size_t width, size_t height, size_t depth);
void __run_kernel(queue_t queue, kernel_t *kernel);
cl_platform_id __create_platform(void);

#endif /* _CLLIB_CLLIB_H */
