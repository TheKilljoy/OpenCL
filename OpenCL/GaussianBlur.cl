__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

__kernel void gaussianBlur(read_only image2d_t originalPicture, write_only image2d_t outImage, __global float* mask, __private int maskSize)
{
	int2 pos = {get_global_id(0), get_global_id(1)};
	float4 temp;
	float4 outputPixel;
	float4 sum = {0.0f, 0.0f, 0.0f, 0.0f};

	for(int a = -maskSize; a < maskSize + 1; a++) {
		for(int b = -maskSize; b < maskSize + 1; b++){
			temp = convert_float4(read_imagef(originalPicture, sampler, pos + (int2)(a,b)));
			sum.x += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * temp.x;
			sum.y += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * temp.y;
			sum.z += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * temp.z;
		}
	}
	outputPixel.x = sum.x;
	outputPixel.y = sum.y;
	outputPixel.z = sum.z;
	outputPixel.w = 1.0f;
	write_imagef(outImage, pos, outputPixel);
}

__kernel void gaussianBlurOld(read_only image2d_t originalPicture, write_only image2d_t outImage, __constant float* mask, __private int maskSize)
{
	int2 pos = {get_global_id(0), get_global_id(1)};
	float4 temp;
	float4 outputPixel;

    // Collect neighbor values and multiply with Gaussian for R component
    float sum = 0.0f;
    for(int a = -maskSize; a < maskSize+1; a++) {
        for(int b = -maskSize; b < maskSize+1; b++) {
			temp = convert_float4(read_imagef(originalPicture, sampler, pos + (int2)(a,b)));
            sum += mask[a+maskSize+(b+maskSize)*(maskSize*2+1)]*temp.x;
        }
    }
	outputPixel.x = sum;
	    // Collect neighbor values and multiply with Gaussian for G component
    sum = 0.0f;
    for(int a = -maskSize; a < maskSize+1; a++) {
        for(int b = -maskSize; b < maskSize+1; b++) {
			temp = convert_float4(read_imagef(originalPicture, sampler, pos + (int2)(a,b)));
            sum += mask[a+maskSize+(b+maskSize)*(maskSize*2+1)]*temp.y;
        }
    }
	outputPixel.y = sum;
	    // Collect neighbor values and multiply with Gaussian for B component
    sum = 0.0f;
    for(int a = -maskSize; a < maskSize+1; a++) {
        for(int b = -maskSize; b < maskSize+1; b++) {
			temp = convert_float4(read_imagef(originalPicture, sampler, pos + (int2)(a,b)));
            sum += mask[a+maskSize+(b+maskSize)*(maskSize*2+1)]*temp.z;
        }
    }
	outputPixel.z = sum;
	outputPixel.w = 1.0f;
	write_imagef(outImage, pos, outputPixel);

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