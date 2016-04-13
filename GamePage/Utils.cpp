#include "stdafx.h"
#include "Utils.h"
#include "Helper/CommonAlgorithm.h"
#include "Helper/md5.h"
#include "p2phash/hash.h"
#include <BaseModule\360MID\360MID.h>
#include <ShlObj.h>

#include <InitGuid.h>
#include <dxdiag.h>

#include <boost/regex.hpp>

#define MAX_KEY_LENGTH (255)
#define MAX_VALUE_NAME (16383)

HRESULT GetDirectXVersionViaDxDiag( DWORD* pdwDirectXVersionMajor, DWORD* pdwDirectXVersionMinor, TCHAR* pcDirectXVersionLetter );
HRESULT GetDirectXVersionViaFileVersions( DWORD* pdwDirectXVersionMajor, DWORD* pdwDirectXVersionMinor, TCHAR* pcDirectXVersionLetter );
HRESULT GetFileVersion( TCHAR* szPath, ULARGE_INTEGER* pllFileVersion );
ULARGE_INTEGER MakeInt64( WORD a, WORD b, WORD c, WORD d );
int CompareLargeInts( ULARGE_INTEGER ullParam1, ULARGE_INTEGER ullParam2 );

BOOL CUtils::IsInstalled(REG_INFO & reg_info)
{
	for (size_t i = 0; i < reg_info.size(); ++i)
	{
        REG_GROUP & group = reg_info[i];

        if (group.hKey == NULL || group.szSubKey.IsEmpty()) continue;

        LPCWSTR lpcwValue = NULL;
        if (!group.szValue.IsEmpty())
        {
            lpcwValue = group.szValue;
        }

        CRegKey key;
        LONG lRes = key.Open(group.hKey, group.szSubKey, KEY_READ|(group.bIsX64?KEY_WOW64_64KEY:KEY_WOW64_32KEY));
        if (ERROR_SUCCESS != lRes)
        {
            continue;
        }

        // 如果value为空，则说明只需要验证键是否存在
        if (group.szValue.IsEmpty())
        {
            return TRUE;
        }

        WCHAR szPath[MAX_PATH] = {};
        DWORD dwLen = MAX_PATH;
        lRes = key.QueryStringValue(group.szValue, szPath, &dwLen);
        if (ERROR_SUCCESS != lRes)
        {
            continue;
        }

        CString szPathFull(szPath);
        szPathFull.TrimLeft();
        szPathFull.TrimRight();
        if (szPathFull.IsEmpty()) continue;

        if (szPathFull.Left(1) == L"\"")
        {
            szPathFull = szPathFull.Right(szPathFull.GetLength() - 1);
        }
        if (szPathFull.Right(1) == L"\"")
        {
            szPathFull = szPathFull.Left(szPathFull.GetLength() - 1);
        }

        if (!group.szExe.IsEmpty())
        {
            PathAppend(szPathFull.GetBufferSetLength(MAX_PATH), group.szExe);
            szPathFull.ReleaseBuffer();
        }

        if (PathFileExists(szPathFull)) return TRUE;
    }

    return FALSE;
}

BOOL CUtils::IsInstalled2( REG_INFO & reg_info )
{
    for (size_t i = 0; i < reg_info.size(); ++i)
    {
        REG_GROUP & group = reg_info[i];

        if (group.hKey == NULL || group.szSubKey.IsEmpty()) 
            continue;

        CRegKey key;
        LONG lRes = key.Open(group.hKey, group.szSubKey, KEY_READ|(group.bIsX64?KEY_WOW64_64KEY:KEY_WOW64_32KEY));
        if (ERROR_SUCCESS != lRes)
        {
            continue;
        }

        // 如果value为空，则说明只需要验证键是否存在
        if (group.szValue.IsEmpty())
        {
            return TRUE;
        }

        TCHAR szKeyName[MAX_KEY_LENGTH] = {};
        DWORD dwKeyName = MAX_KEY_LENGTH;
        TCHAR szSubValue[MAX_VALUE_NAME] = {};
        ULONG nChars = MAX_VALUE_NAME;
        for (int iIndex = 0; lRes == ERROR_SUCCESS; ++iIndex)
        {
            szKeyName[0] = '\0';
            dwKeyName = MAX_KEY_LENGTH;
            lRes = key.EnumKey(iIndex, szKeyName, &dwKeyName, NULL);
            if (ERROR_SUCCESS == lRes && szKeyName[0] != '\0')
            {
                TCHAR szSubKey[MAX_KEY_LENGTH] = { 0 };
                ::PathCombine(szSubKey, group.szSubKey, szKeyName);

                CRegKey subKey;
                LONG lSubRes = subKey.Open(group.hKey, szSubKey, KEY_READ|(group.bIsX64?KEY_WOW64_64KEY:KEY_WOW64_32KEY));
                if (ERROR_SUCCESS == lSubRes)
                {
                    szSubValue[0] = '\0';
                    nChars = MAX_VALUE_NAME;
                    LONG lSubRes = subKey.QueryStringValue(group.szValue, szSubValue, &nChars);
                    if (ERROR_SUCCESS == lSubRes && szSubValue[0] != '\0')
                    {
                        try
                        {
                            boost::wregex reg(group.szExe, boost::regex::icase|boost::regex::no_except);
                            if (boost::regex_match(szSubValue, reg))
                            {
                                return TRUE;
                            }
                        }
                        catch (...)
                        {

                        }
                    }
                }
            }
        }
    }

    return FALSE;
}

