
#include <common.h>
#include <cllib/cllib.h>
#include <gllib/gllib.h>
#include <clgl.h>

void cl_test()
{
	int a1[] = {0, 1, 2, 3, 4};
	int a2[] = {5, 6, 7, 8, 9};

	device_t device = create_device();
	context_t context = create_context(device);
	kernel_t kernel = create_kernel(device, context,
			//"#include <source/path_tracer.cl>\n#define V 1", "pathTracer");
			"#include <source/array_sum.cl>\n#define V 2", "array_sum");
	queue_t queue = create_queue(context, device);

	buffer_t b1 = create_buffer(context, read_write, 5 * sizeof(int));
	buffer_t b2 = create_buffer(context, read_write, 5 * sizeof(int));

	fill_buffer(queue, b1, a1, true);
	fill_buffer(queue, b2, a2, true);

	set_kernel_arg_buffer(kernel, b1);
	set_kernel_arg_buffer(kernel, b2);

	run_kernel(queue, kernel, 10, 10);

	dump_buffer(queue, b1, a1, true);
	flush_queue(queue);

	for (int i = 0; i < 5; ++i) {
		printf("%d ", a1[i]);
	}
	printf("\n");
}

void glfw_test()
{
	window_t window = create_window(800, 600);
	while (window_opened(window));
}

void gl_test()
{
	create_rbo();
}

void direct_cl_test()
{
	device_t device = create_device();
	context_t context = create_gl_context(device);
	queue_t queue = create_queue(context, device);

	render_buffer_t rbo = create_rbo();
	window_t window = create_window(800, 600);

	buffer_t buffer = create_buffer_from_rbo(context, read_write, rbo.__rbo_id);

	(void)window; (void)queue; (void)buffer;
}

int main()
{
	//cl_test();
	//glfw_test();
	//gl_test();
	direct_cl_test();
}
