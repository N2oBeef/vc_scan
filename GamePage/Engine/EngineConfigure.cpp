#include "stdafx.h"
#include "EngineConfigure.h"
#include "Software/ICppInterfaceBridge.h"

#include "Item.h"
#include "ItemDirectX.h"
#include "ItemMVCRT.h"

#include "Utils.h"
#include "json/JsonHelper.h"
#include "resource.h"
#include <boost/regex.hpp>

using namespace Engine;
CEngineConfigure::CEngineConfigure()
: m_pEngine(NULL)
{

}

CEngineConfigure::~CEngineConfigure()
{

}

BOOL CEngineConfigure::Init( CEngine *pEngine )
{
    ATLASSERT(pEngine);
    m_pEngine = pEngine;

    //boost::wregex reg(L"Microsoft Visual C\\+\\+ 2005 .*?(x86)?.*", boost::regex::perl|boost::regex::icase);
    //bool bMatch = boost::regex_match(L"Microsoft Visual C++ 2005 Redistributable x86", reg);

    InitItems();

    CItemDirectX* pItemDirectX = NULL;

    if (CUtils::IsWin7() || CUtils::IsWin8() || CUtils::IsWin8_1())
    {
        pItemDirectX = new CItemDirectX();
        pItemDirectX->m_szName = L"Microsoft DirectX 11";
        pItemDirectX->m_str360SoftMgrId = L"102799569";
        pItemDirectX->m_strUrl = L"";
        pItemDirectX->m_strMD5 = L"d5d0061159545ad155acd0e6e5fcf43b";
        pItemDirectX->m_strSigner = L"";
        pItemDirectX->m_installPackageType = INSTALL_PACKAGE_TYPE_ZIP;
        pItemDirectX->m_strRunPath = L"DirectX\\DXSETUP.exe";
        pItemDirectX->m_strRunParam = L"";
        pItemDirectX->m_strKeyword = L"11";
        pItemDirectX->m_vecNotFoundFile.push_back(L"xinput1_3.dll");
    }
    else if(CUtils::IsXP())
    {
        pItemDirectX = new CItemDirectX();
        pItemDirectX->m_szName = L"Microsoft DirectX 9.0";
        pItemDirectX->m_str360SoftMgrId = L"10056";
        pItemDirectX->m_strUrl = L"";
        pItemDirectX->m_strMD5 = L"01590dc17adde6fdfac883903c6a0cb9";
        pItemDirectX->m_strSigner = L"";
        pItemDirectX->m_installPackageType = INSTALL_PACKAGE_TYPE_EXE;
        pItemDirectX->m_strRunPath = L"";
        pItemDirectX->m_strRunParam = L"";
        pItemDirectX->m_strKeyword = L"9";
        pItemDirectX->m_vecNotFoundFile.push_back(L"xinput1_3.dll");
    }

    m_pEngine->AddItem(pItemDirectX);

    return TRUE;
}

