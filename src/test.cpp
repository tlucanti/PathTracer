
#define SPHERES_NUM 5

#include <source/path_tracer.cl>

EXTERN_C

unsigned int *g_canvas = NULL;

void run()
{
	g_canvas = (unsigned int *)malloc(SCREEN_WIDTH * SCREEN_HEIGHT *
					  sizeof(unsigned int));

	struct Sphere spheres[SPHERES_NUM];

	spheres[0] = { .color = RED,
		       .position = FLOAT3(-0.3, -0.8, 9),
		       .emissionStrength = 0.0,
		       .radius = 1 };
	spheres[1] = { .color = PURPLE,
		       .position = FLOAT3(0, -100, 0),
		       .emissionStrength = 0.0,
		       .radius = 99 };
	spheres[2] = { .color = BLUE,
		       .position = FLOAT3(0, 0, 7),
		       .emissionStrength = 0.0,
		       .radius = 0.8 };
	spheres[3] = { .color = WHITE,
		       .position = FLOAT3(-8, 8, 10),
		       .emissionStrength = 1,
		       .radius = 10 };
	spheres[4] = { .color = CYAN,
		       .position = FLOAT3(1.3, -0.3, 7),
		       .emissionStrength = 0.0,
		       .radius = 0.7 };

	for (int y = 0; y < SCREEN_HEIGHT; ++y) {
		__dim.y = y;
		for (int x = 0; x < SCREEN_WIDTH; ++x) {
			__dim.x = x;
			runKernel(g_canvas, spheres);
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

int main()
{
	run();
	end();
}

EXTERN_C_END
