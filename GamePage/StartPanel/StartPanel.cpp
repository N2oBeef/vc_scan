#include "stdafx.h"
#include "StartPanel.h"
#include <interface/IPage.h>
#include "Config/Configure.h"

#include "Software/SoftwareMgrWrapper.h"

#include <boost/bind.hpp>

#define ULR_START_PAGE L"http://www.ludashi.com/cms/pc/gamecheck/uncheck.php"
#define TOP_HEIGHT 111

CStartPanel::CStartPanel(void)
{
	m_nPanelID = STARTUP_PANEL;	
}

HRESULT CStartPanel::OnBtnStartScan( WORD wNotifyCode, WORD wID, HWND hWndCtl, Isite* site )
{
	if (m_pPage == NULL) return E_FAIL;
    
	m_pPage->SetCurrentPanel(SCANNING_PANEL);

	return S_OK;
}

void CStartPanel::InitEvent( void )
{
	if (m_spPanelSite == NULL) return;
	
	DWORD cookie = 0;
	
	// 开始扫描按钮
	IsitePtr spBtnSite = m_spPanelSite->Getitem_by_cookie(IDC_BTN_EXAMINE, true);
	ATLASSERT(spBtnSite);
	if (spBtnSite)
	{
		AtlAdvise(spBtnSite, (Isite_events*)this, DIID_Isite_events, &cookie);
	}

	//重试
	spBtnSite = m_spPanelSite->Getitem_by_cookie(IDC_BTN_RETRY, true);
	ATLASSERT(spBtnSite);
	if (spBtnSite)
	{
		AtlAdvise(spBtnSite, (Isite_events*)this, DIID_Isite_events, &cookie);
	}
}

void  __stdcall CStartPanel::OnPanelActivate( const CRect &rct )
{
	BOOL bFirstCreate = (m_spPanelSite == NULL);

	//top
	CPanelBase::OnPanelActivate(rct);
	m_siteRes.Init(m_szXmlFile);

	//bottom
	InitBottom();
	InitCtrl();
	CSoftwareMgrWrapper::GetInstance()->InitSoftwareMgr(m_pPage->GetPageWnd());
	CSiteUIHelper::HideSiteUI(m_spPanelFail);

	// 事件只能挂接一次
	if (bFirstCreate)
	{
		InitEvent();
		InitUI();
	}
	else
	{
		CSiteUIHelper::HideSiteUI(m_spWndAttacher);
		m_webBrowser.NavigateUrl(ULR_START_PAGE);

		CSiteUIHelper::HideSiteUI(m_spPanelFail);
		CSiteUIHelper::ShowSiteUI(m_spPanelLoading);
	}
}

void CStartPanel::InitUI(void)
{
	int nUnFixCount = CConfigure::GetInstance()->GetUnfixProblem();
	Ipicture_sitePtr spPic = CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_START_PIC_STATUS);
	ATLASSERT(spPic);
	spPic->load_image_by_id(m_siteRes.GetResId(nUnFixCount ? L"IDI_START_STAT_ERROR": L"IDI_START_STAT_NORMAL"));
	
	if (nUnFixCount > 0)
	{
		Istatic_sitePtr spFirstLine = CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_START_TITLE_FIRST);
		ATLASSERT(spFirstLine);
		spFirstLine->set_text(_bstr_t(m_siteRes.LoadStringById(L"IDS_ERROR_TITLE")));
	}
}

void CStartPanel::InitBottom( void )
{
	if (m_webBrowser.IsWindow()) return;
	
	g_SiteUI.GetFactory()->create_site(__uuidof(window_attcher), __uuidof(Iwindow_attcher), _bstr_t(L"default"), (void **)&m_spWndAttacher);
	
	m_spPanelSite->add_item(m_spWndAttacher);
	CRect rct = m_spPanelSite->get_bound();
	rct.top += TOP_HEIGHT;

	DWORD dwStyle = WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	HWND hWnd = m_webBrowser.Create(m_pPage->GetPageWnd(), rct, ULR_START_PAGE,dwStyle);
	if(hWnd != NULL)
	{
		m_spWndAttacher->raw_attach((OLE_HANDLE)hWnd);
	}

	CSiteUIHelper::HideSiteUI(m_spWndAttacher);
	m_webBrowser.SetExternalDispath(CSoftwareMgrWrapper::GetInstance()->GeDispatch());
	m_webBrowser.SetLoadCompleteCallback(boost::bind(&CStartPanel::OnWebLoadComplete, this, _1));
	CSoftwareMgrWrapper::GetInstance()->SetWebReadyCallback(boost::bind(&CStartPanel::OnWebReady, this));
}

void CStartPanel::InitCtrl( void )
{
	m_spPanelFail = CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_PANEL_FAIL);
	ATLASSERT(m_spPanelFail);

	m_spPanelLoading = CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_PANEL_LOADING);
	ATLASSERT(m_spPanelLoading);
}

void CStartPanel::OnWebLoadComplete( BOOL bSuccess )
{
	LOG(L"CStartPanel::OnWebLoadComplete: %d", bSuccess);

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


HRESULT CStartPanel::OnBtnRetry( WORD wNotifyCode, WORD wID, HWND hWndCtl, Isite* site )
{	
	CSiteUIHelper::HideSiteUI(m_spWndAttacher);
	m_webBrowser.NavigateUrl(ULR_START_PAGE);

	CSiteUIHelper::HideSiteUI(m_spPanelFail);
	CSiteUIHelper::ShowSiteUI(m_spPanelLoading);

	return S_OK;
}


void CStartPanel::OnWebReady(void)
{
	LOG(L"CStartPanel::OnWebReady");

	if (!IsActive()) return;
	
	CSiteUIHelper::ShowSiteUI(m_spWndAttacher);
	CSiteUIHelper::HideSiteUI(m_spPanelFail);
}

void __stdcall CStartPanel::SetDownMgrIconPos( CPoint pt )
{
	CSoftwareMgrWrapper::GetInstance()->SetDownMgrIconPos(pt.x, pt.y);
}

void __stdcall CStartPanel::UpdateScores( void )
{
	LOG(L"CStartPanel::UpdateScores");

	CSiteUIHelper::HideSiteUI(m_spWndAttacher);
	m_webBrowser.NavigateUrl(ULR_START_PAGE);

	CSiteUIHelper::HideSiteUI(m_spPanelFail);
	CSiteUIHelper::ShowSiteUI(m_spPanelLoading);
}
