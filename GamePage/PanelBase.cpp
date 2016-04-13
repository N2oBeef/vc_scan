#include "stdafx.h"
#include "PanelBase.h"

CPanelBase::CPanelBase(void)
	: m_nPanelID(UNKNOWN_PANEL)
	, m_spPanelSite(NULL)
	, m_pPage(NULL)
	, m_spParentSite(NULL)
	, m_bActive(FALSE)
{
}

Isite * __stdcall CPanelBase::InitPanel( Isite * pParentSite ,const CString & strXmlFile )
{
	if (pParentSite == NULL) return NULL;

	m_spParentSite = pParentSite;
	m_szXmlFile = strXmlFile;

	return m_spPanelSite;
}

PANEL_ID __stdcall CPanelBase::GetPanelID( void )
{
	return m_nPanelID;
}

Isite * __stdcall CPanelBase::GetPanel( void )
{
	ATLASSERT(m_spPanelSite);

	return m_spPanelSite;
}

void  __stdcall CPanelBase::OnPanelActivate( const CRect &rct )
{
	if (m_spPanelSite == NULL)
	{
		if (m_spParentSite == NULL) return;	

		IbuilderPtr pb;
		SiteUICreator(pb).CreateInstance(__uuidof(builder));
		if(pb == NULL) return;

		m_spPanelSite = pb->build_siteEx(_bstr_t(m_szXmlFile), m_spParentSite, TRUE);
		Iresource_mgrPtr pResMgr =  g_SiteUI.GetFactory()->get_resource_mgr();
		pResMgr->get_cache_builder(_bstr_t(m_szXmlFile));
		ATLASSERT(m_spPanelSite);
	}

	if (m_spPanelSite)
	{
		CRect rctPanel = rct;
		m_spPanelSite->set_bound(rctPanel, true);		
		m_spPanelSite->set_common_style(WS_VISIBLE, 0);
	}

	m_bActive = TRUE;
}

void __stdcall CPanelBase::OnPanelDeactivate( void )
{
	if(m_spPanelSite)
	{
		m_spPanelSite->set_common_style(0, WS_VISIBLE);
	}

	m_bActive = FALSE;
}

void __stdcall CPanelBase::SetPage( IPage *pCleanPage )
{
	ATLASSERT(pCleanPage);

	m_pPage = pCleanPage;
}

BOOL __stdcall CPanelBase::IsBusy( void )
{
	return FALSE;
}

int __stdcall CPanelBase::OnPreClose( void )
{
	return 0;
}

BOOL __stdcall CPanelBase::DispathCommand( LPCWSTR lpcwCmd )
{
	return FALSE;
}

void __stdcall CPanelBase::SetDownMgrIconPos( CPoint pt )
{
	
}

BOOL CPanelBase::PreTranslateMessage( MSG* pMsg )
{
	return TRUE;
}

void __stdcall CPanelBase::UpdateScores( void )
{
	
}

void __stdcall CPanelBase::UpdateScoresRank( void )
{

}

BOOL __stdcall CPanelBase::IsActive( void )
{
	return m_bActive;
}
