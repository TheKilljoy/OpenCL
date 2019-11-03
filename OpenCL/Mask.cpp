#include "pch.h"
#include "Mask.h"

float* Mask::createBlurMask(const float sigma, int* maskSizePointer) {
	int maskSize = (int)ceil(2.57f * sigma);
	double s = (2 * sigma * sigma);
	float* mask = new float[(maskSize * 2 + 1) * (maskSize * 2 + 1)];
	float dsq;
	float temp;
	float sum = 0.0f;
	for (int a = -maskSize; a < maskSize + 1; a++) {
		for (int b = -maskSize; b < maskSize + 1; b++) {
			dsq = (a * a + b * b);
			temp = exp(-dsq / s) / (M_PI * s);
			sum += temp;
			mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] = temp;
		}
	}
	// Normalize the mask
	for (int i = 0; i < (maskSize * 2 + 1) * (maskSize * 2 + 1); i++)
		mask[i] = mask[i] / sum;

	*maskSizePointer = maskSize;

	return mask;
}