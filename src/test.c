
#include <common.h>
#include <cllib/cllib.h>

#define SPHERES_NUM 5

const char *compile_flags =
	"-I . -I source"
	" -D SPHERES_NUM=" STR(SPHERES_NUM)
	" -D SCREEN_WIDTH=" STR(SCREEN_WIDTH)
	" -D SCREEN_HEIGHT=" STR(SCREEN_HEIGHT);

unsigned int g_canvas[SCREEN_WIDTH * SCREEN_HEIGHT];

typedef cl_float3 float3;
#define FLOAT3(X, Y, Z)                \
	(float3)                       \
	{                              \
		.x = X, .y = Y, .z = Z \
	}

#include "source/struct.cl"

unsigned int *run()
{

	struct Sphere spheres[SPHERES_NUM];

	spheres[0] = (struct Sphere){ .color = RED,
				      .position = FLOAT3(-0.3, -0.8, 9),
				      .emissionStrength = 0.0,
				      .radius = 1 };
	spheres[1] = (struct Sphere){ .color = PURPLE,
				      .position = FLOAT3(0, -100, 0),
				      .emissionStrength = 0.0,
				      .radius = 99 };
	spheres[2] = (struct Sphere){ .color = BLUE,
				      .position = FLOAT3(0, 0, 7),
				      .emissionStrength = 0.05,
				      .radius = 0.8 };
	spheres[3] = (struct Sphere){ .color = WHITE,
				      .position = FLOAT3(-8, 8, 10),
				      .emissionStrength = 1,
				      .radius = 10 };
	spheres[4] = (struct Sphere){ .color = CYAN,
				      .position = FLOAT3(1.3, -0.3, 7),
				      .emissionStrength = 0.0,
				      .radius = 0.7 };

	device_t device = create_device(gpu_type);
	context_t context = create_context(device);
	kernel_t kernel = create_kernel(device, context,
					"#include <source/path_tracer.cl>",
					"runKernel", compile_flags);
	queue_t queue = create_queue(context, device);

	buffer_t canvas = create_buffer(context, read_write, sizeof(g_canvas));
	buffer_t sp = create_buffer(context, read_only, sizeof(spheres));

	fill_buffer(queue, sp, sizeof(spheres), spheres, true);

	set_kernel_arg(kernel, canvas);
	set_kernel_arg(kernel, sp);
	set_kernel_size_2d(kernel, SCREEN_WIDTH, SCREEN_HEIGHT);

	run_kernel(queue, kernel);

	dump_buffer(queue, canvas, sizeof(g_canvas), g_canvas, true);
	flush_queue(queue);
	return (unsigned int *)g_canvas;
}

void local_sync_test()
{
	device_t device = create_device(gpu_type);
	context_t context = create_context(device);
	queue_t queue = create_queue(context, device);

	kernel_t kernel = create_kernel(device, context,
					"#include <source/test.cl>",
					"runKernel", compile_flags);

	unsigned int pix[4];
	buffer_t buf =
		create_buffer(context, write_only | dump_only, sizeof(pix));

	set_kernel_arg(kernel, buf);
	set_kernel_size_2d(kernel, 8, 4);
	set_kernel_local_size_2d(kernel, 8, 1);

	run_kernel(queue, kernel);

	dump_buffer(queue, buf, sizeof(pix), pix, true);

	for (int i = 0; i < (int)ARRAY_SIZE(pix); ++i) {
		printf("%d ", pix[i]);
	}
	printf("\n");
}

int main()
{
	local_sync_test();
}
