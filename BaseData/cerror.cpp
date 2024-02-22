#include "pch.h"
#include "cerror.h"

using namespace std;

std::ostream& operator<<(std::ostream& os, const CError& var) {
    
    os << "[errinfo]: " << var.msg() << endl;
    return os;
}

CError::CError(int i) : errIndex(i), errInfo("none")
{
}

CError::CError(int i, string info) : errIndex(i), errInfo(info)
{

}


void CError::setInfo(const string& info)
{
    errInfo = info;
}

void CError::setCode(const int code)
{
    errIndex = code;
}

CError::operator int()
{
    return errIndex;
}



