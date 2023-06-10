
#ifndef PATH_TRACER_CL
#define PATH_TRACER_CL

#include <common.cl>
#include <random.cl>

EXTERN_C

#define SPHERES_NUM 2

/**
 * setPixelColor() sets rgb color of pixel in pixel buffer in given coordinates
 *
 * @param canvas pixel color buffer in rgb format
 * @param x coordinate in color buffer
 * @param y coordinate in color buffer
 * @param color rgb color with float [0, 1] color intensity
 */
__always_inline void setPixelColor(__global unsigned int *canvas,
				   unsigned short x, unsigned short y,
				   const float3 *__restrict color)
{
	unsigned int bit_color;

	bit_color = (unsigned int)(color->z * 255);
	bit_color |= (unsigned int)(color->y * 255) << 8;
	bit_color |= (unsigned int)(color->x * 255) << 16;
	canvas[y * SCREEN_WIDTH + x] = bit_color;
}

/**
 * createViewVector() creates vector corresponding to position on a screen with
 * given coordinates. Camera is in coordinates (0, 0, 0) and at the start is
 * directed to (0, 0, 1) direction.
 *
 * @param ray place where to store resulted ray
 * @param x position of pixel on the screen
 * @param y position of pixel on the screen
 */
void createViewVector(struct Ray *__restrict ray, short x, short y)
{
	ray->origin = FLOAT3(0, 0, 0);
	x -= SCREEN_WIDTH / 2;
	y -= SCREEN_HEIGHT / 2;
	ray->direction.z = 1;
	ray->direction.x = (float)x / (float)SCREEN_WIDTH;
	ray->direction.y = (float)y / (float)SCREEN_HEIGHT;
	ray->direction = normalize(ray->direction);
}

/**
 * intersectSphere() computes distance to given sphere from ray
 *
 * @param hitDistance place where store distance to sphere from ray
 * @param sphere the sphere to which the distance is calculated
 * @return true if sphere is intersected by ray or false otherwise
 *
 * TODO: optimize
 */
bool intersectSphere(float *__restrict hitDistance,
		     const struct Ray *__restrict ray,
		     sphere_t *__restrict sphere)
{
	float3 oc = ray->origin - sphere->position;

	float a = dot(ray->direction,
		      ray->direction); // ! this always equals to 1
	float b = 2 * dot(oc, ray->direction);
	float c = dot(oc, oc) - square(sphere->radius);

	float discriminant = b * b - 4 * a * c; // ! compute `2 * a` before
	if (discriminant < EPS) {
		return false;
	}

	discriminant = sqrt(discriminant);
	float x1 = (-b + discriminant) / (2 * a); // ! compute `2 * a` before
	float x2 = (-b - discriminant) / (2 * a); // ! compute `2 * a` before

	float closestRoot = min(x1, x2);
	if (closestRoot < EPS) {
		closestRoot = max(x1, x2);
	}
	if (closestRoot < EPS) {
		return false;
	}

	*hitDistance = closestRoot;
	return true;
}

/**
 * intersectAllSpheres() finds closest intersection to spheres in scene with ray
 *
 * @param viewVector the ray with which the intersection is calculated
 * @param hitInfo place where resulter hit info is stored
 * @param spheres array of inspecting spheres
 */
void intersectAllSpheres(const struct Ray *__restrict viewVector,
			 HitInfo *__restrict hitInfo,
			 sphere_t *__restrict spheres)
{
	float closestHit = INFINITY;
	float hitDistance;
	int closestHitId = -1;
	sphere_t *closestSphere;

	for (int i = 0; i < SPHERES_NUM; ++i) {
		if (intersectSphere(&hitDistance, viewVector, &spheres[i])) {
			if (hitDistance < closestHit) {
				closestHit = hitDistance;
				closestHitId = i;
			}
		}
	}
	if (closestHitId == -1) {
		hitInfo->didHit = false;
		return;
	}
	closestSphere = &spheres[closestHitId];
	hitInfo->didHit = true;
	hitInfo->hitColor = closestSphere->color;
	hitInfo->hitPoint =
		viewVector->origin + viewVector->direction * closestHit;
	hitInfo->normal = hitInfo->hitPoint - closestSphere->position;
	hitInfo->normal = normalize(hitInfo->normal); // ! divide by radius
	hitInfo->emissionStrength = closestSphere->emissionStrength;
}

void tracePath(float3 *__restrict incomingLight,
	       struct Ray *__restrict viewVector, sphere_t *__restrict spheres,
	       unsigned int *seed)
{
	float3 rayColor = FLOAT3(1, 1, 1);
	struct HitInfo hitInfo;

	for (int i = 0; i <= TRACE_BOUNCE_COUNT; ++i) {
		intersectAllSpheres(viewVector, &hitInfo, spheres);
		if (!hitInfo.didHit) {
			break;
		}
		float3 emittedLight =
			hitInfo.hitColor * hitInfo.emissionStrength;
		*incomingLight += vec_mul(emittedLight, rayColor);
		rayColor = vec_mul(rayColor, hitInfo.hitColor);

		viewVector->origin = hitInfo.hitPoint;
		randomHemiSphere(&hitInfo.normal, &viewVector->direction, seed);
		// ! dont run last two lines in the last iteration of loop
	}
}

__always_inline void pathTracer(__global unsigned int *canvas,
				__constant struct Sphere *spheres)
{
	struct Ray viewVector;
	const short x = get_global_id(0);
	const short y = get_global_id(1);
	float3 pixelColor = FLOAT3(0, 0, 0);
	unsigned int seed = (x << 12) + y;

	createViewVector(&viewVector, x, y);
	tracePath(&pixelColor, &viewVector, spheres, &seed);
	setPixelColor(canvas, x, y, &pixelColor);
}

__unused __always_inline void testKernel(__global unsigned int *canvas,
					 __constant struct Sphere *spheres)
{
	(void)spheres;
	const short x = get_global_id(0);
	const short y = get_global_id(1);
	float cx = (float)x / (float)SCREEN_WIDTH;
	float cy = (float)y / (float)SCREEN_HEIGHT;
	float c = cx / 2 + cy / 2;
	float3 pixelColor = FLOAT3(c, c, c);
	setPixelColor(canvas, x, y, &pixelColor);
}

__kernel void kernel(__global unsigned int *canvas,
		     __constant struct Sphere *spheres)
{
	pathTracer(canvas, spheres);
}

EXTERN_C_END

#endif /* PATH_TRACER_CL */
