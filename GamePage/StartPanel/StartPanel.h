#pragma once

#include "PanelBase.h"
#include "Include/start_wnd_id.h"
#include "Browser/WebbrowserView.h"


class CStartPanel
	: public IDispEventSimpleImpl</*nID =*/ 1, CStartPanel, &DIID_Isite_events> //这个必须放第一个，方便强转
	, public CPanelBase
{
public:
	CStartPanel(void);
	BEGIN_SINK_MAP(CStartPanel)
		SINK_ENTRY_INFO(/*nID =*/ 1, DIID_Isite_events, 1, on_event, &on_event_info)
	END_SINK_MAP()

	//////////////////////////////////////////////////////////////////////////
	//事件消息宏    
	BEGIN_SITE_MSG_MAP(CStartPanel)
		SITE_COMMAND_CODE_HANDLER_BY_ID(IDC_BTN_EXAMINE, BN_CLICKED, OnBtnStartScan)
		SITE_COMMAND_CODE_HANDLER_BY_ID(IDC_BTN_RETRY, BN_CLICKED, OnBtnRetry)
 	END_SITE_MSG_MAP()
	//事件消息宏结束
	//////////////////////////////////////////////////////////////////////////

public:
	virtual void  __stdcall OnPanelActivate( const CRect &rct );
	virtual void __stdcall SetDownMgrIconPos(CPoint pt);
	virtual void __stdcall UpdateScores(void);

public:
	// 开始体检的按钮点击
	HRESULT OnBtnStartScan(WORD wNotifyCode, WORD wID, HWND hWndCtl, Isite* site );
	HRESULT OnBtnRetry(WORD wNotifyCode, WORD wID, HWND hWndCtl, Isite* site );

private:
	void InitEvent(void);
	void InitBottom(void);
	void InitCtrl(void);
	void InitUI(void);

	void OnWebLoadComplete(BOOL bSuccess);
	void OnWebReady(void);

private:
	CWebbrowserView      m_webBrowser;
	Iwindow_attcherPtr	 m_spWndAttacher;
	Ipanel_sitePtr       m_spPanelLoading;
	Ipanel_sitePtr       m_spPanelFail;
	CSiteUIRes           m_siteRes;
};
