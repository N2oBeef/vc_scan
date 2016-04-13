#pragma once
#include "tinyxml.h"
#include <string>

namespace XmlParser
{
    std::string GetString(TiXmlElement* pElement, const std::string& name, const std::string& defaultValue);
    int GetInt(TiXmlElement* pElement, const std::string& name, int defaultValue);
    unsigned int GetUint(TiXmlElement* pElement, const std::string& name, unsigned int defaultValue);
    __int64 Getint64(TiXmlElement* pElement, const std::string& name, __int64 defaultValue);
    unsigned __int64 GetUint64(TiXmlElement* pElement, const std::string& name, unsigned __int64 defaultValue);
    double GetDouble(TiXmlElement* pElement, const std::string& name, double defaultValue);
    float GetFloat(TiXmlElement* pElement, const std::string& name, float defaultValue);
};
