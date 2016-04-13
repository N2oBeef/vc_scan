#include "stdafx.h"
#include "ListItem.h"

#include "Base/UIThreadWindow.h"
#include "Engine/Engine.h"
using namespace Engine;

#include <atlsafe.h>

CListItem::CListItem(Ipanel_sitePtr spItemTemplate)
: m_nID(0)
, m_pList(NULL)
, m_pEngineItem(NULL)
, m_pSiteRes(NULL)
, m_bHasProblem(FALSE)
{
	if (m_spItemPanel != NULL || spItemTemplate == NULL) return ;

	CopyCreateSite(g_SiteUI.GetFactory(), m_spItemPanel, __uuidof(panel_site), spItemTemplate);
	ATLASSERT(m_spItemPanel);	
}

Ipanel_sitePtr  STDMETHODCALLTYPE CListItem::GetItemPanel( void )
{
	ATLASSERT(m_spItemPanel);

	return m_spItemPanel;
}

int STDMETHODCALLTYPE CListItem::GetID( void )
{	
	return m_nID;
}


void STDMETHODCALLTYPE CListItem::SetID( int nID )
{
	m_nID = nID;
}


CListItem::~CListItem()
{
	m_itemContainer.clear();
}

ItemPtr STDMETHODCALLTYPE CListItem::GetChildItem( int nID )
{
	ItemIterator itBegin = m_itemContainer.begin();
	ItemIterator itEnd = m_itemContainer.end();

	for (; itBegin != itEnd; ++itBegin)
	{
		ItemPtr spItem = *itBegin;
		if (spItem && spItem->GetID() == nID)
		{
			return spItem;
		}
	}

	return ItemPtr();
}

size_t STDMETHODCALLTYPE CListItem::GetChildCount(void)
{
	if (m_spItemPanel == NULL) return 0;

	return  m_itemContainer.size();
}

ItemPtr STDMETHODCALLTYPE CListItem::GetChild( size_t nIdx )
{
	if (m_spItemPanel == NULL) return ItemPtr();

	size_t nCount = GetChildCount();
	if (nIdx < 0 || nIdx > nCount -1) return ItemPtr();

	ItemIterator itBegin = m_itemContainer.begin();

	for (size_t i = 0; i < nIdx; i++)
	{
		itBegin++;
	}

	return *itBegin;
}

void STDMETHODCALLTYPE CListItem::RemoveChild( ItemPtr spItem )
{
	if (spItem  == NULL || m_spItemPanel == NULL) return;

	ItemIterator itBegin = m_itemContainer.begin();
	ItemIterator itEnd = m_itemContainer.end();

	for (; itBegin != itEnd; ++itBegin)
	{
		ItemPtr spItemFind = *itBegin;
		if (spItemFind == spItem)
		{	
			//spItemFind->SetParentItem(0);
			Ipanel_sitePtr spPanel = spItemFind->GetItemPanel();
			if (spPanel)
			{
				m_spItemPanel->remove_child(spPanel);
			}

			m_itemContainer.erase(itBegin);
			return;
		}
	}
}

void STDMETHODCALLTYPE CListItem::SetList( IList *pList )
{
	ATLASSERT(pList);

	m_pList = pList;	
}


void STDMETHODCALLTYPE CListItem::InsertChild( size_t nIndex, ItemPtr spChildItem )
{
	if (m_spItemPanel == NULL) return;
	if (!spChildItem) return;

	nIndex = max(0, nIndex);
	nIndex = min(nIndex, GetChildCount());

	Ipanel_sitePtr spPanel = spChildItem->GetItemPanel();
	if (!spPanel) return;

	IsitePtr spSiteAfter = NULL;
	m_spItemPanel->get_child_item(nIndex, &spSiteAfter);

	m_spItemPanel->insert_child(spSiteAfter, spPanel);

	spChildItem->SetParentItem(shared_from_this());//添加父项指针
	m_itemContainer.push_back(spChildItem);	
}


void STDMETHODCALLTYPE CListItem::SetParentItem( WeakItemPtr spWeakParent )
{
	ATLASSERT(!spWeakParent.expired());

	m_spWeakParentItem = spWeakParent;
}

WeakItemPtr STDMETHODCALLTYPE CListItem::GetParentItem()
{
	return m_spWeakParentItem;
}

void STDMETHODCALLTYPE CListItem::SetIDS( const CString & szIDS )
{
	m_szIDS = szIDS;
}

const CString & STDMETHODCALLTYPE CListItem::GetIDS( void )
{
	return m_szIDS;
}

void CListItem::Init(Engine::IItem *pEngineItem, CSiteUIRes * siteRes)
{
	if (!m_spItemPanel) return;
	ATLASSERT(pEngineItem);
	m_pEngineItem = pEngineItem;
	m_pSiteRes = siteRes;

	InitUI();
}

