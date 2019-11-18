__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

__kernel void coolTest(__read_only image2d_t org, __write_only image2d_t out)
{
	int2 pos = (int2)(get_global_id(0), get_global_id(1));

	write_imagef(out, pos, (float4)(0.0f, 0.0f, 0.0f, 0.0f));
}