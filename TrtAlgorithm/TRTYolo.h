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
		* @brief getDefectName: ����Mes����
		*/
		MesDefectStruct getDefectName();

		/**
		@berif: ����ͼ��Ԥ�������
		@param��mean��batch_size���ֵ
		@param��std��gpuѡ�����
		@param��isNormal�����ͼ����
		*/
		void setMeanSubStd(cv::Scalar mean, cv::Scalar std, bool isNormal = false);

	public:

		void setModelPath(const QString filepath);

		/**
		@berif: ����ģ�Ͳ���
		@function: ����CUDA�ڴ棬ģ�����������¼��
		@param��nMaxBatch��batch_size���ֵ
		@param��nDevice��gpuѡ�����
		@param��nMaxInputSizeW�����ͼ����
		@param��nMaxInputSizeH�����ͼ��߶�
		@param��bDoubInput�����ͼ��߶�
		@param��nStreamNum�����ͼ��߶�
		*/
		CError setModelParam(int nMaxBatch, int nDevice, int nMaxInputSizeW,
			int nMaxInputSizeH, bool bDoubInput, int nStreamNum, int classNum);

		//��ȡ�Կ����������ơ�gpu�����ʡ��Դ�ʹ���ʡ��¶ȵ� by ls 20220620
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