typedef struct _HKEYItem
{
	HKEY hKey;
	LPCTSTR lpszHKEY;
}HKEY_ITEM;


#define DEFINE_HKEYITEM( __hkey )	{ __hkey, _T( # __hkey ) }

static const HKEY_ITEM g_arrHKeyItem[] = 
{
	DEFINE_HKEYITEM(HKEY_LOCAL_MACHINE),
	DEFINE_HKEYITEM(HKEY_CURRENT_USER),
	DEFINE_HKEYITEM(HKEY_USERS),
	DEFINE_HKEYITEM(HKEY_CLASSES_ROOT),
	DEFINE_HKEYITEM(HKEY_CURRENT_CONFIG),
};

HKEY CUtils::StringToHKEY( LPCWSTR lpszHKEY )
{
	if(lpszHKEY == NULL) return HKEY_LOCAL_MACHINE;

	const int nCount = _countof(g_arrHKeyItem);
	for(int i = 0;i < nCount;i ++)
	{
		const HKEY_ITEM & item = g_arrHKeyItem[i];
		if(_tcsicmp(item.lpszHKEY, lpszHKEY) == 0) 
		{
			return item.hKey;
		}
	}

	return HKEY_LOCAL_MACHINE;
}

LPCWSTR CUtils::HKEYToString( HKEY hKey )
{
	const int nCount = _countof(g_arrHKeyItem);
	for(int i = 0;i < nCount;i ++)
	{
		const HKEY_ITEM & item = g_arrHKeyItem[i];
		if(item.hKey == hKey) 
		{
			return item.lpszHKEY;
		}
	}

	return NULL;
}


CString CUtils::GetIconPath(const CString & szUrl )
{
	if (szUrl.IsEmpty()) return L"";

	CString szRawUrl = szUrl;

	BYTE aMD5[16] = {};
	CCommonAlgorithm::MakeBuffMd5((LPVOID)szRawUrl.GetBuffer(), szRawUrl.GetLength() * sizeof(TCHAR), aMD5);
	szRawUrl.ReleaseBuffer();

	WCHAR szMD5[MAX_PATH] = {};
	CCommonAlgorithm::ConvertMD5SumToChar(aMD5, szMD5, MAX_PATH);

	static WCHAR szSetFile[MAX_PATH] = {};
	if (szSetFile[0] == 0)
	{
		SHGetSpecialFolderPath(NULL, szSetFile, CSIDL_APPDATA, FALSE);
		PathAppend(szSetFile, L"Ludashi\\softmgr\\icon\\");
	}

	CString szPath(szSetFile);
	szPath += szMD5;
	szPath += L".png";

	return szPath;
}

LPCWSTR CUtils::GetMIDStringW()
{
    static BOOL g_bFirst = TRUE;
    static WCHAR g_szMID[MAX_PATH] = {};

    if (g_bFirst)
    {
        g_bFirst = FALSE;
        Get360SafeMIDW(g_szMID, MAX_PATH);
    }

    return g_szMID;
}

LPCSTR CUtils::GetMIDStringA()
{
    static BOOL g_bFirst = TRUE;
    static CHAR g_szMID[MAX_PATH] = {};

    if (g_bFirst)
    {
        g_bFirst = FALSE;
        Get360SafeMIDA(g_szMID, MAX_PATH);
    }

    return g_szMID;
}

UINT64 CUtils::GetFileSize( const CString& strFilePath )
{
    if(!PathFileExists(strFilePath) || PathIsDirectory(strFilePath))
        return 0;

    ULARGE_INTEGER nFileSize = {};

    WIN32_FILE_ATTRIBUTE_DATA attri;
    ZeroMemory(&attri, sizeof(attri));	
    if(GetFileAttributesEx(strFilePath, GetFileExInfoStandard, &attri))
    {
        nFileSize.LowPart = attri.nFileSizeLow;
        nFileSize.HighPart = attri.nFileSizeHigh;
    }
    return nFileSize.QuadPart;
}

BOOL CUtils::SafeDeleteFile( const CString& strFilePath )
{
    if(PathFileExists(strFilePath))
    {
        SetFileAttributes(strFilePath, FILE_ATTRIBUTE_NORMAL);
        return DeleteFile(strFilePath); 
    }
    return TRUE;
}

