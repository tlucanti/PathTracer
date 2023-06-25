
#ifndef STRUCT_CL
# define STRUCT_CL

struct RotateMatrix {
	float3 row1;
	float3 row2;
	float3 row3;
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

struct Sphere {
	float3 color;
	float3 position;
	float emissionStrength;
	float radius; // ! save squared instead
};

# define RED FLOAT3(1, 0, 0)
# define GREEN FLOAT3(0, 1, 0)
# define BLUE FLOAT3(0, 0, 1)
# define CYAN FLOAT3(0, 1, 1)
# define PURPLE FLOAT3(1, 0, 1)
# define YELLOW FLOAT3(1, 1, 0)
# define WHITE FLOAT3(1, 1, 1)
# define BLACK FLOAT3(0, 0, 0)

# define _LIGHTNESS 0.3
# define LRED FLOAT3(1, _LIGHTNESS, _LIGHTNESS)
# define LGREEN FLOAT3(_LIGHTNESS, 1, _LIGHTNESS)
# define LBLUE FLOAT3(_LIGHTNESS, _LIGHTNESS, 1)
# define LPURPLE FLOAT3(1, _LIGHTNESS, 1)
# define LYELLOW FLOAT3(1, 1, _LIGHTNESS)
# define GREY FLOAT3(_LIGHTNESS, _LIGHTNESS, _LIGHTNESS)

#endif /* STRUCT_CL */
