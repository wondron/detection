#pragma once

/*!
 ***************************************************************************
 * Copyright (C) 2024
 *
 * \file       cnode.h
 * \brief
 * \details
 *
 * \author     wondron
 * \version    v1.0.0
 * \date       2024-01-23
 ***************************************************************************
 * revision v1.0.0 :
 * 
 ***************************************************************************
 */

#include <opencv2/opencv.hpp>
#include "comDef.h"
#include <NvInferRuntime.h>
#include <cnode.h>
#include "common/commonDef.h"

namespace Trt
{
	class TRTSegmentPrivate;

	class __declspec(dllexport)TRTSegment : public Node::CGroup
	{
	public:
		explicit TRTSegment(std::string name, std::string des, CNode* parent = nullptr);

		~TRTSegment();

	public:
		//步骤1，2，3
		CError loadModel(const std::string& modelPath);

		CError setModelParam();

		CError detect(const std::vector<cv::Mat>& inputImg, std::vector<cv::Mat>& maskImg);  //接口函数

	public:
		void setMeanSubStd(const cv::Scalar mean, const cv::Scalar std);

		void setModelInputOutput(const std::string& inputName, const std::vector<std::string>& outputName);

	private:
		CError splitVector(std::vector<cv::Mat> vecMat, std::vector<std::vector<cv::Mat>>& outputMat);

		CError convertToMat(float* data, cv::Mat& outputMat, int imgWidth, int imgHeight, int memSize);

		//图像预处理
		CError preProImage(const std::vector<cv::Mat>& input, float* pfInputDevBuffer);

		CError inferImage(const std::vector<cv::Mat>& input, std::vector<cv::Mat>& resImage);

		CError inferImageMtd0(const std::vector<cv::Mat>& input, std::vector<cv::Mat>& resImage);

	public:
		Node::CBool* m_getCheck;
		Node::CInt * m_imgPred; 
		Node::CInt* m_inferMtd;
		Node::CInt * m_batchSize;
		Node::CInt * m_deviceIndex;

	private:
		TRTSegmentPrivate* d;
		bool m_bHasDevice = false;
	};
}