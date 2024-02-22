#pragma once

#include <iostream>
#include <typeindex>
#include <memory>
#include <stdexcept>
#include <any>
#include <sstream>

namespace Node
{
    class CDataBase {
    public:
        virtual ~CDataBase() = default;
    };

    template <typename T>
    class CVariantImpl : public CDataBase {
    public:
        T value;

        CVariantImpl(const T& val) : value(val) {}

        CVariantImpl(const CVariantImpl<T>& other) : value(other.value) {}
    };

    class CVariant {

    public://构造函数
        CVariant();

        template <typename T>
        CVariant(const T& val): data(std::make_shared<CVariantImpl<T>>(val)), type(typeid(T)) {}

        CVariant(const CVariant& other);

        CVariant(const char* val) : data(std::make_shared<CVariantImpl<const char*>>(val)), type(typeid(const char*)) {}

    public://符号重载
        CVariant& operator=(const CVariant& other);

        template <typename T>
        CVariant& operator=(const T& val) {
            data = std::make_shared<CVariantImpl<T>>(val);
            type = typeid(T);
            return *this;
        }

        // 重载 == 操作符
        bool operator==(const CVariant& other) const;

    public://属性值
        inline std::string typeName() { return type.name(); }

        bool isValid() const;

        bool isEmpty() const;

        int toInt() const;

        bool toBool() const;

        float toFloat() const;

        double toDouBle() const;

        std::string toString() const;

        template <typename T>
        static CVariant fromString(const std::string& str);

    public:
        friend std::ostream& operator<<(std::ostream& os, const CVariant& obj);

        //取值方法
        template <typename T>
        T value() const {
            if (type != typeid(T)) {
                throw std::invalid_argument("使用CVariant::value失败，保存type和模版T不对应");
            }
            return static_cast<CVariantImpl<T>*>(data.get())->value;
        }
    private:
        //共享指针
        std::shared_ptr<CDataBase> data;
        std::type_index type;
    };
}
