#pragma once

#include "Interface/IPage.h"
#include <vector>

class CMainWnd 
	: public CWindowImpl<CMainWnd>
	, public IPage
{
public:
	CMainWnd(void);
	~CMainWnd(void);
	
	BEGIN_MSG_MAP_EX(CMainWnd)
		MSG_WM_CREATE(OnCreate)
		MESSAGE_HANDLER(WM_START_EXAMINE, OnMsgStartExamine)
		MESSAGE_HANDLER(WM_REFRESH_RANK, OnMsgRefreshRank)
		END_MSG_MAP()

public:
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void SetDownMgrIconPos(long x, long y);
	void UpdateScores(void);

public:
	virtual PANEL_ID __stdcall GetCurrentPanelID( void );
	virtual IPanel * __stdcall GetCurrentPanel( void );
	virtual void __stdcall SetCurrentPanel( PANEL_ID panelID);
	virtual HWND __stdcall GetPageWnd(void);

private:
	BOOL InitUI(void);
	void InitPanels(void);
	LRESULT OnMsgStartExamine( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );
	LRESULT OnMsgRefreshRank( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );

private:
	Iwindow_sitePtr m_spManager;
	IcontainerPtr	m_spContainer;

	typedef std::vector<IPanel *> PanelContainer;
	typedef PanelContainer::iterator PanelItor;

	PanelContainer m_panelContainer;
	IPanel * m_pCurrentPanel;
};