
#include <source/path_tracer.cl>

EXTERN_C

unsigned int *g_canvas = NULL;

void run()
{
	g_canvas = (unsigned int *)malloc(SCREEN_WIDTH * SCREEN_HEIGHT *
				 sizeof(unsigned int));
	struct Sphere spheres[SPHERES_NUM] = { { .color = RED,
						 .position = FLOAT3(0, 0, 3),
						 .emissionStrength = 1,
						 .radius = 1 } };

	for (int x = 0; x < SCREEN_WIDTH; ++x) {
		__dim.x = x;
		for (int y = 0; y < SCREEN_HEIGHT; ++y) {
			__dim.y = y;
			kernel(g_canvas, spheres);
		}
	}
}

unsigned int get(unsigned int x, unsigned int y)
{
	return g_canvas[y * SCREEN_WIDTH + x];
}

void end()
{
	free(g_canvas);
	g_canvas = NULL;
}

EXTERN_C_END
