// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#define _WTL_NO_CSTRING
#define _USE_32BIT_TIME_T

#include "resource.h"
#include <atlbase.h>
#include <atlapp.h>
#include <atlmisc.h>

#include <atlstr.h>
#include <atlcom.h>
#include <atlcrack.h>
using namespace ATL;

#include <GdiPlus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

// check sign
#include <CommonHelper/VerifySignHelper.h>

extern HMODULE g_hModule;

#include <SiteUI/Include/afx_site.h> // 1
#include <LuDaShiSite/SiteUIApp.h>   // 2
#include <SiteUI/Include/site_consts.h> 
extern  CSiteUIApp g_SiteUI;         // 3

class CCleanPage;
extern CCleanPage * g_pCleanPage;

#include <LuDaShiSite/sites_ui_res.h>
#include <LuDaShiSite/SiteCreatorHelper.h>
#include <LuDaShiSite/AtlAdviser.h>
#include <LuDaShiSite/SiteUIHelper.h>

#include "Log/Log.h"
#include "Helper/GdiplusInitializer.h"

#import <SoftMgr\SoftMgr.tlb>
using namespace SoftMgrLib;

extern _ATL_FUNC_INFO case_event_4_param;
extern _ATL_FUNC_INFO case_event_2_param;

//开始检测
#define WM_START_EXAMINE (WM_USER + 4587)

//刷新跑分排行
#define WM_REFRESH_RANK (WM_USER + 4588)

//release do not check sign
//#define DISABLE_RELEASE_CHECK_SIGN