BOOL CUtils::RunExeCreateProcess( const CString& strExePath, const CString& strRunParam, BOOL bWait)
{
    BOOL bRet =	FALSE;

    STARTUPINFO si = {0};
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    GetStartupInfo( &si );
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWNORMAL;

    PROCESS_INFORMATION pi = {0};
    ZeroMemory(&pi, sizeof(pi));

    CString strCmdLine;

    if (!strRunParam.IsEmpty())
    {
        strCmdLine.Format(L"\"%s\" %s", strExePath, strRunParam);
    }
    else
    {
        strCmdLine = strExePath;
    }

    bRet = CreateProcess( NULL, (LPWSTR)strCmdLine.GetBuffer(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi );
    if( bRet )
    {
        if (pi.hProcess)
        {
            if (bWait)
            {
                WaitForSingleObject(pi.hProcess, INFINITE); 
            }
            CloseHandle(pi.hProcess);
        }

        if (pi.hThread)
        {
            CloseHandle(pi.hThread);
        }
    }

    return bRet;
}

BOOL CUtils::RunExeShellExecuteEx(const CString& strExePath, const CString& strRunParam, BOOL bWait)
{
    BOOL bRet =	FALSE;

    SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
    sei.fMask = SEE_MASK_NOCLOSEPROCESS|SEE_MASK_FLAG_NO_UI;
    sei.hwnd = NULL;
    sei.lpFile = strExePath;
    sei.lpParameters = strRunParam;
    sei.nShow = SW_SHOWNORMAL;

    bRet = ::ShellExecuteEx(&sei);

    if(bRet)
    {
        if (sei.hProcess)
        {
            if (bWait)
            {
                WaitForSingleObject(sei.hProcess, INFINITE); 
            }
            CloseHandle(sei.hProcess);
        }
    }

    return bRet;
}

CString CUtils::CalcFileP2PID( const CString& strFilePath )
{
    CString strFileP2PID;
    if (PathFileExists(strFilePath))
    {
        strFileP2PID = CA2W(GetFileP2PID(strFilePath));
    }
    return strFileP2PID;
}

CString CUtils::CalcFileMD5( const CString& strFilePath )
{
    CString strFileMD5;
    if (PathFileExists(strFilePath))
    {
        BYTE md5[16] = {};
        CCommonAlgorithm::MakeFileMd5(strFilePath, md5);

        WCHAR szMd5[MAX_PATH] = {};
        CCommonAlgorithm::ConvertMD5SumToChar(md5, szMd5, MAX_PATH);

        strFileMD5 = szMd5;
    }
    return strFileMD5;
}

BOOL CUtils::IsWin64( void )
{
    /* 判断是否为64位系统 */
    typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);

    static int nIsWin64Flag = -1;
    if(nIsWin64Flag >= 0) return (nIsWin64Flag != FALSE);

    nIsWin64Flag = FALSE;

    OSVERSIONINFOEX osvi;
    SYSTEM_INFO si;
    PGNSI pGNSI;
    BOOL bOsVersionInfoEx;

    ZeroMemory(&si, sizeof(SYSTEM_INFO));
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
    {
        osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
        if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
            return FALSE;
    }

    // Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.

    pGNSI = (PGNSI) GetProcAddress(
        GetModuleHandle(TEXT("kernel32.dll")), 
        "GetNativeSystemInfo");
    if(NULL != pGNSI)
    {   
        pGNSI(&si);
    }
    else 
    {
        return FALSE;
    }
    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || 
        si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )   
    {
        nIsWin64Flag = TRUE;
        return TRUE;	   
    }

    else	   
    { 
        return FALSE;
    } 
}

CString CUtils::GetTempFilePath()
{
    WCHAR szTempFilePath[MAX_PATH] = {};

    WCHAR szTmpPath[MAX_PATH] = {};
    DWORD dwSize = MAX_PATH;
    DWORD dwRet = GetTempPath(dwSize, szTmpPath);
    if (0 < dwRet && dwRet < MAX_PATH)
    {
        dwRet = GetTempFileName(szTmpPath, NULL, 0, szTempFilePath);
        if (0 == dwRet)
        {
            szTempFilePath[0] = L'\0';
        }
    }

    return szTempFilePath;
}

BOOL CUtils::RemoveDirectory( LPCTSTR lpszDirectoryPath )
{
    if(!::PathFileExists(lpszDirectoryPath))
        return TRUE;

    if(::PathIsRoot(lpszDirectoryPath))
    {
        ATLASSERT(FALSE);
        return FALSE;
    }

    CString strDirPathTemp = lpszDirectoryPath;

    strDirPathTemp.AppendChar(_T('\0'));

    SHFILEOPSTRUCT FileOp; 
    ZeroMemory((void*)&FileOp, sizeof(SHFILEOPSTRUCT));

    FileOp.wFunc = FO_DELETE; 
    FileOp.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;	
    FileOp.pFrom = strDirPathTemp; 
    FileOp.pTo = NULL; 

    return SHFileOperation(&FileOp) == 0;
}

BOOL CUtils::GetResourceData( HMODULE hModule, const WCHAR * lpName, const WCHAR * lpType, ResourceData & data )
{   
    ZeroMemory(&data, sizeof(data));

    HRSRC hResInfo = ::FindResource(hModule, lpName, lpType);
    if(hResInfo == NULL) return FALSE;

    int nSize = SizeofResource(hModule, hResInfo);

    HGLOBAL hGlobal = ::LoadResource(hModule, hResInfo);
    if(hGlobal == NULL) return FALSE;

    data.lpData = (BYTE *)::LockResource(hGlobal);
    data.nSize = nSize;

    return TRUE;
}


