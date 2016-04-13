#pragma once

//////////////////////////////////////////////////////////////////////////
//IEngine拥有所有的IItem的所有权，外部只能用，不能删
//////////////////////////////////////////////////////////////////////////

namespace Engine
{
	
	class IEngine;
	class IScanCallback;
	class IFixCallback;
	class IItem;

	typedef enum _FIX_EVENT_ID
	{
		FIX_EVENT_START_INST = 1, //开始安装，界面不能取消
	}FIX_EVENT_ID;

    class IEngine
    {
    public:
        virtual BOOL __stdcall Init(void) = 0;

        virtual void __stdcall Uninit(void) = 0;

        virtual int  __stdcall GetItemCount(void) = 0;

        virtual  IItem * __stdcall GetItem(int nIdx) = 0;

        //取消之后，还是需要回调OnScanEnd或OnFixEnd, 在OnScanEnd或OnFixEnd回调之前重复调用Scan或Fix无效

        //异步扫描
        virtual void __stdcall ScanAsync(IScanCallback *pScanCallback) = 0;
        virtual void __stdcall CancelScan(void) = 0;

        //单项修复
        virtual void __stdcall FixOneAsync(IItem* pItem, IFixCallback *pFixCallback) = 0;
        //一键修复
        virtual void __stdcall FixAllAsync(IFixCallback *pFixCallback) = 0;

        virtual void __stdcall CancelFix(void) = 0;
    };

    class IItem
    {
    public:
        virtual void __stdcall Destroy() = 0;

        virtual void __stdcall GetName(WCHAR * pszName, int nMaxCount) = 0;

        //是否有问题
        virtual BOOL __stdcall HasProblem(void) = 0; 

        //方法供引擎内部使用
        virtual void __stdcall Scan(IScanCallback *pCallback) = 0;
        virtual void __stdcall CancelScan(void) = 0;

        virtual void __stdcall Fix(IFixCallback *pFixCallback) = 0;
        virtual void __stdcall CancelFix(void) = 0;
    };

	//////////////////////////////////////////////////////////////////////////
	//callback 会从调用Init的线程回调
	class IScanCallback
	{
	public:
		virtual void __stdcall OnScanStart(void) = 0;
		virtual void __stdcall OnScanItemStart(IItem *pItem) = 0;
		virtual void __stdcall OnScanItemEnd(IItem *pItem) = 0;

		//nProgress从0-100
		virtual void __stdcall OnScanProgress(int nProgress) = 0;

		virtual void __stdcall OnScanEnd(void) = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	//callback 会从调用Init的线程回调
	class IFixCallback
	{
	public:
		virtual void __stdcall OnFixStart(void) = 0;
		
		//单项修复时，也全部回调
		virtual void __stdcall OnFixItemStart(IItem *pItem) = 0;
		virtual void __stdcall OnFixItemEnd(IItem *pItem, BOOL bSuccess) = 0;

		virtual void __stdcall OnFixEvent(IItem *pItem, int nEvent) = 0;

		//nProgress从0-100
		virtual void __stdcall OnFixProgress(IItem *pItem, int nProgress) = 0;

		virtual void __stdcall OnFixEnd(void) = 0;
	};
};