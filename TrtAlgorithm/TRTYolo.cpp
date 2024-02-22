#include "TRTYolo.h"
#include "commonUsage/algorithm.h"
#include "detectItem/CSingleCircle.h"
#include "detectItem/CBlobInfo.h"
#include "qfileinfo.h"
#include "NvInferPlugin.h"
#include <fstream>
#include "logger.h"
#include<xmmintrin.h>
#include <immintrin.h> //SSE
#include <nvml.h>
#include <Windows.h>
#include <time.h>

namespace PSL
{
	using namespace Detect;
	using namespace HalconCpp;
	using namespace nvinfer1;

	struct Object
	{
		cv::Rect rect;
		int label;
		float conf;
	};

	class SegLogger : public ILogger {
	public:
		void setShowName(std::string name) {
			m_name = name;
		}

		//虚函数，虚函数使用的其核心目的是通过基类访问派生类定义的函数
		virtual void log(Severity severity, AsciiChar const* msg) noexcept override {
			//可以通过判断日志的级别来决定打印哪些内容
			std::string level = "";
			switch (severity)
			{
			case Severity::kINTERNAL_ERROR:
				level = "[I " + m_name + "]";
				break;
			case Severity::kERROR:
				level = "[E " + m_name + "]";
				break;
			case Severity::kWARNING:
				level = "[W " + m_name + "]";
				break;
			}
			if (severity <= Severity::kWARNING) {
				qDebug() << level.c_str() << " " << msg;
			}
		}

		std::string m_name = "class";
	};

	class TRTYoloPrivate
	{
	public:
		TRTYoloPrivate() {
			m_pLogger = new SegLogger();
		}

		~TRTYoloPrivate() {
			delete m_pLogger;

			if (m_bIsSetPam) {
				cudaFree(m_pBuffers[0]);
				cudaFree(m_pBuffers[1]);

				cudaStreamDestroy((__device_builtin__ struct CUstream_st*)m_Stream);
				cudaFree(img_device);
				cudaStreamDestroy(m_Stream);
				cudaFreeHost(m_pData);
				delete[] m_pOut;

				m_bIsSetPam = false;
			}

			if (m_pContext) {
				m_pContext->destroy();
				m_pContext = nullptr;
			}

			if (m_pRuntime) {
				m_pRuntime->destroy();
				m_pRuntime = nullptr;
			}

			if (m_pEngine) {
				m_pEngine->destroy();
				m_pEngine = nullptr;
			}
		}

	public:
		int getCheck = 1;
		int imgPreMtd = 0;

	public:
		bool iniFlag = false;
		bool isInitial = false;

		int batchSize = 1;
		int deviceIndex = 0;
		int inputSizeW = 1024;		//最大输入宽
		int inputSizeH = 1024;		//最大输入高
		int streamNum = 1;			//输出层数
		int classNum = 1;			//分类数量

	public:

		QString inputName = "images";
		QString OUTPUT_BLOB_NAME = "output0";

		SegLogger* m_pLogger = nullptr;
		IRuntime* m_pRuntime = nullptr;
		ICudaEngine* m_pEngine = nullptr;
		bool m_bIsDynamicShape = false;
		IExecutionContext* m_pContext = nullptr;

		bool m_bIsSetPam = false;
		void* m_pBuffers[2];
		cudaStream_t m_Stream;
		long m_nOutputSize = 0;

		float* m_pData = nullptr;
		unsigned char* img_device;   //所使用的GPU名称

		float* m_pOut = nullptr;
	};

	TRTYolo::TRTYolo()
	{
		CError err;
		std::vector<TRTDTCT::cudaDevicePropCuston> deviceInfo;
		err = getDeviceInfo(deviceInfo);

		hasDevice = deviceInfo.size();
		if (hasDevice)
			d = new TRTYoloPrivate();
	}

	TRTYolo::~TRTYolo()
	{
		if (hasDevice)
			delete d;
	}

