#pragma once

#include "Software/ICppInterfaceBridge.h"

struct ResourceData
{
    void * lpData;
    int nSize;
};

class CUtils
{
public:
    static BOOL IsInstalled(REG_INFO & reg_info);
    static BOOL IsInstalled2(REG_INFO & reg_info);
    static HKEY StringToHKEY(LPCWSTR lpszHKEY);
    static LPCWSTR HKEYToString(HKEY hKey);
    static CString GetIconPath(const CString & szUrl);

    static LPCWSTR GetMIDStringW();
    static LPCSTR GetMIDStringA();

    static UINT64 GetFileSize(const CString& strFilePath);
    static BOOL SafeDeleteFile(const CString& strFilePath);

    static BOOL RunExeCreateProcess(const CString& strExePath, const CString& strRunParam, BOOL bWait);
    static BOOL RunExeShellExecuteEx(const CString& strExePath, const CString& strRunParam, BOOL bWait);

    static CString CalcFileP2PID(const CString& strFilePath);
    static CString CalcFileMD5(const CString& strFilePath);

    static BOOL IsWin64(void);

    static CString GetTempFilePath();

    static BOOL RemoveDirectory(LPCTSTR lpszDirectoryPath);

    static BOOL GetResourceData( HMODULE hModule, const WCHAR * lpName, const WCHAR * lpType, ResourceData & data);

    static HRESULT GetDXVersion( DWORD* pdwDirectXVersion, TCHAR* strDirectXVersion, int cchDirectXVersion );

    static HRESULT GetDXVersionString( CString& strDXVersionString );

    static CString GetCurrentDownloadPath(void);

    static void SetCurrentDownloadPath(const CString & szDir);

    static BOOL IsVistaOrHigher();
    static BOOL IsXP();
    static BOOL IsVista();
    static BOOL IsWin7();
    static BOOL IsWin8();
    static BOOL IsWin8_1();
	static BOOL IsWin10();

	static CString GetOsName(void);
};