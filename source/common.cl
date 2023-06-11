
#ifndef COMMON_CL
# define COMMON_CL

# define EPS 1e-2

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

# ifdef __clcpp__
# define EXTERN_C extern "C" {
# define EXTERN_C_END }
#  include <clcpp.hpp>
#  define vec_mul(a, b) vec_mul_arch(a, b)
# else
#  define FLOAT3(x, y, z) (float3)(x, y, z)
#  define vec_mul(a, b) ((a) * (b))
#  define EXTERN_C
#  define EXTERN_C_END
# endif /* __clcpp__ */

# include <struct.cl>

typedef __constant const struct Sphere sphere_t;

__must_check __inline float square(float x)
{
	return x * x;
}

#endif /* COMMON_CL */