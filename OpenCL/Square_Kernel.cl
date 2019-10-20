__kernel void square(__constant float* input, __global float* output)
{
	int i = get_global_id(0);
	output[i] = input[i] * input[i];
}