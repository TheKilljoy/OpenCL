#pragma once

class Mask
{
public:
	Mask();
	~Mask();
	static float* createBlurMask(const float sigma, int* maskSizePointer);
};