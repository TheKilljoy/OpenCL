__kernel void convertYCrCb2BGR(__global uchar* originalPicture, __global uchar* result, __private int width)
{
	int index = get_global_id(0) * 3 +  get_global_id(1) * width;
	result[index + 2] = originalPicture[index] + 1.403 * (originalPicture[index + 1] - 128);
	result[index + 1] = originalPicture[index] - 0.714 * (originalPicture[index + 1] - 128) - 0.344 * (originalPicture[index + 2] - 128);
	result[index]     = originalPicture[index] + 1.773 * (originalPicture[index + 2] - 128);
}