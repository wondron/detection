#include "pch.h"
#include "TRTSegment.h"
#include "TRTClassifier.h"
#include "TRTInference.h"

using namespace Trt;

int test() {
	return 1;
}

std::vector<AlgoMap> getAlgoMaps()
{
	AlgoMap single;
	std::vector<AlgoMap> algoList;

	single.algoName = "segment";
	single.algoDesc = "�ָ�ģ��";
	single.description = "666";
	algoList.push_back(single);

	single.algoName = "classifier";
	single.algoDesc = "����ģ��";
	single.description = "777";
	algoList.push_back(single);

	return algoList;
}

Node::CGroup* GetAlgoNode(std::string algoKey) {
	if (algoKey == "segment") {
		std::cout << "enter segment initial ... ";
		return new Trt::TRTSegment("segment", "�ָ�ģ��");
	}
	else if (algoKey == "classifier") {
		std::cout << "enter classifier initial ... ";
		return new Trt::TRTClassifier("classifier", "�ָ�ģ��");
	}
	else {
		std::cout << "GetAlgoNode�޷���Ӧ������ֵΪ��" << algoKey;
		return new Node::CGroup();
	}
}

CError ProcessAutoAlgo(Node::CGroup* node, const std::string& modelPath, std::vector<cv::Mat>input, std::vector<cv::Mat>& maskImg)
{
	CError err;
	if (node->name() == "segment") {
		std::cout << "����ProcessAutoAlgo����";
		auto pam = static_cast<TRTSegment*>(node);
		err = pam->loadModel(modelPath);
		CHECKERR(err);
		err = pam->setModelParam();
		CHECKERR(err);
		err = pam->detect(input, maskImg);
		CHECKERR(err);

		return 0;
	}
	else if (node->name() == "classifier") {
		return CError(PAMVALUE, u8"δ��������㷨�����ش���");
	}
	else {
		std::string showMsg= "�㷨������,����node��nameΪ�� " + node->name();
		return CError(PAMVALUE, showMsg);
	}

	return 0;
}