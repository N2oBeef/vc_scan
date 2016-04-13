#include "StdAfx.h"
#include "JsonHelper.h"

#include "Helper/CharacterCodeHelper.h"

namespace JsonHelper
{
	std::string GetString( cJSON *pJsonObj, const std::string& name, const std::string& defaultValue )
	{
		if (pJsonObj == NULL || name.empty())
		{
			ATLASSERT(FALSE);
			return defaultValue;
		}

		std::string value = defaultValue;

		cJSON* pCurObj = cJSON_GetObjectItem(pJsonObj, name.c_str());
		if(pCurObj && pCurObj->type == cJSON_String && pCurObj->valuestring)
		{
			value = pCurObj->valuestring;
		}

		return value;
	}

	CString GetStringW(cJSON *pJsonObj, const std::string& name, const CString& defaultValue)
	{ 
		if (pJsonObj == NULL || name.empty())
		{
			ATLASSERT(FALSE);
			return defaultValue;
		}

		CString value = defaultValue;

		cJSON* pCurObj = cJSON_GetObjectItem(pJsonObj, name.c_str());
		if(pCurObj && pCurObj->type == cJSON_String && pCurObj->valuestring)
		{
			std::string valueU8 = pCurObj->valuestring;

			Utf16 u16(valueU8.c_str());
			value = u16.AsWz();
		}

		return value;
	}

	CString GetStringWTeset(cJSON *pJsonObj, const std::string& name, const CString& defaultValue)
	{ 
		if (pJsonObj == NULL || name.empty())
		{
			ATLASSERT(FALSE);
			return defaultValue;
		}

		CString value = defaultValue;

		cJSON* pCurObj = cJSON_GetObjectItem(pJsonObj, name.c_str());
		if(pCurObj && pCurObj->type == cJSON_String && pCurObj->valuestring)
		{
			std::string valueU8 = pCurObj->valuestring;
			
			value = CA2W(valueU8.c_str(), 936);
		}

		return value;
	}

	int GetInt( cJSON *pJsonObj, const std::string& name, int defaultValue )
	{
		if (pJsonObj == NULL || name.empty())
		{
			ATLASSERT(FALSE);
			return defaultValue;
		}

		int value = defaultValue;

		cJSON* pCurObj = cJSON_GetObjectItem(pJsonObj, name.c_str());
		if(pCurObj && pCurObj->type == cJSON_Number)
		{
			value = pCurObj->valueint;
		}

		return value;
	}

	double GetDouble( cJSON *pJsonObj, const std::string& name, double defaultValue )
	{
		if (pJsonObj == NULL || name.empty())
		{
			ATLASSERT(FALSE);
			return defaultValue;
		}

		double value = defaultValue;

		cJSON* pCurObj = cJSON_GetObjectItem(pJsonObj, name.c_str());
		if(pCurObj && pCurObj->type == cJSON_Number)
		{
			value = pCurObj->valuedouble;
		}

		return value;
	}

	bool GetBool( cJSON *pJsonObj, const std::string& name, bool defaultValue )
	{
		if (pJsonObj == NULL || name.empty())
		{
			ATLASSERT(FALSE);
			return defaultValue;
		}

		bool value = defaultValue;

		cJSON* pCurObj = cJSON_GetObjectItem(pJsonObj, name.c_str());
		if(pCurObj)
		{
			if (pCurObj->type == cJSON_False)
			{
				value = false;
			}
			else if (pCurObj->type == cJSON_True)
			{
				value = true;
			}
		}

		return value;
	}

	std::string GetArrayString( cJSON *pJsonArray, int item, const std::string& defaultValue )
	{
		if (pJsonArray == NULL)
		{
			ATLASSERT(FALSE);
			return defaultValue;
		}

		std::string value = defaultValue;

		cJSON* pItem = cJSON_GetArrayItem(pJsonArray, item);
		if (pItem && cJSON_String == pItem->type && pItem->valuestring)
		{
			value = pItem->valuestring;
		}

		return value;
	}

	int GetArrayInt( cJSON *pJsonArray, int item, int defaultValue )
	{
		if (pJsonArray == NULL)
		{
			ATLASSERT(FALSE);
			return defaultValue;
		}

		int value = defaultValue;

		cJSON* pItem = cJSON_GetArrayItem(pJsonArray, item);
		if (pItem && cJSON_Number == pItem->type)
		{
			value = pItem->valueint;
		}

		return value;
	}

	double GetArrayDouble( cJSON *pJsonArray, int item, double defaultValue )
	{
		if (pJsonArray == NULL)
		{
			ATLASSERT(FALSE);
			return defaultValue;
		}

		double value = defaultValue;

		cJSON* pItem = cJSON_GetArrayItem(pJsonArray, item);
		if (pItem && cJSON_Number == pItem->type)
		{
			value = pItem->valueint;
		}

		return value;
	}

	bool GetArrayBool( cJSON *pJsonArray, int item, bool defaultValue )
	{
		if (pJsonArray == NULL)
		{
			ATLASSERT(FALSE);
			return defaultValue;
		}

		bool value = defaultValue;

		cJSON* pItem = cJSON_GetArrayItem(pJsonArray, item);
		if (pItem)
		{
			if (pItem->type == cJSON_False)
			{
				value = false;
			}
			else if (pItem->type == cJSON_True)
			{
				value = true;
			}
		}

		return value;
	}



};