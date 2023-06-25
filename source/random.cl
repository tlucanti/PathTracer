
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
	prev *= (prev + 195439) * (prev + 124395) * (prev + 845921);
	return prev;
	//return (prev * 0x5DEECE66D + 0xB);
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
 * @param seed used to generate numbers in random number sequence
 *
 * TODO: validate distribution
 * ! update description
 */
__inline static float3 randomDirection(unsigned int *__restrict seed)
{
	float3 dir = FLOAT3(nextRandomFloatNeg(seed),
			    nextRandomFloatNeg(seed),
			    nextRandomFloatNeg(seed));
	return normalize(dir);
}

/**
 * randomHemiSphere() generates 3-dimentional vector randomly directed in
 * hemisphere of radious 1. Hemisphere is oriented using normal vector, so
 * generated vector will be at an angle less then pi / 2 to the normal
 *
 * @param normal orientation of hemisphere direction
 * @param seed used to generate numbers in random number sequence
 *
 * TODO: validate distribution
 * ! update description
 */
__inline static float3 randomHemiSphere(const float3 normal,
				      unsigned int *__restrict seed)
{
	float3 dir = randomDirection(seed);
	if (dot(dir, normal) < 0)
		dir = -dir;
	return dir;
}

EXTERN_C_END

#endif /* RANDOM_CL */