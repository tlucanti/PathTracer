
#ifndef CLCPP_HPP
#define CLCPP_HPP

#include <iostream>
#include <climits>
#include <cmath>

#define __global
#define __kernel
#define __constant

#define FLOAT3(x, y, z) \
	(float3)        \
	{               \
		x, y, z \
	}

using std::max;
using std::min;
using std::sqrt;

struct __dimention_manager {
	int x;
	int y;
};

inline __dimention_manager __dim{ 0, 0 };

__inline unsigned int get_global_id(unsigned int dim)
{
	if (dim == 0) {
		return __dim.x;
	} else if (dim == 1) {
		return __dim.y;
	} else {
		std::abort();
	}
}

struct float3 {
	float x;
	float y;
	float z;

	float3 operator-() const
	{
		return { -x, -y, -z };
	}

	float3 operator+(float3 f) const
	{
		return { x + f.x, y + f.y, z + f.z };
	}

	float3 operator-(float3 f) const
	{
		return { x - f.x, y - f.y, z - f.z };
	}

	float3 operator*(float f) const
	{
		return { x * f, y * f, z * f };
	}

	void operator+=(float3 f)
	{
		x += f.x, y += f.y, z += f.z;
	}

	void operator*=(float f)
	{
		x *= f, y *= f, z *= f;
	}

	float3 mul(float3 f)
	{
		return { x * f.x, y * f.y, z * f.z };
	}
};

__must_check __inline float dot(float3 a, float3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

__must_check __inline float square(float x)
{
	return x * x;
}

__must_check __inline float length(float3 vector)
{
	return sqrt(dot(vector, vector));
}

__must_check __inline float3 normalize(float3 vector)
{
	return vector * (1 / length(vector));
}

__inline float3 vec_mul_arch(float3 a, float3 b)
{
	return a.mul(b);
}

#endif /* CLCPP_HPP */