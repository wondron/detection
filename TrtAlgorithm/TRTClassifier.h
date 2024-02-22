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
 * @1: ʹ�ô���ͼ�������RGB��ʽ��
 * @2: ���裺�½�����---���ò���----ģ�Ͷ�ȡ----ͼ������
 * @2��ʹ��ʾ����
 * 	using namespace std;
	Trt::TRTClassifier clas("clssifer", "����ģ��");
	vector<string> outputName = { "output", "532" };

	clas.setModelInputOutput("input", outputName);
	clas.m_getCheck->setValue(1);
	clas.m_classNum->setValue(6);

	clas.loadModel("D:/01-detection/VisualInspection/images/fbf/fbfClassifier.engine");
	clas.setModelParam();
	cv::Mat img;
	img = cv::imread("D:/01-detection/VisualInspection/images/fbf/0.bmp");
	float confident;
	int classIndex;
	CError err = clas.detect(img, confident, classIndex);
	cout << "confident: " << confident << "  " << classIndex;
 *
 ***************************************************************************
 */

#include <opencv2/core.hpp>
#include "comDef.h"
#include <NvInferRuntime.h>
#include <cnode.h>
#include "common/commonDef.h"

namespace Trt
{
	class TRTClassifierPrivate;

	class __declspec(dllexport)TRTClassifier : public Node::CGroup
	{
	public:
		explicit TRTClassifier(std::string name, std::string des, CNode* parent = nullptr);

		~TRTClassifier();

	public:
		CError detect(const cv::Mat& input, float& confidence, int& classIdx);  //�ӿں���

	public:
		//loadModel
		CError loadModel(const std::string & modelPath);

		void setMeanSubStd(const cv::Scalar mean, const cv::Scalar std);

		void setModelInputOutput(const std::string& inputName, const std::vector<std::string>& outputName);

		CError setModelParam();

	private:
		//ͼ��Ԥ����
		CError preProImage(const cv::Mat& input, float* pfInputDevBuffer);

		CError preProImageMtd0(const cv::Mat& input, float* pfInputDevBuffer);

		//ͼ������

		CError inferImage(const cv::Mat& input, float& confidence, int& classIdx);

		CError inferImageMtd0(const cv::Mat& input, float& confidence, int& classIdx);

	public:
		Node::CBool* m_getCheck;
		Node::CInt* m_imgPred;
		Node::CInt* m_inferMtd;
		Node::CInt* m_deviceIndex;
		Node::CInt* m_batchSize;
		Node::CInt* m_inputSizeW;
		Node::CInt* m_inputSizeH;
		Node::CInt* m_classNum;

	private:
		TRTClassifierPrivate* d;
		bool m_bHasDevice = false;
	};
}