void CListItem::InitUI(void)
{
	if (!m_spItemPanel) return;
	
	//name
	WCHAR szName[MAX_PATH] = {};
	m_pEngineItem->GetName(szName, MAX_PATH);
	SetText(Istatic_sitePtr(), IDC_NAME, szName);
	
	m_bHasProblem = m_pEngineItem->HasProblem();
	Ipicture_sitePtr spIcon = CSiteUIHelper::FindSitePtr(m_spItemPanel, IDC_PIC_ICON);
	ATLASSERT(spIcon);
	unsigned long uImg = m_pSiteRes->GetResId(m_bHasProblem ? L"IDI_ITEM_ERROR" : L"IDI_ITEM_OK");
	spIcon->load_image_by_id(uImg);

	_bstr_t szHead = m_pSiteRes->LoadStringById(m_bHasProblem ? L"IDS_NOT_INSTALL": L"IDS_INSTALLED");
	SetText(Istatic_sitePtr(), IDC_HEAD_TEXT, szHead);

	Istatic_sitePtr spInstall = CSiteUIHelper::FindSitePtr(m_spItemPanel, IDC_LINK_INST);
	ATLASSERT(spInstall);
	if (m_bHasProblem)
	{
		CSiteUIHelper::ShowSiteUI(spInstall);
		m_atlAdviser.AtlAdvise(spInstall, (Isite_events*)this, DIID_Isite_events);
	}
	else
	{
		CSiteUIHelper::HideSiteUI(spInstall);
	}
}


HRESULT CListItem::OnLinkInst( WORD wNotifyCode, WORD wID, HWND hWndCtl, Isite* site )
{
	ATLASSERT(m_pEngineItem && m_fixSingleItemFunction);
	
	if (m_fixSingleItemFunction)
	{
		m_fixSingleItemFunction(this);
	}

	return 0;
}

void __stdcall CListItem::OnFixItemStart( Engine::IItem *pItem )
{
	WCHAR szName[MAX_PATH] = {};
	pItem->GetName(szName, MAX_PATH);
	LOG(L"CListItem::OnFixItemStart : name = %s", szName);

	SetText(Istatic_sitePtr(), IDC_HEAD_TEXT, L"[修复中]");

	Ipicture_sitePtr spIcon = CSiteUIHelper::FindSitePtr(m_spItemPanel, IDC_PIC_ICON);
	CSiteUIHelper::HideSiteUI(spIcon);

	Ipicture_sitePtr spFixing = CSiteUIHelper::FindSitePtr(m_spItemPanel, IDC_PIC_FIXING);
	unsigned long uImg = m_pSiteRes->GetResId(L"IDI_ITEM_FIXING");
	spFixing->load_image_by_id(uImg);

	CComSafeArray<unsigned int> safeArray;
	for (int i =0; i < 11; ++i)
	{
		safeArray.Add(50);
	}
	
	CComVariant varArray(safeArray.Detach());
	spFixing->set_repeat(pt_repeat, 0);
	spFixing->set_frame(12, 0, varArray);

	CSiteUIHelper::ShowSiteUI(spFixing);
}

void __stdcall CListItem::OnFixItemEnd( Engine::IItem *pItem, BOOL bSuccess )
{
	WCHAR szName[MAX_PATH] = {};
	pItem->GetName(szName, MAX_PATH);
	LOG(L"CListItem::OnFixItemEnd : success = %d, name = %s", bSuccess, szName);

	SetText(Istatic_sitePtr(), IDC_HEAD_TEXT, bSuccess ? L"[已修复]" : L"[未安装]");
	Istatic_sitePtr spInstall = CSiteUIHelper::FindSitePtr(m_spItemPanel, IDC_LINK_INST);
	if (bSuccess)
	{
		CSiteUIHelper::HideSiteUI(spInstall);
	}

	Ipicture_sitePtr spIcon = CSiteUIHelper::FindSitePtr(m_spItemPanel, IDC_PIC_ICON);
	CSiteUIHelper::ShowSiteUI(spIcon);
	unsigned long uImg = m_pSiteRes->GetResId(bSuccess ? L"IDI_ITEM_OK" : L"IDI_ITEM_ERROR");
	spIcon->load_image_by_id(uImg);
	spIcon->repaint(FALSE);

	Ipicture_sitePtr spFixing = CSiteUIHelper::FindSitePtr(m_spItemPanel, IDC_PIC_FIXING);
	CSiteUIHelper::HideSiteUI(spFixing);
	
	if (bSuccess)
	{
		m_pList->RemoveItem(shared_from_this());
		m_pList->InsertItem(-1, shared_from_this());
		m_pList->UpdateLayout();
	}
}

Engine::IItem * STDMETHODCALLTYPE CListItem::GetEngineItem( void )
{
	return m_pEngineItem;
}

void STDMETHODCALLTYPE CListItem::Enable( BOOL bEnable )
{
	if (!m_bHasProblem) return;

	Istatic_sitePtr spInstall = CSiteUIHelper::FindSitePtr(m_spItemPanel, IDC_LINK_INST);
	if (bEnable)
	{
		CSiteUIHelper::EnableSiteUI(spInstall);
	}
	else
	{
		CSiteUIHelper::DisableSiteUI(spInstall);
	}
}
