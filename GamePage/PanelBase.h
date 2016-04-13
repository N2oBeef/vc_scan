#pragma once

#include <Interface\IPanel.h>

class IPage;

class CPanelBase
	: public IPanel
{
public:
	CPanelBase(void);

public:
	virtual Isite * __stdcall InitPanel(Isite * pParentSite ,const CString & strXmlFile ) ;
	virtual PANEL_ID __stdcall GetPanelID(void) ;
	virtual Isite * __stdcall GetPanel(void) ;
	virtual void  __stdcall OnPanelActivate( const CRect &rct );
	virtual void __stdcall OnPanelDeactivate( void ) ;
	virtual void __stdcall SetPage( IPage *pCleanPage);
	virtual BOOL __stdcall IsBusy(void);
	virtual int __stdcall OnPreClose(void);
	virtual BOOL __stdcall DispathCommand(LPCWSTR lpcwCmd);
	virtual void __stdcall SetDownMgrIconPos(CPoint pt);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void __stdcall UpdateScores(void);
	virtual void __stdcall UpdateScoresRank(void);
	virtual BOOL __stdcall IsActive(void);

protected:
	//´°¿Ú¹ÜÀí
	IcontainerPtr m_spPanelSite;
	IcontainerPtr m_spParentSite;
	PANEL_ID m_nPanelID;

	IPage*    m_pPage;
	CString         m_szXmlFile;
	CRect           m_rctPanelBound;
	BOOL            m_bActive;
};
