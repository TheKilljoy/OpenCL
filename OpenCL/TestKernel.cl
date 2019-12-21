__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

__kernel void coolTest(__read_only image2d_t org, __write_only image2d_t out)
{
	int2 pos = (int2)(get_global_id(0), get_global_id(1));
	float4 outPixel = read_imagef(org, sampler, pos);
	outPixel.x += 30.0f;

	//float4 outPixel = {0.0f, 0.0f, 0.0f, 0.0f};
	//outPixel.s0 = 0.0f;
	//outPixel.s1 = 255.0f;
	//outPixel.s2 = 0.0f;
	//outPixel.s3 = 1.0f;

	write_imagef(out, pos, outPixel);
}

__kernel void someFunction(__read_only image2d_t org, __write_only image2d_t out)
{
	int2 pos = (int2)(get_global_id(0), get_global_id(1));
	float4 outPixel = read_imagef(org, sampler, pos);
	outPixel.s2 += 50.0f;
	write_imagef(out, pos, outPixel);
}