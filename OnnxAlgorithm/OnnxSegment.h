#pragma once

#include <cnode.h>
#include <opencv2/opencv.hpp>

namespace Onnx
{
	class OnnxSegmentPrivate;

	class __declspec(dllexport)OnnxSegment : public Node::CGroup
	{
	public:
		explicit OnnxSegment(std::string name, std::string des, CNode* parent = nullptr);
		~OnnxSegment();

	public:
		CError loadModel(const std::string& modelPath);

		CError setModelParam();

	public:
		CError detect(const cv::Mat& input, cv::Mat& output);

		CError preImage(const cv::Mat& input);

		CError preImageMtd0(const cv::Mat& input);

		CError inferImage(cv::Mat& output);

		CError inferImageMtd0(cv::Mat& output);

	private:
		OnnxSegmentPrivate* d;

	public:
		Node::CString* m_inputModelName;
		Node::CString* m_outputModelName;
		Node::CInt* m_batchSize;
		Node::CInt* m_imgPred;
		Node::CInt* m_inferMtd;
	};
}