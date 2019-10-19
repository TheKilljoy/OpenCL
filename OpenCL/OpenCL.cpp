#include "pch.h"
#include "OpenCLProgram.h"
#include "CL/cl.hpp"
#include "Timer.h"

#define buffer (1024)

int main(int argc, char** argv)
{
	float* inData = new float[buffer];
	float* outData = new float[buffer];

	for (size_t i = 0; i < buffer; ++i)
	{
		inData[i] = (float)i;
	}

	Timer timer;
	timer.start("GPU All");

	OpenCLProgram openCLProgram("Square_Kernel.cl", "square");
	openCLProgram.setArg(0, (void*)inData, buffer * sizeof(float), true);
	openCLProgram.setArg(1, (void*)outData, buffer * sizeof(float), false);

	openCLProgram.startProgram(buffer);

	auto result = openCLProgram.getResult(sizeof(float) * buffer);
	timer.stop();

	for (int i = 0; i < result.size(); ++i)
	{
		for (int j = 0; j < buffer; ++j)
		{
			std::cout << ((float*)(result[i]))[j] << '\n';
		}
	}

	// CPU
	timer.start("CPU calculations");
	for (int i = 0; i < buffer; ++i)
	{
		outData[i] = inData[i] * inData[i];
	}
	timer.stop();

	// Beispiel für direktes Anwenden von OpenCL

	/*const char *KernelSource = "\n" \
		"__kernel void square(                          \n" \
		"   __global float* input,                      \n" \
		"   __global float* output)                     \n" \
		"{                                              \n" \
		"   int i = get_global_id(0);                   \n" \
		"   output[i] = input[i] * input[i];            \n" \
		"}                                              \n" \
		"\n";

	cl_int err;

	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	cl::Platform platform = platforms[0];
	
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	cl::Device device = devices[0];

	cl::Context context(device, nullptr, nullptr, nullptr, &err);
	if (err != CL_SUCCESS) std::cout << "ERROR: " << err <<" at " << __LINE__ << '\n';

	cl::Buffer inBuffer(context, CL_MEM_READ_ONLY, buffer * sizeof(float), inData, &err);
	if (err != CL_SUCCESS) std::cout << "ERROR: " << err << " at " << __LINE__ << '\n';
	cl::Buffer outBuffer(context, CL_MEM_WRITE_ONLY, buffer * sizeof(float), outData, &err);
	if (err != CL_SUCCESS) std::cout << "ERROR: " << err << " at " << __LINE__ << '\n';

	cl::Program program(context, KernelSource, true, &err);
	if (err != CL_SUCCESS) std::cout << "ERROR: " << err << " at " << __LINE__ << '\n';

	cl::CommandQueue queue(context);

	queue.enqueueWriteBuffer(inBuffer, true, NULL, buffer * sizeof(float), inData);

	cl::Kernel kernel(program, "square", &err);
	if (err != CL_SUCCESS) std::cout << "ERROR: " << err << " at " << __LINE__ << '\n';

	kernel.setArg(0, inBuffer);
	kernel.setArg(1, outBuffer);

	queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(buffer), cl::NullRange);
	queue.finish();
	queue.enqueueReadBuffer(outBuffer, true, NULL, buffer * sizeof(float), outData);

	for (size_t i = 0; i < buffer; ++i)
	{
		std::cout << outData[i] << '\n';
	}*/

	delete[] inData;
	delete[] outData;

	return 0;
}