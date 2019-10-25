__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

uchar4 YCrCb2BGRA(const uchar4 pixel)
{
	uchar4 BGRA;
	BGRA.s0 = pixel.s0 + 1.403 * (pixel.s1 - 128);
	BGRA.s1 = pixel.s0 - 0.714 * (pixel.s1 - 128) - 0.344 * (pixel.s2 - 128);
	BGRA.s2 = pixel.s0 + 1.773 * (pixel.s2 - 128);
	BGRA.s3 = pixel.s3;
	return BGRA;
}

__kernel void convertYCrCb2BGRA(__global uchar4* originalPicture, __global uchar4* result, __private int width)
{
	int index = get_global_id(0) +  get_global_id(1) * width;
	uchar4 BGRA = YCrCb2BGRA(originalPicture[index]);
	result[index].s0 = BGRA.s0;
	result[index].s1 = BGRA.s1;
	result[index].s2 = BGRA.s2;
	result[index].s3 = BGRA.s3;
}