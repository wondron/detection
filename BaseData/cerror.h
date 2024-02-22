#pragma once

#include <string>
#include "commonDef.h"
#include <iostream>

class CError
{
public:
    CError(int i = 0);
    CError(int i, std::string info);
    ~CError() {}

    /**
     * @brief msg： 获取错误信息
     */
    std::string msg() const { return errInfo; }

    /**
     * @brief code : 获取错误序列
     */
    int code() const { return errIndex; }

    /**
     * @brief setInfo: 设置错误信息
     */
    void setInfo(const std::string& info);

    /**
    * @brief setInfo: 设置错误信息
    */
    void setCode(const int);

    /**
     * @brief isWrong: 判定结果是都正确
     */
    bool isWrong() const { return errIndex != 0; }

    operator int();//用于int类型的直接赋值

public:
    friend std::ostream& operator<<(std::ostream& os, const CError& var);

private:
    int errIndex;
    std::string errInfo = "";
};

