#include "pch.h"
#include <sys/stat.h>
#include "OnnxSegment.h"
#include <onnxruntime_cxx_api.h>
#include <string>
#include <locale>
#include <codecvt>

namespace Onnx
{
	using namespace std;
	using namespace Node;

	class OnnxSegmentPrivate
	{
	public:
		OnnxSegmentPrivate() {}
	
		~OnnxSegmentPrivate(){}

		void clearModelData() {
			if (!iniFlag) return;
			iniFlag = false;

			delete session;
			inputIndex = -1;
			outputIndex = -1;
		}

		void clearInferData() {
			if (!isSetPam) return;
			isSetPam = false;
			delete pData;
			pData = nullptr;
		}

	public:
		cv::Scalar mean = cv::Scalar(0.485, 0.456, 0.406);
		cv::Scalar std = cv::Scalar(0.229, 0.224, 0.225);

		bool iniFlag = false;
		string modelPath = "";
		Ort::Session* session;

		bool isSetPam = false;
		float* pData;
		int inputIndex = -1;
		int outputIndex = -1;
		int inputW;
		int inputH;
		int inputSize;
	};

	OnnxSegment::OnnxSegment(string name, string des, CNode* parent)
		:Node::CGroup(name, des, parent), d(new OnnxSegmentPrivate())
	{
		m_inputModelName = new CString("inputModelName", "模型输出名称", this);
		m_outputModelName = new CString("outputModelName", "模型输入名称", this);
		m_batchSize = new CInt("batchSize", "batch值", this);
		m_imgPred = new CInt("imgPred", "图像处理方法", this);
		m_inferMtd = new CInt("inferMtd", "推理方法选择", this);
	}

	OnnxSegment::~OnnxSegment()
	{
		delete m_inputModelName;
		delete m_outputModelName;
		delete m_batchSize;
		delete m_imgPred;
		delete m_inferMtd;
	}

