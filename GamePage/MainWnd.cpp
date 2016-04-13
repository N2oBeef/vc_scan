#include "stdafx.h"
#include "MainWnd.h"
#include "Interface/IPanel.h"
#include "StartPanel/StartPanel.h"
#include "ScanPanel/ScanPanel.h"
#include "GamesPanel/GamesPanel.h"
#include "Config/Configure.h"
#include "Software/SoftwareMgrWrapper.h"

CMainWnd::CMainWnd( void )
: m_pCurrentPanel(NULL)
{
		
}

CMainWnd::~CMainWnd( void )
{
	if (IsWindow())
	{
		DestroyWindow();
	}
}

int CMainWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	InitUI();
	InitPanels();
	
	BOOL bHasExamined = CConfigure::GetInstance()->HasExamined();
	int nUnFixCount = CConfigure::GetInstance()->GetUnfixProblem();

	//debug
	//bHasExamined = TRUE;
	//nUnFixCount = 0;

	SetCurrentPanel(bHasExamined && nUnFixCount <= 0 ? GAMES_PANEL : STARTUP_PANEL);

	return 0;
}

BOOL CMainWnd::InitUI(void)
{
	IbuilderPtr spbuild;
	HRESULT hr = g_SiteUI.CreateInstance(__uuidof(builder),spbuild);
	if(FAILED(hr)) return FALSE;

	CString strUIPath = g_SiteUI.GetDefaultSkinPath();

	CString strThemePath = strUIPath;
	strThemePath += L"\\GamePage\\GamePage_theme.xml";

	Itheme_managerPtr mgr = g_SiteUI.GetFactory()->get_theme_mgr();
	Imoduletheme_managerPtr spModuleTheme = mgr;
	if (!spModuleTheme)  return FALSE;

	hr = spModuleTheme->load_module_theme(_bstr_t(strThemePath));
	if (FAILED(hr)) return FALSE;

	CString strDlgXMLPath = strUIPath;
	strDlgXMLPath += (L"\\GamePage\\root_wnd.xml");

	m_spManager = spbuild->load_config((_bstr_t)strDlgXMLPath,(OLE_HANDLE)(DWORD_PTR)m_hWnd,NULL);
	if(m_spManager == NULL) return FALSE;

	m_spContainer = m_spManager->get_root_container();
	if(m_spContainer == NULL) return FALSE;

	Ires_builderPtr spResbuild;
	hr = g_SiteUI.CreateInstance(__uuidof(res_builder),spResbuild);
	if(SUCCEEDED(hr))
	{
		CString strResourcePath = strUIPath;
		strResourcePath += (L"\\Common\\XML\\CommonResource.xml");

		spResbuild->set_config(NULL,_bstr_t(strResourcePath));
		g_SiteUI.GetFactory()->get_resource_mgr()->add_builder((OLE_HANDLE)(DWORD_PTR)m_hWnd,spResbuild);
	}

	m_spManager->draw_non_client(1);
	return TRUE;
}

PANEL_ID __stdcall CMainWnd::GetCurrentPanelID( void )
{
	return m_pCurrentPanel->GetPanelID();
}

IPanel * __stdcall CMainWnd::GetCurrentPanel( void )
{
	return m_pCurrentPanel;
}

void __stdcall CMainWnd::SetCurrentPanel( PANEL_ID panelID )
{	
	if (!m_spContainer) return;

	PanelItor iter = m_panelContainer.begin();
	PanelItor iterEnd = m_panelContainer.end();
	IPanel * pPanel = NULL;

	CRect rctPanel = m_spContainer->get_bound();
	for(; iter != iterEnd; iter ++)
	{
		pPanel = * iter;
		if(pPanel == NULL) continue;
		
		if (pPanel->GetPanelID() == panelID)
		{
			pPanel->OnPanelActivate(rctPanel);
			m_pCurrentPanel = pPanel;

		}else
		{
			pPanel->OnPanelDeactivate();
		}
	}
}

void CMainWnd::InitPanels( void )
{
	CString strUIPath = g_SiteUI.GetDefaultSkinPath();

	CStartPanel *pStartPanel = new CStartPanel();
	pStartPanel->InitPanel(m_spContainer, strUIPath + L"\\GamePage\\start_wnd.xml");
	pStartPanel->SetPage(this);
	m_panelContainer.push_back(pStartPanel);

	CScanPanel *pScanPanel = new CScanPanel();
	pScanPanel->InitPanel(m_spContainer, strUIPath + L"\\GamePage\\scan_wnd.xml");
	pScanPanel->SetPage(this);
	m_panelContainer.push_back(pScanPanel);

	CGamesPanel *pGamesPanel = new CGamesPanel();
	pGamesPanel->InitPanel(m_spContainer, strUIPath + L"\\GamePage\\games_wnd.xml");
	pGamesPanel->SetPage(this);
	m_panelContainer.push_back(pGamesPanel);	
}

HWND __stdcall CMainWnd::GetPageWnd( void )
{
	return m_hWnd;
}

void CMainWnd::SetDownMgrIconPos( long x, long y )
{
	CSoftwareMgrWrapper::GetInstance()->SetDownMgrIconPos(x, y);
}

LRESULT CMainWnd::OnMsgStartExamine( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
	SetCurrentPanel(SCANNING_PANEL);
	return 0;
}

void CMainWnd::UpdateScores( void )
{
	if (m_pCurrentPanel)
	{
		m_pCurrentPanel->UpdateScores();
	}
}

LRESULT CMainWnd::OnMsgRefreshRank( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
	if (m_pCurrentPanel)
	{
		m_pCurrentPanel->UpdateScoresRank();
	}
	return 0;
}
