#include "pch.h"
#include "TRTSegment.h"
#include <nvml.h>
#include <sys/stat.h>
#include <NvInferRuntime.h>
#include <opencv2/imgproc.hpp>
#include "trtpublicuse.h"

namespace Trt
{
	using namespace std;
	using namespace Node;
	using namespace nvinfer1;

	class TRTSegmentPrivate
	{
	public:
		TRTSegmentPrivate() {
			pLogger = new WLogger();
			pLogger->log(ILogger::Severity::kINTERNAL_ERROR, "get class initial... ...\n");
		}

		~TRTSegmentPrivate() {
			delete pLogger;
			pLogger = nullptr;

			clearModelData();
			clearInferData();
		}

	public:
		void clearModelData() {
			iniFlag = false;
			modelPath = string();

			if (pContext) {
				pContext->destroy();
				pContext = nullptr;
			}

			if (pRuntime) {
				pRuntime->destroy();
				pRuntime = nullptr;
			}

			if (pEngine) {
				pEngine->destroy();
				pEngine = nullptr;
			}
		}

		void mallocInferData() {
			if (!isSetPam) return;
			isMalloc = true;

			//CUDA内存申请
			cudaMalloc(&pBuffers[0], inputSize * sizeof(float));

			for (int i = 0; i < modelOutIndex.size(); i++)
				cudaMalloc(&pBuffers[modelOutIndex[i]], sizeof(float) * outputSizeList[i]);

			//CPU内存申请
			pData = new float[inputSize];
			pProb = new float[outputSizeList[0]];
			pSegRes = new float[outputSizeList[0]];
		}

		void clearInferData() {
			if (!isMalloc) return;
			isMalloc = false;

			cudaFree(pBuffers[0]);
			for (auto i : modelOutIndex)
				cudaFree(pBuffers[i]);

			modelOutIndex.clear();
			cudaStreamDestroy((__device_builtin__ struct CUstream_st*)stream);
			cudaFree(img_device);
			cudaStreamDestroy(stream);
			cudaFreeHost(pData);
			
			delete[] pProb;
			delete[] pSegRes;
			
			isSetPam = false;
		}

	public:
		cv::Scalar m_mean = cv::Scalar(0.485, 0.456, 0.406);
		cv::Scalar m_std = cv::Scalar(0.229, 0.224, 0.225);

		bool iniFlag = false;
		std::string modelPath;
		std::string modelInputName = "input";
		std::vector<std::string> modelOutputName;

		WLogger* pLogger = nullptr;
		unsigned char* img_device;   //所使用的GPU名称

		//模型数据缓存
		IRuntime* pRuntime = nullptr;
		ICudaEngine* pEngine = nullptr;
		IExecutionContext* pContext = nullptr;

		bool isSetPam = false;
		//模型输入信息提取
		int inputIndex;
		int inputModelW;
		int inputModelH;
		int inputSize;      //内存总数量
		nvinfer1::DataType inputDataType = nvinfer1::DataType::kFLOAT;

		//模型输出信息提取
		vector<int> modelOutIndex;	//输出序号
		vector<int> outputSizeList;
		vector<nvinfer1::DataType> outputDataTypeList; //模型输出数据类型

		bool isMalloc = false;
		//推理数据缓存
		float* pData = nullptr;		//输入图像
		void*  pBuffers[5];			//输出结果
		float* pProb = nullptr;
		float* pSegRes = nullptr;
		cudaStream_t stream;

		//模型参数缓存
		int nMaxBatch = 1;
		int nBatch = 1;
		int deviceIndex = 0;
		int inputSizeW = 128;
		int inputSizeH = 128;
		int resDataNum = 2;
		int classNum = 1;
	};

