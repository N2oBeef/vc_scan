#pragma once

#include "IList.h"

#include <boost/function.hpp>
#include "Engine/IEngine.h"

class IItem;
typedef boost::function<void(void)> OnDownloadingCallback;

class IItem
	: public Engine::IFixCallback
{
public:
	virtual ~IItem(){}
	
	template <typename T>
	void Init(T param){};

	// 获取列表中一个item的容器panel
	virtual Ipanel_sitePtr STDMETHODCALLTYPE GetItemPanel(void) PURE;

	// 添加子项
	virtual void STDMETHODCALLTYPE InsertChild(size_t nIndex, ItemPtr spChildItem)PURE;

	// 移除子项
	virtual void STDMETHODCALLTYPE RemoveChild(ItemPtr spItem) PURE;

	// 获取子项个数 
	virtual size_t  STDMETHODCALLTYPE GetChildCount(void) PURE;

	// 根据索引获取子项
	virtual ItemPtr STDMETHODCALLTYPE GetChild(size_t nIdx) PURE;

	// 根据id或者子项
	virtual ItemPtr STDMETHODCALLTYPE GetChildItem(int nID) PURE;

	// 获取项的id
	virtual int STDMETHODCALLTYPE GetID(void) = 0;
	virtual void STDMETHODCALLTYPE SetID(int nID) = 0;

    //设置/获取父项
    virtual void STDMETHODCALLTYPE SetParentItem(WeakItemPtr spWeakParent) = 0;
    virtual WeakItemPtr  STDMETHODCALLTYPE GetParentItem() = 0;

	virtual void STDMETHODCALLTYPE SetIDS(const CString & szIDS) PURE;
	virtual const CString & STDMETHODCALLTYPE GetIDS(void) PURE;

	virtual Engine::IItem * STDMETHODCALLTYPE GetEngineItem(void) PURE;

	virtual void STDMETHODCALLTYPE Enable(BOOL bEnable) PURE; 
};
