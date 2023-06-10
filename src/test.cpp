
#include <source/path_tracer.cl>

int main()
{
	unsigned int canvas[SCREEN_WIDTH * SCREEN_HEIGHT];
	struct Sphere spheres[SPHERES_NUM] = {
		{
			.color=RED,
			.position=FLOAT3(0, 0, 3),
			.emissionStrength=1,
			.radius=1
		}
	};

	for (int x = 0; x < SCREEN_WIDTH; ++x)
	{
		for (int y = 0; y < SCREEN_HEIGHT; ++y) {
			pathTracer(canvas, spheres);
			__dim.y = y;
		}
		__dim.x = x;
	}
}