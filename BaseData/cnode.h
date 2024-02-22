#pragma once

#include<string>
#include<vector>
#include<map>
#include"cvariant.h"
#include "csettings.h"

namespace Node
{
    enum Id
    {
        ID_CNode = 0,
        ID_CGroup = 1,

        ID_CVariant = 100,
        ID_CBool = 101,
        ID_CInt = 102,
        ID_CReal = 103,
        ID_CReal2 = 104,
        ID_CString = 105,

        ID_CPoint = 200,
        ID_CSize = 201,
        ID_CRect = 202,

        ID_CList = 300,

        ID_CCombo = 400,
        ID_CStringCombo = 401,
        ID_CCheck = 402,

        ID_CFile = 450,
        ID_CImage = 500,

        ID_User = 50000
    };

    enum ObtainKey {
        Obtain_Default = 0,

        Obtain_User = 50000
    };

    enum ExecuteKey {
        Execute_Default = 0,

        Execute_User = 50000
    };

    enum Operation : unsigned int {
        Opt_None = 0x00000000,
        Opt_All = 0xffffffff,
        Opt_Obtain = 0x00000001,
        Opt_Execute = 0x00000002,
        Opt_Save = 0x00000004,
        Opt_Load = 0x00000008,
    };

    enum WidgetFlag {
        WidgetVisible = 0x0001,
        WidgetEnabled = 0x0002,
        WidgetHideChildren = 0x0004,
    };

    enum SaveValueChangedLogFlag {
        SaveValueChangedLogNull = 0x0001,
        SaveValueChangedLogEnable = 0x0002,
        SaveValueChangedLogDisable = 0x0004,
    };

    class CNode;

    enum CNodeNotifyType
    {
        Notify_DataChanged,
        Notify_FlagsChanged,        //arg: QPair<int, int>(oldflags,newflags)
        Notify_HideChildrenChanged,
        Notify_BeginInsertChild,
        Notify_EndInsertChild,
        Notify_BeginRemoveChild,
        Notify_EndRemoveChild
    };

    class CNodePrivate;

    class CNode
    {
    public:
        enum {
            Key_Value = 0,
            Key_Default = 1,
            Key_LastValue = 2,
            Key_Min = 3,
            Key_Max = 4,
        };

        static const char* staticClassName() { return "CNode"; }
        virtual const char* className() { return (*this).name().c_str(); }
        const static int id = ID_CNode;

    public:
        explicit CNode(CNode* parent = nullptr);
        explicit CNode(const std::string& name, const std::string& des = std::string(), CNode* parent = nullptr);
    
        CNode(const CNode&) = default;
        CNode(CNode&&) = default;
        CNode& operator=(const CNode&) = default;
        CNode& operator=(CNode&&) = default;
    
        virtual ~CNode();
        virtual bool isGroup() const { return false; }
    
        std::string name() const;
        void setName(const std::string& name);
    
        std::string description() const;
        void setDescription(const std::string& des);
        
        CNode* parent() const;
        void setParent(CNode* parent);
    
        int index() const;
    
        std::vector<CNode*> children() const;

        int getChildIndex(CNode* node);
        int childCount() const;

        CNode* child(int index) const;
        CNode* child(const std::string& name) const;
        CNode* firstChild()const;
        CNode* lastChild()const;

        void insertChild(int index, CNode* node);
        void prependChild(CNode* node);
        void appendChild(CNode* node);

        CNode* takeChild(CNode* node);
        CNode* takeChildAt(int index);
        CNode* takeFirstChild();
        CNode* takeLastChild();
    
        /*
        void setWidgetFlag(WidgetFlag flag, bool value = true);
        void setWidgetFlags(int widgetFlags);
        int widgetFlags() const;

        void setSaveValueChangedLogFlag(SaveValueChangedLogFlag flag);
        SaveValueChangedLogFlag saveValueChangedLogFlag() const;

        void setOperation(Operation opt, bool enable);
        void setOperations(int opts);
        int operations() const;
        */

        virtual std::string typeString() const;
        virtual std::string text() const;

        std::string path() const;

        CSettings* settings() const;
        void setSettings(CSettings* st);
        std::string resoureDirPath() const;

        void setSettingsEnable(bool enable);
        bool isSettingsEnable()const;

        bool contains(int role) const;

        std::map<int, CVariant> datas() const;
        virtual CVariant data(int key, const CVariant& defaultValue = CVariant()) const;
        virtual void setData(int key, const CVariant& val, bool checkEqual = true);

        virtual void load();    //!<加载
        virtual void save();    //!<保存
        virtual void remove();  //!<移除

        //void obtain(int key = Obtain_Default, const QVariant& arg = QVariant());  //!<触发获取信号
        //void execute(int key = Execute_Default, const QVariant& arg = QVariant()); //!<触发执行信号
        //virtual QWidget* widget(QWidget* parent) { Q_UNUSED(parent) return nullptr; }//预留

        void deleteLater();

    protected:
        virtual void customLoad(const CVariant& v);
        virtual CVariant customSave();
        virtual void customRemove();
        //void update(int key);
        //void notify(int type, const void* val = nullptr);

    private:
        void setIndex(int idx);

        CNodePrivate* d;
        friend class CNodePrivate;
    };

    class CGroup : public CNode
    {
    public:
        static const char* staticClassName() { return "CGroup"; }
        const char* className() override { return typeid(*this).name(); }
        const static int id = ID_CGroup;

    public:
        using CNode::CNode;
        bool isGroup() const override { return true; }
        std::string typeString() const override { return "Group"; }
    };