//-----------------------------------------------------------------------------
// Name: GetDirectXVersionViaDxDiag()
// Desc: Tries to get the DirectX version from DxDiag's COM interface
//-----------------------------------------------------------------------------
HRESULT GetDirectXVersionViaDxDiag( DWORD* pdwDirectXVersionMajor,
                                   DWORD* pdwDirectXVersionMinor,
                                   TCHAR* pcDirectXVersionLetter )
{
    HRESULT hr;
    bool bCleanupCOM = false;

    bool bSuccessGettingMajor = false;
    bool bSuccessGettingMinor = false;
    bool bSuccessGettingLetter = false;

    // Init COM.  COM may fail if its already been inited with a different
    // concurrency model.  And if it fails you shouldn't release it.
    hr = CoInitialize(NULL);
    bCleanupCOM = SUCCEEDED(hr);

    // Get an IDxDiagProvider
    bool bGotDirectXVersion = false;
    IDxDiagProvider* pDxDiagProvider = NULL;
    hr = CoCreateInstance( CLSID_DxDiagProvider,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_IDxDiagProvider,
        (LPVOID*) &pDxDiagProvider );
    if( SUCCEEDED(hr) )
    {
        // Fill out a DXDIAG_INIT_PARAMS struct
        DXDIAG_INIT_PARAMS dxDiagInitParam;
        ZeroMemory( &dxDiagInitParam, sizeof(DXDIAG_INIT_PARAMS) );
        dxDiagInitParam.dwSize                  = sizeof(DXDIAG_INIT_PARAMS);
        dxDiagInitParam.dwDxDiagHeaderVersion   = DXDIAG_DX9_SDK_VERSION;
        dxDiagInitParam.bAllowWHQLChecks        = false;
        dxDiagInitParam.pReserved               = NULL;

        // Init the m_pDxDiagProvider
        hr = pDxDiagProvider->Initialize( &dxDiagInitParam );
        if( SUCCEEDED(hr) )
        {
            IDxDiagContainer* pDxDiagRoot = NULL;
            IDxDiagContainer* pDxDiagSystemInfo = NULL;

            // Get the DxDiag root container
            hr = pDxDiagProvider->GetRootContainer( &pDxDiagRoot );
            if( SUCCEEDED(hr) )
            {
                // Get the object called DxDiag_SystemInfo
                hr = pDxDiagRoot->GetChildContainer( L"DxDiag_SystemInfo", &pDxDiagSystemInfo );
                if( SUCCEEDED(hr) )
                {
                    VARIANT var;
                    VariantInit( &var );

                    // Get the "dwDirectXVersionMajor" property
                    hr = pDxDiagSystemInfo->GetProp( L"dwDirectXVersionMajor", &var );
                    if( SUCCEEDED(hr) && var.vt == VT_UI4 )
                    {
                        if( pdwDirectXVersionMajor )
                            *pdwDirectXVersionMajor = var.ulVal;
                        bSuccessGettingMajor = true;
                    }
                    VariantClear( &var );

                    // Get the "dwDirectXVersionMinor" property
                    hr = pDxDiagSystemInfo->GetProp( L"dwDirectXVersionMinor", &var );
                    if( SUCCEEDED(hr) && var.vt == VT_UI4 )
                    {
                        if( pdwDirectXVersionMinor )
                            *pdwDirectXVersionMinor = var.ulVal;
                        bSuccessGettingMinor = true;
                    }
                    VariantClear( &var );

                    // Get the "szDirectXVersionLetter" property
                    hr = pDxDiagSystemInfo->GetProp( L"szDirectXVersionLetter", &var );
                    if( SUCCEEDED(hr) && var.vt == VT_BSTR && SysStringLen( var.bstrVal ) != 0 )
                    {
#ifdef UNICODE
                        *pcDirectXVersionLetter = var.bstrVal[0];
#else
                        char strDestination[10];
                        WideCharToMultiByte( CP_ACP, 0, var.bstrVal, -1, strDestination, 10*sizeof(CHAR), NULL, NULL );
                        if( pcDirectXVersionLetter )
                            *pcDirectXVersionLetter = strDestination[0];
#endif
                        bSuccessGettingLetter = true;
                    }
                    VariantClear( &var );

                    // If it all worked right, then mark it down
                    if( bSuccessGettingMajor && bSuccessGettingMinor && bSuccessGettingLetter )
                        bGotDirectXVersion = true;

                    pDxDiagSystemInfo->Release();
                }

                pDxDiagRoot->Release();
            }
        }

        pDxDiagProvider->Release();
    }

    if( bCleanupCOM )
        CoUninitialize();

    if( bGotDirectXVersion )
        return S_OK;
    else
        return E_FAIL;
}

HRESULT GetDirectXVersionViaDxDiag(CString& strDXVersionString)
{
    HRESULT hr;
    bool bCleanupCOM = false;

    bool bSuccessGettingLetter = false;

    // Init COM.  COM may fail if its already been inited with a different
    // concurrency model.  And if it fails you shouldn't release it.
    hr = CoInitialize(NULL);
    bCleanupCOM = SUCCEEDED(hr);

    // Get an IDxDiagProvider
    bool bGotDirectXVersion = false;
    IDxDiagProvider* pDxDiagProvider = NULL;
    hr = CoCreateInstance( CLSID_DxDiagProvider,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_IDxDiagProvider,
        (LPVOID*) &pDxDiagProvider );
    if( SUCCEEDED(hr) )
    {
        // Fill out a DXDIAG_INIT_PARAMS struct
        DXDIAG_INIT_PARAMS dxDiagInitParam;
        ZeroMemory( &dxDiagInitParam, sizeof(DXDIAG_INIT_PARAMS) );
        dxDiagInitParam.dwSize                  = sizeof(DXDIAG_INIT_PARAMS);
        dxDiagInitParam.dwDxDiagHeaderVersion   = DXDIAG_DX9_SDK_VERSION;
        dxDiagInitParam.bAllowWHQLChecks        = false;
        dxDiagInitParam.pReserved               = NULL;

        // Init the m_pDxDiagProvider
        hr = pDxDiagProvider->Initialize( &dxDiagInitParam );
        if( SUCCEEDED(hr) )
        {
            IDxDiagContainer* pDxDiagRoot = NULL;
            IDxDiagContainer* pDxDiagSystemInfo = NULL;

            // Get the DxDiag root container
            hr = pDxDiagProvider->GetRootContainer( &pDxDiagRoot );
            if( SUCCEEDED(hr) )
            {
                // Get the object called DxDiag_SystemInfo
                hr = pDxDiagRoot->GetChildContainer( L"DxDiag_SystemInfo", &pDxDiagSystemInfo );
                if( SUCCEEDED(hr) )
                {
                    VARIANT var;
                    VariantInit( &var );

                    // Get the "szDirectXVersionLetter" property
                    hr = pDxDiagSystemInfo->GetProp( L"szDirectXVersionLongEnglish", &var );
                    if( SUCCEEDED(hr) && var.vt == VT_BSTR && SysStringLen( var.bstrVal ) != 0 )
                    {
                        strDXVersionString = CComBSTR(var.bstrVal);

                        bSuccessGettingLetter = true;
                    }
                    VariantClear( &var );

                    // If it all worked right, then mark it down
                    if( bSuccessGettingLetter )
                        bGotDirectXVersion = true;

                    pDxDiagSystemInfo->Release();
                }

                pDxDiagRoot->Release();
            }
        }

        pDxDiagProvider->Release();
    }

    if( bCleanupCOM )
        CoUninitialize();

    if( bGotDirectXVersion )
        return S_OK;
    else
        return E_FAIL;
}

