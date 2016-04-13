#pragma once

#include "IItem.h"
#include "IList.h"
#include <list>

#include "Include/scan_wnd_id.h"
#include <boost/enable_shared_from_this.hpp>
typedef boost::function<void(IItem*)> FixSingleItemFucntion;

class CSoftwareEventSink;

class CListItem
	: public IDispEventSimpleImpl</*nID =*/ 1, CListItem, &DIID_Isite_events> //这个必须放第一个，方便强转
	, public IItem
	, public boost::enable_shared_from_this<CListItem>
{
public:
	BEGIN_SINK_MAP(CListItem)
		SINK_ENTRY_INFO(/*nID =*/ 1, DIID_Isite_events, 1, on_event, &on_event_info)
	END_SINK_MAP()

	//////////////////////////////////////////////////////////////////////////
	//事件消息宏    
	BEGIN_SITE_MSG_MAP(CListItem)
		SITE_COMMAND_CODE_HANDLER_BY_ID(IDC_LINK_INST, BN_CLICKED, OnLinkInst)		
	END_SITE_MSG_MAP()
	//事件消息宏结束
	//////////////////////////////////////////////////////////////////////////

	HRESULT OnLinkInst(WORD wNotifyCode, WORD wID, HWND hWndCtl, Isite* site );
	
public:
	void STDMETHODCALLTYPE SetList( IList *pList );
	void Init(Engine::IItem *pEngineItem, CSiteUIRes * siteRes);
	void FixSingleItemFunction(FixSingleItemFucntion fucn)
	{
		m_fixSingleItemFunction = fucn;
	}

public:
	CListItem(Ipanel_sitePtr spItemTemplate);
	~CListItem();
	
	// 获取列表中一个item的容器panel
	virtual Ipanel_sitePtr STDMETHODCALLTYPE GetItemPanel(void);

	// 添加子项
	virtual void STDMETHODCALLTYPE InsertChild(size_t nIndex, ItemPtr spChildItem);

	// 移除子项
	virtual void STDMETHODCALLTYPE RemoveChild(ItemPtr spItem);

	// 获取子项个数 
	virtual size_t  STDMETHODCALLTYPE GetChildCount(void);

	// 根据索引获取子项
	virtual ItemPtr STDMETHODCALLTYPE GetChild(size_t nIdx);

	// 根据id或者子项
	virtual ItemPtr STDMETHODCALLTYPE GetChildItem(int nID);

	// 获取项的id
	virtual int STDMETHODCALLTYPE GetID(void);
	virtual void STDMETHODCALLTYPE SetID(int nID);

	//设置/获取父项
	virtual void STDMETHODCALLTYPE SetParentItem(WeakItemPtr spWeakParent);
	virtual WeakItemPtr STDMETHODCALLTYPE GetParentItem();

	virtual void STDMETHODCALLTYPE SetIDS(const CString & szIDS);
	virtual const CString & STDMETHODCALLTYPE GetIDS(void);
	virtual Engine::IItem * STDMETHODCALLTYPE GetEngineItem(void);
	virtual void STDMETHODCALLTYPE Enable(BOOL bEnable);

public:
	virtual void __stdcall OnFixStart(void){};

	//单项修复时，只回调OnFixItemStart，OnFixItemEnd和OnProgress
	virtual void __stdcall OnFixItemStart(Engine::IItem *pItem);
	virtual void __stdcall OnFixItemEnd(Engine::IItem *pItem, BOOL bSuccess);

	virtual void __stdcall OnFixEvent(Engine::IItem *pItem, int nEvent){};

	//nProgress从0-100
	virtual void __stdcall OnFixProgress(Engine::IItem *pItem, int nProgress){};

	virtual void __stdcall OnFixEnd(void){};

private:
	void InitUI(void);

	template<typename T>
	void SetText(T, UINT uID, const _bstr_t & _bstr)
	{
		if (!m_spItemPanel) return;

		T spSite = m_spItemPanel->Getitem_by_cookie(uID, false);
		if (!spSite) return;

		spSite->set_text(_bstr);
		spSite->repaint(false);
	}
	
protected:
	Ipanel_sitePtr	  m_spItemPanel;
	ItemConatiner     m_itemContainer;

	CString           m_szIDS;
	int               m_nID;
	IList            *m_pList;

    WeakItemPtr       m_spWeakParentItem;
    CAtlAdviser       m_atlAdviser;
	 
	Engine::IItem *m_pEngineItem;
	CSiteUIRes * m_pSiteRes;
	FixSingleItemFucntion m_fixSingleItemFunction;

	BOOL              m_bHasProblem;
};

