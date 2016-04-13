// stdafx.cpp : source file that includes just the standard includes
// GamePage.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#include <CommonHelper/VerifySignHelper.cpp>

CSiteUIApp g_SiteUI;

//定义事件调用约定
_ATL_FUNC_INFO on_event_info = {CC_STDCALL, VT_I4, 4, {VT_UI4, VT_UI4, VT_UI4, VT_DISPATCH} };
_ATL_FUNC_INFO on_state_info = {CC_STDCALL, VT_I4, 3, { VT_UI4, VT_UI4, VT_DISPATCH} };


_ATL_FUNC_INFO case_event_4_param = {CC_STDCALL, VT_I4, 4, {VT_I4, VT_I4, VT_BSTR, VT_UNKNOWN} };
_ATL_FUNC_INFO case_event_2_param = {CC_STDCALL, VT_I4, 2, {VT_I8, VT_I8} };
