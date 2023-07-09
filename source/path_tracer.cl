
#ifndef PATH_TRACER_CL
#define PATH_TRACER_CL

#include <common.cl>
#include <random.cl>

EXTERN_C

#define TRACE_BOUNCE_COUNT 5

/**
 * rotateVector() function applies rotation to vector using precomputed rotation
 * matri
 *
 * @param vector initial vector, rotated vector will be saved inplace to this
 * 	pointer
 * @param matrix rotation matrix
 */
__always_inline void rotateVector(float3 *__restrict vector,
				  const struct RotateMatrix *__restrict matrix)
{
	float x = dot(matrix->row1, *vector);
	float y = dot(matrix->row2, *vector);
	float z = dot(matrix->row3, *vector);
	*vector = FLOAT3(x, y, z);
}

__always_inline float3 reflectRay(float3 ray, float3 normal)
{
	return normal * (2 * dot(ray, normal)) - ray;
}

/**
 * setPixelColor() sets rgb color of pixel in pixel buffer in given coordinates
 *
 * @param canvas pixel color buffer in rgb format
 * @param x coordinate in color buffer
 * @param y coordinate in color buffer
 * @param color rgb color with float [0, 1] color intensity
 */
__always_inline void setPixelColor(__write_only image2d_t canvas,
				   unsigned short x, unsigned short y,
				   float3 color)
{
	float4 fcolor = FLOAT4(color.x, color.y, color.z, 1);
	int2 coords = INT2(x, SCREEN_HEIGHT - y - 1);

	write_imagef(canvas, coords, fcolor);
}