	CError TRTYolo::pamRead(QMap<QString, QMap<QString, QVariant>>& pamMap, QMap<QString, QMap<QString, QVariant>>& prodMap)
	{
		if (!hasDevice) return 0;

		auto res = pamMap.value(m_taskName);
		auto prod = prodMap.value(m_taskName);
		int nMaxBatch, nDevice, nMaxInputSizeW, nMaxInputSizeH, bDoubInput, nStreamNum, nClassNum;

		READPAMINTDFT(d->getCheck, "getCheck", res, 0);
		READPAMINTDFT(d->imgPreMtd, "imgPreMtd", res, 0);

		READPAMINTDFT(nMaxBatch, "batchSize", res, 1);
		READPAMINTDFT(nDevice, "deviceIndex", res, 0);
		READPAMINTDFT(nMaxInputSizeW, "inputSizeW", res, 1024);
		READPAMINTDFT(nMaxInputSizeH, "inputSizeH", res, 1024);
		READPAMINTDFT(nStreamNum, "streamNum", res, 1);
		READPAMINTDFT(nClassNum, "classNum", res, 1);

		setModelParam(nMaxBatch, nDevice, nMaxInputSizeW, nMaxInputSizeH, false, nStreamNum, nClassNum);
		return 0;

	}

	void TRTYolo::setTaskName(const QString& task)
	{
		d->m_pLogger->setShowName(task.toStdString());
		m_taskName = task;
	}

	MesDefectStruct TRTYolo::getDefectName()
	{
		MesDefectStruct mesData;
		mesData.Name = PNameTrans::getDefectName(DefectType::POSUN);
		mesData.Type = MES_DATA_BOOL_TYPE;

		return mesData;
	}

	void TRTYolo::setModelPath(const QString filepath) {
		if (filepath == m_modelPath)
			return;

		CError err;
		m_modelPath = filepath;
		err = initialModel();
		if (err.isWrong())
			AlgoSysLog::log(ALGO_LOG_RUNNING, err.msg());
	}

	CError TRTYolo::initialModel()
	{
		using namespace std;
		try
		{
			CError  err;

			AlgoSysLog::log(ALGO_LOG_RUNNING, QString(u8"%1，开始模型初始化，模型位置：%2").arg(m_taskName).arg(m_modelPath));

			if (!hasDevice) {
				AlgoSysLog::log(ALGO_LOG_ALGO, QString(u8"%1， 不存在GPU设备模型初始化失败"));
				return CError(NG, QString(u8"%1， 不存在GPU设备模型初始化失败"));
			}

			if (d->isInitial)
				return CError(!d->iniFlag, QString(u8"%1,已进行初始化，直接返回结果（%2）").arg(m_taskName).arg(d->iniFlag));

			d->iniFlag = false;
			d->isInitial = true;

			QFileInfo info(m_modelPath);
			if (!info.isFile())
				return CError(PAMVALUE, QString(u8"%1, 文件位置不存在，初始化失败！").arg(m_modelPath));

			std::ifstream file(m_modelPath.toStdString().c_str(), std::ios::binary);
			char* trtModelStream = nullptr;
			size_t size = 0;
			file.seekg(0, file.end);
			size = file.tellg();
			file.seekg(0, file.beg);
			trtModelStream = new char[size];
			file.read(trtModelStream, size);
			file.close();

			d->m_pRuntime = createInferRuntime(*d->m_pLogger);
			initLibNvInferPlugins(d->m_pLogger, "");
			d->m_pEngine = d->m_pRuntime->deserializeCudaEngine(trtModelStream, size);
			if (d->m_pEngine == nullptr)
				return CError(UNEXCEPTION, QString(u8"%1, engine信息读取失败！").arg(m_taskName));

			//解析engine信息的功能
			for (int i = 0; i < d->m_pEngine->getNbBindings(); i++) {
				nvinfer1::Dims dims = d->m_pEngine->getBindingDimensions(i);
				for (int i = 0; i < dims.nbDims; i++) {
					if (dims.d[i] == -1) {
						d->m_bIsDynamicShape = true;
						break;
					}
				}
			}

			d->m_pContext = d->m_pEngine->createExecutionContext();
			if (d->m_pContext == nullptr)
				return CError(UNEXCEPTION, QString(u8"%1, engine转换成context失败！").arg(m_taskName));

			d->iniFlag = true;

			AlgoSysLog::log(ALGO_LOG_RUNNING, QString(u8"%1，开始模型初始化成功！")
				.arg(m_taskName));

			delete[] trtModelStream;
			return 0;
		}
		catch (...)
		{
			return CError(UNEXCEPTION, QString(u8"%1，初始化进入异常，执行失败！").arg(m_taskName));
		}
	}

