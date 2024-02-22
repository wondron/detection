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

			//CUDA�ڴ�����
			cudaMalloc(&pBuffers[0], inputSize * sizeof(float));

			for (int i = 0; i < modelOutIndex.size(); i++)
				cudaMalloc(&pBuffers[modelOutIndex[i]], sizeof(float) * outputSizeList[i]);

			//CPU�ڴ�����
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
		unsigned char* img_device;   //��ʹ�õ�GPU����

		//ģ�����ݻ���
		IRuntime* pRuntime = nullptr;
		ICudaEngine* pEngine = nullptr;
		IExecutionContext* pContext = nullptr;

		bool isSetPam = false;
		//ģ��������Ϣ��ȡ
		int inputIndex;
		int inputModelW;
		int inputModelH;
		int inputSize;      //�ڴ�������
		nvinfer1::DataType inputDataType = nvinfer1::DataType::kFLOAT;

		//ģ�������Ϣ��ȡ
		vector<int> modelOutIndex;	//������
		vector<int> outputSizeList;
		vector<nvinfer1::DataType> outputDataTypeList; //ģ�������������

		bool isMalloc = false;
		//�������ݻ���
		float* pData = nullptr;		//����ͼ��
		void*  pBuffers[5];			//������
		float* pProb = nullptr;
		float* pSegRes = nullptr;
		cudaStream_t stream;

		//ģ�Ͳ�������
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
		//��������
		m_getCheck =	new CBool("getCheck",	"�������",			false,	this);
		m_deviceIndex = new CInt("deviceIndex", "GPU�豸ѡ��",		0,		this);
		m_imgPred =		new CInt("imgPred",		"ͼ��Ԥ������ѡ��",	0,		this);
		m_inferMtd =	new CInt("inferMtd",	"ͼ��������",		0,		this);
		m_batchSize =	new CInt("batchSize",	"batchSizeֵ",		1,		this);
	
		//״̬���
		TRTPublicUse::setCudaLazy();
		
		//�Կ���Ϣ��ȡ
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
				return CError(NG, "GPU�豸�����ڣ��޷�����ģ�Ͳ�����");

			if (!d->iniFlag) 
				return CError(NG, "ģ�ͳ�ʼ��δ��ɣ��޷�����ģ�Ͳ�����");

			if ((d->deviceIndex == m_deviceIndex->value()) &&
				d->isSetPam) {
				cout << "ģ���������δ�����仯��ֱ�ӷ���OK��" << endl;
				return 0;
			}

			CError err;

			//������ֵ
			d->isSetPam = true;
			d->deviceIndex = m_deviceIndex->value();
			d->clearInferData();

			//����б�����
			d->modelOutIndex.clear();
			d->outputSizeList.clear();
			d->outputDataTypeList.clear();

			cudaSetDevice(d->deviceIndex);
			//�������Զ�ȡ
			int index = d->pEngine->getBindingIndex(d->modelInputName.c_str());
			if (index == -1)	
				return CError(PAMVALUE, string(u8"ģ�������������Ų�Ϊ0��ģ�Ͳ�������ʧ��"));
			d->inputIndex = index;
			auto inputDim = d->pEngine->getBindingDimensions(index);

			d->inputModelH   = inputDim.d[2];                                         
			d->inputModelW   = inputDim.d[3];
			d->nMaxBatch     = d->pEngine->getMaxBatchSize();
			d->inputDataType = d->pEngine->getBindingDataType(index);
			d->inputSize = 1;
			for (size_t i = 0; i < inputDim.nbDims; i++)
				d->inputSize *= std::abs(inputDim.d[i]);

			//������Զ�ȡ
			for (auto i : d->modelOutputName) {
				index = d->pEngine->getBindingIndex(i.c_str());
				if (index == -1) return CError(PAMVALUE, string(u8"ģ���������Ϊ�ҵ���ģ�Ͳ�������ʧ��"));
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
			cout << "ģ�Ͳ������óɹ���" << endl;

			return 0;
		}
		catch (...) {
			return CError(UNEXCEPTION, description() + ", ��ʼ�������쳣��ִ��ʧ�ܣ�");
		}
	}

	CError TRTSegment::splitVector(std::vector<cv::Mat> vecMat, std::vector<std::vector<cv::Mat>>& outputMat)
	{
		try
		{
			if (!vecMat.size())
				return CError(PAMVALUE, "TRTSegment::splitVector����ͼƬΪ�շ���NG��");

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
			return CError(NG, "TRTSegment::splitVectorִ��ʧ�ܣ�");
		}
	}

	CError TRTSegment::convertToMat(float* data, cv::Mat& outputMat, int imgWidth, int imgHeight, int memSize)
	{
		try
		{
			if (data == nullptr)
				return CError(EMPTYOBJ, "����ͼ��Ϊ�գ�����NG��");

			if (!d->outputDataTypeList.size())
				return CError(EMPTYOBJ, "d->outputDataTypeListΪ0���޷�����convertToMat��");

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
				return CError(NG, "SegmentTRT::convertToMat�������ͳ�����ѡ��Χ, ����NG��");
			}
			return 0;
		}
		catch (...)
		{
			return CError(UNEXCEPTION, "TRTSegment::convertToMatִ��ʧ�ܣ�");
		}
	}

	CError TRTSegment::loadModel(const std::string& modelPath)
	{
		try
		{
			//����
			cout << description() << " :" << "��ʼ����ģ��: " << modelPath << endl;

			if (!m_bHasDevice)
				return CError(NG, "û��GPU�豸����NG!");

			if (d->modelPath == modelPath) {
				cout << "ģ��λ����ͬ��Ĭ��ģ�ͼ��سɹ������أ�" << endl;
				return 0;
			}

			struct stat buffer;
			if (stat(modelPath.c_str(), &buffer) != 0){
				std::cout << "File does not exist." << std::endl;
				return CError(EMPTYOBJ, "ģ���ļ�Ϊ�գ�" + modelPath);
			}

			//ж��ģ���ļ�
			d->clearModelData();

			//ģ�Ͷ�ȡ
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
				return CError(UNEXCEPTION, description() + " :engine��Ϣ��ȡʧ��");
			
			d->nMaxBatch = d->pEngine->getMaxBatchSize();

			d->pContext = d->pEngine->createExecutionContext();
			if (d->pContext == nullptr)
				return CError(UNEXCEPTION, description() + " :engineת����contextʧ�ܣ�");

			delete[] modelStream;
			cout << "ģ�ͳ�ʼ���ɹ�!" << endl;

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
			//����
			if (!m_bHasDevice)
				return 0;

			if (!m_getCheck->value())
				return 0;

			if (!d->iniFlag)
				return CError(NG, "����ģ�ͳ�ʼ��ʧ�ܣ�" + d->modelPath);

			if (!d->isMalloc)
				return CError(NG, "ģ���ڴ�Ϊ���룬����NG��");

			if (!inputImg.size())
				return CError(EMPTYOBJ, "����ͼ��Ϊ�գ�����NG��");

			/*if (m_batchSize->value() > d->nMaxBatch)
				return CError(PAMVALUE, "������batchSizeֵ > ģ�͵����batchsizi_" + std::to_string(d->nMaxBatch));*/

			maskImg.clear();

			//����
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
				//maskImg��resMat�����һ��
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
			return CError(PAMVALUE, "ͼ��Ԥ������ѡ�񳬹�0��");
		}
	}

	CError TRTSegment::inferImage(const std::vector<cv::Mat>& input, std::vector<cv::Mat>& resImage)
	{
		switch (m_imgPred->value())
		{
		case 0:
			return inferImageMtd0(input, resImage);
		default:
			return CError(PAMVALUE, "ͼ��������ѡ�񳬹�0��");
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
			return CError(UNEXCEPTION, "TRTSegment::inferImageMtd0 ִ��ʧ�ܣ�");
		}
	}
}