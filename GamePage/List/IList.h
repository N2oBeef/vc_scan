#pragma once
#include <list>
using namespace std;

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "Engine/IEngine.h"

class IItem;

typedef boost::shared_ptr<IItem> ItemPtr;
typedef list<ItemPtr>  ItemConatiner;
typedef ItemConatiner::iterator ItemIterator;
typedef ItemConatiner::value_type ItemType;

typedef boost::weak_ptr<IItem> WeakItemPtr;

class IList
{
public:
	STDMETHOD_(void,InsertItem)(int nIndexAfter, ItemPtr spItem) PURE;
	STDMETHOD_(void,RemoveItem)(ItemPtr spItem) PURE;
	STDMETHOD_(void,Clear)(void) PURE;
	STDMETHOD_(Irich_list_sitePtr,GetProxyList)(void) PURE;
	STDMETHOD_(ItemPtr,GetItem)(const CString & szIDS) PURE;
	STDMETHOD_(ItemPtr,GetItem)(int nId) PURE;
	STDMETHOD_(ItemPtr,GetItem)(Engine::IItem *pEngineItem) PURE;
	STDMETHOD_(size_t,GetChildCount)(void) PURE;
	STDMETHOD_(ItemPtr,GetChild)(size_t nIndex) PURE;
	STDMETHOD_(void,UpdateLayout)(void) PURE;
	STDMETHOD_(void,Repaint)(BOOL bEraseBkgnd) PURE;
	STDMETHOD_(Ipanel_sitePtr,GetTemplateItem)(void) PURE;	
};
