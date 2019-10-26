__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

__kernel void convertRgbToYcbcr(read_only image2d_t originalPicture, write_only image2d_t outImage)
{
	int2 coord = (int2)(get_global_id(0), get_global_id(1));
	uint4 pixel = read_imageui(originalPicture, sampler, coord);

	uchar r = pixel.s0;
	uchar g = pixel.s1;
	uchar b = pixel.s2;
	uchar y = 0.299 * r + 0.587 * g + 0.114 * b;
	uchar cr = (r - y) * 0.713 + 128;
	uchar cb = (b - y) * 0.564 + 128;
	pixel.s0 = cb;
	pixel.s1 = cr;
	pixel.s2 = y;

	write_imageui(outImage, coord, pixel);
}