	CError TRTYolo::setModelParam(int nMaxBatch, int nDevice, int nMaxInputSizeW, int nMaxInputSizeH, bool bDoubInput, int nStreamNum, int classNum)
	{
		try
		{
			if (!hasDevice) return 0;
			if (!d->iniFlag) return 0;

			if ((nMaxBatch == d->batchSize) &&
				(nDevice == d->deviceIndex) &&
				(nMaxInputSizeW == d->inputSizeW) &&
				(nMaxInputSizeH == d->inputSizeH) &&
				(nStreamNum == d->streamNum) &&
				(classNum == d->classNum))
				return 0;

			cudaSetDevice(nDevice);

			int inputIndex = d->m_pEngine->getBindingIndex(d->inputName.toStdString().c_str());
			if (inputIndex != 0)
				return CError(PAMVALUE, QString(u8"模型输入层名称（%1）无法对应").arg(d->inputName));

			int outputIndex = d->m_pEngine->getBindingIndex(d->OUTPUT_BLOB_NAME.toStdString().c_str());

			if (d->m_bIsSetPam) {
				cudaFree(d->m_pBuffers[inputIndex]);
				cudaFree(d->m_pBuffers[outputIndex]);
				cudaStreamDestroy(d->m_Stream);
				cudaFreeHost(d->m_pData);
				delete[] d->m_pOut;
			}
			d->m_bIsSetPam = true;

			//申请内存
			int layerNum, inputNum;
			layerNum = 3;
			d->m_nOutputSize = nMaxInputSizeH * nMaxInputSizeW;
			inputNum = layerNum * d->m_nOutputSize;
			cudaMalloc((void**)&d->img_device, nMaxBatch * inputNum);
			cudaMalloc(&d->m_pBuffers[inputIndex], nMaxBatch * inputNum * sizeof(float));
			cudaMalloc(&d->m_pBuffers[outputIndex], 25200 * (5 + classNum) * sizeof(float));
			cudaStreamCreate((cudaStream_t*)&d->m_Stream);
			cudaMallocHost((void**)&d->m_pData, 1 * inputNum * sizeof(float));

			d->m_pOut = new float[25200 * (5 + classNum)];

			d->batchSize = nMaxBatch;
			d->streamNum = nStreamNum;
			d->inputSizeW = nMaxInputSizeW;
			d->inputSizeH = nMaxInputSizeH;
			d->classNum = classNum;
			d->deviceIndex = nDevice;

			return 0;
		}
		catch (...) {
			return CError(UNEXCEPTION, QString(u8"%1，初始化进入异常，执行失败！").arg(m_taskName));
		}
	}

