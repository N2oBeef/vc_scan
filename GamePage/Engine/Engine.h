#pragma once
#include "IEngine.h"
#include "Base/SingletonT.h"
#include "Base/ContainerT.h"
#include "ScanCallbackOnUI.h"
#include "FixCallbackOnUI.h"

namespace Engine
{
    template<class T, class A1, class A2>
    class ThreadInfo
    {
    public:
        T* pT;
        A1* a1;
        A2* a2;
    };

    class CEngine
        : public Engine::IEngine
        , public CSingletonT<CEngine>
    {
        SINGLETON(CEngine);
        CEngine();

    public:
        virtual ~CEngine();

        void AddItem(IItem *pItem);

    public:
        virtual BOOL __stdcall Init(void);

        virtual void __stdcall Uninit(void);

        virtual int  __stdcall GetItemCount(void);

        virtual  IItem * __stdcall GetItem(int nIdx);

        //“Ï≤Ω…®√Ë
        virtual void __stdcall ScanAsync(IScanCallback *pScanCallback);
        virtual void __stdcall CancelScan(void);
		   
        //µ•œÓ–ﬁ∏¥
        virtual void __stdcall FixOneAsync(IItem* pItem, IFixCallback *pFixCallback);
        //“ªº¸–ﬁ∏¥
        virtual void __stdcall FixAllAsync(IFixCallback *pFixCallback);
        virtual void __stdcall CancelFix(void);

    private:
        void ScanThread();
        void FixOneThread(IItem* pItem);
        void FixThread();

        void StopScanThread();
        void StopFixThread();

        static unsigned int __stdcall ThreadProcScan(LPVOID lpParam);
        static unsigned int __stdcall ThreadProcFixOne(LPVOID lpParam);
        static unsigned int __stdcall ThreadProcFix(LPVOID lpParam);

        BOOL IsNeedStopScan(DWORD dwTimeout);
        BOOL IsNeedStopFix(DWORD dwTimeout);

    private:
        typedef CContainerT<IItem*> ItemContainer;
        ItemContainer m_itemContainer;

        HANDLE m_hThreadScan;
        HANDLE m_hEventStopScan; 
        CScanCallbackOnUI m_ScanCallbackUI;
        volatile LONG m_bCancelScan;

        HANDLE m_hThreadFix;
        HANDLE m_hEventStopFix; 
        CFixCallbackOnUI m_FixCallbackUI;
        volatile LONG m_bCancelFix;
    };
}
