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
	single.algoDesc = "分割模型";
	single.description = "666";
	algoList.push_back(single);

	single.algoName = "classifier";
	single.algoDesc = "分类模型";
	single.description = "777";
	algoList.push_back(single);

	return algoList;
}

Node::CGroup* GetAlgoNode(std::string algoKey) {
	if (algoKey == "segment") {
		std::cout << "enter segment initial ... ";
		return new Trt::TRTSegment("segment", "分割模型");
	}
	else if (algoKey == "classifier") {
		std::cout << "enter classifier initial ... ";
		return new Trt::TRTClassifier("classifier", "分割模型");
	}
	else {
		std::cout << "GetAlgoNode无法对应，输入值为：" << algoKey;
		return new Node::CGroup();
	}
}

CError ProcessAutoAlgo(Node::CGroup* node, const std::string& modelPath, std::vector<cv::Mat>input, std::vector<cv::Mat>& maskImg)
{
	CError err;
	if (node->name() == "segment") {
		std::cout << "进入ProcessAutoAlgo操作";
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
		return CError(PAMVALUE, u8"未导入分类算法，返回错误！");
	}
	else {
		std::string showMsg= "算法不存在,输入node的name为： " + node->name();
		return CError(PAMVALUE, showMsg);
	}

	return 0;
}