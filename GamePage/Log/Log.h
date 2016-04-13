#ifndef ___TRACE_H___
#define ___TRACE_H___

#define ENABLE_LOG     //写日志开关
#define LOG_TO_FILE //写到文件

#include "SmartLogger.h"

extern CSmartLogger g_SmartLogger;
extern BOOL         g_bEnableLog;
#ifdef  ENABLE_LOG
#	define _MYDEBUGFLAG_A ""
#	define _MYDEBUGFLAG_W L""

void _cdecl WriteTraceToFile();
void _cdecl LOGW(LPCWSTR lpszFormat, ...);
void _cdecl LOGA(LPCSTR lpszFormat, ...);

#	ifdef _UNICODE
#		define LOG LOGW
#	else
#		define LOG LOGA
#	endif

#else//////////////

#	define LOGA (void)0
#	define LOGW (void)0
#	define LOG (void)__noop
#	define WriteTraceToFile (void)0

#endif

#endif