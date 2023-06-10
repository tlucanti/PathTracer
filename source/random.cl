
#ifndef RANDOM_CL
#define RANDOM_CL

#include <common.cl>

EXTERN_C

#ifndef RAND_MAX
#define RAND_MAX UINT_MAX
#endif /* RAND_MAX */
#define RAND_HALF (UINT_MAX / 2)

/**
 * nextRandomInt() generates integer number sequence in uniform distribution
 *
 * @param prev previous generated number. In first function call prev can be
 * 	used as seen
 * @return next random integer using previous generatee number
 *
 * TODO: validate distribution
 */
__always_inline __must_check static unsigned int
nextRandomInt(unsigned int prev)
{
	return prev * 0x5DEECE66D + 0xB;
}

/**
 * nextRandomFloat() generates float number sequence in uniform distribution
 * in range [0, 1]
 *
 * @param seed used to generate next random number. Seed is modifyed after
 * 	every call of the function. In first function call *seed value need
 * 	to be set to random value to get different sequence each time
 * @return next random float in range [0, 1] from previous generated number
 *
 * TODO: validate distribution
 */
__always_inline __must_check static float
nextRandomFloat(unsigned int *__restrict seed)
{
	*seed = nextRandomInt(*seed);
	return (float)(*seed) / (float)RAND_MAX;
}

/**
 * nextRandomFloatNeg() generates float number sequence in uniform distribution
 * in range [-1, 1]. Function works like a `nextRandomFloat` function
 *
 * @param seed used to generate next random number in sequence
 * @return next random float in range [-1, 1] from previous generated number
 *
 * TODO: validate distribution
 */
__always_inline __must_check static float
nextRandomFloatNeg(unsigned int *__restrict seed)
{
	*seed = nextRandomInt(*seed);
	return (float)(*seed) / (float)RAND_HALF - (float)1;
}

/**
 * randomDirection() generates 3-dimentional vector randomly directed in
 * sphere of radius 1. Distribution of direction is close to uniform, but with
 * small irregularites due to mapping vector from random cube to sphere.
 *
 * @param direction vector where to save result
 * @param seed used to generate numbers in random number sequence
 *
 * TODO: validate distribution
 */
__inline static void randomDirection(float3 *__restrict direction,
				     unsigned int *__restrict seed)
{
	direction->x = nextRandomFloatNeg(seed);
	direction->y = nextRandomFloatNeg(seed);
	direction->z = nextRandomFloatNeg(seed);
	*direction = normalize(*direction);
}

/**
 * randomHemiSphere() generates 3-dimentional vector randomly directed in
 * hemisphere of radious 1. Hemisphere is oriented using normal vector, so
 * generated vector will be at an angle less then pi / 2 to the normal
 *
 * @param normal orientation of hemisphere direction
 * @param direction vector where to save result
 * @param seed used to generate numbers in random number sequence
 *
 * TODO: validate distribution
 */
__inline static void randomHemiSphere(const float3 *__restrict normal,
				      float3 *__restrict direction,
				      unsigned int *__restrict seed)
{
	randomDirection(direction, seed);
	if (dot(*direction, *normal) < 0)
		*direction = -(*direction);
}

EXTERN_C_END

#endif /* RANDOM_CL */