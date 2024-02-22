#pragma once

#include <vector>
#include <cerror.h>
#include <opencv2/core.hpp>
#include "common/commonDef.h"

namespace Trt {

	class TRTPublicUse
	{ 
	public:
		static void setCudaLazy();

		static CError getDeviceInfo(std::vector<cudaDevicePropCuston>& deviceInfo);

		static CError mat2ptr(const std::vector<cv::Mat>& vecMat, const cv::Size& imgSize, const cv::Scalar& mean, const cv::Scalar& std, float* data);

		static CError standardImage(const std::vector<cv::Mat>& vecMat, const cv::Scalar& mean, const cv::Scalar& std, std::vector<cv::Mat>& outputVecMat);
	};

}

