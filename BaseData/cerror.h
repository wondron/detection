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
     * @brief msg�� ��ȡ������Ϣ
     */
    std::string msg() const { return errInfo; }

    /**
     * @brief code : ��ȡ��������
     */
    int code() const { return errIndex; }

    /**
     * @brief setInfo: ���ô�����Ϣ
     */
    void setInfo(const std::string& info);

    /**
    * @brief setInfo: ���ô�����Ϣ
    */
    void setCode(const int);

    /**
     * @brief isWrong: �ж�����Ƕ���ȷ
     */
    bool isWrong() const { return errIndex != 0; }

    operator int();//����int���͵�ֱ�Ӹ�ֵ

public:
    friend std::ostream& operator<<(std::ostream& os, const CError& var);

private:
    int errIndex;
    std::string errInfo = "";
};

