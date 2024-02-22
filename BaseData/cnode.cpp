#include "pch.h"
#include "cnode.h"
#include <variant>
#include <sys/stat.h>

namespace Node {
    using namespace std;

    class CNodePrivate
    {

        //void writeChangeLog(QSettings* st, const QString& key, const QVariant& newData) {
        //    //qDebug()<<key<<newData;
        //    QVariant oldData = st->value(key);
        //    if (oldData.isValid()) {
        //        if (!oldData.convert(newData.type()))return;
        //        if (oldData == newData)return;
        //    }
        //    CLog::instance()->write(key, oldData, newData);
        //}

    public:
        map<string, void*> meta;
        bool settingsEnable : 1;
        SaveValueChangedLogFlag saveValueChangedLogFlag;

        int widgetFlags;
        int operations;
        CSettings* settings;
        //CSignalsObject* signalsObject;
        //CNotifyObject* notifyObject;
        CNode* parent;
        int index;
        vector<CNode*> children;
        map<int, CVariant> datas;
    };

    CNode::CNode(CNode* parent) :
        d(new CNodePrivate)
    {
        d->widgetFlags = WidgetVisible | WidgetEnabled;
        d->settings = nullptr;
        //d->signalsObject = nullptr;
        //d->notifyObject = nullptr;
        d->parent = nullptr;
        d->index = -1;
        d->operations = Opt_Load | Opt_Save;

        d->settingsEnable = true;
        d->saveValueChangedLogFlag = SaveValueChangedLogNull;
        if (parent)setParent(parent);
    }

    CNode::CNode(const std::string& name, const std::string& des, CNode* parent):
        CNode(parent)
    {
        if (!name.empty())setName(name);
        if (!des.empty())setDescription(des);
    }


    CNode::~CNode()
    {
        //if (d->signalsObject)d->signalsObject->deleteLater();
        //if (d->notifyObject)d->notifyObject->deleteLater();
        if (d->parent) {
            d->parent->takeChild(this);
        }
        auto vector = d->children;
        for (auto i : vector)delete i;
        delete d;
    }

    //属性设置
    void CNode::setName(const string& name)
    {
        if (name.empty())return;

        d->meta["name"] = static_cast<void*>(new string(name));
    }

    string CNode::name() const
    {
        // 检查键是否存在
        auto it = d->meta.find("name");

        if (it != d->meta.end()) {
            string* strPtr = static_cast<string*>(it->second);
            return *strPtr;
        }
        else {
            return "[" + std::to_string(d->index) + "]";
        }
    }

    void CNode::setDescription(const string& des) {
        if (des.empty())return;

        d->meta["description"] = static_cast<void*>(new string(des));
    }

    string CNode::description() const
    {
        // 检查键是否存在
        auto it = d->meta.find("description");
        if (it != d->meta.end()) {
            string* strPtr = static_cast<string*>(it->second);
            return *strPtr;
        }
        else {
            return "[" + std::to_string(d->index) + "]";
        }
    }

    CNode* CNode::parent() const
    {
        return d->parent;
    }

    void CNode::setParent(CNode* parent)
    {
        if (d->parent == parent)return;
        if (d->parent)d->parent->takeChild(this);
        if (parent)parent->appendChild(this);
    }

    int CNode::index() const
    {
        return d->index;
    }

    void CNode::setIndex(int idx)
    {
        d->index = idx;
    }

    //子节点操作
    std::vector<CNode*> CNode::children() const
    {
        return d->children;
    }

    int CNode::getChildIndex(CNode* node)
    {
        if (!node) return -1;

        if (node->parent() != this) return -1;

        auto it = std::find(d->children.begin(), d->children.end(), node);

        if (it != d->children.end())
            return std::distance(d->children.begin(), it);

        return -1;
    }

    int CNode::childCount() const
    {
        return d->children.size();
    }

    CNode* CNode::child(int index) const
    {
        if (index >= d->children.size())
            return nullptr;

        return d->children[index];
    }

    CNode* CNode::child(const string& name) const
    {
        for (CNode* i : d->children) {
            if (i->name() == name) return i;
        }
        return nullptr;
    }