	CError OnnxSegment::loadModel(const std::string& modelPath)
	{
		try
		{
			Ort::Env env(ORT_LOGGING_LEVEL_WARNING, name().c_str());
			Ort::AllocatorWithDefaultOptions allocator;
			CError err;

			cout << "开始模型初始化，模型位置：" << modelPath << endl;
			if (modelPath == d->modelPath && d->iniFlag)
				return 0;
			
			struct stat buffer;
			if (stat(modelPath.c_str(), &buffer) != 0) {
				std::cout << "File does not exist." << std::endl;
				return CError(EMPTYOBJ, "模型文件为空，" + modelPath);
			}

			//卸载模型文件
			d->clearModelData();
			std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
			std::wstring wideStr = converter.from_bytes(modelPath);

			Ort::SessionOptions session_options;
			session_options.SetIntraOpNumThreads(1);
			session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_BASIC);
			d->session = new Ort::Session(env, wideStr.c_str(), session_options);

			string nameList;
			auto inputSize = d->session->GetInputCount();
			for (int i = 0; i < inputSize; i++) {
				nameList = d->session->GetInputNameAllocated(i, allocator).get();
				if (m_inputModelName->text() == nameList)
					d->inputIndex = i;
			}
			if (d->inputIndex < 0) 
				return CError(PAMVALUE, "输入名称不对应，模型初始化失败！");

			inputSize = d->session->GetOutputCount();
			for (int i = 0; i < inputSize; i++) {
				nameList = d->session->GetOutputNameAllocated(i, allocator).get();
				if (m_outputModelName->text() == nameList)
					d->outputIndex = i;
			}
			if (d->outputIndex < 0) 
				return CError(PAMVALUE, "输出名称不对应，模型初始化失败！");

			d->iniFlag = true;

			err = setModelParam();
			CHECKERR(err);

			return 0;
		}
		catch (const std::exception& e)
		{
			return CError(UNEXCEPTION, e.what());
		}
	}

	CError OnnxSegment::setModelParam()
	{
		try
		{
			if(!d->iniFlag)
				return CError(NG, "模型初始化未完成，无法设置模型参数！");
			
			d->clearInferData();

			d->inputSize = 1;
			auto inputDims = d->session->GetInputTypeInfo(d->inputIndex).GetTensorTypeAndShapeInfo().GetShape();
			for (auto dim : inputDims)
				d->inputSize *= abs(dim);

			d->pData = new float[d->inputSize];
			std::fill(d->pData, d->pData + d->inputSize, 0.0f);
			d->inputH = inputDims[2];
			d->inputW = inputDims[3];


			d->isSetPam = true;
			return 0;
		}
		catch (const std::exception& e)
		{
			return CError(UNEXCEPTION, e.what());
		}
	}

	CError OnnxSegment::preImage(const cv::Mat& input) 
	{
		switch (m_imgPred->value())
		{
		case 0: 
			return preImageMtd0(input);
		default:
			return CError(PAMVALUE, "预处理方法超过0， 返回NG！");
		}
	}

	CError OnnxSegment::preImageMtd0(const cv::Mat& input)
	{
		try
		{
			if (d->pData == nullptr)
				return CError(EMPTYOBJ, "TRTPublicUse::mat2ptr，输入指针未分配内存NG!");

			const int volChl = d->inputH * d->inputW;

			cv::Mat src = input.clone();
			cv::resize(src, src, cv::Size(d->inputW, d->inputH));
			src.convertTo(src, CV_32FC3);
			src /= 255.0;
			cv::subtract(src, d->mean, src, cv::noArray(), CV_32F);
			cv::divide(src, d->std, src, 1, CV_32F);

			cout << "ccc  ";
			for (int i = 9999; i < 10010; i++)
				cout << d->pData[i] << "   " << d->pData[i + volChl] << "    " << d->pData[i + 2 * volChl] << endl;

			cv::Mat rgb[3] = {
				cv::Mat(d->inputH, d->inputW, CV_32FC1,(void*)(d->pData)),
				cv::Mat(d->inputH, d->inputW, CV_32FC1,(void*)(d->pData + 1 * volChl)),
				cv::Mat(d->inputH, d->inputW, CV_32FC1,(void*)(d->pData + 2 * volChl)),
			};
			cv::split(src, rgb);

			cout << "ddd  ";
			for (int i = 9999; i < 10010; i++)
				cout << d->pData[i] << "   " << d->pData[i + volChl] << "    " << d->pData[i + 2 * volChl] << endl;

			return 0;
		}
		catch (const std::exception& e)
		{
			return CError(UNEXCEPTION, e.what());
		}
	}

	CError OnnxSegment::inferImage(cv::Mat& output)
	{
		switch (m_imgPred->value())
		{
		case 0:
			return inferImageMtd0(output);
		default:
			return CError(PAMVALUE, "预处理方法超过0， 返回NG！");
		}
	}

	CError OnnxSegment::inferImageMtd0(cv::Mat& output)
	{
		try
		{
			int volChl = d->inputW * d->inputH;
			cout << "fff  ";
			for (int i = 9999; i < 10010; i++)
				cout << d->pData[i] << "   " << d->pData[i + volChl] << "    " << d->pData[i + 2 * volChl] << endl;

			/*Ort::AllocatorWithDefaultOptions allocator;
			Ort::Value input_tensor_{ nullptr };
			std::array<int64_t, 4> input_shape_{ 1, 3, d->inputH, d->inputW }; //NCHW, 1x3xHxW
			auto memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
			input_tensor_ = Ort::Value::CreateTensor<float>(memory_info, d->pData, d->inputSize, input_shape_.data(), input_shape_.size());

			string inputStr = d->session->GetInputNameAllocated(d->inputIndex, allocator).get();
			string outputStr = d->session->GetOutputNameAllocated(d->outputIndex, allocator).get();
			const char* inputName[] = { inputStr.c_str() };	//输入节点名

			vector<string> outputList;
			int outputSize = d->session->GetOutputCount();
			for (int i = 0; i < outputSize; i++)
				outputList.push_back(d->session->GetOutputNameAllocated(i, allocator).get());
			const char** outputName = new const char* [outputList.size()];
			for (size_t i = 0; i < outputList.size(); ++i)
				outputName[i] = outputList[i].c_str();

			auto ort_outputs = d->session->Run(Ort::RunOptions{}, inputName, &input_tensor_, 1, outputName, outputList.size());

			auto type_info = ort_outputs[d->outputIndex].GetTensorTypeAndShapeInfo();
			
			float* dataFloat;
			int32_t* dataInt32;
			cv::Mat showMat;

			switch (type_info.GetElementType())
			{
			case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT:
				dataFloat = ort_outputs[0].GetTensorMutableData<float>();
				showMat = cv::Mat(384, 384, CV_32FC1, dataFloat);
				break;
			case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32:
				dataInt32 = ort_outputs[0].GetTensorMutableData<int32_t>();
				showMat = cv::Mat(384, 384, CV_32SC1, dataInt32);
				break;
			case ONNX_TENSOR_ELEMENT_DATA_TYPE_UNDEFINED:
			case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT8:
			case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT8:
			case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT16:
			case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT16:
			case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT64:
			case ONNX_TENSOR_ELEMENT_DATA_TYPE_STRING:
			case ONNX_TENSOR_ELEMENT_DATA_TYPE_BOOL:
			case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16:
			case ONNX_TENSOR_ELEMENT_DATA_TYPE_DOUBLE:
			case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT32:
			case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT64:
			case ONNX_TENSOR_ELEMENT_DATA_TYPE_COMPLEX64:
			case ONNX_TENSOR_ELEMENT_DATA_TYPE_COMPLEX128:
			case ONNX_TENSOR_ELEMENT_DATA_TYPE_BFLOAT16:
				return CError(PAMVALUE, "模型输出类型算法暂未开发，返回NG！");
			default:
				return CError(PAMVALUE, "位置模型参数类型，返回NG！");
			}*/

			return 0;
		}
		catch (const std::exception& e)
		{
			return CError(UNEXCEPTION, e.what());
		}
	}

	CError OnnxSegment::detect(const cv::Mat& input, cv::Mat& output)
	{
		using namespace std;
		Ort::Env env(ORT_LOGGING_LEVEL_WARNING, name().c_str());
		CError err;

		if (!d->iniFlag)
			return CError(AIINITIAL, "Ai初始化未成功，无法开启检测！");

		if (!d->isSetPam)
			return CError(EMPTYOBJ, "模型参数为初始化，无法开启检测！");

		err = preImage(input);
		CHECKERR(err);

		err = inferImage(output);
		CHECKERR(err);

		return 0;
	}
}