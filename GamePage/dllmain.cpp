// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "GamePage_i.h"
#include "dllmain.h"

CGamePageModule _AtlModule;
HMODULE  g_hModule = NULL;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{   
	g_hModule = hInstance;

	return _AtlModule.DllMain(dwReason, lpReserved); 
}   
