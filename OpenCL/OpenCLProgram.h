#pragma once

namespace cl
{
	class Platform;
	class Device;
	class Context;
	class Program;
	class Kernel;
	class CommandQueue;
	class Buffer;
	class Image2D;
	class NDRange;
}

class Timer;

class FileWriter;

//useful infos:
//opencl errors: https://streamhpc.com/blog/2013-04-28/opencl-error-codes/
//opencl 1.2 reference card (C): https://www.khronos.org/registry/OpenCL/sdk/1.2/docs/OpenCL-1.2-refcard.pdf
//opencl 1.2 reference card (C++): https://www.khronos.org/files/OpenCLPP12-reference-card.pdf
//opencl 1.2 c++ wrapper specs: https://www.khronos.org/registry/OpenCL/specs/opencl-cplusplus-1.2.pdf

class OpenCLProgram
{
public:
	OpenCLProgram(const std::string& filepath, const char* kernelName);
	~OpenCLProgram();

	void setArg(int index, int arg) const;
	void setArg(const int index, void* buffer, size_t size, bool readOnly);
	void setImage2D(const int index, const size_t width, const size_t height, const size_t pixelSize, const unsigned char* imageData, bool readOnly);
	void startProgram(const cl::NDRange& global) const;
	void startProgram(const cl::NDRange& local, const cl::NDRange& global) const;
	std::vector<void*> getResults(FileWriter& fWriter, const int row, const int col) const;

private:
	const std::string* loadKernelFile(const std::string& filepath) const;
	const void init(const std::string* kernelContent, const char* kernelName);

private:
	std::unique_ptr<cl::Platform> m_Platform;
	std::unique_ptr<cl::Device> m_Device;
	std::unique_ptr<cl::Context> m_Context;
	std::unique_ptr<cl::Program> m_Program;
	std::unique_ptr<cl::Kernel> m_Kernel;
	std::unique_ptr<cl::CommandQueue> m_Queue;
	std::vector<std::pair<std::unique_ptr<cl::Buffer>, void*>> m_OutBuffers;
	std::vector<std::unique_ptr<cl::Buffer>> m_ReadOnlyBuffers;
	std::vector<size_t> outputSizes;
	std::vector<std::pair<std::unique_ptr<cl::Image2D>, void*>> m_OutImages2D;
	std::vector<std::unique_ptr<cl::Image2D>> m_ReadOnlyImages2D;
	std::vector<int> outputImgDimensions;
	std::unique_ptr<Timer> timer;
};

