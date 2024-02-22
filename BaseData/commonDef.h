#pragma once

#ifndef CONSTIMG
#define CONSTIMG const HalconCpp::HObject&
#endif

#ifndef RHOBJ
#define RHOBJ HalconCpp::HObject&
#endif

#ifndef HOBJ
#define HOBJ HalconCpp::HObject
#endif

#ifndef HTUPLE
#define HTUPLE HalconCpp::HTuple
#endif

#ifndef PIVAL
#define PIVAL std::acos(-1)
#endif

#ifndef RAD90
#define RAD90 std::acos(-1)/2.0
#endif

typedef const int cint;

#pragma region err process
#define CHECKERR(a) \
    if(a.isWrong()){ \
        std::cout << "err msg():" << a.msg();\
        return a; \
    }

#define RETURNERR(a) \
    if(a.isWrong()){ \
        return a; \
    }
#pragma endregion

#pragma region Halcon state check
#define CHECKEMPIMG(img, info) \
    if(Algorithm::objIsEmpty(img).isWrong()) \
        return CError(EMPTYOBJ, info);

#define CHECKEMPTUPLE(tup, info) \
if(Algorithm::tupleisEmpty(tup).isWrong())\
    return CError(EMPTYOBJ, info);

#define CHECKTHREVALUE(value, info) \
    if(value < 0 || value > 255) \
        return CError(PAMVALUE, info);
#pragma endregion


#define CLEAR_PTR(ptr)\
    if (ptr) {\
        delete ptr;\
        ptr = nullptr;\
    }

enum errIndex {
    OK = 0,
    NG,

    PAMVALUE,           //参数设置错误
    EMPTYOBJ,           //获取空区域
    PAMREAD,            //参数读取错误
    IMGTYPE,            //图像类型错误
    FILEPATH,
    AIINITIAL,

    UNEXCEPTION = 99,   //未知错误
    NOCHECK             //不检测直接返回
};

struct AlgoMap
{
    const char* algoName;         //算法名称
    const char* algoDesc;         //
    const char* description;      //描述
};