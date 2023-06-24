
#ifndef CLCPP_HPP
#define CLCPP_HPP

#include <iostream>
#include <climits>
#include <cmath>

#define __global
#define __kernel
#define __constant
#define write_only

#define FLOAT3(x, y, z) (float3){x, y, z}
#define FLOAT4(x, y, z, w) (float4){x, y, z, w};
#define INT2(x, y) (int2){x, y};

typedef unsigned int *image2d_t;

using std::max;
using std::min;
using std::sqrt;
using std::abs;

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

struct float4 {
	float x;
	float y;
	float z;
	float w;
};

struct int2 {
	int x;
	int y;
};

__must_check __inline float dot(float3 a, float3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
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

__inline void write_imagef(image2d_t canvas, const int2 coords, const float4 fcolor)
{
	unsigned int blue = (int)(fcolor.z * 255);
	unsigned int green = (int)(fcolor.y * 255);
	unsigned int red = (int)(fcolor.x * 255);

	blue = min(255u, blue);
	green = min(255u, green) << 8;
	red = min(255u, red) << 16;

	canvas[coords.y * SCREEN_WIDTH + coords.x] = red | green | blue;
}

#endif /* CLCPP_HPP */