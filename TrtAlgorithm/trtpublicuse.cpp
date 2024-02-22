#include "pch.h"
#include "trtpublicuse.h"
#include <cstdlib>
#include <nvml.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace Trt;

void TRTPublicUse::setCudaLazy()
{
    char* env = nullptr;
    size_t len = 0;

    // ʹ�� _dupenv_s ��� getenv
#ifdef _WIN32
    if (_dupenv_s(&env, &len, "CUDA_MODULE_LOADING") == 0 && env == nullptr){
        _putenv("CUDA_MODULE_LOADING=LAZY");
        free(env);
    }
#else
    env = getenv("CUDA_MODULE_LOADING");
    if (env == nullptr)
        putenv("CUDA_MODULE_LOADING=LAZY");
#endif
}

CError TRTPublicUse::getDeviceInfo(std::vector<cudaDevicePropCuston>& deviceInfo)
{
	deviceInfo.clear();
	nvmlDevice_t device;   //GPU�豸����
	nvmlUtilization_t utilization;
	cudaDevicePropCuston singleInfo;
	unsigned int device_count, nTemperature;

	try {
		nvmlReturn_t result = nvmlInit(); //��ʼ��
		result = nvmlDeviceGetCount(&device_count);
		if (NVML_SUCCESS != result)
			return CError(NG, u8"��ȡGPU����ʧ�ܣ�");

		for (int i = 0; i < device_count; i++) {
			result = nvmlDeviceGetHandleByIndex(i, &device);

			if (NVML_SUCCESS != result)
				return CError(NG, u8"��ȡGPU��Ϣʧ�ܣ�");

			//��ȡ����
			result = nvmlDeviceGetName(device, singleInfo.name, NVML_DEVICE_NAME_BUFFER_SIZE);

			//ʹ����
			result = nvmlDeviceGetUtilizationRates(device, &utilization);

			//�¶�
			nTemperature = -1;
			result = nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &nTemperature);

			//��ȡ�����Դ���Ϣ
			nvmlMemory_t memory;
			result = nvmlDeviceGetMemoryInfo(device, &memory);

			//��ֵ
			singleInfo.nDeviceNum = i;
			singleInfo.nGPUutilization = utilization.gpu;
			singleInfo.nCudaMemutilization = utilization.memory;
			singleInfo.nDeviceTemperature = nTemperature;
			singleInfo.nTotalGPUMemory = memory.total / 1024 / 1024;
			singleInfo.nUsedGpuMemory = memory.used / 1024 / 1024;
			singleInfo.nFreeGpuMemory = memory.free / 1024 / 1024;
			deviceInfo.push_back(singleInfo);
		}

		cout << "GPU��Ϣ��ȡ��ɣ�����Ϊ��" << deviceInfo.size() << endl;

		return 0;
	}
	catch (...) {
		return CError(1, "GPU��Ϣ��ȡʧ�ܣ�");
	}
}

CError TRTPublicUse::mat2ptr(const std::vector<cv::Mat>& vecMat, const cv::Size& imgSize, const cv::Scalar& mean, const cv::Scalar& std, float* data)
{
	try
	{
		if (vecMat.size() == 0)
			return CError(EMPTYOBJ, "TRTPublicUse::mat2ptr������ͼ��Ϊ�շ���NG!");

		if (data == nullptr)
			return CError(EMPTYOBJ, "TRTPublicUse::mat2ptr������ָ��δ�����ڴ�NG!");

		int h = imgSize.height;
		int w = imgSize.width;
		const int inputC = vecMat[0].channels();
		const int volChl = h * w;
		const int volImg = inputC * volChl;
		
		for (int i = 0; i < vecMat.size(); i++) {
			cv::Mat src = vecMat[i];
			cv::resize(src, src, imgSize);
			src.convertTo(src, CV_32FC3);
			src /= 255.0;
			cv::subtract(src, mean, src, cv::noArray(), CV_32F);
			cv::divide(src, std, src, 1, CV_32F);

			cv::Mat rgb[3] = {
				cv::Mat(h,w,CV_32FC1,(void*)(data + i * volImg + 0 * volChl)),
				cv::Mat(h,w,CV_32FC1,(void*)(data + i * volImg + 1 * volChl)),
				cv::Mat(h,w,CV_32FC1,(void*)(data + i * volImg + 2 * volChl)),
			};
			cv::split(src, rgb);
		}

		return 0;
	}
	catch (const std::exception& e)
	{
		return CError(UNEXCEPTION, e.what());
	}
}

CError TRTPublicUse::standardImage(const std::vector<cv::Mat>& vecMat, const cv::Scalar& mean, const cv::Scalar& std, std::vector<cv::Mat>& outputVecMat)
{
	try
	{
		if (vecMat.size() == 0)
			return CError(EMPTYOBJ, "SegmentTRT::preImageSegformer��ʼ������ͼƬΪ�գ�");

		outputVecMat.clear();

		for (int i = 0; i < vecMat.size(); i++) {
			cv::Mat src = vecMat[i];
			src.convertTo(src, CV_32FC3);
			cv::subtract(src, mean, src, cv::noArray(), CV_32FC3);
			cv::divide(src, std, src, 1, CV_32FC3);
			outputVecMat.push_back(src);
		}

		return 0;
	}
	catch (std::exception& e)
	{
		return CError(UNEXCEPTION, e.what());
	}
}
