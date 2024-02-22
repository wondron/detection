#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include "cerror.h"
#include "cvariant.h"

namespace Node {
    class CSettings {
    public:
        CSettings();

        // ���������б���������Ϣ
        ~CSettings() {}

        CSettings(std::string path);

        void setValue(const std::string& key, const CVariant& value);

        CVariant value(const std::string& key, const CVariant& defaultValue = "") const;

        void setFilePath(const std::string& filePath);

        const std::string& filePath() { return m_filePath; }

        CError saveToFile() const;

    //��������
    public:
        void remove(const std::string& key);

        void clear();

    private:
        std::map<std::string, std::string> m_settings;
        void loadFromFile();
        std::string m_filePath;
    };
}