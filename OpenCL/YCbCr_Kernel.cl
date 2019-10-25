__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

__kernel void convertRgbToYcbcr(read_only image2d_t originalPicture, write_only image2d_t outImage)
{
	int2 coord = (int2)(get_global_id(0), get_global_id(1));
	uint4 pixel = read_imageui(originalPicture, sampler, coord);
	unsigned int y, cb, cr;

	y = ((int)(65.481 * pixel.s0 + 128.553 * pixel.s1 + 24.966 * pixel.s2) >> 8) + 16;
	cb = ((int)(-37.797 * pixel.s0 - 74.203 * pixel.s1 + 112.0 * pixel.s2) >> 8) + 128;
	cr = ((int)(112.0 * pixel.s0 - 93.786 * pixel.s1 - 18.214 * pixel.s2) >> 8) + 128;
	// y = 0.299 * pixel.s0 + 0.587 * pixel.s1 + 0.114 * pixel.s2;
	// cb = -0.169 * pixel.s0 - 0.331 * pixel.s1 + 0.5 * pixel.s2;
	// cr = 0.5 * pixel.s0 - 0.419 * pixel.s1 - 0.081 * pixel.s2;
	
	pixel.s0 = y;
	pixel.s1 = cb;
	pixel.s2 = cr;

	write_imageui(outImage, coord, pixel);
}