    CNode* CNode::firstChild() const
    {
        int size = d->children.size();
        return size > 0 ? d->children[0] : nullptr;
    }

    CNode* CNode::lastChild() const
    {
        int size = d->children.size();
        return size > 0 ? d->children[size - 1] : nullptr;
    }

    //添加操作
    void CNode::insertChild(int index, CNode* node)
    {
        if (!node)return;

        if (node->parent() == this) {
            int idx = getChildIndex(node);
            if (idx == index || idx == -1) return;
            takeChildAt(idx);
        }
        else if (node->parent()) {
            node->parent()->takeChild(node);
        }

        if (index < 0) index = 0;
        else if (index > d->children.size())index = d->children.size();
        
        //notify(Notify_BeginInsertChild, index);
        
        d->children.insert(d->children.begin() + index, node);
        
        node->d->parent = this;
        int i = 0;
        for (auto n : d->children) { n->setIndex(i); i++; }
        // node->save();
        //notify(Notify_EndInsertChild);
        auto n = this;
        while (1) {
            if (!n->isGroup())break;
            //if (n->d->signalsObject)emit n->d->signalsObject->childChanged();
            if (!n->parent())break;
            n = n->parent();
        }
    }

    void CNode::prependChild(CNode* node)
    {
        insertChild(0, node);
    }

    void CNode::appendChild(CNode* node)
    {
        insertChild(d->children.size(), node);
    }

    //移除操作
    CNode* CNode::takeChild(CNode* node)
    {
        auto it = std::find(d->children.begin(), d->children.end(), node);
        if (it != d->children.end()) {
            // 元素找到，输出索引
            int index = std::distance(d->children.begin(), it);
            return takeChildAt(index);
        }
        else {
            return nullptr;
        }
    }

    CNode* CNode::takeChildAt(int index)
    {
        if (index < 0 || index >= d->children.size())return nullptr;
        //notify(Notify_BeginRemoveChild, index);
        //d->children.value(index)->remove();

        CNode* node = d->children[index];
        d->children.erase(d->children.begin() + index);

        node->d->parent = nullptr;
        node->setIndex(-1);
        int i = 0;
        for (auto n : d->children) { n->setIndex(i); i++; }
        //notify(Notify_EndRemoveChild);

        auto n = this;
        while (1) {
            if (!n->isGroup())break;
            //if (n->d->signalsObject)emit n->d->signalsObject->childChanged();
            if (!n->parent())break;
            n = n->parent();
        }
        return node;
    }

    CNode* CNode::takeFirstChild()
    {
        return takeChildAt(0);
    }

    CNode* CNode::takeLastChild()
    {
        return takeChildAt(d->children.size() - 1);
    }

    string CNode::typeString() const
    {
        return string();
    }

    string CNode::text() const
    {
        return string();
    }

    //数据操作
    string CNode::path() const
    {
        const CNode* node = const_cast<const CNode*>(this);
        std::string pathStr;
        vector<string> list;

        while (node) {
            list.push_back(node->name().empty() ? std::to_string(index()) : node->name());
            node = node->parent();
        }

        std::reverse(list.begin(), list.end());
        for (const auto& str : list) {
            pathStr += (pathStr.empty() ? "" : "->") + str;
        }

        return pathStr;
    }

    string CNode::resoureDirPath() const
    {
        auto st = settings();

        if (!st) return string();

        return string();
    }

    void CNode::setSettingsEnable(bool enable)
    {
        d->settingsEnable = enable;
    }

    bool CNode::isSettingsEnable() const
    {
        return d->settingsEnable;
    }

    bool CNode::contains(int role)const
    {
        auto it = d->datas.find(role);

        if (it != d->datas.end()) {
            return true;
        }
        else {
            return false;
        }
    }

    //setting设置
    CSettings* CNode::settings() const
    {
        auto node = this;
        while (true) {
            if (node->d->settings)return node->d->settings;
            if (node->parent())node = node->parent();
            else return nullptr;
        }
    }

    void CNode::setSettings(CSettings* st)
    {
        d->settings = st;
        if (!d->settings) return;
    }

