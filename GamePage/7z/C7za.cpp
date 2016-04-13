#include "StdAfx.h"
#include "C7za.h"
#include "7za.h"

namespace C7za
{
    BOOL CompressFile( const CString& str7zDllPath, 
        const std::vector<CString>& vecFile, 
        const CString& strArchiveFile, 
        const CString& strPassword, 
        BOOL bAppend )
    {
        BOOL bRet = FALSE;

        HMODULE h7zDll = NULL;

        do 
        {
			if (bAppend)
			{
				if (!PathFileExists(strArchiveFile))
				{
					break;
				}
			}
			else
			{
				DeleteFile(strArchiveFile);
			}

			if (!PathFileExists(str7zDllPath))
			{
				break;
			}
 
#ifndef _DEBUG
            HMODULE h7zDll = VerifySignHelper::LoadLibrary_qs(str7zDllPath);
#else
            HMODULE h7zDll = LoadLibrary_api(str7zDllPath);
#endif
            if (!h7zDll)
            {
                break;
            }

			FunX7za pfnX7za = (FunX7za)GetProcAddress(h7zDll, "X7za");
			if (!pfnX7za)
			{
				break;
			}

			CStringW strCommandLine;

			strCommandLine.Format(L"a -t7z \"%s\"",
				strArchiveFile);

			if (!strPassword.IsEmpty())
			{
				strCommandLine.AppendFormat(_T(" -p%s -mhe"), strPassword);
			}

			for(std::size_t i = 0; i < vecFile.size(); ++i)
			{
				strCommandLine.AppendFormat(_T(" \"%s\""), vecFile[i]);
			}

			N7ZAExitCode::EEnum nX7zaRet = (N7ZAExitCode::EEnum)(*pfnX7za)(strCommandLine);
			bRet = (N7ZAExitCode::kSuccess == nX7zaRet);

		} while (FALSE);

		if (h7zDll)
		{
			FreeLibrary(h7zDll);
		}

		return bRet;
	}

	BOOL UncompressFile( const CString& str7zDllPath, 
		const CString& strArchiveFile, 
		const CString& strOutDirectory, 
		const CString& strPassword, 
		const CString& strEntryName )
	{
		BOOL bRet = FALSE;

		HMODULE h7zDll = NULL;

		do 
		{
			if (!PathFileExists(str7zDllPath))
			{
				break;
			}

			if (!PathFileExists(strArchiveFile))
			{
				break;
			}

			if (strOutDirectory.IsEmpty())
			{
				break;
			}

#ifndef _DEBUG
            HMODULE h7zDll = VerifySignHelper::LoadLibrary_qs(str7zDllPath);
#else
            HMODULE h7zDll = LoadLibrary_api(str7zDllPath);
#endif
            if (!h7zDll)
            {
                break;
            }

			FunX7za pfnX7za = (FunX7za)GetProcAddress(h7zDll, "X7za");
			if (!pfnX7za)
			{
				break;
			}

			CStringW strCommandLine;

			strCommandLine.Format(L"x \"%s\" -y -o\"%s\"", 
				strArchiveFile, strOutDirectory);

			if (!strPassword.IsEmpty())
			{
				strCommandLine.AppendFormat(_T(" -p%s"), strPassword);
			}

			if (!strEntryName.IsEmpty())
			{
				strCommandLine.AppendFormat(_T(" \"%s\""), strEntryName);
			}

			N7ZAExitCode::EEnum nX7zaRet = (N7ZAExitCode::EEnum)(*pfnX7za)(strCommandLine);
			bRet = (N7ZAExitCode::kSuccess == nX7zaRet);

		} while (FALSE); 

		if (h7zDll)
		{
			FreeLibrary(h7zDll);
		}

		return bRet;
	}
}

C7zaHelper::C7zaHelper()
: m_hThread(NULL)
{
}

C7zaHelper::~C7zaHelper()
{
    if (m_hThread != NULL)
    {
        if(WAIT_TIMEOUT == WaitForSingleObject(m_hThread, 200))
        {
            TerminateThread(m_hThread, -1);
        }
        CloseHandle(m_hThread);
        m_hThread = NULL;
    }
}

BOOL C7zaHelper::StartUncompressFile(const CString& str7zDllPath, 
									 const CString& strArchiveFile, 
									 const CString& strOutDirectory, 
									 const CString& strPassword, 
									 const CString& strEntryName)
{
    UncompressFileParameter* pParemeter = new UncompressFileParameter;
    pParemeter->pThis = this;
    pParemeter->str7zDllPath = str7zDllPath;
    pParemeter->strArchiveFile = strArchiveFile;
    pParemeter->strOutDirectory = strOutDirectory;
    pParemeter->strPassword = strPassword;
    pParemeter->strEntryName = strEntryName;

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, pParemeter, 0, NULL);

	if (m_hThread != INVALID_HANDLE_VALUE && m_hThread != NULL)
	{
		BOOL bExitApp = FALSE;
		BOOL bExit = FALSE;
		while(!bExit)
		{
			HANDLE handles[] = { m_hThread };
			DWORD dwRet = MsgWaitForMultipleObjects(_countof(handles), handles, FALSE, INFINITE, QS_ALLEVENTS);
			switch(dwRet)
			{
			case WAIT_OBJECT_0:
				{
					bExit = TRUE;
				}
				break;
			case WAIT_OBJECT_0 + 1:
				{
					MSG msg = {};
					while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
					{
						if (msg.message == WM_QUIT)
						{
							bExitApp = TRUE;
							bExit = TRUE;
							break;
						}

						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
				break;
			default:
				{
					// Òì³£È¡Ïû
					bExit = TRUE;
				}
				break;
			}
		}

        if(bExitApp)
        {
            PostQuitMessage(0);
            return TRUE;
        }

        if (m_hThread != NULL)
        {
            if(WAIT_TIMEOUT == WaitForSingleObject(m_hThread, 200))
            {
                TerminateThread(m_hThread, -1);
            }
            CloseHandle(m_hThread);
            m_hThread = NULL;
        }

        return TRUE;
    }
    return FALSE;
}

unsigned int WINAPI C7zaHelper::ThreadProc( void * pParam )
{
    UncompressFileParameter *pParemeter = reinterpret_cast<UncompressFileParameter*>(pParam);
    ATLASSERT(pParemeter);
    return pParemeter->pThis->DoUncompressFile(pParemeter->str7zDllPath, 
        pParemeter->strArchiveFile,
        pParemeter->strOutDirectory,
        pParemeter->strPassword,
        pParemeter->strEntryName);
}

BOOL C7zaHelper::DoUncompressFile(const CString& str7zDllPath, 
                                  const CString& strArchiveFile, 
                                  const CString& strOutDirectory, 
                                  const CString& strPassword, 
                                  const CString& strEntryName)
{
    return C7za::UncompressFile(str7zDllPath, strArchiveFile, strOutDirectory, strPassword, strEntryName);
}