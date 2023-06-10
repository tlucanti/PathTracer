
#ifndef COMMON_CL
# define COMMON_CL

# define EPS 1e-2

# ifndef DEFINED_SCREEN_SIZE
#  define SCREEN_WIDTH 800
#  define SCREEN_HEIGHT 600
# endif

# ifndef __always_inline
#  define __always_inline inline __attribute__((__always_inline__))
# endif
# ifndef __inline
#  define __inline inline
# endif

# ifndef __must_check
#  define __must_check __attribute__((__warn_unused_result__))
# endif

# ifndef __unused
#  define __unused __attribute__((__unused__))
# endif

# ifndef __used
#  define __used __attribute__((__used__))
# endif

# define EXTERN_C extern "C" {
# define EXTERN_C_END }

# ifdef __clcpp__
#  include <clcpp.hpp>
#  define vec_mul(a, b) vec_mul_arch(a, b)
# else
#  define FLOAT3(x, y, z) (float3)(x, y, z)
#  define vec_imul(a, b) ((a) * (b))
# endif /* __clcpp__ */

# define RED FLOAT3(1, 0, 0)
# define GREEN FLOAT3(0, 1, 0)
# define BLUE FLOAT3(0, 0, 1)
# define CYAN FLOAT3(0, 1, 1)
# define PURPLE FLOAT3(1, 0, 1)
# define YELLOW FLOAT3(1, 1, 0)
# define WHITE FLOAT3(1, 1, 1)
# define BLACK FLOAT3(0, 0, 0)

struct Sphere {
	float3 color;
	float3 position;
	float emissionStrength;
	float radius; // ! save squared instead
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

typedef __constant const struct Sphere sphere_t;

#endif /* COMMON_CL */