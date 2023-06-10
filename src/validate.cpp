
#include <source/random.cl>

EXTERN_C

struct float_seed {
	float number;
	unsigned int seed;
};

struct float3_seed {
	float x;
	float y;
	float z;
	unsigned int seed;
};

__used int _call_nextRandomInt(unsigned int seed)
{
	return nextRandomInt(seed);
}

__used struct float_seed _call_nextRandomFloat(unsigned int seed)
{
	float number = nextRandomFloat(&seed);
	return { .number = number, .seed = seed };
}

__used struct float_seed _call_nextRandomFloatNeg(unsigned int seed)
{
	float number = nextRandomFloatNeg(&seed);
	return { .number = number, .seed = seed };
}

__used struct float3_seed _call_randomDirection(unsigned int seed)
{
	float3 vector;
	randomDirection(&vector, &seed);
	return { .x = vector.x, .y = vector.y, .z = vector.z, .seed = seed };
}

__used struct float3_seed _call_randomHemiSphere(unsigned int seed, float nx,
						float ny, float nz)
{
	float3 vector;
	float3 normal = FLOAT3(nx, ny, nz);
	randomHemiSphere(&normal, &vector, &seed);
	return { .x = vector.x, .y = vector.y, .z = vector.z, .seed = seed };
}

EXTERN_C_END
