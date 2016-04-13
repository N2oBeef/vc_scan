#include "stdafx.h"
#include <BaseModule/Common/AutoLocker.h>
#include "SmartLogger.h"

#include <locale>
#include <shlobj.h>
#include <strsafe.h>

#define LOG_PREFIX_NAME	L""
#define LOG_PREFIX_LEN	_countof(LOG_PREFIX_NAME)

static const int const_log_size_limit = 3 * 1024 * 1024; //1G 

CSmartLogger::CSmartLogger()
	: m_bFirstWrite(TRUE)
{
}

CSmartLogger::~CSmartLogger()
{
	Clear();
}

void CSmartLogger::InitLog(void)
{
	TCHAR szLogFile[MAX_PATH] = { 0 };
	GetModuleFileName(g_hModule, szLogFile, MAX_PATH);
	PathRemoveFileSpec(szLogFile);
	PathRemoveFileSpec(szLogFile);

	m_strFilePath = szLogFile;
	m_strFilePath += L"\\Log\\GamePage.log";

	PathAppend(szLogFile, L"\\Log\\GamePage.log.on");
	g_bEnableLog = PathFileExists(szLogFile);
}

int CSmartLogger::AddLog( const CString & szLine )
{
	_BaseModule::CAutoLocker __locker( & m_cs );

	CString szText = szLine;
	if (szText.GetLength() > 0 && szText.GetAt(szText.GetLength()-1) != '\n')
	{
		szText += _T("\n");
	}

	SYSTEMTIME   st = { 0 };
	GetLocalTime(&st);

	WCHAR szTime[100] = { 0 };
	StringCbPrintf(szTime, 1024, L"%04d-%02d-%02d %02d:%02d:%02d ",
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	m_container.push_back(szTime + szText);
	if (m_container.size() > 100)
	{
		SaveToFile();
	}

	return static_cast<int>(m_container.size());
}

int CSmartLogger::AddLog( LPCTSTR lpszFormat, va_list argList )
{
	_BaseModule::CAutoLocker __locker( & m_cs );

	if(!lpszFormat) return static_cast<int>(m_container.size());

	SYSTEMTIME   st = { 0 };
	GetLocalTime(&st);

	WCHAR szTime[100] = { 0 };
	StringCbPrintf(szTime, 1024, L"%04d-%02d-%02d %02d:%02d:%02d ",
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	const int nBufSize = 2048;
	WCHAR szCommonBuffer_Full[nBufSize + 200] = LOG_PREFIX_NAME;
	WCHAR * szCommonBuffer = szCommonBuffer_Full + LOG_PREFIX_LEN;

	StringCbVPrintf(szCommonBuffer, 1024 - LOG_PREFIX_LEN, lpszFormat, argList);
	int len = wcslen(szCommonBuffer) - 1;
	if(szCommonBuffer[len] != L'\n')
	{
		szCommonBuffer[len + 1] = L'\n';
		szCommonBuffer[len + 2] = L'\0';
	}

	m_container.push_back(CString(szTime) + szCommonBuffer_Full);

	return static_cast<int>(m_container.size());
}

void CSmartLogger::Clear( void )
{
	_BaseModule::CAutoLocker __locker( & m_cs );

	m_container.clear();
}

BOOL CSmartLogger::SaveToFile(void)
{
	_BaseModule::CAutoLocker __locker( & m_cs );

	BOOL bResult = FALSE;

	int nFileNameLen = _tcslen(m_strFilePath) + 1;
	LPTSTR lpszPath = new TCHAR[nFileNameLen];
	memset(lpszPath, 0, nFileNameLen * sizeof(TCHAR));
	StringCbCopy(lpszPath,sizeof(TCHAR) * (nFileNameLen),m_strFilePath);
	::PathRemoveFileSpec(lpszPath);

	if ( !::PathFileExists(lpszPath) )
	{
		::SHCreateDirectoryEx(NULL, lpszPath, NULL);
	}

	if(::PathFileExists(lpszPath))
	{
		//MessageBox(NULL,L"Exist",NULL,MB_OK);
		SYSTEMTIME   st = { 0 };
		GetLocalTime(&st);
		
		WCHAR szTime[100] = { 0 };
		StringCbPrintf(szTime, 1024, L"%04d-%02d-%02d %02d:%02d:%02d ",
					st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		
		FILE* pLogFile = _wfopen(m_strFilePath, L"a");
		if (pLogFile)
		{
			bResult = TRUE;

			if (m_bFirstWrite)
			{
				fwprintf(pLogFile, L"\r\n================== Start Log:%s ==================\n", szTime);
				m_bFirstWrite = FALSE;
			}
			
			int nTotalCnt = m_container.size();
			for(int i = 0;i < nTotalCnt; i++)
			{
				fwprintf(pLogFile, m_container[i]);
			}

			fflush(pLogFile);
			fclose(pLogFile);
		}
	}

	if (lpszPath)
	{
		delete[] lpszPath;
	}
	Clear();

	return bResult;
}

void CSmartLogger::SetLogFileName( const CString & strLogFile )
{
// 	TCHAR szLogFile[MAX_PATH] = { 0 };
// 	SHGetSpecialFolderPath(NULL, szLogFile, CSIDL_APPDATA, FALSE);
// 	PathAppend( szLogFile, DIR_360SAFE );
// 	PathAppend( szLogFile, strLogFile );
	m_strFilePath = strLogFile;

	if (PathFileExists(m_strFilePath))
	{
		LARGE_INTEGER lgSize = GetFileSize(m_strFilePath);
		if (lgSize.QuadPart > const_log_size_limit)
		{
			::DeleteFile(m_strFilePath);
		}
	}
}

LARGE_INTEGER CSmartLogger::GetFileSize( CString & strFile)
{
	LARGE_INTEGER llFileSize; 
	llFileSize.QuadPart = 0;

	if (strFile.IsEmpty()) 
		return llFileSize;

	HANDLE hFile; 
	hFile = CreateFile(strFile, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) 
		return llFileSize;

	::GetFileSizeEx(hFile, &llFileSize);

	::CloseHandle(hFile);

	return llFileSize;
}