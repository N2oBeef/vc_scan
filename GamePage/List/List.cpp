#include "stdafx.h"
#include "List.h"
#include <algorithm>

#include "IItem.h"
#include <SiteUI\Helper\SiteUIHelper.h>
#include "Include\scan_wnd_id.h"

CList::CList()
{
}

CList::~CList()
{
	ClearAll();
}

void CList::ClearAll( void )
{
	ItemIterator itBegin = m_itemContainer.begin();
	ItemIterator itEnd = m_itemContainer.end();

	if (m_spRichList)
	{
		m_spRichList->remove_all_item();
		m_spRichList->reset_scrollbar();
		m_spRichList->repaint(FALSE);
	}

	m_itemContainer.clear();
}

STDMETHODIMP_(void) CList::Clear( void )
{
	ClearAll();
}

STDMETHODIMP_(ItemPtr) CList::GetItem( const CString & szIDS )
{
	ItemIterator itBegin = m_itemContainer.begin();
	ItemIterator itEnd = m_itemContainer.end();

	for (; itBegin != itEnd; ++itBegin)
	{
		ItemPtr spItem = *itBegin;
		if (spItem && spItem->GetIDS().CompareNoCase(szIDS) == 0)
		{
			return spItem;
		}
	}

	return ItemPtr();
}

STDMETHODIMP_(ItemPtr) CList::GetItem( int nId )
{
	ItemIterator itBegin = m_itemContainer.begin();
	ItemIterator itEnd = m_itemContainer.end();

	for (; itBegin != itEnd; ++itBegin)
	{
		ItemPtr spItem = *itBegin;
		if (spItem && spItem->GetID() == nId)
		{
			return spItem;
		}
	}

	return ItemPtr();
}

STDMETHODIMP_(ItemPtr) CList::GetItem( Engine::IItem *pEngineItem )
{
	ItemIterator itBegin = m_itemContainer.begin();
	ItemIterator itEnd = m_itemContainer.end();

	for (; itBegin != itEnd; ++itBegin)
	{
		ItemPtr spItem = *itBegin;
		if (spItem && spItem->GetEngineItem() == pEngineItem)
		{
			return spItem;
		}
	}

	return ItemPtr();
}

STDMETHODIMP_(void) CList::UpdateLayout( void )
{
	if (m_spRichList == NULL) return;

	m_spRichList->update_layout();
}


STDMETHODIMP_(Irich_list_sitePtr) CList::GetProxyList( void )
{
	ATLASSERT(m_spRichList);

	return m_spRichList;
}

void CList::Init( Irich_list_sitePtr spRichList)
{
	ATLASSERT(spRichList );
	if (spRichList == NULL ) return;

	m_spRichList = spRichList;

#ifdef _DEBUG
    CRect rc = m_spRichList->get_bound();
#endif
	m_spRichList->set_state(ss_expanded, ss_none, FALSE);
	m_spRichList->set_control_style(rs_force_draw_hover, 0);

	InitControls();
	m_spRichList->remove_all_item();
	m_spRichList->reset_scrollbar();
	m_spRichList->set_item_light_cap(0, 0);
	InitEvent();
}

void CList::InitControls( void )
{
	// 保存列表
	if (m_spRichList == NULL) return;

	CRect rctList = m_spRichList->get_bound();

	// 保存列表中的三级item，当做动态创建的模板
	if (m_sp1stLevelItem == NULL)
	{
		m_sp1stLevelItem = m_spRichList->Getitem_by_cookie(IDC_LIST_ITEM, false);	
		ATLASSERT(m_sp1stLevelItem);
		if (!m_sp1stLevelItem) return;

		CRect item_pos = m_sp1stLevelItem->get_bound();
		item_pos.right = rctList.right;
		m_sp1stLevelItem->set_bound(&item_pos, true);
	}
}

STDMETHODIMP_(size_t) CList::GetChildCount( void )
{
	return m_itemContainer.size();
}

STDMETHODIMP_(ItemPtr) CList::GetChild( size_t nIndex )
{
	if (nIndex < 0 || nIndex >= GetChildCount()) return ItemPtr();

	ItemIterator it = m_itemContainer.begin();
	for (size_t i = 0; i < nIndex; ++i)
	{
		it++;
	}

	return *it;
}


STDMETHODIMP_(void) CList::InsertItem( int nIndexAfter, ItemPtr spItem )
{
	if (!spItem) return;	
	if (m_spRichList == NULL) return;

	int nCount = m_spRichList->get_count();
	nIndexAfter = min(nIndexAfter, nCount - 1);

	Ipanel_sitePtr spItemPanel = spItem->GetItemPanel();
	if (spItemPanel == NULL) return;

	if (nIndexAfter == -1)// 插入到最后
	{
		m_spRichList->add_item(spItemPanel);
	}
	else
	{
		m_spRichList->insert_item(nIndexAfter, spItemPanel);
	}

	m_itemContainer.push_back(spItem);
}

STDMETHODIMP_(void) CList::Repaint( BOOL bEraseBkgnd)
{
	if (m_spRichList)
	{
		m_spRichList->repaint(bEraseBkgnd);
	}
}


void CList::InitEvent( void )
{
	if (!m_spRichList) return;

	m_atlAdviser.AtlAdvise(m_spRichList, (Isite_events *)this, DIID_Isite_events);
}


STDMETHODIMP_(void) CList::RemoveItem( ItemPtr spItem)
{
	if (!m_spRichList) return;

	ItemIterator itBegin = m_itemContainer.begin();
	ItemIterator itEnd = m_itemContainer.end();

	for (; itBegin != itEnd; ++itBegin)
	{
		ItemPtr spItemFind = *itBegin;
		if (spItemFind == spItem)
		{
			m_itemContainer.erase(itBegin);
			m_spRichList->remove_item(spItem->GetItemPanel());		
			//m_spRichList->reset_scrollbar();
			m_spRichList->update_layout();
			m_spRichList->repaint(false);
			return;
		}
	}
}

STDMETHODIMP_(Ipanel_sitePtr) CList::GetTemplateItem( void )
{
	ATLASSERT(m_sp1stLevelItem);
	return m_sp1stLevelItem;
}

void CList::Enable( BOOL bEnable )
{
	ItemIterator itBegin = m_itemContainer.begin();
	ItemIterator itEnd = m_itemContainer.end();

	for (; itBegin != itEnd; ++itBegin)
	{
		ItemPtr spItem = *itBegin;
		if (spItem)
		{
			spItem->Enable(bEnable);
		}
	}
}
