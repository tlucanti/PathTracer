
#include <common.h>
#include <cllib/cllib.h>

int main()
{
	int a1[] = {0, 1, 2, 3, 4};
	int a2[] = {5, 6, 7, 8, 9};

	device_t device = create_device();
	context_t context = create_context(device);
	kernel_t kernel = create_kernel(device, context,
			"#include <source/path_tracer.cl>\n#define V 1", "pathTracer");
	queue_t queue = create_queue(context, device);

	buffer_t b1 = create_buffer(context, read_write, 5 * sizeof(int));
	buffer_t b2 = create_buffer(context, read_write, 5 * sizeof(int));

	fill_buffer(queue, b1, a1, true);
	fill_buffer(queue, b2, a2, true);

	//set_kernel_arg_buffer(kernel, b1);
	//set_kernel_arg_buffer(kernel, b2);

	run_kernel(queue, kernel, 10, 10);

	dump_buffer(queue, b1, a1, true);
	flush_queue(queue);

	for (int i = 0; i < 5; ++i) {
		printf("%d ", a1[i]);
	}
	printf("\n");
	return 0;
}
