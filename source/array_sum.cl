
__kernel void array_sum(__global int *a, __global int *b)
{
	int i = get_global_id(0);
	a[i] += b[i];
}
