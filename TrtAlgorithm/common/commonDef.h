#pragma once

#include <NvInferRuntimeCommon.h>

namespace Trt {

	//�Կ���ʾ��������
	struct cudaDevicePropCuston
	{
		char  name[256];//�Կ�����
		unsigned int nDeviceNum;//�Կ�����
		unsigned int nGPUutilization;//GPU������,���أ���5����Ϊ5%��
		unsigned int nCudaMemutilization;//�Դ�������,���أ���5����Ϊ5%��
		unsigned int nDeviceTemperature;//��ǰ�Կ��豸�¶�

		unsigned int nTotalGPUMemory;//�豸�ܹ��Դ�
		unsigned int nUsedGpuMemory;//�豸ʹ���Դ�
		unsigned int nFreeGpuMemory;//�豸�豸
	};

	//��־�����ʽ����
	class WLogger : public nvinfer1::ILogger {
	public:

		void setShowName(std::string name) { m_name = name; }

		//�麯�����麯��ʹ�õ������Ŀ����ͨ��������������ඨ��ĺ���
		virtual void log(Severity severity, nvinfer1::AsciiChar const* msg) noexcept override {
			//����ͨ���ж���־�ļ�����������ӡ��Щ����
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