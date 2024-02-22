#pragma once

#include <vector>
#include <cnode.h>
#include <string>

API int test();

API std::vector<AlgoMap> getAlgoMaps();

API Node::CGroup* GetAlgoNode(std::string algoKey);

API CError ProcessAutoAlgo(Node::CGroup* node, const std::string& modelPath, std::vector<cv::Mat>input, std::vector<cv::Mat>& maskImg);