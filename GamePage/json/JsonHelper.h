#pragma once
#include "cJSON.h"
#include <string>

namespace JsonHelper
{
    std::string GetString(cJSON *pJsonObj, const std::string& name, const std::string& defaultValue);
	CString GetStringW(cJSON *pJsonObj, const std::string& name, const CString& defaultValue = L"");
    int GetInt(cJSON *pJsonObj, const std::string& name, int defaultValue);
    double GetDouble(cJSON *pJsonObj, const std::string& name, double defaultValue);
    bool GetBool(cJSON *pJsonObj, const std::string& name, bool defaultValue);

    std::string GetArrayString(cJSON *pJsonArray, int item, const std::string& defaultValue);
    int GetArrayInt(cJSON *pJsonArray, int item, int defaultValue);
    double GetArrayDouble(cJSON *pJsonArray, int item, double defaultValue);
    bool GetArrayBool(cJSON *pJsonArray, int item, bool defaultValue);

	CString GetStringWTeset(cJSON *pJsonObj, const std::string& name, const CString& defaultValue = L"");
	
};