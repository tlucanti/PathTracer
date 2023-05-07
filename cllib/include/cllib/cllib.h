
#ifndef _CLLIB_CLLIB_H
# define _CLLIB_CLLIB_H

# include <CL/cl.h>
# include <common.h>

# if defined(CONFIG_DEVICE_TYPE_CPU)
#  define CLLIB_DEVICE_TYPE CL_DEVICE_TYPE_CPU
# elif defined(CONFIG_DEVICE_TYPE_GPU)
#  define CLLIB_DEVICE_TYPE CL_DEVICE_TYPE_GPU
# else
#  error "device type not defined"
# endif

# define CLLIB_BUILD_OPTIONS "-I ."

# ifdef CONFIG_PRINT_PROGRAM_LOG
#  define CLLIB_PRINT_PROGRAM_LOG true
# else
#  define CLLIB_PRING_PRIGRAM_LOG false
# endif

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
	size_t __size;
} buffer_t;

enum buffer_type {
	read_write = CL_MEM_READ_WRITE,
	write_only = CL_MEM_WRITE_ONLY,
	read_only = CL_MEM_READ_ONLY
};

device_t create_device();
context_t create_context(device_t device);
kernel_t create_kernel(device_t device, context_t context, const char *source, const char *kernel_name);
queue_t create_queue(context_t context, device_t device);
buffer_t create_buffer(context_t context, enum buffer_type, size_t size);
buffer_t create_buffer_from_rbo(context_t context, enum buffer_type type, unsigned int rbo);
void fill_buffer(queue_t queue, buffer_t buffer, void *data, bool blocking_write);
void dump_buffer(queue_t queue, buffer_t buffer, void *data, bool blocking_read);
void __set_kernel_arg(cl_kernel kernel, unsigned int arg_index, size_t arg_size, void *arg_value);
void __run_kernel(cl_command_queue queue, cl_kernel kernel, unsigned int h, unsigned int w);
void flush_queue(queue_t queue);

# define set_kernel_arg(kernel, arg) do {	\
	static_assert(sizeof(arg) <= 8, "structures in kernel args are not allowed");	\
	__set_kernel_arg(kernel.__kernel, kernel.__arg++, sizeof(arg), &arg); \
} while (false)

# define set_kernel_arg_buffer(kernel, buffer) do {	\
	(void)(&buffer == (buffer_t *)0);	\
	static_assert(sizeof(buffer) == sizeof(buffer_t), "expected struct buffer"); \
	__set_kernel_arg(kernel.__kernel, kernel.__arg++, sizeof(cl_mem), &(buffer).__buffer);	\
} while (false)

# define run_kernel(queue, kernel, w, h) do {	\
	__run_kernel(queue.__queue, kernel.__kernel, w, h);	\
	kernel.__arg = 0;	\
} while (false)

#endif /* _CLLIB_CLLIB_H */