__always_inline float3 getPixelColor(__read_only image2d_t canvas,
				     unsigned short x, unsigned short y)
{
	int2 coords = INT2(x, SCREEN_HEIGHT - y - 1);
	float4 fcolor = read_imagef(canvas, coords);

	return FLOAT3(fcolor.x, fcolor.y, fcolor.z);
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
void createViewVector(struct Ray *__restrict ray, short x, short y,
		      float3 position,
		      const struct RotateMatrix *__restrict matrix)
{
	float ratio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;

	ray->origin = position;
	x -= SCREEN_WIDTH / 2;
	y -= SCREEN_HEIGHT / 2;
	ray->direction.z = 1;
	ray->direction.x = (float)x * ratio / (float)SCREEN_WIDTH;
	ray->direction.y = (float)y / (float)SCREEN_HEIGHT;
	ray->direction = normalize(ray->direction);
	rotateVector(&ray->direction, matrix);
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
			 struct HitInfo *__restrict hitInfo,
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
	hitInfo->reflective = closestSphere->reflective;
}

float3 skyBoxColor(struct Ray *__restrict viewVector)
{
	float y = viewVector->direction.y;
	float3 beg;
	float3 end;

	const float HORIZON_BORDER = 0.2;

	const float3 SKY_COLOR = FLOAT3(0.56, 0.7, 1.0);
	const float3 HORIZON_COLOR = FLOAT3(0.7, 0.8, 1.0);
	const float3 GROUND_COLOR = FLOAT3(0.7, 0.4, 0.3);

	y = (y + 1) / 2;
	y = square(y);
	if (y > HORIZON_BORDER) {
		y = (y - HORIZON_BORDER) / (1.0 - HORIZON_BORDER);
		beg = HORIZON_COLOR;
		end = SKY_COLOR;
	} else {
		y = y / HORIZON_BORDER;
		beg = GROUND_COLOR;
		end = HORIZON_COLOR;
	}

	return lerp(beg, end, y);
}

float3 sunLight(struct Ray *__restrict viewVector)
{
	float sunPower = dot(SUN_DIRECTION, viewVector->direction);
	float3 sun;
	if (sunPower < 0) {
		sun = BLACK;
	} else {
		sun = sunPower * WHITE;
		sun = pow(sun, 100) * 1e2f;
	}

	return sun;
}

void tracePath(float3 *__restrict incomingLight,
	       struct Ray *__restrict viewVector, sphere_t *__restrict spheres,
	       unsigned int *seed)
{
	float3 rayColor = FLOAT3(1, 1, 1);
	struct HitInfo hitInfo;
	int i;

	for (i = 1; i <= TRACE_BOUNCE_COUNT + 1; ++i) {
		intersectAllSpheres(viewVector, &hitInfo, spheres);
		if (!hitInfo.didHit) {
			break;
		}
		viewVector->origin = hitInfo.hitPoint;
		float3 diffuseDir = normalize(randomHemiSphere(hitInfo.normal, seed) + hitInfo.normal * 0.5f);
		float3 specularDir = reflectRay(-viewVector->direction, hitInfo.normal);

		float3 emittedLight = hitInfo.hitColor * hitInfo.emissionStrength;

		if (hitInfo.specular >= nextRandomFloat(seed)) {
			viewVector->direction = specularDir;
			*incomingLight += emittedLight;
		} else {
			viewVector->direction = normalize(lerp(diffuseDir, specularDir, hitInfo.reflective));
			*incomingLight += vec_mul(emittedLight, rayColor);
			rayColor = vec_mul(rayColor, hitInfo.hitColor);
		}


		// ! dont run last two lines in the last iteration of loop
	}


	float3 sun = sunLight(viewVector);
	float3 sky = skyBoxColor(viewVector);
	*incomingLight += vec_mul(fmin(4.0f, sun + sky), rayColor);

}

__always_inline void pathTracer(__write_only image2d_t canvas,
				__read_only image2d_t c2,
				sphere_t *__restrict spheres, float3 position,
				const struct RotateMatrix *matrix,
				__local float3 *rayBuffer, bool resetCanvas,
				unsigned int frameNumber)
{
	struct Ray viewVector;
	const short x = get_global_id(0);
	const short y = get_global_id(1);
	const short l = get_local_id(2);
	float3 pixelColor = FLOAT3(0, 0, 0);
	float3 prevColor;
	unsigned int seed = (y * 2048) + x + frameNumber * 37421;
	//unsigned int seed = (((l * 512) + y) * 2048 + x) frameNumber;

#if 1
	(void)rayBuffer;

	for (int i = 0; i < RAYS_PER_PIXEL; ++i) {
		createViewVector(&viewVector, x, y, position, matrix);
		tracePath(&pixelColor, &viewVector, spheres, &seed);
	}
	pixelColor *= (float)1.0 / (float)RAYS_PER_PIXEL;

	if (!resetCanvas) {
		prevColor = getPixelColor(c2, x, y);
		float ratio = (float)1.0 / (float)frameNumber;
		pixelColor = prevColor * (1 - ratio) + pixelColor * ratio;
	}
	setPixelColor(canvas, x, y, pixelColor);

#else
	createViewVector(&viewVector, x, y, position, matrix);
	tracePath(&pixelColor, &viewVector, spheres, &seed);
	rayBuffer[l] = pixelColor;
	barrier(CLK_LOCAL_MEM_FENCE);

	if (l == 0) {
		for (int i = 1; i < RAYS_PER_PIXEL; ++i) {
			pixelColor += rayBuffer[i];
		}
		pixelColor *= (float)1.0 / (float)RAYS_PER_PIXEL;
		setPixelColor(canvas, x, y, pixelColor);
	}
#endif
}

__unused __always_inline void
testKernel(__write_only image2d_t canvas, __constant struct Sphere *spheres,
	   float3 position, const struct RotateMatrix *matrix,
	   __local float3 *rayBuffer, int resetCanvas)
{
	(void)spheres;
	(void)rayBuffer;
	(void)resetCanvas;
	const short x = get_global_id(0);
	const short y = get_global_id(1);
	const short l = get_local_id(2);
	struct Ray vec;

	if (l != 0) {
		return;
	}
	createViewVector(&vec, x, y, position, matrix);
	vec.direction.x = fabs(vec.direction.x) * 2;
	vec.direction.y = fabs(vec.direction.y) * 2;
	vec.direction.z = fabs(vec.direction.z) * 0.5;
	setPixelColor(canvas, x, y, vec.direction);
}

__kernel void runKernel(__write_only image2d_t canvas,
			__constant struct Sphere *spheres, float3 position,
			struct RotateMatrix matrix, int resetCanvas,
			__read_only image2d_t c2, unsigned int frameNumber)
{
	__local float3 rayBuffer[RAYS_PER_PIXEL];

	pathTracer(canvas, c2, spheres, position, &matrix, rayBuffer,
		   resetCanvas, frameNumber);
}

EXTERN_C_END

#endif /* PATH_TRACER_CL */
