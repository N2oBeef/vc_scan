// GamePage.cpp : Implementation of CGamePage

#include "stdafx.h"
#include "GamePage.h"

#include "Config/Configure.h"

// CGamePage


HRESULT STDMETHODCALLTYPE CGamePage::CreatePage( /* [in] */ OLE_HANDLE hParentWnd,
												 /* [in] */ LONG nX, 
												 /* [in] */ LONG nY, 
												 /* [in] */ LONG nWidth, 
												 /* [in] */ LONG nHeight, 
												 /* [retval][out] */ OLE_HANDLE *hWnd )
{
	Init();

	if (!m_MainWnd.IsWindow())
	{ 
		m_hParentWnd = (HWND)hParentWnd;
		m_MainWnd.Create(m_hParentWnd, CRect(nX, nY, nX + nWidth, nY + nHeight),NULL, 
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		if (hWnd)
		{
			*hWnd = (OLE_HANDLE)m_MainWnd.m_hWnd;
		}
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CGamePage::OnActive( void )
{
	m_bPageActive = TRUE;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CGamePage::OnDeactive( void )
{
	m_bPageActive = FALSE;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CGamePage::DispatchCommand( /* [in] */ BSTR bstrCmd, 
													  /* [in] */ LONG lCode, 
													  /* [in] */ LONG lSubCode, 
													  /* [retval][out] */ LONG *pProcessed )
{
	CComBSTR bstr = bstrCmd;
	CString szCmd = bstr;
	if (szCmd.CompareNoCase(L"set_downmgr_icon_pos") == 0)
	{
		m_MainWnd.SetDownMgrIconPos(lCode, lSubCode);
	}
	
	if (szCmd.CompareNoCase(L"set_benchmark_cpu_gpu") == 0)
	{
		CConfigure::GetInstance()->CpuScore(lCode);
		CConfigure::GetInstance()->GpuScore(lSubCode);
		LOG(L"cpu score: %ld, gpu score: %ld",lCode, lSubCode);
		m_MainWnd.UpdateScores();
	}

	if (szCmd.CompareNoCase(L"set_benchmark_mem_size") == 0)
	{
		CConfigure::GetInstance()->MemSize(lCode);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CGamePage::OnPreClose( /* [retval][out] */ LONG *pCanClose )
{

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CGamePage::IsBusy( /* [retval][out] */ LONG *pIsBusy )
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CGamePage::DestroyPage( void )
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CGamePage::GetWnd( /* [retval][out] */ OLE_HANDLE *pChildWnd )
{
	if (pChildWnd == NULL) return E_POINTER;

	*pChildWnd = (OLE_HANDLE)m_MainWnd.m_hWnd;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CGamePage::GetParentWnd( /* [retval][out] */ OLE_HANDLE *pParentWnd )
{
	if (pParentWnd == NULL) return E_POINTER;

	*pParentWnd = (OLE_HANDLE)m_hParentWnd;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CGamePage::Call( /* [in] */ BSTR bstrCmd, 
										   /* [in] */ LONG lCode, 
										   /* [in] */ LONG lSubCode, 
										   /* [retval][out] */ LONG *pResult )
{
	return E_NOTIMPL;
}

void CGamePage::Init()
{
	InitSite();
	Init360Base();
	m_gdiInitializer.Init();
	g_SmartLogger.InitLog();
}

void CGamePage::InitSite(void)
{
	WCHAR szPath[MAX_PATH] = {};
	GetModuleFileName(g_hModule, szPath, MAX_PATH);
	PathRemoveFileSpec(szPath);
	PathRemoveFileSpec(szPath);

	g_SiteUI.Init(szPath);
}


void CGamePage::Init360Base(void)
{
	WCHAR szPath[MAX_PATH] = {};
	GetModuleFileName(g_hModule, szPath, MAX_PATH);
	PathRemoveFileSpec(szPath);
	PathRemoveFileSpec(szPath);

	PathAppend(szPath, DLLNAME_360BASE);

	VerifySignHelper::Load360BaseDll(szPath);
}