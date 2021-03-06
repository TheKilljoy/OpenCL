__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

__kernel void gaussianBlur(__read_only image2d_t originalPicture, __write_only image2d_t outImage, __global float* mask, __private int maskSize)
{
	int2 pos = {get_global_id(0), get_global_id(1)};
	uint4 temp;
	uint4 outputPixel;
	float4 sum = {0.0f, 0.0f, 0.0f, 255.0f};

	for(int a = -maskSize; a < maskSize + 1; a++) {
		for(int b = -maskSize; b < maskSize + 1; b++){
			temp = read_imageui(originalPicture, sampler, pos + (int2)(a,b));
			sum.s0 += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * temp.s0;
			sum.s1 += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * temp.s1;
			sum.s2 += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * temp.s2;
		}
	}
	
	outputPixel = convert_uint4_sat_rte(sum);
	write_imageui(outImage, pos, outputPixel);
}

__kernel void createMaskParallel(__global float* mask, __global int* sum, __private float sigma, __private int masksize)
{
	int2 pos = (int2)(get_global_id(0), get_global_id(1));
	float temp;
	temp = (float)(exp(-((float)(pos.x * pos.x + pos.y * pos.y) / (2 * sigma * sigma))));
	*sum += temp;
	mask[pos.x + masksize + (pos.y + masksize) * (masksize * 2 + 1)] = temp;
}

/*
__kernel void createMask(__global int* masksize, __global float* mask, __private float sigma)
{
	int size = (int)(ceil(2.57 * sigma));
	float* newMask = new float[(size * 2 + 1) * (size * 2 + 1)];
	float sum = 0.0f;
	float temp;
	for(int x = -size; x < size + 1; x++){
		for (int y = -size; y < size + 1; y++){
			temp = (float)(exp(-((float)(x * x + y * y) / (2 * sigma * sigma)));
			sum += temp;
			newMask[x + size + (y + size) * (size * 2 + 1)] = temp;
		}
	}
	for(int i = 0; i < (size * 2 + 1) * (size * 2 + 1); i++) {
		newMask[i] = newMask[i] / sum;
	}
	*masksize = size;
	mask = newMask;
}
*/