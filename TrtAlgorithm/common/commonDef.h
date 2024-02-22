#pragma once

#include <NvInferRuntimeCommon.h>

namespace Trt {

	//显卡显示参数设置
	struct cudaDevicePropCuston
	{
		char  name[256];//显卡名称
		unsigned int nDeviceNum;//显卡数量
		unsigned int nGPUutilization;//GPU利用率,返回：如5，意为5%；
		unsigned int nCudaMemutilization;//显存利用率,返回：如5，意为5%；
		unsigned int nDeviceTemperature;//当前显卡设备温度

		unsigned int nTotalGPUMemory;//设备总共显存
		unsigned int nUsedGpuMemory;//设备使用显存
		unsigned int nFreeGpuMemory;//设备设备
	};

	//日志输出格式设置
	class WLogger : public nvinfer1::ILogger {
	public:

		void setShowName(std::string name) { m_name = name; }

		//虚函数，虚函数使用的其核心目的是通过基类访问派生类定义的函数
		virtual void log(Severity severity, nvinfer1::AsciiChar const* msg) noexcept override {
			//可以通过判断日志的级别来决定打印哪些内容
			std::string level = "";
			switch (severity)
			{
			case Severity::kINTERNAL_ERROR:
				level = "[I " + m_name + "]";
				break;
			case Severity::kERROR:
				level = "[E " + m_name + "]";
				break;
			case Severity::kWARNING:
				level = "[W " + m_name + "]";
				break;
			}
			if (severity <= Severity::kWARNING) {
				std::cout << level.c_str() << ": " << msg << std::endl;
			}
		}

		std::string m_name = "class";
	};
}