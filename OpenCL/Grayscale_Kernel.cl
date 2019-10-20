
__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

__kernel void grayScale(read_only image2d_t originalPicture, write_only image2d_t outImage)
{
	int2 coord = (int2)(get_global_id(0), get_global_id(1));
	uint4 pixel = read_imageui(originalPicture, sampler, coord);

	unsigned char grayColor = pixel.s0 * 0.07f + pixel.s1 * 0.72f + pixel.s2 * 0.21f;
	pixel.s0 = grayColor;
	pixel.s1 = grayColor;
	pixel.s2 = grayColor;

	write_imageui(outImage, coord, pixel);
}