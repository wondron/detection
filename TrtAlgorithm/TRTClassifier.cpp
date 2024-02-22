#include "pch.h"
#include "TRTClassifier.h"
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

	class TRTClassifierPrivate
	{
	public:
		TRTClassifierPrivate() {
			pLogger = new WLogger();
			pLogger->log(ILogger::Severity::kINTERNAL_ERROR, "get class initial... ...\n");
		}

		~TRTClassifierPrivate() {
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

		void clearInferData() {
			if (!bIsSetPam) return;

			cudaFree(pBuffers[0]);

			for (auto i : modelOutIndex)
				cudaFree(pBuffers[i]);

			modelOutIndex.clear();
			cudaStreamDestroy((__device_builtin__ struct CUstream_st*)stream);
			cudaFree(img_device);
			cudaStreamDestroy(stream);
			cudaFreeHost(pData);
			
			delete[] pProb;
			delete[] pClas;
			
			bIsSetPam = false;
		}

	public:
		cv::Scalar m_mean = cv::Scalar(0.50, 0.50, 0.50);
		cv::Scalar m_std = cv::Scalar(0.50, 0.50, 0.50);

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

		//推理数据缓存
		bool bIsSetPam = false;
		float* pData = nullptr;		//输入图像
		vector<int> modelOutIndex;	//输出序号
		void* pBuffers[5];			//输出结果
		float* pProb = nullptr;
		float* pClas = nullptr;
		cudaStream_t stream;

		//模型参数缓存
		int nBatch = 0;
		int deviceIndex = 0;
		int inputSizeW = 128;
		int inputSizeH = 128;
		int resDataNum = 2;
		int classNum = 1;
	};

	TRTClassifier::TRTClassifier(string name, string des, CNode* parent) : Node::CGroup(name, des, parent)
	{
		//参数声明
		m_getCheck =	new CBool("getCheck",	"开启检测",			false,	this);
		m_deviceIndex = new CInt("deviceIndex", "GPU设备选择",		0,		this);
		m_imgPred =		new CInt("imgPred",		"图像预处理方法选择",	0,		this);
		m_inferMtd =	new CInt("inferMtd",	"推理方法选择",		0,		this);
		m_batchSize =	new CInt("batchSize",	"batchSize值",		1,		this);
		m_inputSizeW =	new CInt("inputSizeW",	"图像输入宽度",		128,	this);
		m_inputSizeH =	new CInt("inputSizeH",	"图像输入高度",		128,	this);
		m_classNum =	new CInt("classNum",	"分类数量选择",		0,		this);
			
		//状态检查
		TRTPublicUse::setCudaLazy();
		
		//显卡信息获取
		std::vector<cudaDevicePropCuston> deviceInfo;
		m_bHasDevice = !TRTPublicUse::getDeviceInfo(deviceInfo).isWrong();
		if (m_bHasDevice) d = new TRTClassifierPrivate();
	}

	TRTClassifier::~TRTClassifier()
	{
		delete m_getCheck;
		delete m_imgPred;
		delete m_inferMtd;
		delete m_deviceIndex;
		delete m_batchSize;
		delete m_inputSizeW;
		delete m_inputSizeH;
		delete m_classNum;
	}

	void TRTClassifier::setMeanSubStd(const cv::Scalar mean, const cv::Scalar std)
	{
		d->m_mean = mean;
		d->m_std = std;
	}

	void TRTClassifier::setModelInputOutput(const std::string& inputName, const std::vector<std::string>& outputName)
	{
		if (inputName.empty() || outputName.size() == 0) return;

		d->modelInputName = inputName;
		d->modelOutputName = outputName;
	}

	CError TRTClassifier::setModelParam()
	{
		try
		{
			if (!m_bHasDevice) return CError(NG, "GPU设备不存在，无法设置模型参数！");

			if (!d->iniFlag) return CError(NG, "模型初始化未完成，无法设置模型参数！");

			if ((d->nBatch == m_batchSize->value()) &&
				(d->deviceIndex == m_deviceIndex->value()) &&
				(d->inputSizeW == m_inputSizeW->value()) &&
				(d->inputSizeH == m_inputSizeH->value()) &&
				(d->classNum == m_classNum->value()) &&
				d->bIsSetPam) {
				cout << "模型推理参数未发生变化，直接返回OK！" << endl;
				return 0;
			}

			d->clearInferData();
			//参数赋值
			d->bIsSetPam = true;
			d->nBatch = m_batchSize->value();
			d->deviceIndex = m_deviceIndex->value();
			d->inputSizeW = m_inputSizeW->value();
			d->inputSizeH = m_inputSizeH->value();
			d->classNum = m_classNum->value();

			cudaSetDevice(d->deviceIndex);
			int index = d->pEngine->getBindingIndex(d->modelInputName.c_str());
			if (index != 0)	return CError(PAMVALUE, string(u8"模型输入层名称序号不为0，模型参数设置失败"));

			for (auto i : d->modelOutputName) {
				index = d->pEngine->getBindingIndex(i.c_str());
				if (index == -1) return CError(PAMVALUE, string(u8"模型输出名称为找到，模型参数设置失败"));
				d->modelOutIndex.push_back(index);
			}

			cudaStreamCreate((cudaStream_t*)&d->stream);														//steam分配

			cudaMalloc((void**)&d->img_device, d->nBatch * 20);													//内存分配

			cudaMalloc(&d->pBuffers[0], d->nBatch * 3 * d->inputSizeW * d->inputSizeH * sizeof(float));			//输入的载体buffer

			for (auto idx : d->modelOutIndex)																	//输出结果
				cudaMalloc(&d->pBuffers[idx], d->nBatch * d->classNum * sizeof(float));

			cudaMallocHost((void**)&d->pData, d->nBatch * 3 * d->inputSizeW * d->inputSizeH * sizeof(float));	//图像数据

			d->pProb = new float[d->nBatch * d->classNum];
			d->pClas = new float[d->nBatch * d->classNum];

			cout << "模型参数设置成功！" << endl;

			return 0;
		}
		catch (...) {
			return CError(UNEXCEPTION, description() + ", 初始化进入异常，执行失败！");
		}
	}

	CError TRTClassifier::loadModel(const std::string& modelPath)
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
	
	CError TRTClassifier::detect(const cv::Mat& input, float& confidence, int& classIdx)
	{
		CError err;
		cv::Mat resImg, showImg;
		confidence = 0;
		classIdx = 0;

		if (!m_bHasDevice)
			return 0;

		if (!m_getCheck->value())
			return 0;

		if (!d->iniFlag)
			return CError(NG, "分类模型初始化失败：" + d->modelPath);

		err = inferImage(input, confidence, classIdx);
		CHECKERR(err);

		return 0;
	}

	CError TRTClassifier::preProImage(const cv::Mat& input, float* pfInputDevBuffer)
	{
		switch (m_imgPred->value())
		{
		case 0:
			return preProImageMtd0(input, pfInputDevBuffer);
		default:
			break;
		}
	}

	CError TRTClassifier::preProImageMtd0(const cv::Mat& input, float* pfInputDevBuffer)
	{
		//不足64，留白补上-->resize到128
		using namespace cv;
		try
		{
			int imgW, imgH, size;
			imgW = m_inputSizeW->value();
			imgH = m_inputSizeH->value();
			size = imgW * imgH;

			Mat detectMat, resizeMat, splitMat[3];
			detectMat = input.clone();
			
			cv::resize(detectMat, resizeMat, Size(imgW, imgH));
			resizeMat.convertTo(resizeMat, CV_32FC3);
			resizeMat /= 255.0;
			subtract(resizeMat, d->m_mean, resizeMat, noArray(), CV_32F);
			divide(resizeMat, d->m_std, resizeMat, 1, CV_32F);

			Mat rgb[3] = {
				Mat(imgH, imgW, CV_32FC1, (void*)(pfInputDevBuffer + 0 * size)),
				Mat(imgH, imgW, CV_32FC1, (void*)(pfInputDevBuffer + 1 * size)),
				Mat(imgH, imgW, CV_32FC1, (void*)(pfInputDevBuffer + 2 * size)),
			};

			split(resizeMat, rgb);
			return 0;
		}
		catch (...)
		{
			return CError(UNEXCEPTION, description() + "图像预处理方法1执行失败！");
		}
	}

	CError TRTClassifier::inferImage(const cv::Mat& input, float& confidence, int& classIdx)
	{
		switch (m_inferMtd->value())
		{
		case 0:
			return inferImageMtd0(input, confidence, classIdx);
		default:
			return CError(PAMVALUE, "推理方法选择超过0");
		}
	}

	CError TRTClassifier::inferImageMtd0(const cv::Mat& input, float& confidence, int& classIdx)
	{
		try
		{
			bool isSuccess;
			CError err;
			confidence = 0;
			if (input.empty())
				return CError(EMPTYOBJ, "预处理输入图像为空！");

			int size = 3 * d->inputSizeH * d->inputSizeW;
			err = preProImage(input, d->pData);
			RETURNERR(err);

			cudaMemcpyAsync(d->pBuffers[0], d->pData, size * sizeof(float), cudaMemcpyHostToDevice, d->stream);

			isSuccess = d->pContext->enqueueV2(d->pBuffers, d->stream, nullptr);

			cudaMemcpyAsync(d->pProb, d->pBuffers[1], d->nBatch * d->classNum * sizeof(float), cudaMemcpyDeviceToHost, d->stream);

			cudaMemcpyAsync(d->pClas, d->pBuffers[2], d->nBatch * d->classNum * sizeof(float), cudaMemcpyDeviceToHost, d->stream);

			cudaStreamSynchronize(d->stream);

			//旧模型
			classIdx = d->pClas[0];
			confidence = d->pProb[0];
			return 0;
		}
		catch (...)
		{
			return CError(UNEXCEPTION, description() + "，图像推理执行失败！");
		}
	}

}