	TRTSegment::TRTSegment(string name, string des, CNode* parent) : Node::CGroup(name, des, parent)
	{
		//参数声明
		m_getCheck =	new CBool("getCheck",	"开启检测",			false,	this);
		m_deviceIndex = new CInt("deviceIndex", "GPU设备选择",		0,		this);
		m_imgPred =		new CInt("imgPred",		"图像预处理方法选择",	0,		this);
		m_inferMtd =	new CInt("inferMtd",	"图像推理方法",		0,		this);
		m_batchSize =	new CInt("batchSize",	"batchSize值",		1,		this);
	
		//状态检查
		TRTPublicUse::setCudaLazy();
		
		//显卡信息获取
		std::vector<cudaDevicePropCuston> deviceInfo;
		m_bHasDevice = !TRTPublicUse::getDeviceInfo(deviceInfo).isWrong();
		if (m_bHasDevice) {
			d = new TRTSegmentPrivate();
			d->pLogger->setShowName(name);
		}
	}

	TRTSegment::~TRTSegment()
	{
		delete m_getCheck;
		delete m_imgPred;
		delete m_batchSize;
		delete m_deviceIndex;
		delete m_inferMtd;
	}

	void TRTSegment::setMeanSubStd(const cv::Scalar mean, const cv::Scalar std)
	{
		d->m_mean = mean;
		d->m_std = std;
	}

	void TRTSegment::setModelInputOutput(const std::string& inputName, const std::vector<std::string>& outputName)
	{
		if (inputName.empty() || outputName.size() == 0) return;

		d->modelInputName = inputName;
		d->modelOutputName = outputName;
	}

	CError TRTSegment::setModelParam()
	{
		try
		{
			if (!m_bHasDevice) 
				return CError(NG, "GPU设备不存在，无法设置模型参数！");

			if (!d->iniFlag) 
				return CError(NG, "模型初始化未完成，无法设置模型参数！");

			if ((d->deviceIndex == m_deviceIndex->value()) &&
				d->isSetPam) {
				cout << "模型推理参数未发生变化，直接返回OK！" << endl;
				return 0;
			}

			CError err;

			//参数赋值
			d->isSetPam = true;
			d->deviceIndex = m_deviceIndex->value();
			d->clearInferData();

			//输出列表清理
			d->modelOutIndex.clear();
			d->outputSizeList.clear();
			d->outputDataTypeList.clear();

			cudaSetDevice(d->deviceIndex);
			//输入属性读取
			int index = d->pEngine->getBindingIndex(d->modelInputName.c_str());
			if (index == -1)	
				return CError(PAMVALUE, string(u8"模型输入层名称序号不为0，模型参数设置失败"));
			d->inputIndex = index;
			auto inputDim = d->pEngine->getBindingDimensions(index);

			d->inputModelH   = inputDim.d[2];                                         
			d->inputModelW   = inputDim.d[3];
			d->nMaxBatch     = d->pEngine->getMaxBatchSize();
			d->inputDataType = d->pEngine->getBindingDataType(index);
			d->inputSize = 1;
			for (size_t i = 0; i < inputDim.nbDims; i++)
				d->inputSize *= std::abs(inputDim.d[i]);

			//输出属性读取
			for (auto i : d->modelOutputName) {
				index = d->pEngine->getBindingIndex(i.c_str());
				if (index == -1) return CError(PAMVALUE, string(u8"模型输出名称为找到，模型参数设置失败"));
				d->modelOutIndex.push_back(index);
			}

			for (auto dex : d->modelOutIndex) {
				int outputSize = 1;
				auto outputDim = d->pEngine->getBindingDimensions(dex);
				d->outputDataTypeList.push_back(d->pEngine->getBindingDataType(dex));

				for (size_t j = 0; j < outputDim.nbDims; j++)
					outputSize *= std::abs(outputDim.d[j]);

				d->outputSizeList.push_back(outputSize);
			}

			d->mallocInferData();
			cout << "模型参数设置成功！" << endl;

			return 0;
		}
		catch (...) {
			return CError(UNEXCEPTION, description() + ", 初始化进入异常，执行失败！");
		}
	}

