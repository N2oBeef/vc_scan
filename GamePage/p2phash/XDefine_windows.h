#ifndef __X_DATA_DEFINE_WINDOWS_
#define __X_DATA_DEFINE_WINDOWS_

//
// Windows Version Build Option
//

#ifndef WINVER                      // 允许使用 Windows 95 和 Windows NT 4 或更高版本的特定功能。

	#ifdef _MSC_VER
		#if ( _MSC_VER > 1200 )
		#define WINVER 0x0500   //为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
		#endif
	#endif

#endif	// WINVER

#ifndef _WIN32_WINNT                // 允许使用 Windows NT 4 或更高版本的特定功能。
	#define _WIN32_WINNT 0x0500     //为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
#endif						

#ifndef _WIN32_WINDOWS              // 允许使用 Windows 98 或更高版本的特定功能。
	#define _WIN32_WINDOWS 0x0500   //为 Windows Me 及更新版本改变为适当的值。
#endif

#ifndef _WIN32_IE                   // 允许使用 IE 4.0 或更高版本的特定功能。
	#define _WIN32_IE 0x0500        //为 IE 5.0 及更新版本改变为适当的值。
#endif


// 必须要先包含winsock2

#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <tchar.h>
#include <mmsystem.h>


// sockets
typedef int socklen_t;

//#include <atlstr.h>
//using   ATL::CString;

#pragma warning(disable:4214)   // bit field types other than int
#pragma warning(disable:4201)   // nameless struct/union
#pragma warning(disable:4127)   // condition expression is constant
#pragma warning(disable:4115)   // named type definition in parentheses
#pragma warning(disable:4996)
#pragma warning(disable:4244)   // 从“P2PHANDLE”转换到“uint32”，可能丢失数据
#pragma warning(disable:4311)
#pragma warning(disable:4267)	// 从“size_t”转换到“int”，可能丢失数据



#endif	// __X_DATA_DEFINE_WINDOWS_ 
