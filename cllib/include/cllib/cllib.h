
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
	unsigned int __arg;
} kernel_t;

typedef struct {
	cl_command_queue __queue;
} queue_t;

typedef struct {
	cl_mem __buffer;
} buffer_t;

enum buffer_type {
	read_write = CL_MEM_READ_WRITE,
	write_only = CL_MEM_WRITE_ONLY,
	read_only = CL_MEM_READ_ONLY
};

enum device_type {
	cpu_type = CL_DEVICE_TYPE_CPU,
	gpu_type = CL_DEVICE_TYPE_GPU
};

device_t create_device(enum device_type type);
context_t create_context(device_t device);
kernel_t create_kernel(device_t device, context_t context, const char *source,
		       const char *kernel_name, const char *options);
queue_t create_queue(context_t context, device_t device);
buffer_t create_buffer(context_t context, enum buffer_type, size_t size);
buffer_t create_buffer_from_rbo(context_t context, enum buffer_type type,
				unsigned int rbo);
void fill_buffer(queue_t queue, buffer_t buffer, size_t size, void *data,
		 bool blocking_write);
void dump_buffer(queue_t queue, buffer_t buffer, size_t size, void *data,
		 bool blocking_read);
void __set_kernel_arg(cl_kernel kernel, unsigned int arg_index, size_t arg_size,
		      void *arg_value);
void __run_kernel(cl_command_queue queue, cl_kernel kernel, unsigned int h,
		  unsigned int w);
void flush_queue(queue_t queue);

#define set_kernel_arg(kernel, arg)                                            \
	do {                                                                   \
		static_assert(sizeof(arg) <= 8,                                \
			      "structures in kernel args are not allowed");    \
		__set_kernel_arg(kernel.__kernel, kernel.__arg++, sizeof(arg), \
				 &arg);                                        \
	} while (false)

#define run_kernel(queue, kernel, w, h)                             \
	do {                                                        \
		__run_kernel(queue.__queue, kernel.__kernel, w, h); \
		kernel.__arg = 0;                                   \
	} while (false)

#endif /* _CLLIB_CLLIB_H */
