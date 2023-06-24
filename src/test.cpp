
#define SPHERES_NUM 5

#include <source/path_tracer.cl>
#include <linalg.h>

EXTERN_C

unsigned int *g_canvas = NULL;

struct Camera {
	float3 position;
	float alpha;
	float theta;
	struct RotateMatrix matrix;
};

static struct Sphere *init_scene(void)
{
	static struct Sphere spheres[SPHERES_NUM];

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

	return spheres;
}

unsigned int *run()
{
	g_canvas = (unsigned int *)malloc(SCREEN_WIDTH * SCREEN_HEIGHT *
					  sizeof(unsigned int));
	struct Sphere *scene = init_scene();
	struct Camera camera = { .position = FLOAT3(0, 0, 0),
				 .alpha = 0,
				 .theta = 0 };

	compute_rotation_matrix(&camera.matrix, camera.alpha, camera.theta);
	printf("   ");
	for (int y = 0; y < SCREEN_HEIGHT; ++y) {
		__dim.y = y;
		printf("\b\b\b%2d%%", y * 100 / SCREEN_HEIGHT);
		fflush(stdout);
		for (int x = 0; x < SCREEN_WIDTH; ++x) {
			__dim.x = x;
			runKernel(g_canvas, scene, camera.position,
				  camera.matrix);
		}
	}
	printf("\b\b\b");
	fflush(stdout);
	return g_canvas;
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