void CEngineConfigure::InitItems(void)
{
    cJSON* pRootJson = NULL;
	 
    do 
    {
        ResourceData resData;
        if (!CUtils::GetResourceData(g_hModule, MAKEINTRESOURCE(IDR_XML_GAME_FIX_CONFIG), L"XML", resData))
        {
            ATLASSERT(FALSE);
            break;
        }

        pRootJson = cJSON_Parse((const char*)resData.lpData);
        if (!pRootJson)
        {
            break;
        }

        cJSON* pJsonGameFix = cJSON_GetObjectItem(pRootJson, "gamefix");
        if (!pJsonGameFix)
        {
            break;
        }

        std::string strText;
        int nValue;

        int nSize = cJSON_GetArraySize(pJsonGameFix);
        for(int i = 0; i < nSize; ++i)
        {
            cJSON* pItem = cJSON_GetArrayItem(pJsonGameFix, i);
            if (!pItem)
            {
                continue;
            }

            // pf = 1, 表示只支持x86
            // pf = 2, 表示只支持x64
            // pf = 3, 表示支持x86, x64
            nValue = JsonHelper::GetInt(pItem, "pf", 0);
            if (1 == nValue)
            {
                if (CUtils::IsWin64())
                {
                    continue;
                }
            }
            else if(2 == nValue)
            {
                if (!CUtils::IsWin64())
                {
                    continue;
                }
            }
            else if(3 == nValue)
            {

            }
            else
            {
                continue;
            }

            CItem *pGameFixItem = NULL;

            // detecttype = 1, 表示注册表键值对检测
            // detecttype = 2, 表示遍历注册表找关键字检测
            nValue = JsonHelper::GetInt(pItem, "detecttype", 0);
            if (1 == nValue)
            {
                pGameFixItem = new CItem();
            }
            else if(2 == nValue)
            {
                pGameFixItem = new CItemMVCRT();
            }

            if (!pGameFixItem)
            {
                break;
            }

            cJSON* pItemRegGroup = cJSON_GetObjectItem(pItem, "reggroup");
            if (pItemRegGroup)
            {
                int nSizeRegGroup = cJSON_GetArraySize(pItemRegGroup);
                if (nSizeRegGroup > 0)
                {
                    for (int j = 0; j < nSizeRegGroup; ++j)
                    {
                        cJSON* pSubItem = cJSON_GetArrayItem(pItemRegGroup, j);
                        if (!pSubItem)
                        {
                            continue;
                        }

                        REG_GROUP regGroup;

                        strText = JsonHelper::GetString(pSubItem, "hkey", "");
                        CString strKey = CA2T(strText.c_str(), CP_UTF8);
                        regGroup.hKey = CUtils::StringToHKEY(strKey);

                        strText = JsonHelper::GetString(pSubItem, "subkey", "");
                        regGroup.szSubKey = CA2T(strText.c_str(), CP_UTF8);

                        strText = JsonHelper::GetString(pSubItem, "value", "");
                        regGroup.szValue = CA2T(strText.c_str(), CP_UTF8);

                        strText = JsonHelper::GetString(pSubItem, "exe", "");
                        regGroup.szExe = CA2T(strText.c_str(), CP_UTF8);

                        regGroup.bIsX64 = JsonHelper::GetBool(pSubItem, "is64", false) ? TRUE : FALSE;

                        pGameFixItem->m_RegInfo.push_back(regGroup);
                    }
                }
            }

            strText = JsonHelper::GetString(pItem, "name", "");
            pGameFixItem->m_szName = CA2T(strText.c_str(), CP_UTF8);

            strText = JsonHelper::GetString(pItem, "360softmgrid", "");
            pGameFixItem->m_str360SoftMgrId = CA2T(strText.c_str(), CP_UTF8);

            strText = JsonHelper::GetString(pItem, "urls", "");
            pGameFixItem->m_strUrl = CA2T(strText.c_str(), CP_UTF8);

            strText = JsonHelper::GetString(pItem, "md5", "");
            pGameFixItem->m_strMD5 = CA2T(strText.c_str(), CP_UTF8);

            strText = JsonHelper::GetString(pItem, "signer", "");
            pGameFixItem->m_strSigner = CA2T(strText.c_str(), CP_UTF8);

            nValue = JsonHelper::GetInt(pItem, "installtype", 0);
            pGameFixItem->m_installPackageType = (INSTALL_PACKAGE_TYPE)nValue;

            strText = JsonHelper::GetString(pItem, "runpath", "");
            pGameFixItem->m_strRunPath = CA2T(strText.c_str(), CP_UTF8);

            strText = JsonHelper::GetString(pItem, "runparam", "");
            pGameFixItem->m_strRunParam = CA2T(strText.c_str(), CP_UTF8);

            m_pEngine->AddItem(pGameFixItem);
        }

    } while (0);

    if (pRootJson)
    {
        cJSON_Delete(pRootJson);
        pRootJson = NULL;
    }
}