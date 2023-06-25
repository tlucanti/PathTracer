
__kernel void runKernel(__global unsigned int *pix)
{
	__local unsigned int buf[8];

	size_t id = get_global_id(1);
	size_t ld = get_local_id(0);
	size_t s = get_local_size(0);

	buf[ld] = ld;

	barrier(CLK_LOCAL_MEM_FENCE);

	unsigned int res = 0;
	if (ld == 0) {
		printf("%d %d %d %d %d %d %d %d\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
		for (int i = 0; i < s; ++i) {
			res += buf[i];
		}
		pix[id] = res;
	}


	printf("global: %lu, local %lu, ls %lu\n", id, ld, s);
	//printf("global 1: %d, global 2: %d, local 1: %d, local 2 %d\n",
	//	get_global_id(0), get_global_id(1), get_local_id(0), get_local_id(1));
}