	CError TRTYolo::getDeviceInfo(std::vector<TRTDTCT::cudaDevicePropCuston>& deviceInfo)
	{
		using namespace TRTDTCT;
		deviceInfo.clear();
		nvmlDevice_t device;   //GPU设备对象
		nvmlUtilization_t utilization;
		cudaDevicePropCuston singleInfo;
		unsigned int device_count, nTemperature;

		try {
			nvmlReturn_t result = nvmlInit(); //初始化
			result = nvmlDeviceGetCount(&device_count);
			if (NVML_SUCCESS != result)
				return CError(NG, u8"获取GPU数量失败！");

			for (int i = 0; i < device_count; i++) {
				result = nvmlDeviceGetHandleByIndex(i, &device);

				if (NVML_SUCCESS != result)
					return CError(NG, u8"获取GPU信息失败！");

				//获取名字
				result = nvmlDeviceGetName(device, singleInfo.name, NVML_DEVICE_NAME_BUFFER_SIZE);

				//使用率
				result = nvmlDeviceGetUtilizationRates(device, &utilization);

				//温度
				nTemperature = -1;
				result = nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &nTemperature);

				//获取具体显存信息
				nvmlMemory_t memory;
				result = nvmlDeviceGetMemoryInfo(device, &memory);

				//赋值
				singleInfo.nDeviceNum = i;
				singleInfo.nGPUutilization = utilization.gpu;
				singleInfo.nCudaMemutilization = utilization.memory;
				singleInfo.nDeviceTemperature = nTemperature;
				singleInfo.nTotalGPUMemory = memory.total / 1024 / 1024;
				singleInfo.nUsedGpuMemory = memory.used / 1024 / 1024;
				singleInfo.nFreeGpuMemory = memory.free / 1024 / 1024;
				deviceInfo.push_back(singleInfo);
			}
			return 0;
		}
		catch (...) {
			return 1;
		}
	}

	CError TRTYolo::preImage(cv::Mat src, float* data)
	{
		switch (d->imgPreMtd)
		{
		case 0:
			return preImageMtd0(src, data);
		default:
			return CError(UNEXCEPTION, QString(u8"preImage 超过0，返回NG！"));
		}
	}

	CError TRTYolo::preImageMtd0(cv::Mat src, float* data)
	{
		try
		{
			//yolo poSun检测前处理
			cv::Mat resizeMat = src.clone();
			cv::resize(resizeMat, resizeMat, cv::Size(d->inputSizeW, d->inputSizeH));
			const int volChl = d->inputSizeW * d->inputSizeH;
			resizeMat.convertTo(resizeMat, CV_32FC3, 1.0 / 255.0);			
			cv::Mat rgb[3] = {
				cv::Mat(d->inputSizeH, d->inputSizeW, CV_32FC1, (void*)(data + 0 * volChl)),
				cv::Mat(d->inputSizeH, d->inputSizeW, CV_32FC1, (void*)(data + 1 * volChl)),
				cv::Mat(d->inputSizeH, d->inputSizeW, CV_32FC1, (void*)(data + 2 * volChl)),
			};

			cv::split(resizeMat, rgb);

			return 0;
		}
		catch (...)
		{
			return CError(UNEXCEPTION, QString(u8"%1，图像预处理方法0执行失败！").arg(m_taskName));
		}
	}

	CError TRTYolo::detect(const std::vector<SegMatData>& imgs, std::vector<YoloResData>& result)
	{
		CError err;
		result.clear();
		if (!d->getCheck) return 0;

		if (!hasDevice)	return 0;

		if (!d->iniFlag) {
			AlgoSysLog::log(ALGO_LOG_ALGO, QString(u8"%1, 初始化失败！").arg(m_taskName));
			return CError(NG, u8"初始化失败！");
		}

		YoloResData data;
		std::vector<YoloResData> singleRes;
		for (auto img : imgs) {
			singleRes.clear();
			err = detectSingle(img.detectImg, singleRes);
			CHECKERR(err);

			for (auto res : singleRes) {
				data = res;
				data.iniCol = res.iniCol + img.startCol;
				data.iniRow = res.iniRow + img.startRow;
				result.push_back(data);
			}
		}

		return 0;
	}

	CError TRTYolo::detectSingle(const cv::Mat input, std::vector<YoloResData>& result)
	{
		try
		{
			result.clear();

			if (input.empty()) return 0;

			CError err;
			YoloResData yoloRes;
			int srcHeight, srcWidth, shapeWidth, shapeHeight, size, shapeMax, totalSize;
			float x, y, x2, y2, scaleX, scaleY, lastx;

			srcHeight = input.rows;
			srcWidth = input.cols;
			shapeWidth = d->inputSizeW;
			shapeHeight = d->inputSizeH;

			shapeMax = shapeHeight > shapeWidth ? shapeHeight : shapeWidth;
			totalSize = 63 * (shapeMax / 32) * (shapeMax / 32);

			size = 3 * shapeHeight * shapeWidth;
			scaleX = srcWidth / (shapeWidth * 1.0);
			scaleY = srcHeight / (shapeHeight * 1.0);

			err = preImage(input, d->m_pData);
			CHECKERR(err);
			d->m_pContext->setBindingDimensions(0, Dims4(1, 3, shapeHeight, shapeWidth));

			cudaMemcpyAsync(d->m_pBuffers[0], d->m_pData, 1 * size * sizeof(float), cudaMemcpyHostToDevice, d->m_Stream);
			d->m_pContext->enqueueV2(d->m_pBuffers, d->m_Stream, nullptr);


			cudaMemcpyAsync(d->m_pOut, d->m_pBuffers[1], totalSize * (5 + d->classNum) * d->batchSize * sizeof(float), cudaMemcpyDeviceToHost, d->m_Stream);
			cudaStreamSynchronize(d->m_Stream);

			cv::Mat segMat(totalSize, 5 + d->classNum, CV_32FC1, (void*)d->m_pOut);
			result = PPublicUse::getYolo5Result(segMat, 0.3, 0.3);

			for (YoloResData& singleRes : result) {
				singleRes.iniRow *= scaleY;
				singleRes.iniCol *= scaleX;
				singleRes.width *= scaleX;
				singleRes.height *= scaleY;
			}
			return 0;
		}
		catch (...)
		{
			return CError(UNEXCEPTION, u8"顶贴片yolo检测执行失败！");
		}
	}
}