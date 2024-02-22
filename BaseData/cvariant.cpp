#include "pch.h"
#include "cvariant.h"
#include <string>
#include <exception>

namespace Node
{
    using namespace std;

    CVariant::CVariant() : data(nullptr), type(typeid(void *)){}

    CVariant::CVariant(const CVariant& other) : type(other.type)
    {
        if (type == typeid(int)) {
            int otherValue = static_cast<CVariantImpl<int>*>(other.data.get())->value;
            data = std::make_shared<CVariantImpl<int>>(otherValue);
        }
        else if (type == typeid(double)) {
            int otherValue = static_cast<CVariantImpl<double>*>(other.data.get())->value;
            data = std::make_shared<CVariantImpl<double>>(otherValue);
        }
        else if (type == typeid(float)) {
            int otherValue = static_cast<CVariantImpl<float>*>(other.data.get())->value;
            data = std::make_shared<CVariantImpl<float>>(otherValue);
        }
        else if (type == typeid(bool)) {
            int otherValue = static_cast<CVariantImpl<bool>*>(other.data.get())->value;
            data = std::make_shared<CVariantImpl<bool>>(otherValue);
        }
        else if (type == typeid(string)) {
            std::string& otherValue = static_cast<CVariantImpl<std::string>*>(other.data.get())->value;
            data = std::make_shared<CVariantImpl<std::string>>(otherValue);
        }
        else if (type == typeid(const char*)) {
            const char* otherValue = static_cast<CVariantImpl<const char*>*>(other.data.get())->value;
            data = std::make_shared<CVariantImpl<const char*>>(otherValue);
        }
        else {
            data = nullptr;
        }
    }

    CVariant& CVariant::operator=(const CVariant& other)
    {
        if (this != &other) {
            // 如果 other 有数据
            if (other.data) {
                // 根据 other 的类型进行不同的处理
                if (other.type == typeid(int)) {
                    int otherValue = static_cast<CVariantImpl<int>*>(other.data.get())->value;
                    data = std::make_shared<CVariantImpl<int>>(otherValue);
                }
                else if (other.type == typeid(double)) {
                    double otherValue = static_cast<CVariantImpl<double>*>(other.data.get())->value;
                    data = std::make_shared<CVariantImpl<double>>(otherValue);
                }
                else if (other.type == typeid(float)) {
                    float otherValue = static_cast<CVariantImpl<float>*>(other.data.get())->value;
                    data = std::make_shared<CVariantImpl<float>>(otherValue);
                }
                else if (other.type == typeid(bool)) {
                    bool otherValue = static_cast<CVariantImpl<bool>*>(other.data.get())->value;
                    data = std::make_shared<CVariantImpl<bool>>(otherValue);
                }
                else if (other.type == typeid(std::string)) {
                    std::string otherValue = static_cast<CVariantImpl<std::string>*>(other.data.get())->value;
                    data = std::make_shared<CVariantImpl<std::string>>(otherValue);
                }
                else if (other.type == typeid(const char*)) {
                    const char* otherValue = static_cast<CVariantImpl<const char*>*>(other.data.get())->value;
                    data = std::make_shared<CVariantImpl<const char*>>(otherValue);
                }
                else {
                    // 其他类型处理，根据需要添加
                    data = nullptr;
                }
                // 复制类型信息
                type = other.type;
            }
            else {
                // 如果 other 没有数据，将当前对象置为空
                data.reset();
                type = typeid(void);
            }
        }
        return *this;
    }

    bool CVariant::operator==(const CVariant& other) const {
        if (type != other.type) {
            return false; // Different types, not equal
        }

        if (type == typeid(int)) {
            return value<int>() == other.value<int>();
        }
        else if (type == typeid(double)) {
            return value<double>() == other.value<double>();
        }
        else if (type == typeid(float)) {
            return value<float>() == other.value<float>();
        }
        else if (type == typeid(bool)) {
            return value<bool>() == other.value<bool>();
        }
        else if (type == typeid(string)) {
            return value<string>() == other.value<string>();
        }
        else if (type == typeid(const char*)) {
            return value<const char*>() == other.value<const char*>();
        }
        else {
            throw std::invalid_argument("CVariant::operator== failed! ");
        }


        return true;
    }

