#include "pch.h"
#include "csettings.h"

namespace Node
{
    using namespace std;

    CSettings::CSettings() :CSettings("settings.txt") {}

    CSettings::CSettings(std::string path) : m_filePath(path)
    {
        loadFromFile();
    }

    void CSettings::setValue(const std::string& key, const CVariant& value)
    {
        m_settings[key] = value.toString();
    }

    CVariant CSettings::value(const std::string& key, const CVariant& defaultValue) const
    {
        auto it = m_settings.find(key);
        if (it != m_settings.end()) {
            //cout << "saved value :\t\t" << it->second << endl;
            return it->second;
        }
        else {
            //cout << "default value :\t\t" << defaultValue << endl;
            return defaultValue;
        }
    }

    void CSettings::setFilePath(const std::string& filepath)
    {
        if (filepath == m_filePath) return;

        m_filePath = filepath;

        loadFromFile();
    }

    CError CSettings::saveToFile() const {
        std::ofstream file(m_filePath, std::ios::trunc);
        if (file.is_open()) {
            for (const auto& pair : m_settings) {
                file << pair.first << "=" << pair.second << "\n";
            }

            file.close();
            return 0;
        }
        else {
            return CError(FILEPATH, "配置文件无法写入，文件位置：" + m_filePath);
        }
    }

    void CSettings::remove(const std::string& key)
    {
        m_settings.erase(key);
    }

    void CSettings::clear()
    {
        m_settings.clear();
    }

    void CSettings::loadFromFile() {
        std::ifstream file(m_filePath);
        if (file.is_open()) {
            m_settings.clear();

            std::string line;
            while (std::getline(file, line)) {
                std::istringstream iss(line);
                std::string key, value;
                if (std::getline(iss, key, '=') && std::getline(iss, value)) {
                    m_settings[key] = value;
                }
            }
            file.close();
        }
        else {
            cout << "未打开配置文件，无法进行参数读取，文件位置：" + m_filePath;
        }
    }

}
