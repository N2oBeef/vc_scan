#pragma once

#include "IPage.h"

class IPanel
{
public:
	virtual Isite * __stdcall InitPanel(Isite * pParentSite ,const CString & strXmlFile ) = 0;
	virtual PANEL_ID __stdcall GetPanelID(void) = 0;
	virtual Isite * __stdcall GetPanel(void) = 0;
	virtual void  __stdcall OnPanelActivate( const CRect &rct ) = 0;
	virtual void __stdcall OnPanelDeactivate( void ) = 0;
	virtual void __stdcall SetPage( IPage *pCleanPage) = 0;
	virtual BOOL __stdcall IsBusy(void) = 0;
	virtual int __stdcall OnPreClose(void) = 0;
	virtual BOOL __stdcall DispathCommand(LPCWSTR lpcwCmd) = 0;
	virtual void __stdcall SetDownMgrIconPos(CPoint pt) = 0;
	virtual BOOL PreTranslateMessage(MSG* pMsg) = 0;
	virtual void __stdcall UpdateScores(void) = 0;
	virtual void __stdcall UpdateScoresRank(void) = 0;
	virtual BOOL __stdcall IsActive(void) = 0;
};

