
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define TRACE_BOUNCE_COUNT 1
#define RAND_MAX UINT_MAX
#define RAND_HALF (UINT_MAX / 2)

struct Sphere {
	float3 color;
	float3 position;
	float emissionStrength;
};

struct Ray {
	float3 origin;
	float3 direction;
};

struct HitInfo {
	float3 hitColor;
	float3 hitPoint;
	float3 normal;
	float emissionStrength;
	bool didHit;
};

/* returns next random integer from previous number */
inline unsigned int nextRandomInt(unsigned int prev)
{
	return prev * 0x5DEECE66D + 0xB;
}

/* returns next random float in range [0, 1] from previous generated number */
inline float nextRandomFloat(unsigned int *seed)
{
	*seed = nextRandomInt(*seed);
	return (float)(*seed) / RAND_MAX;
}

/* returns next random float in range [-1, 1] from previous generated number */
inline float nextRandomFloatNeg(unsigned int *seed)
{
	*seed = nextRandomInt(*seed);
	return (float)(*seed) / RAND_HALF - (float)1;
}

inline void setPixelColor(__global unsigned int *canvas, unsigned short x,
			unsigned short y, float3 color)
{
	unsigned int bit_color;

	bit_color = (unsigned int)(color.z * 255);
	bit_color |= (unsigned int)(color.y * 255) >> 8;
	bit_color |= (unsigned int)(color.x * 255) >> 16;
	canvas[(y) * SCREEN_WIDTH + (x)] = bit_color;
}

void createViewVector(struct Ray *ray, unsigned short x, unsigned short y)
{

}

void intersectSphere(struct Ray *ray, struct HitInfo *hitInfo, __constant struct Sphere *spheres)
{

}

float3 randomHemiSphere(float3 normal, unsigned int *seed)
{
	float3 direction;

	direction.x = nextRandomFloatNeg(seed);
	direction.y = nextRandomFloatNeg(seed);
	direction.z = nextRandomFloatNeg(seed);
	return direction;
}

float3 tracePath(struct Ray *viewVector, __constant struct Sphere *spheres)
{
	float3 incomingLight = (float3)(0, 0, 0);
	float3 rayColorIntensity = (float3)(1, 1, 1);
	struct HitInfo hitInfo;

	for (int i = 0; i < TRACE_BOUNCE_COUNT; ++i) {
		intersectSphere(viewVector, &hitInfo, spheres);
		if (!hitInfo.didHit) {
			break ;
		}
		incomingLight += hitInfo.hitColor * hitInfo.emissionStrength;
		rayColorIntensity *= hitInfo.hitColor;

		viewVector->origin = hitInfo.hitPoint;
		viewVector->direction = randomHemiSphere(hitInfo.normal, 0);
	}
	return incomingLight;
}

__kernel void pathTracer()
{}

void _pathTracer(__global unsigned int *canvas, __constant struct Sphere *spheres)
{
	return ;
	struct Ray viewVector;
	const unsigned short x = get_global_id(0);
	const unsigned short y = get_global_id(1);

	createViewVector(&viewVector, x, y);
	float3 pixelColor = tracePath(&viewVector, spheres);
	setPixelColor(canvas, x, y, pixelColor);
	return ;
}
