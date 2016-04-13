#ifndef __X_DATA_DEFINE__
#define __X_DATA_DEFINE__

// 通用的头放在前面，先于数据类型定义，不然比如deque里面有冲突

// linux下支持大文件读写
#ifndef WIN32 
	#define _LARGEFILE64_SOURCE
	#define _FILE_OFFSET_BITS 64
#endif

// time使用32bit，与linux保持一致
#define _USE_32BIT_TIME_T
#include <time.h>

#include <assert.h>
#include <stdio.h>

#include <list>
#include <vector>
#include <map>
#include <string>
#include <deque>
#include <algorithm>




#ifdef WIN32 

	#include "XDefine_windows.h"

#else

	#include "XDefine_linux.h"
	
#endif // WIN32


// 跨平台数据类型定义
//
// type defined
//
typedef unsigned long long	U64;
typedef unsigned int	U32;
typedef unsigned short	U16;
typedef unsigned char		U8;


typedef unsigned char		uchar;
typedef unsigned char		uint8;
typedef	  signed char		sint8;

typedef unsigned short		uint16;
typedef	  signed short		sint16;

typedef unsigned int		uint32;
typedef	  signed int		sint32;

typedef unsigned long long	uint64;
typedef   signed long long	sint64;


// 如果指针是0或者是非法地址则返回false，同时debug版会出断言
// 对于debug版，不会掩盖错误
BOOL IS_BAD_READ_PTR(const VOID* ReadPtr, UINT_PTR ucb, IN TCHAR* FileName,IN DWORD LineNumber);

#ifdef _UNICODE
	#define BAD_READ_PTR(ReadPtr, ucb) IS_BAD_READ_PTR(ReadPtr, ucb, _CRT_WIDE(__FILE__), __LINE__)
#else
	#define BAD_READ_PTR(ReadPtr, ucb) IS_BAD_READ_PTR(ReadPtr, ucb, (__FILE__), __LINE__)
#endif




#define P2P_ASSERT assert

#ifndef ASSERT
	#define ASSERT assert
#endif

#endif
