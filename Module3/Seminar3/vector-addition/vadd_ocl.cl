// OpenCL kernel: compute sum of 3 vectors
// Will be executed on OpenCL device(s) within a context
__kernel void add(const int size, const __global int *v1, const __global int *v2, __global int *v3) {
	const int globalIndex = get_global_id(0);   
	v3[globalIndex] = v1[globalIndex] + v2[globalIndex];
}