    template <class T>
    class CTemplate : public CNode
    {
    public:
        explicit CTemplate(CNode* parent = nullptr) : CNode(parent) {
            setData(Key_Default, T());
        }

        explicit CTemplate(const std::string& name, const std::string& des, CNode* parent = nullptr) :
            CNode(name, des, parent) {
            setData(Key_Default, T());
        }

        explicit CTemplate(const std::string& name, const std::string& des, const T& defaultValue, CNode* parent = nullptr) :
            CNode(name, des, parent) {
            setData(Key_Default, defaultValue);
        }

        std::string typeString() const override { return typeid(T).name(); }
        inline operator T() const { return value(); }

        inline T value() const {
            std::string haha;
            if (contains(Key_Value)) {
                haha = data(Key_Value).toString();
            }
            else {
                haha = data(Key_Default).toString();
            }
            std::stringstream ss(haha);
            T result;
            ss >> result;
            return result;
        }
        
        void setValue(const T& val) {
            if (val == value())
                return;
            setData(Key_LastValue, value());
            setData(Key_Value, val);
        }
        
        inline T lastValue()const { return contains(Key_LastValue) ? data(Key_LastValue).value<T>() : value(); }
        inline T defaultValue() const { return data(Key_Default).value<T>(); }
        inline void setDefaultValue(const T& val) { setData(Key_Default, val); }
    protected:
        void customLoad(const CVariant& v) override { setData(Key_Value, v); }
        CVariant customSave() override { return data(Key_Value); }
    };

    template <class T>
    class CRangeTemplate : public CTemplate<T>
    {
    public:
        using CTemplate<T>::CTemplate;

        void setValue(const T& val) {
            if (this->contains(this->Key_Min) && val < minimum())return;
            if (this->contains(this->Key_Max) && val > maximum())return;
            CTemplate<T>::setValue(val);
        }

        inline T minimum() const { return this->data(this->Key_Min).template value<T>(); }
        inline void setMinimum(const T& min) { this->setData(this->Key_Min, min); }
        inline T maximum() const { return this->data(this->Key_Max).template value<T>(); }
        inline void setMaximum(const T& max) { this->setData(this->Key_Max, max); }

        inline void setRange(const T& min, const T& max) { this->setData(this->Key_Min, min); this->setData(this->Key_Max, max); }
    };

    class CAbstractList : public CNode
    {
    public:
        static const char* staticClassName() { return "CAbstractList"; }
        const char* className() override { return typeid(*this).name(); }
        const static int id = ID_CList;

    public:
        using CNode::CNode;
        std::string typeString() const override { return "List"; };
        void resize(int s);
        int size()const;
        void appendOne();
        void removeOne();
    protected:
        void customLoad(const CVariant& v) override;
        CVariant customSave() override;
        virtual CNode* create() = 0;
    };

    template<class T>
    class CList : public CAbstractList
    {
    public:
        using CAbstractList::CAbstractList;
        std::string text() const override { return "[" + std::to_string(size()) + "]"; }
        virtual CNode* create() override { return new T(this); }
        T& value(int idx) const { return *static_cast<T*>(child(idx)); }
        T& operator [](int idx) { return *static_cast<T*>(child(idx)); }
    };

    class CBool : public CTemplate<bool>
    {
    public:
        static const char* staticClassName() { return "CBool"; }
        const char* className() override { return typeid(*this).name(); }
        const static int id = ID_CBool;

    public:
        using CTemplate<bool>::CTemplate;
        std::string text() const override { return value() ? "true" : "false"; }
        inline CBool& operator=(const bool& val) { setValue(val); return *this; }
    };

    //整数类型
    class CInt : public CRangeTemplate<int>
    {
    public:
        static const char* staticClassName() { return "CInt"; }
        const char* className() override { return typeid(*this).name(); }
        const static int id = ID_CInt;
    public:
        using CRangeTemplate<int>::CRangeTemplate;
        std::string text() const override { return  std::to_string(value()); }
        int minimum() const { return data(Key_Min, -10000).toInt(); }
        int maximum() const { return data(Key_Max, 10000).toInt(); }
        inline CInt& operator=(const int& val) { setValue(val); return *this; }
    };

    //浮点数类型
    class CReal : public CRangeTemplate<double>
    {
    public:
        static const char* staticClassName() { return "CReal"; }
        const char* className() override { return typeid(*this).name(); }
        const static int id = ID_CReal;
    public:
        using CRangeTemplate<double>::CRangeTemplate;
        std::string text() const override { return std::to_string(value()); }
        double minimum() const { return data(Key_Min, double(-100000)).toDouBle(); }
        double maximum() const { return data(Key_Max, double(100000)).toDouBle(); }
        inline CReal& operator=(const double& val) { setValue(val); return *this; }
    };

    class CString : public CNode
    {
    public:
        static const char* staticClassName() { return "CString"; }
        const char* className() override { return typeid(*this).name(); }
        const static int id = ID_CString;

    public:
        explicit CString(CNode* parent = nullptr);
        explicit CString(const std::string& name, const std::string& des = std::string(), CNode* parent = nullptr);
        explicit CString(const std::string& name, const std::string& des, const std::string& defaultValue, CNode* parent = nullptr);
        explicit CString(const std::string& name, const std::string& des, const std::string& defaultValue, const std::string& mask, CNode* parent = nullptr);
        std::string text() const override;
        void setText(const std::string& txt);
        std::string mask() const { return m_mask; }
    private:
        std::string m_mask;
    };
}