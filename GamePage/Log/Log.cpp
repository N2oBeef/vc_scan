#include "stdafx.h"
#include "Log.h"

#include <strsafe.h>
#include "SmartLogger.h"

CSmartLogger g_SmartLogger;
BOOL         g_bEnableLog = FALSE;

#ifdef ENABLE_LOG
#ifdef LOG_TO_FILE


void _cdecl LOGW(LPCWSTR lpszFormat, ...)
{
	if (!g_bEnableLog) return;

	va_list args;
	va_start(args, lpszFormat);

	WCHAR szBuffer[1024] = { 0 };
	WCHAR szOutText[1024+20] = { 0 };

	HRESULT hr = StringCbVPrintfW(szBuffer, sizeof(szBuffer), lpszFormat, args);
	//ATLASSERT(SUCCEEDED(hr)); //Output truncated as it was > sizeof(szBuffer)
	if (SUCCEEDED(hr))
	{
		StringCbCopyW(szOutText, sizeof(szOutText), _MYDEBUGFLAG_W);
		StringCbCatW(szOutText, sizeof(szOutText), szBuffer);

		g_SmartLogger.AddLog(szOutText);

		WriteTraceToFile();
	}
}
//
//void _cdecl LOGA(LPCSTR lpszFormat, ...)
//{
//	va_list args;
//	va_start(args, lpszFormat);
//
//	CHAR szBuffer[512] ={ 0 };
//	CHAR szOutText[512+20] = { 0 };
//
//	HRESULT hr = StringCbVPrintfA(szBuffer, sizeof(szBuffer), lpszFormat, args);
//	if(SUCCEEDED(hr))
//	{
//		StringCbCopyA(szOutText, sizeof(szOutText), _MYDEBUGFLAG_A);
//		StringCbCatA(szOutText, sizeof(szOutText), szBuffer);
//
//		g_SmartLogger.AddLog(szOutText);
//
//		WriteTraceToFile();
//	}	
//}

void _cdecl WriteTraceToFile()
{
	if (!g_bEnableLog) return;
	
	g_SmartLogger.SaveToFile();
	g_SmartLogger.Clear();
}

#else
void _cdecl LOGW(LPCWSTR lpszFormat, ...)
{  
	va_list args;
	va_start(args, lpszFormat);

	WCHAR szBuffer[512];
	WCHAR szOutText[512+20];

	HRESULT hr = StringCbVPrintfW(szBuffer, sizeof(szBuffer), lpszFormat, args);
	ATLASSERT(SUCCEEDED(hr)); //Output truncated as it was > sizeof(szBuffer)

	StringCbCopyW(szOutText, sizeof(szOutText), _MYDEBUGFLAG_W);
	StringCbCatW(szOutText, sizeof(szOutText), szBuffer);

	int nLen = wcslen(szOutText);
	if (szOutText[nLen-1] != '\n')
	{
		szOutText[nLen] = '\n';
		szOutText[nLen+1] = '\0';
	}
	
	OutputDebugStringW(szOutText);
	va_end(args);
}

void _cdecl LOGA(LPCSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);

	CHAR szBuffer[512];
	CHAR szOutText[512+20];

	HRESULT hr = StringCbVPrintfA(szBuffer, sizeof(szBuffer), lpszFormat, args);
	ATLASSERT(SUCCEEDED(hr)); //Output truncated as it was > sizeof(szBuffer)

	StringCbCopyA(szOutText, sizeof(szOutText), _MYDEBUGFLAG_A);
	StringCbCatA(szOutText, sizeof(szOutText), szBuffer);

	OutputDebugStringA(szBuffer);
	va_end(args);
}

void _cdecl WriteTraceToFile()
{

}

#endif //#LOG_TO_FILE

#endif//#ifdef  ENABLE_LOG