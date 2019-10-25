__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

__kernel void gaussianBlur(read_only image2d_t originalPicture, __global float* outImage, __constant float* mask, __private int masksize)
{
	int2 coord = (int2)(get_global_id(0), get_global_id(1));

	float sum = 0.0f;

	for(int i = -masksize; i < masksize+1; i++) {
		for (int j = -masksize; j < masksize+1; j++) {
			sum += mask[i + masksize + (j + masksize) * (masksize * 2 + 1)] * read_imagef(originalPicture, sampler, coord + (int2)(i, j)).x;
		}
	}

	outImage[coord.x + coord.y * get_global_size(0)] = sum;
}

__kernel void createMaskParallel(__readOnly float sigma, __global int masksize, __global float* mask, __global int* sum)
{
	int2 pos = (int2)(get_global_id(0), get_global_id(1));
	float temp;
	temp = (float)(exp(-((float)(pos.x * pos.x + pos.y * pos.y) / (2 * sigma * sigma))));
	*sum += temp;
	mask[pos.x + masksize + (pos.y + masksize) * (masksize * 2 + 1)] = temp;
}

__kernel void createMask(__readOnly float sigma, __global int* masksize, __global float* mask)
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