	CError TRTSegment::splitVector(std::vector<cv::Mat> vecMat, std::vector<std::vector<cv::Mat>>& outputMat)
	{
		try
		{
			if (!vecMat.size())
				return CError(PAMVALUE, "TRTSegment::splitVector输入图片为空返回NG！");

			outputMat.clear();
			int step = m_batchSize->value();
			int startIndex = 0, endIndex = vecMat.size(), valIndex = startIndex;
			while (startIndex < endIndex) {
				valIndex = startIndex + step > endIndex ? endIndex : startIndex + step;
				std::vector<cv::Mat> newMatVector(vecMat.begin() + startIndex, vecMat.begin() + valIndex);
				startIndex = valIndex;
				outputMat.push_back(newMatVector);
			}

			return 0;
		}
		catch (...)
		{
			return CError(NG, "TRTSegment::splitVector执行失败！");
		}
	}

	CError TRTSegment::convertToMat(float* data, cv::Mat& outputMat, int imgWidth, int imgHeight, int memSize)
	{
		try
		{
			if (data == nullptr)
				return CError(EMPTYOBJ, "输入图像为空，返回NG！");

			if (!d->outputDataTypeList.size())
				return CError(EMPTYOBJ, "d->outputDataTypeList为0，无法进行convertToMat！");

			switch (d->outputDataTypeList[0])
			{
			case nvinfer1::DataType::kFLOAT:
				outputMat = cv::Mat(imgHeight, imgWidth, CV_32FC1, data + memSize);
				break;
			case nvinfer1::DataType::kHALF:
				outputMat = cv::Mat(imgHeight, imgWidth, CV_16FC1, data + memSize);
				break;
			case nvinfer1::DataType::kINT8:
				outputMat = cv::Mat(imgHeight, imgWidth, CV_8SC1, data + memSize);
				break;
			case nvinfer1::DataType::kINT32:
				outputMat = cv::Mat(imgHeight, imgWidth, CV_32SC1, data + memSize);
				break;
			case nvinfer1::DataType::kUINT8:
				outputMat = cv::Mat(imgHeight, imgWidth, CV_8UC1, data + memSize);
				break;
			default:
				return CError(NG, "SegmentTRT::convertToMat数据类型超出可选择范围, 返回NG！");
			}
			return 0;
		}
		catch (...)
		{
			return CError(UNEXCEPTION, "TRTSegment::convertToMat执行失败！");
		}
	}

	CError TRTSegment::loadModel(const std::string& modelPath)
	{
		try
		{
			//防呆
			cout << description() << " :" << "开始加载模型: " << modelPath << endl;

			if (!m_bHasDevice)
				return CError(NG, "没有GPU设备返回NG!");

			if (d->modelPath == modelPath) {
				cout << "模型位置相同，默认模型加载成功并返回！" << endl;
				return 0;
			}

			struct stat buffer;
			if (stat(modelPath.c_str(), &buffer) != 0){
				std::cout << "File does not exist." << std::endl;
				return CError(EMPTYOBJ, "模型文件为空，" + modelPath);
			}

			//卸载模型文件
			d->clearModelData();

			//模型读取
			std::ifstream file(modelPath.c_str(), std::ios::binary);
			size_t size = 0;
			file.seekg(0, file.end);
			size = file.tellg();
			file.seekg(0, file.beg);
			char* modelStream = new char[size];
			file.read(modelStream, size);
			file.close();

			d->pRuntime = createInferRuntime(*d->pLogger);

			d->pEngine = d->pRuntime->deserializeCudaEngine(modelStream, size);
			if (d->pEngine == nullptr)
				return CError(UNEXCEPTION, description() + " :engine信息读取失败");
			
			d->nMaxBatch = d->pEngine->getMaxBatchSize();

			d->pContext = d->pEngine->createExecutionContext();
			if (d->pContext == nullptr)
				return CError(UNEXCEPTION, description() + " :engine转换成context失败！");

			delete[] modelStream;
			cout << "模型初始化成功!" << endl;

			d->iniFlag = true;
			d->modelPath = modelPath;

			return 0;
		}
		catch (const std::exception& e)
		{
			return CError(UNEXCEPTION, e.what());
		}
	}
	
