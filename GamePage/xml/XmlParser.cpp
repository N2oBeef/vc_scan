#include "StdAfx.h"
#include "XmlParser.h"

namespace XmlParser
{
    std::string GetString( TiXmlElement* pElement, const std::string& name, const std::string& defaultValue )
    {
        ATLASSERT(pElement);

        std::string value = defaultValue;

        TiXmlElement *pChild = pElement->FirstChildElement(name);
        if (pChild && pChild->GetText())
        {
            value = pChild->GetText();
        }
        return value;
    }

    int GetInt( TiXmlElement* pElement, const std::string& name, int defaultValue )
    {
        ATLASSERT(pElement);

        int ival = defaultValue;

        std::string strValue = GetString(pElement, name, "");
        if (sscanf_s(strValue.c_str(), "%d", &ival) == 1)
            return ival;
        return defaultValue;
    }

    unsigned int GetUint( TiXmlElement* pElement, const std::string& name, unsigned int defaultValue )
    {
        ATLASSERT(pElement);
        return GetInt(pElement, name, defaultValue);
    }

    __int64 Getint64( TiXmlElement* pElement, const std::string& name, __int64 defaultValue )
    {
        ATLASSERT(pElement);

        __int64 ival = defaultValue;

        std::string strValue = GetString(pElement, name, "");
        if (sscanf_s(strValue.c_str(), "%I64d", &ival) == 1)
            return ival;
        return defaultValue;
    }

    unsigned __int64 GetUint64( TiXmlElement* pElement, const std::string& name, unsigned __int64 defaultValue )
    {
        ATLASSERT(pElement);
        return Getint64(pElement, name, defaultValue);
    }

    double GetDouble( TiXmlElement* pElement, const std::string& name, double defaultValue )
    {
        ATLASSERT(pElement);

        double dval = defaultValue;

        std::string strValue = GetString(pElement, name, "");
        if (sscanf_s(strValue.c_str(), "%lf", &dval) == 1)
            return dval;
        return defaultValue;
    }

    float GetFloat( TiXmlElement* pElement, const std::string& name, float defaultValue )
    {
        ATLASSERT(pElement);
        return GetDouble(pElement, name, defaultValue);
    }

};
