__kernel void convertBGR2YCrCb(__global uchar* originalPicture, __global uchar* result, __private int width)
{
	//Index eines 2D-Arrays in einem 1D array berechnen.
	//Beachte: Ein Pixel besteht aus 3 Bytes, daher x * 3.
	//OpenCV Reihenfolge der Farben ist BGR.
	int index = get_global_id(0) * 3 + get_global_id(1) * width;
	result[index + 0] = 0.299 * originalPicture[index + 2] + 0.587 * originalPicture[index + 1] + 0.114 * originalPicture[index];
	result[index + 1] = (originalPicture[index + 2] - result[index]) * 0.713 + 128;
	result[index + 2] = (originalPicture[index] - result[index]) * 0.564 + 128;
}