//-----------------------------------------------------------------------------
// Name: GetDirectXVersionViaFileVersions()
// Desc: Tries to get the DirectX version by looking at DirectX file versions
//-----------------------------------------------------------------------------
HRESULT GetDirectXVersionViaFileVersions( DWORD* pdwDirectXVersionMajor,
                                         DWORD* pdwDirectXVersionMinor,
                                         TCHAR* pcDirectXVersionLetter )
{
    ULARGE_INTEGER llFileVersion;
    TCHAR szPath[512];
    TCHAR szFile[512];
    BOOL bFound = false;

    if( GetSystemDirectory( szPath, MAX_PATH ) != 0 )
    {
        szPath[MAX_PATH-1]=0;

        // Switch off the ddraw version
        StringCchCopy( szFile, 512, szPath );
        StringCchCat( szFile, 512, TEXT("\\ddraw.dll") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 2, 0, 95 ) ) >= 0 ) // Win9x version
            {
                // flle is >= DirectX1.0 version, so we must be at least DirectX1.0
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 1;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }

            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 3, 0, 1096 ) ) >= 0 ) // Win9x version
            {
                // flle is is >= DirectX2.0 version, so we must DirectX2.0 or DirectX2.0a (no redist change)
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 2;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }

            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 4, 0, 68 ) ) >= 0 ) // Win9x version
            {
                // flle is is >= DirectX3.0 version, so we must be at least DirectX3.0
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 3;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }
        }

        // Switch off the d3drg8x.dll version
        StringCchCopy( szFile, 512, szPath );
        StringCchCat( szFile, 512, TEXT("\\d3drg8x.dll") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 4, 0, 70 ) ) >= 0 ) // Win9x version
            {
                // d3drg8x.dll is the DirectX3.0a version, so we must be DirectX3.0a or DirectX3.0b  (no redist change)
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 3;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT('a');
                bFound = true;
            }
        }

        // Switch off the ddraw version
        StringCchCopy( szFile, 512, szPath );
        StringCchCat( szFile, 512, TEXT("\\ddraw.dll") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 5, 0, 155 ) ) >= 0 ) // Win9x version
            {
                // ddraw.dll is the DirectX5.0 version, so we must be DirectX5.0 or DirectX5.2 (no redist change)
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 5;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }

            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 6, 0, 318 ) ) >= 0 ) // Win9x version
            {
                // ddraw.dll is the DirectX6.0 version, so we must be at least DirectX6.0
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 6;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }

            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 6, 0, 436 ) ) >= 0 ) // Win9x version
            {
                // ddraw.dll is the DirectX6.1 version, so we must be at least DirectX6.1
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 6;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 1;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }
        }

        // Switch off the dplayx.dll version
        StringCchCopy( szFile, 512, szPath );
        StringCchCat( szFile, 512, TEXT("\\dplayx.dll") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 6, 3, 518 ) ) >= 0 ) // Win9x version
            {
                // ddraw.dll is the DirectX6.1 version, so we must be at least DirectX6.1a
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 6;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 1;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT('a');
                bFound = true;
            }
        }

        // Switch off the ddraw version
        StringCchCopy( szFile, 512, szPath );
        StringCchCat( szFile, 512, TEXT("\\ddraw.dll") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 7, 0, 700 ) ) >= 0 ) // Win9x version
            {
                // TODO: find win2k version

                // ddraw.dll is the DirectX7.0 version, so we must be at least DirectX7.0
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 7;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }
        }

        // Switch off the dinput version
        StringCchCopy( szFile, 512, szPath );
        StringCchCat( szFile, 512, TEXT("\\dinput.dll") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 4, 7, 0, 716 ) ) >= 0 ) // Win9x version
            {
                // ddraw.dll is the DirectX7.0 version, so we must be at least DirectX7.0a
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 7;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT('a');
                bFound = true;
            }
        }

        // Switch off the ddraw version
        StringCchCopy( szFile, 512, szPath );
        StringCchCat( szFile, 512, TEXT("\\ddraw.dll") );
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( (HIWORD(llFileVersion.HighPart) == 4 && CompareLargeInts( llFileVersion, MakeInt64( 4, 8, 0, 400 ) ) >= 0) || // Win9x version
                (HIWORD(llFileVersion.HighPart) == 5 && CompareLargeInts( llFileVersion, MakeInt64( 5, 1, 2258, 400 ) ) >= 0) ) // Win2k/WinXP version
            {
                // ddraw.dll is the DirectX8.0 version, so we must be at least DirectX8.0 or DirectX8.0a (no redist change)
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 8;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }
        }

        StringCchCopy( szFile, 512, szPath );
        StringCchCat( szFile, 512, TEXT("\\d3d8.dll"));
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( (HIWORD(llFileVersion.HighPart) == 4 && CompareLargeInts( llFileVersion, MakeInt64( 4, 8, 1, 881 ) ) >= 0) || // Win9x version
                (HIWORD(llFileVersion.HighPart) == 5 && CompareLargeInts( llFileVersion, MakeInt64( 5, 1, 2600, 881 ) ) >= 0) ) // Win2k/WinXP version
            {
                // d3d8.dll is the DirectX8.1 version, so we must be at least DirectX8.1
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 8;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 1;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }

            if( (HIWORD(llFileVersion.HighPart) == 4 && CompareLargeInts( llFileVersion, MakeInt64( 4, 8, 1, 901 ) ) >= 0) || // Win9x version
                (HIWORD(llFileVersion.HighPart) == 5 && CompareLargeInts( llFileVersion, MakeInt64( 5, 1, 2600, 901 ) ) >= 0) ) // Win2k/WinXP version
            {
                // d3d8.dll is the DirectX8.1a version, so we must be at least DirectX8.1a
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 8;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 1;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT('a');
                bFound = true;
            }
        }

        StringCchCopy( szFile, 512, szPath );
        StringCchCat( szFile, 512, TEXT("\\mpg2splt.ax"));
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( CompareLargeInts( llFileVersion, MakeInt64( 6, 3, 1, 885 ) ) >= 0 ) // Win9x/Win2k/WinXP version
            {
                // quartz.dll is the DirectX8.1b version, so we must be at least DirectX8.1b
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 8;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 1;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT('b');
                bFound = true;
            }
        }

        StringCchCopy( szFile, 512, szPath );
        StringCchCat( szFile, 512, TEXT("\\dpnet.dll"));
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            if( (HIWORD(llFileVersion.HighPart) == 4 && CompareLargeInts( llFileVersion, MakeInt64( 4, 9, 0, 134 ) ) >= 0) || // Win9x version
                (HIWORD(llFileVersion.HighPart) == 5 && CompareLargeInts( llFileVersion, MakeInt64( 5, 2, 3677, 134 ) ) >= 0) ) // Win2k/WinXP version
            {
                // dpnet.dll is the DirectX8.2 version, so we must be at least DirectX8.2
                if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 8;
                if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 2;
                if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
                bFound = true;
            }
        }

        StringCchCopy( szFile, 512, szPath );
        StringCchCat( szFile, 512, TEXT("\\d3d9.dll"));
        if( SUCCEEDED( GetFileVersion( szFile, &llFileVersion ) ) )
        {
            // File exists, but be at least DirectX9
            if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 9;
            if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
            if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
            bFound = true;
        }
    }

    if( !bFound )
    {
        // No DirectX installed
        if( pdwDirectXVersionMajor ) *pdwDirectXVersionMajor = 0;
        if( pdwDirectXVersionMinor ) *pdwDirectXVersionMinor = 0;
        if( pcDirectXVersionLetter ) *pcDirectXVersionLetter = TEXT(' ');
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetFileVersion()
// Desc: Returns ULARGE_INTEGER with a file version of a file, or a failure code.
//-----------------------------------------------------------------------------
HRESULT GetFileVersion( TCHAR* szPath, ULARGE_INTEGER* pllFileVersion )
{
    if( szPath == NULL || pllFileVersion == NULL )
        return E_INVALIDARG;

    DWORD dwHandle;
    UINT  cb;
    cb = GetFileVersionInfoSize( szPath, &dwHandle );
    if (cb > 0)
    {
        BYTE* pFileVersionBuffer = new BYTE[cb];
        if( pFileVersionBuffer == NULL )
            return E_OUTOFMEMORY;

        if (GetFileVersionInfo( szPath, 0, cb, pFileVersionBuffer))
        {
            VS_FIXEDFILEINFO* pVersion = NULL;
            if (VerQueryValue(pFileVersionBuffer, TEXT("\\"), (VOID**)&pVersion, &cb) &&
                pVersion != NULL)
            {
                pllFileVersion->HighPart = pVersion->dwFileVersionMS;
                pllFileVersion->LowPart  = pVersion->dwFileVersionLS;
                delete[] pFileVersionBuffer;
                return S_OK;
            }
        }

        delete[] pFileVersionBuffer;
    }

    return E_FAIL;
}

//-----------------------------------------------------------------------------
// Name: MakeInt64()
// Desc: Returns a ULARGE_INTEGER where a<<48|b<<32|c<<16|d<<0
//-----------------------------------------------------------------------------
ULARGE_INTEGER MakeInt64( WORD a, WORD b, WORD c, WORD d )
{
    ULARGE_INTEGER ull;
    ull.HighPart = MAKELONG(b,a);
    ull.LowPart = MAKELONG(d,c);
    return ull;
}

//-----------------------------------------------------------------------------
// Name: CompareLargeInts()
// Desc: Returns 1 if ullParam1 > ullParam2
//       Returns 0 if ullParam1 = ullParam2
//       Returns -1 if ullParam1 < ullParam2
//-----------------------------------------------------------------------------
int CompareLargeInts( ULARGE_INTEGER ullParam1, ULARGE_INTEGER ullParam2 )
{
    if( ullParam1.HighPart > ullParam2.HighPart )
        return 1;
    if( ullParam1.HighPart < ullParam2.HighPart )
        return -1;

    if( ullParam1.LowPart > ullParam2.LowPart )
        return 1;
    if( ullParam1.LowPart < ullParam2.LowPart )
        return -1;

    return 0;
}

HRESULT CUtils::GetDXVersion( DWORD* pdwDirectXVersion, TCHAR* strDirectXVersion, int cchDirectXVersion )
{
    bool bGotDirectXVersion = false;

    // Init values to unknown
    if( pdwDirectXVersion )
        *pdwDirectXVersion = 0;
    if( strDirectXVersion && cchDirectXVersion > 0 )
        strDirectXVersion[0] = 0;

    DWORD dwDirectXVersionMajor = 0;
    DWORD dwDirectXVersionMinor = 0;
    TCHAR cDirectXVersionLetter = ' ';

    //// First, try to use dxdiag's COM interface to get the DirectX version.
    //// The only downside is this will only work on DirectX9 or later.
    //if( SUCCEEDED( GetDirectXVersionViaDxDiag( &dwDirectXVersionMajor, &dwDirectXVersionMinor, &cDirectXVersionLetter ) ) )
    //    bGotDirectXVersion = true;

    if( !bGotDirectXVersion )
    {
        // Getting the DirectX version info from DxDiag failed,
        // so most likely we are on DirectX8.x or earlier
        if( SUCCEEDED( GetDirectXVersionViaFileVersions( &dwDirectXVersionMajor, &dwDirectXVersionMinor, &cDirectXVersionLetter ) ) )
            bGotDirectXVersion = true;
    }

    // If both techniques failed, then return E_FAIL
    if( !bGotDirectXVersion )
        return E_FAIL;

    // Set the output values to what we got and return
    cDirectXVersionLetter = (char)tolower(cDirectXVersionLetter);

    if( pdwDirectXVersion )
    {
        // If pdwDirectXVersion is non-NULL, then set it to something
        // like 0x00080102 which would represent DirectX8.1b
        DWORD dwDirectXVersion = dwDirectXVersionMajor;
        dwDirectXVersion <<= 8;
        dwDirectXVersion += dwDirectXVersionMinor;
        dwDirectXVersion <<= 8;
        if( cDirectXVersionLetter >= 'a' && cDirectXVersionLetter <= 'z' )
            dwDirectXVersion += (cDirectXVersionLetter - 'a') + 1;

        *pdwDirectXVersion = dwDirectXVersion;
    }

    if( strDirectXVersion && cchDirectXVersion > 0 )
    {
        // If strDirectXVersion is non-NULL, then set it to something
        // like "8.1b" which would represent DirectX8.1b
        if( cDirectXVersionLetter == ' ' )
            StringCchPrintf( strDirectXVersion, cchDirectXVersion, TEXT("%d.%d"), dwDirectXVersionMajor, dwDirectXVersionMinor );
        else
            StringCchPrintf( strDirectXVersion, cchDirectXVersion, TEXT("%d.%d%c"), dwDirectXVersionMajor, dwDirectXVersionMinor, cDirectXVersionLetter );
    }

    return S_OK;
}

HRESULT CUtils::GetDXVersionString( CString& strDXVersionString )
{
    HRESULT hr = GetDirectXVersionViaDxDiag(strDXVersionString);
    return hr;
}

CString CUtils::GetCurrentDownloadPath( void )
{
	WCHAR szFile[MAX_PATH] = {};
	GetModuleFileName(g_hModule, szFile, MAX_PATH);
	PathRemoveFileSpec(szFile);
	PathRemoveFileSpec(szFile);

	PathAppend(szFile, L"computerz.set");

	WCHAR szPath[MAX_PATH] = {};
	GetPrivateProfileString(L"SoftMgr", L"DownloadDir", L"", szPath, MAX_PATH, szFile);

	return szPath;
}

void CUtils::SetCurrentDownloadPath( const CString & szDir )
{
	if (szDir.IsEmpty()) return;
	
	WCHAR szFile[MAX_PATH] = {};
	GetModuleFileName(g_hModule, szFile, MAX_PATH);
	PathRemoveFileSpec(szFile);
	PathRemoveFileSpec(szFile);

	PathAppend(szFile, L"computerz.set");

	WCHAR szPath[MAX_PATH] = {};
	WritePrivateProfileString(L"SoftMgr", L"DownloadDir", szDir, szFile);
}

BOOL CUtils::IsVistaOrHigher()
{
    static BOOL g_bFirst = TRUE;
    static BOOL g_bResult = FALSE;

    if (g_bFirst)
    {
        OSVERSIONINFO osif = { 0 };

        osif.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        GetVersionEx(&osif);

        if(osif.dwMajorVersion >= 6)
        {
            g_bResult = TRUE;
        }

        g_bFirst = FALSE;
    }

    return g_bResult;
}

BOOL CUtils::IsXP()
{
    static BOOL g_bFirst = TRUE;
    static BOOL g_bResult = FALSE;

    if (g_bFirst)
    {
        OSVERSIONINFO osif = { 0 };

        osif.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        GetVersionEx(&osif);

        if(osif.dwMajorVersion == 5 && osif.dwMinorVersion == 1)
        {
            g_bResult = TRUE;
        }

        g_bFirst = FALSE;
    }

    return g_bResult;
}

BOOL CUtils::IsVista()
{
    static BOOL g_bFirst = TRUE;
    static BOOL g_bResult = FALSE;

    if (g_bFirst)
    {
        OSVERSIONINFO osif = { 0 };

        osif.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        GetVersionEx(&osif);

        if(osif.dwMajorVersion == 6 && osif.dwMinorVersion == 0)
        {
            g_bResult = TRUE;
        }

        g_bFirst = FALSE;
    }

    return g_bResult;
}

BOOL CUtils::IsWin7()
{
    static BOOL g_bFirst = TRUE;
    static BOOL g_bResult = FALSE;

    if (g_bFirst)
    {
        OSVERSIONINFO osif = { 0 };

        osif.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        GetVersionEx(&osif);

        if(osif.dwMajorVersion == 6 && osif.dwMinorVersion == 1)
        {
            g_bResult = TRUE;
        }

        g_bFirst = FALSE;
    }

    return g_bResult;
}

BOOL CUtils::IsWin8()
{
    static BOOL g_bFirst = TRUE;
    static BOOL g_bResult = FALSE;

    if (g_bFirst)
    {
        OSVERSIONINFO osif = { 0 };

        osif.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        GetVersionEx(&osif);

        if(osif.dwMajorVersion == 6 && osif.dwMinorVersion == 2)
        {
            WCHAR szNtoskrnl[MAX_PATH] = {};
            SHGetSpecialFolderPath(NULL, szNtoskrnl, CSIDL_SYSTEM, FALSE);
            PathAppend(szNtoskrnl, L"ntoskrnl.exe");

            CString strVersion;
            CCommonAlgorithm::GetFileVer(szNtoskrnl, strVersion.GetBufferSetLength(256), strVersion.GetLength());
            strVersion.Trim();

            if (strVersion.Find(L"6.2") == 0)
            {
                g_bResult = TRUE;
            }
        }

        g_bFirst = FALSE;
    }

    return g_bResult;
}

BOOL CUtils::IsWin10()
{
	static BOOL g_bFirst = TRUE;
	static BOOL g_bResult = FALSE;

	if (g_bFirst)
	{
		g_bFirst = FALSE;
		OSVERSIONINFO osif = { 0 };

		osif.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&osif);

		if(osif.dwMajorVersion == 6 && osif.dwMinorVersion == 4)
		{
			g_bResult = TRUE;
		}
        else if(osif.dwMajorVersion == 6 && osif.dwMinorVersion == 2)
        {
            WCHAR szNtoskrnl[MAX_PATH] = {};
            SHGetSpecialFolderPath(NULL, szNtoskrnl, CSIDL_SYSTEM, FALSE);
            PathAppend(szNtoskrnl, L"ntoskrnl.exe");

            CString strVersion;
            CCommonAlgorithm::GetFileVer(szNtoskrnl, strVersion.GetBufferSetLength(256), strVersion.GetLength());
            strVersion.Trim();

            if (strVersion.Find(L"6.4") == 0 || strVersion.Find(L"10.0") == 0)
            {
                g_bResult = TRUE;
            }
        }

	}

	return g_bResult;
}