    bool CVariant::isValid() const
    {
        return type == typeid(int) || type == typeid(double) || type == typeid(float) 
               || type == typeid(bool) || type == typeid(string) ||type == typeid(const char*);
    }

    bool CVariant::isEmpty() const
    {
        return toString().empty();
    }

    int CVariant::toInt() const
    {
        if (type == typeid(int)) {
            return value<int>();
        }
        else if (type == typeid(double)) {
            return static_cast<int>(value<double>());
        }
        else if (type == typeid(float)) {
            return static_cast<int>(value<float>());
        }
        else if (type == typeid(bool)) {
            return static_cast<int>(value<bool>());
        }
        else if (type == typeid(std::string)) {
            return 0;
        }
        else if (type == typeid(const char*)) {
            return 0;
        }
        else {
            throw std::invalid_argument("CVariant::toInt() faile ");
        }
    }

    bool CVariant::toBool() const 
    {
        if (type == typeid(int)) {
            return static_cast<bool>(value<int>());
        }
        else if (type == typeid(float)) {
            return static_cast<bool>(value<float>());
        }
        else if (type == typeid(double)) {
            return static_cast<bool>(value<double>());
        }
        else if (type == typeid(bool)) {
            return value<bool>();
        }
        else if (type == typeid(std::string)) {
            return static_cast<bool>(value<std::string>().size());
        }
        else if (type == typeid(const char*)) {
            return static_cast<bool>(std::string(value<const char*>()).size());
        }
        else {
            throw std::invalid_argument("CVariant::toBool() faile ");
        }
    }

    float CVariant::toFloat() const {
        if (type == typeid(int)) {
            return static_cast<float>(value<int>());
        }
        else if (type == typeid(float)) {
            return value<float>();
        }
        else if (type == typeid(double)) {
            return static_cast<float>(value<double>());
        }
        else if (type == typeid(bool)) {
            return static_cast<float>(value<bool>());
        }
        else if (type == typeid(std::string)) {
            return 0;
        }
        else if (type == typeid(const char*)) {
            return 0;
        }
        else {
            throw std::invalid_argument("CVariant::toFloat() faile ");
        }
    }

    double CVariant::toDouBle() const
    {
        if (type == typeid(int)) {
            return static_cast<double>(value<int>());
        }
        else if (type == typeid(float)) {
            return static_cast<double>(value<float>());
        }
        else if (type == typeid(double)) {
            return value<double>();
        }
        else if (type == typeid(bool)) {
            return static_cast<double>(value<bool>());
        }
        else if (type == typeid(std::string)) {
            return 0;
        }
        else if (type == typeid(const char*)) {
            return 0;
        }
        else {
            throw std::invalid_argument("CVariant::toDouBle() faile ");
        }
    }

    std::string CVariant::toString() const
    {
        if (type == typeid(int)) {
            return std::to_string(value<int>());
        }
        else if (type == typeid(double)) {
            return std::to_string(value<double>());
        }
        else if (type == typeid(float)) {
            return std::to_string(value<float>());
        }
        else if (type == typeid(bool)) {
            return value<bool>() ? "1" : "0";
        }
        else if (type == typeid(std::string)) {
            return value<std::string>();
        }
        else if (type == typeid(const char*)) {
            return static_cast<CVariantImpl<const char*>*>(data.get())->value;
        }
        else {
            throw std::invalid_argument("CVariant::toString() faile ");
        }
    }

    template <typename T>
    CVariant CVariant::fromString(const std::string& str) {
        CVariant result;
        std::istringstream iss(str);
        T value;
        iss >> value;

        if (iss.fail() || !iss.eof()) {
            throw std::invalid_argument("Failed to convert string to CVariant");
        }

        result = CVariant(value);
        return result;
    }

    std::ostream& operator<<(std::ostream& os, const CVariant& obj)
    {
        os << obj.toString();
        return os;
    }

}