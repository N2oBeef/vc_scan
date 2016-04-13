#include "stdafx.h"
#include "GamesPanel.h"
#include <interface/IPage.h>
#include "Software/SoftwareMgrWrapper.h"

#include <boost/bind.hpp>

#define ULR_GAMES_PAGE L"http://www.ludashi.com/cms/pc/gamecheck/check.php"

CGamesPanel::CGamesPanel(void)
{
	m_nPanelID = GAMES_PANEL;	
}

void  __stdcall CGamesPanel::OnPanelActivate( const CRect &rct )
{
	BOOL bFirstCreate = (m_spPanelSite == NULL);

	//panel base
	CPanelBase::OnPanelActivate(rct);
	InitCtrl();

	CSoftwareMgrWrapper::GetInstance()->InitSoftwareMgr(m_pPage->GetPageWnd());
	CSiteUIHelper::HideSiteUI(m_spPanelFail);

	// 事件只能挂接一次
	if (bFirstCreate)
	{
		InitEvent();
		InitWeb();
	}
	else
	{
		CSiteUIHelper::HideSiteUI(m_spWndAttacher);
		m_webBrowser.NavigateUrl(ULR_GAMES_PAGE);

		CSiteUIHelper::HideSiteUI(m_spPanelFail);
		CSiteUIHelper::ShowSiteUI(m_spPanelLoading);
	}
}

void CGamesPanel::InitEvent( void )
{
	if (m_spPanelSite == NULL) return;

	DWORD cookie = 0;

	//重试
	IsitePtr spBtnSite = m_spPanelSite->Getitem_by_cookie(IDC_BTN_RETRY, true);
	ATLASSERT(spBtnSite);
	if (spBtnSite)
	{
		AtlAdvise(spBtnSite, (Isite_events*)this, DIID_Isite_events, &cookie);
	}
}

void CGamesPanel::InitCtrl( void )
{
	m_spPanelFail = CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_PANEL_FAIL);
	ATLASSERT(m_spPanelFail);

	m_spPanelLoading = CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_PANEL_LOADING);
	ATLASSERT(m_spPanelLoading);
}

void CGamesPanel::InitWeb( void )
{
	if (m_webBrowser.IsWindow()) return;
	
	g_SiteUI.GetFactory()->create_site(__uuidof(window_attcher), __uuidof(Iwindow_attcher), _bstr_t(L"default"), (void **)&m_spWndAttacher);
	
	m_spPanelSite->add_item(m_spWndAttacher);
	CRect rct = m_spPanelSite->get_bound();

	DWORD dwStyle = WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	HWND hWnd = m_webBrowser.Create(m_pPage->GetPageWnd(), rct, ULR_GAMES_PAGE,dwStyle);
	if(hWnd != NULL)
	{
		m_spWndAttacher->raw_attach((OLE_HANDLE)hWnd);
	}

	CSiteUIHelper::HideSiteUI(m_spWndAttacher);
	m_webBrowser.SetExternalDispath(CSoftwareMgrWrapper::GetInstance()->GeDispatch());
	m_webBrowser.SetLoadCompleteCallback(boost::bind(&CGamesPanel::OnWebLoadComplete, this, _1));
	CSoftwareMgrWrapper::GetInstance()->SetWebReadyCallback(boost::bind(&CGamesPanel::OnWebReady, this));
}

HRESULT CGamesPanel::OnBtnRetry( WORD wNotifyCode, WORD wID, HWND hWndCtl, Isite* site )
{
	CSiteUIHelper::HideSiteUI(m_spWndAttacher);
	m_webBrowser.NavigateUrl(ULR_GAMES_PAGE);

	CSiteUIHelper::HideSiteUI(m_spPanelFail);
	CSiteUIHelper::ShowSiteUI(m_spPanelLoading);

	return 0;
}


void CGamesPanel::OnWebLoadComplete( BOOL bSuccess )
{
	if (bSuccess)
	{
		CComPtr<IDispatch> spScript;
		m_webBrowser.GetJScript(spScript);
		ATLASSERT(spScript);

		CSoftwareMgrWrapper::GetInstance()->SetJsInterfaceBidge(spScript);
	}
	else
	{
		CSiteUIHelper::ShowSiteUI(m_spPanelFail);
	}

	CSiteUIHelper::HideSiteUI(m_spPanelLoading);
}


void CGamesPanel::OnWebReady(void)
{
	CSiteUIHelper::ShowSiteUI(m_spWndAttacher);
	CSiteUIHelper::HideSiteUI(m_spPanelFail);

	m_spPanelSite->repaint(FALSE);

}

BOOL CGamesPanel::PreTranslateMessage( MSG* pMsg )
{
	return m_webBrowser.PreTranslateMessage(pMsg);
}

void __stdcall CGamesPanel::UpdateScores( void )
{
	CSiteUIHelper::HideSiteUI(m_spWndAttacher);

	m_webBrowser.NavigateUrl(ULR_GAMES_PAGE);

	CSiteUIHelper::HideSiteUI(m_spPanelFail);
	CSiteUIHelper::ShowSiteUI(m_spPanelLoading);
}
