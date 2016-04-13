#include "StdAfx.h"
#include "ItemDirectX.h"
#include "Utils.h"
#include <ShlObj.h>

namespace Engine
{
    CItemDirectX::CItemDirectX(void)
    {
    }

    CItemDirectX::~CItemDirectX(void)
    {
    }

    BOOL __stdcall CItemDirectX::CheckProblem()
    {
        BOOL bHasProblem = TRUE;

        do 
        {
            CString strDXVerisonString;

            HRESULT hr = CUtils::GetDXVersionString(strDXVerisonString);
            if (FAILED(hr))
            {
                break;
            }

            if (strDXVerisonString.IsEmpty())
            {
                break;
            }

            if (-1 == strDXVerisonString.Find(m_strKeyword))
            {
                break;
            }

            if (!m_vecNotFoundFile.empty())
            {
                WCHAR szSystemDir[MAX_PATH] = {};
                SHGetSpecialFolderPath(NULL, szSystemDir, CSIDL_SYSTEM, FALSE);

                WCHAR szSystemDirX86[MAX_PATH] = {};
                if (CUtils::IsWin64())
                {
                    SHGetSpecialFolderPath(NULL, szSystemDirX86, CSIDL_SYSTEMX86, FALSE);
                }

                BOOL bFileNotFound = FALSE;

                for (int i = 0; i < m_vecNotFoundFile.size(); ++i)
                {
                    if (szSystemDir[0] != 0)
                    {
                        WCHAR szPath[MAX_PATH] = {};
                        PathCombine(szPath, szSystemDir, m_vecNotFoundFile[i]);
                        if (!PathFileExists(szPath))
                        {
                            bFileNotFound = TRUE;
                            break;
                        }
                    }

                    if (szSystemDirX86[0] != 0)
                    {
                        WCHAR szPath[MAX_PATH] = {};
                        PathCombine(szPath, szSystemDirX86, m_vecNotFoundFile[i]);
                        if (!PathFileExists(szPath))
                        {
                            bFileNotFound = TRUE;
                            break;
                        }
                    }
                }

                if (bFileNotFound)
                {
                    break;
                }
            }

            bHasProblem = FALSE;

        } while (0);

        return bHasProblem;
    }
}