BOOL CUtils::IsWin8_1()
{
    static BOOL g_bFirst = TRUE;
    static BOOL g_bResult = FALSE;

    if (g_bFirst)
    {
        OSVERSIONINFO osif = { 0 };

        osif.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        GetVersionEx(&osif);

        if(osif.dwMajorVersion == 6 && osif.dwMinorVersion == 3)
        {
            g_bResult = TRUE;
        }
        else if(osif.dwMajorVersion == 6 && osif.dwMinorVersion == 2)
        {
            WCHAR szNtoskrnl[MAX_PATH] = {};
            SHGetSpecialFolderPath(NULL, szNtoskrnl, CSIDL_SYSTEM, FALSE);
            PathAppend(szNtoskrnl, L"ntoskrnl.exe");

            CString strVersion;
            CCommonAlgorithm::GetFileVer(szNtoskrnl, strVersion.GetBufferSetLength(256), strVersion.GetLength());
            strVersion.Trim();

            if (strVersion.Find(L"6.3") == 0)
            {
                g_bResult = TRUE;
            }
        }

        g_bFirst = FALSE;
    }

    return g_bResult;
}

CString CUtils::GetOsName( void )
{
	static CString szOsName;
	if (szOsName.IsEmpty())
	{
		if (IsXP())
		{
			szOsName = L"windowsxp";
		}
		if (IsVista())
		{
			szOsName = L"windowsvista";
		}
		if (IsWin7())
		{
			szOsName = L"windows7";
		}
		if (IsWin8())
		{
			szOsName = L"windows8";
		}
		if (IsWin8_1())
		{
			szOsName = L"windows81";
		}
		if (IsWin10())
		{
			szOsName = L"windows10";
		}
	}

	return szOsName;
}