    //数据保存
    map<int, CVariant> CNode::datas() const
    {
        return d->datas;
    }

    CVariant CNode::data(int key, const CVariant& defaultValue) const
    {
        auto it = d->datas.find(key);

        if (it != d->datas.end())
            return it->second;
        else
            return defaultValue;
    }

    void CNode::setData(int key, const CVariant& val, bool checkEqual)
    {
        auto it = d->datas.find(key);
        // 检查是否找到了键
        if (it != d->datas.end())
            if (checkEqual && it->second == val) return;
        d->datas[key] = val;
    }

    void CNode::load()
    {
        if (isSettingsEnable()) {
            auto st = settings();
            if (st) {
                auto val = st->value(path());
                if(!val.isEmpty())
                    customLoad(val);
            }
        }

        for (auto child : children()) 
            child->load();
    }

    void CNode::save()
    {
        if (isSettingsEnable()) {
            auto st = settings();
            if (st) {
                auto v = customSave();
                if (v.isValid()) {
                    std::string key = path();

                    auto node = this;
                    bool logSaveEnable = true;
                    while (true) {
                        if (node->d->saveValueChangedLogFlag != SaveValueChangedLogNull) {
                            if (node->d->saveValueChangedLogFlag == SaveValueChangedLogEnable) {
                                logSaveEnable = true;
                                break;
                            }
                            else if (node->d->saveValueChangedLogFlag == SaveValueChangedLogDisable) {
                                logSaveEnable = false;
                                break;
                            }
                        }
                        if (node->d->saveValueChangedLogFlag == SaveValueChangedLogNull) {
                            if (node->parent()) {
                                node = node->parent();
                            }
                            else {
                                break;
                            }
                        }

                    }
                    /*if (logSaveEnable) {
                        d->writeChangeLog(st, key, v);
                    }*/
                    st->setValue(key, v);
                }
                //else qWarning() << path() << u8"保存失败:值无效";
            }
            //else qWarning() << path() << u8"保存失败:未指定文件";
        }
        for (auto child : children()) child->save();
    }

    void CNode::remove()
    {
        if (isSettingsEnable()) {
            auto st = settings();
            if (st) {
                customRemove();
                st->remove(path());
            }
        }
        for (auto child : children())child->remove();
    }

    void CNode::deleteLater()
    {
        //QTimer::singleShot(0, [this]() {delete this; });
    }

    void CNode::customLoad(const CVariant& v)
    {
        if (v.isEmpty()) 
            return;

        cout << name() << " " << v << endl;
        setData(Key_Value, v);
    }

    CVariant CNode::customSave()
    {
        return data(Key_Value);
    }

    void CNode::customRemove()
    {

    }

    void CAbstractList::resize(int s)
    {
        int cnt = childCount();
        if (s > cnt)for (int i = cnt; i < s; i++) appendOne();
        if (s < cnt)for (int i = s; i < cnt; i++) removeOne();
    }

    int CAbstractList::size() const
    {
        return childCount();
    }

    void CAbstractList::appendOne()
    {
        appendChild(create());
    }

    void CAbstractList::removeOne()
    {
        lastChild()->remove();
        delete takeLastChild();
    }

    void CAbstractList::customLoad(const CVariant& v)
    {
        resize(v.toInt());
    }

    CVariant CAbstractList::customSave()
    {
        return childCount();
    }

    CString::CString(CNode* parent) :
        CNode(parent)
    {

    }

    CString::CString(const string& name, const string& des, CNode* parent) :
        CNode(name, des, parent)
    {

    }

    CString::CString(const string& name, const string& des, const string& defaultValue, CNode* parent) :
        CString(name, des, defaultValue, string(), parent)
    {

    }

    CString::CString(const string& name, const string& des, const string& defaultValue, const string& mask, CNode* parent) :
        CNode(name, des, parent), m_mask(mask)
    {
        if (!defaultValue.empty()) setData(Key_Default, defaultValue);
    }

    string CString::text() const
    {
        return data(contains(Key_Value) ? Key_Value : Key_Default).toString();
    }

    void CString::setText(const string& txt)
    {
        setData(Key_Value, txt);
    }




}

