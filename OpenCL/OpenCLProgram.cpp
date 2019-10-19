#include "pch.h"
#include "OpenCLProgram.h"
#include <CL/cl.hpp>
#include "Timer.h"

// source: https://stackoverflow.com/questions/3692954/add-custom-messages-in-assert
#ifndef NDEBUG
#   define M_Assert(Expr, Msg) \
    __M_Assert(#Expr, Expr, __FILE__, __LINE__, Msg)
#else
#   define M_Assert(Expr, Msg) ;
#endif

void __M_Assert(const char* expr_str, bool expr, const char* file, int line, const char* msg)
{
	if (!expr)
	{
		std::cerr << "Assert failed:\t" << msg << '\n'
			<< "Expected:\t" << expr_str << '\n'
			<< "Source:\t\t" << file << ", line " << line << '\n';
		abort();
	}
}

void internalOpenCLError(const char* errorInfo, const void* privateInfo, size_t cb, void* userData)
{
	printf_s("ERROR: %sSource: %s, line %d\n", errorInfo, __FILE__, __LINE__);
}


OpenCLProgram::OpenCLProgram(const std::string & filepath, const char* kernelName)
{
	timer = std::make_unique<Timer>();
	const std::string* kernelContent = loadKernelFile(filepath);
	init(kernelContent, kernelName);
	delete kernelContent;
	std::cout << "SUCCESS\n";
}

OpenCLProgram::~OpenCLProgram()
{
}

void OpenCLProgram::startProgram(unsigned int count) const
{
	timer->start("GPU calculations");
	m_Queue->enqueueNDRangeKernel(*m_Kernel, cl::NullRange, cl::NDRange(count), cl::NullRange);
}

std::vector<void*> OpenCLProgram::getResult(int size) const
{
	m_Queue->finish();
	timer->stop();
	std::vector<void*> out;
	for (int i = 0; i < m_OutBuffers.size(); ++i)
	{
		m_Queue->enqueueReadBuffer(*m_OutBuffers[i].first, true, NULL, size, m_OutBuffers[i].second);
		out.push_back(m_OutBuffers[i].second);
	}
	return out;
}

const std::string* OpenCLProgram::loadKernelFile(const std::string & filepath) const
{
	std::string* buffer = new std::string();
	std::ifstream f(filepath, std::ifstream::binary);
	if (!f.is_open())
	{
		std::cout << "ERROR: Couldn't load kernel file\n";
		std::exit(1);
	}
	f.seekg(0, std::ios::end);
	buffer->resize(f.tellg());
	f.seekg(0);
	f.read((char*)(buffer->data()), buffer->size());
	f.close();
	return buffer;
}

const void OpenCLProgram::init(const std::string* kernelContent, const char* kernelName)
{
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	M_Assert(platforms.size() > 0, "ERROR: No valid OpenCL Platform found");
	m_Platform = std::make_unique<cl::Platform>(platforms[0]);

	std::vector<cl::Device> devices;
	m_Platform->getDevices(CL_DEVICE_TYPE_GPU, &devices);
	M_Assert(devices.size() > 0, "ERROR: No valid OpenCL GPU Device found");
	m_Device = std::make_unique<cl::Device>(devices[0]);

	cl_int error;
	std::string errorMsg;

	m_Context = std::make_unique<cl::Context>(*m_Device, nullptr, internalOpenCLError, nullptr, &error);
	errorMsg = "ERROR: Couldn't create an OpenCL Context. Errorcode: " + std::to_string(error);
	M_Assert(error == CL_SUCCESS, errorMsg.c_str());

	m_Program = std::make_unique<cl::Program>(*m_Context, *kernelContent, true, &error);
	errorMsg = "ERROR: Couldn't compile OpenCL Program. Errorcode: " + std::to_string(error) + '\n' + m_Program->getBuildInfo<CL_PROGRAM_BUILD_LOG>(*m_Device);
	M_Assert(error == CL_SUCCESS, errorMsg.c_str());

	m_Kernel = std::make_unique<cl::Kernel>(*m_Program, kernelName, &error);
	errorMsg = "ERROR: Couldn't create OpenCL Kernel. Errorcode: " + std::to_string(error);
	M_Assert(error == CL_SUCCESS, errorMsg.c_str());

	m_Queue = std::make_unique<cl::CommandQueue>(*m_Context);
}

void OpenCLProgram::setArg(int index, int arg) const
{
	m_Kernel->setArg(index, arg);
}

void OpenCLProgram::setArg(const int index, void* buffer, size_t size, bool readOnly)
{
	cl_int error;
	std::string errorMsg;

	if (readOnly)
	{
		m_ReadOnlyBuffers.push_back(std::make_unique<cl::Buffer>(*m_Context, CL_MEM_READ_ONLY, size, buffer, &error));
		m_Queue->enqueueWriteBuffer(*m_ReadOnlyBuffers.back(), true, 0, size, buffer);
		errorMsg = "ERROR: Couldn't create OpenCL Buffer. Errorcode: " + std::to_string(error);
		M_Assert(error == CL_SUCCESS, errorMsg.c_str());
		m_Kernel->setArg(index, *m_ReadOnlyBuffers.back());
	}
	else
	{
		m_OutBuffers.push_back(
			std::make_pair(std::make_unique<cl::Buffer>(*m_Context, CL_MEM_WRITE_ONLY, size, buffer, &error), buffer));
		errorMsg = "ERROR: Couldn't create OpenCL Buffer. Errorcode: " + std::to_string(error);
		M_Assert(error == CL_SUCCESS, errorMsg.c_str());
		m_Kernel->setArg(index, *m_OutBuffers.back().first);
	}
}