#pragma once

#include "PanelBase.h"
#include "Include/games_wnd_id.h"
#include "Browser/WebbrowserView.h"

class CGamesPanel
	: public IDispEventSimpleImpl</*nID =*/ 1, CGamesPanel, &DIID_Isite_events> //这个必须放第一个，方便强转
	, public CPanelBase
{
public:
	CGamesPanel(void);
	BEGIN_SINK_MAP(CGamesPanel)
		SINK_ENTRY_INFO(/*nID =*/ 1, DIID_Isite_events, 1, on_event, &on_event_info)
	END_SINK_MAP()

	//////////////////////////////////////////////////////////////////////////
	//事件消息宏    
	BEGIN_SITE_MSG_MAP(CGamesPanel)
		SITE_COMMAND_CODE_HANDLER_BY_ID(IDC_BTN_RETRY, BN_CLICKED, OnBtnRetry)
	END_SITE_MSG_MAP()
	//事件消息宏结束
	//////////////////////////////////////////////////////////////////////////
	HRESULT OnBtnRetry(WORD wNotifyCode, WORD wID, HWND hWndCtl, Isite* site );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void __stdcall UpdateScores(void);

public:
	virtual void  __stdcall OnPanelActivate( const CRect &rct );

private:
	void InitWeb(void);
	void InitCtrl( void );
	void OnWebLoadComplete( BOOL bSuccess );
	void OnWebReady(void);
	void InitEvent( void );
private:
	CWebbrowserView m_webBrowser;
	Iwindow_attcherPtr	 m_spWndAttacher;

	Ipanel_sitePtr       m_spPanelLoading;
	Ipanel_sitePtr       m_spPanelFail;
};
