#pragma once

#include "PanelBase.h"
#include "Include/scan_wnd_id.h"
#include "List/List.h"

#include "Engine/Engine.h"

class CScanPanel
	: public IDispEventSimpleImpl</*nID =*/ 1, CScanPanel, &DIID_Isite_events> //这个必须放第一个，方便强转
	, public CPanelBase
	, public Engine::IScanCallback
	, public Engine::IFixCallback
{
public:
	CScanPanel(void);
	BEGIN_SINK_MAP(CScanPanel)
		SINK_ENTRY_INFO(/*nID =*/ 1, DIID_Isite_events, 1, on_event, &on_event_info)
	END_SINK_MAP()

	//////////////////////////////////////////////////////////////////////////
	//事件消息宏    
	BEGIN_SITE_MSG_MAP(CScanPanel)
		SITE_COMMAND_CODE_HANDLER_BY_ID(IDC_BTN_CANCEL, BN_CLICKED, OnBtnCancel)
		SITE_COMMAND_CODE_HANDLER_BY_ID(IDC_BTN_MORE_GAMES, BN_CLICKED, OnBtnMoreGames)
		SITE_COMMAND_CODE_HANDLER_BY_ID(IDC_CPU_PERCENT, BN_CLICKED, OnLinkPerformance)
		SITE_COMMAND_CODE_HANDLER_BY_ID(IDC_GPU_PERCENT, BN_CLICKED, OnLinkPerformance)
		SITE_COMMAND_CODE_HANDLER_BY_ID(IDC_MEM_PERCENT, BN_CLICKED, OnLinkPerformance)
		
 	END_SITE_MSG_MAP()
	//事件消息宏结束
	//////////////////////////////////////////////////////////////////////////

public:
	virtual void  __stdcall OnPanelActivate( const CRect &rct );
	virtual void __stdcall UpdateScores(void);
	virtual void __stdcall UpdateScoresRank(void);

public:
	virtual void __stdcall OnScanStart(void);
	virtual void __stdcall OnScanItemStart(Engine::IItem *pItem);
	virtual void __stdcall OnScanItemEnd(Engine::IItem *pItem);

	//nProgress从0-100
	virtual void __stdcall OnScanProgress(int nProgress);

	virtual void __stdcall OnScanEnd(void) ;

public:
	virtual void __stdcall OnFixStart(void);

	//单项修复时，只回调OnFixItemStart，OnFixItemEnd和OnProgress
	virtual void __stdcall OnFixItemStart(Engine::IItem *pItem);
	virtual void __stdcall OnFixItemEnd(Engine::IItem *pItem, BOOL bSuccess);
	//nProgress从0-100
	virtual void __stdcall OnFixProgress(Engine::IItem *pItem, int nProgress);

	virtual void __stdcall OnFixEvent(Engine::IItem *pItem, int nEvent);

	virtual void __stdcall OnFixEnd(void);

public:
	// 开始体检的按钮点击
	HRESULT OnBtnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, Isite* site );

	HRESULT OnBtnMoreGames(WORD wNotifyCode, WORD wID, HWND hWndCtl, Isite* site );
	HRESULT OnLinkPerformance(WORD wNotifyCode, WORD wID, HWND hWndCtl, Isite* site );

	
	void OnBtnFixSingleItem(IItem *pItem);

private:
	void InitEvent(void);
	void InitCtrl(void);
	void AddToList(Engine::IItem *pItem);
	void SetBtnCancel(BOOL bCancel);
	void UpdateListHead(void);
	void SetTopTitleText(void);
	void OnBtnFixAll(void);
	void OnBtnCancelFixAll(void);
	void OnBtnCancelSingleFix(void);
	void UpdateStatusIcon(BOOL bError);
	CString GetLevel(int nPercen);
	void __stdcall UpdateBenchRank(void);

private:
	CList  m_list;		
	Iprogress_sitePtr m_spProgress;
	CSiteUIRes        m_siteRes;
	
	Istatic_sitePtr   m_spListHead;
	Istatic_sitePtr   m_spFirstLine;
	Istatic_sitePtr   m_spSecondLine;
	Ibutton_sitePtr   m_spScanFixBtn;
	Ibutton_sitePtr   m_spBtnMoreGames;
	Ipicture_sitePtr  m_spPicRotating;

	CRect             m_rctFirstLine;
	CRect             m_rctSecondLine;
	CRect             m_rctBtnFix;
	CRect             m_rctBtnCancel;
	CRect             m_rctFixProgress;
	CRect             m_rctScanProgress;
	int               m_nProblemCount;
	int               m_nTotalCount;
	DWORD             m_dwCookCpu;
	DWORD             m_dwCookGpu;
	DWORD             m_dwCookMem;
	enum
	{
		FUCNTION_SCAN,
		FUNCTION_CANCEL_SCAN,
		FUNCTION_ONE_KEY_FIX,
		FUNCTION_CANCEL_ONEKEY_FIX,
		FUNCTION_CANCLE_SINGLE_FIX,
	}BTN_FUNCTION;

	int m_nBtnScanFixFunction;
};