	CError TRTSegment::detect(const std::vector<cv::Mat>& inputImg, std::vector<cv::Mat>& maskImg)
	{
		try
		{
			//防呆
			if (!m_bHasDevice)
				return 0;

			if (!m_getCheck->value())
				return 0;

			if (!d->iniFlag)
				return CError(NG, "分类模型初始化失败：" + d->modelPath);

			if (!d->isMalloc)
				return CError(NG, "模型内存为申请，返回NG！");

			if (!inputImg.size())
				return CError(EMPTYOBJ, "输入图像为空，返回NG！");

			/*if (m_batchSize->value() > d->nMaxBatch)
				return CError(PAMVALUE, "参数的batchSize值 > 模型的最大batchsizi_" + std::to_string(d->nMaxBatch));*/

			maskImg.clear();

			//推理
			CError err;
			std::vector<cv::Mat> resMat;
			std::vector<std::vector<cv::Mat>> batchMat;

			vector<cv::Size> sizeList;
			for (auto i : inputImg)
				sizeList.push_back(i.size());

			err = splitVector(inputImg, batchMat);
			CHECKERR(err);

			for (const std::vector<cv::Mat>& singleBatchMat : batchMat){
				err = inferImage(singleBatchMat, resMat);
				CHECKERR(err);
				//maskImg和resMat结果合一起
				maskImg.insert(maskImg.end(), resMat.begin(), resMat.end());
			}

			for (int i = 0; i < maskImg.size(); i++)
				cv::resize(maskImg[i], maskImg[i], sizeList[i]);

			return 0;
		}
		catch (const std::exception& e)
		{
			return CError(UNEXCEPTION, e.what());
		}
	}

	CError TRTSegment::preProImage(const std::vector<cv::Mat>& input, float* pfInputDevBuffer)
	{
		switch (m_imgPred->value())
		{
		case 0:
			return TRTPublicUse::mat2ptr(input, cv::Size(d->inputModelW, d->inputModelH), d->m_mean, d->m_std, pfInputDevBuffer);
		default:
			return CError(PAMVALUE, "图像预处理方法选择超过0！");
		}
	}

	CError TRTSegment::inferImage(const std::vector<cv::Mat>& input, std::vector<cv::Mat>& resImage)
	{
		switch (m_imgPred->value())
		{
		case 0:
			return inferImageMtd0(input, resImage);
		default:
			return CError(PAMVALUE, "图像推理方法选择超过0！");
		}
	}

	CError TRTSegment::inferImageMtd0(const std::vector<cv::Mat>& input, std::vector<cv::Mat>& resImage)
	{
		try
		{
			CError err;
			cv::Mat maskSingle;
			resImage.clear();

			err = preProImage(input, d->pData);
			CHECKERR(err);

			d->pContext->setBindingDimensions(0, nvinfer1::Dims4(m_batchSize->value(), 3, d->inputModelH, d->inputModelW));
			cudaMemcpyAsync(d->pBuffers[0], d->pData,
				m_batchSize->value() * 3 * d->inputModelH * d->inputModelW * sizeof(float),
				cudaMemcpyHostToDevice, d->stream);

			d->pContext->enqueueV2(d->pBuffers, d->stream, nullptr);

			cudaMemcpyAsync(d->pSegRes, d->pBuffers[1],
				m_batchSize->value() * d->inputModelH * d->inputModelW * sizeof(float),
				cudaMemcpyDeviceToHost, d->stream);
			cudaStreamSynchronize(d->stream);

			for (int i = 0; i < d->nBatch; i++) {
				int memSize = i * d->inputModelH * d->inputModelW;
				err = convertToMat(d->pSegRes, maskSingle, d->inputModelW, d->inputModelH, memSize);
				CHECKERR(err);
				maskSingle.convertTo(maskSingle, CV_8UC1);
				resImage.push_back(maskSingle);
			}

			return 0;
		}
		catch (...)
		{
			return CError(UNEXCEPTION, "TRTSegment::inferImageMtd0 执行失败！");
		}
	}
}