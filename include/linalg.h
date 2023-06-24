
#ifndef LINALG_H
#define LINALG_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <CL/cl.h>
#include <math.h>

void sincosf(float x, float *sin, float *cos);
#define sincos sincosf

#define PI M_PI

__always_inline float dot(const float3 *a, const float3 *b)
{
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

__always_inline void cross(const float3 *a, const float3 *b, float3 *result)
{
	result->x = a->y * b->z - a->z * b->y;
	result->y = a->z * b->x - a->x * b->z;
	result->x = a->x * b->y - a->y * b->x;
}

__always_inline void vec_iadd(float3 *vector, const float3 *add)
{
	vector->x += add->x;
	vector->y += add->y;
	vector->z += add->z;
}

__always_inline void rotate_vector(float3 *vector, const struct RotateMatrix *matrix)
{
	float x = dot(&matrix->row1, vector);
	float y = dot(&matrix->row2, vector);
	float z = dot(&matrix->row3, vector);

	vector->x = x;
	vector->y = y;
	vector->z = z;
}

void compute_rotation_matrix(struct RotateMatrix *matrix, float alpha,
			     float theta)
{
	float sin_alpha;
	float cos_alpha;
	float sin_theta;
	float cos_theta;

	sincos(alpha, &sin_alpha, &cos_alpha);
	sincos(theta, &sin_theta, &cos_theta);

	matrix->row1 =
		FLOAT3(cos_alpha, sin_alpha * sin_theta, sin_alpha * cos_theta);
	matrix->row2 = FLOAT3(0, cos_theta, -sin_theta);
	matrix->row3 = FLOAT3(-sin_alpha, sin_theta * cos_alpha,
			     cos_alpha * cos_theta);
}

#endif /* LINALG_H */
