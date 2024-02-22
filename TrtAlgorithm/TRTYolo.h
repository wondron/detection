#pragma once

#include "PPublicUse.h"
#include "includeHeaderFile"
#include "PSLDefine.h"
#include "commonUsage/commonDef.h"
#include "commonUsage/cerror.h"
#include <opencv2/core.hpp>
#include "comDef.h"

namespace PSL
{
	class TRTYoloPrivate;

	class TRTYolo
	{
	public:
		TRTYolo();

		~TRTYolo();

		void setTaskName(const QString& task);
		const QString getTaskName() { return m_taskName; }

		CError pamRead(QMap<QString, QMap<QString, QVariant>>& pamMap,
			QMap<QString, QMap<QString, QVariant>>& prodMap);

		/**
		* @brief getDefectName: 返回Mes参数
		*/
		MesDefectStruct getDefectName();

		/**
		@berif: 设置图像预处理参数
		@param：mean，batch_size最大值
		@param：std，gpu选择参数
		@param：isNormal，最大图像宽度
		*/
		void setMeanSubStd(cv::Scalar mean, cv::Scalar std, bool isNormal = false);

	public:

		void setModelPath(const QString filepath);

		/**
		@berif: 设置模型参数
		@function: 申请CUDA内存，模型输入输出记录。
		@param：nMaxBatch，batch_size最大值
		@param：nDevice，gpu选择参数
		@param：nMaxInputSizeW，最大图像宽度
		@param：nMaxInputSizeH，最大图像高度
		@param：bDoubInput，最大图像高度
		@param：nStreamNum，最大图像高度
		*/
		CError setModelParam(int nMaxBatch, int nDevice, int nMaxInputSizeW,
			int nMaxInputSizeH, bool bDoubInput, int nStreamNum, int classNum);

		//获取显卡数量、名称、gpu利用率、显存使用率、温度等 by ls 20220620
		CError getDeviceInfo(std::vector<TRTDTCT::cudaDevicePropCuston>& deviceInfo);

	public:
		CError detect(const std::vector<SegMatData>& imgs, std::vector<YoloResData>& result);

	private:
		CError initialModel();

		CError preImage(cv::Mat src, float* data);

		CError preImageMtd0(cv::Mat src, float* data);

		CError detectSingle(const cv::Mat input, std::vector<YoloResData>& result);

	private:
		QString m_taskName = "TRTYolo";
		TRTYoloPrivate* d;
		bool hasDevice = false;
		QString m_modelPath = "";
	};
}

