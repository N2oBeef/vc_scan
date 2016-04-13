#include "stdafx.h"
#include "Engine.h"
#include "EngineConfigure.h"
#include "Base/UIThreadWindow.h"

using namespace Engine;

CEngine::CEngine()
: m_hThreadScan(NULL)
, m_hEventStopScan(NULL)
, m_bCancelScan(FALSE)
, m_hThreadFix(NULL)
, m_hEventStopFix(NULL)
, m_bCancelFix(FALSE)
{	
}

CEngine::~CEngine()
{
    Uninit();
}

BOOL __stdcall CEngine::Init( void )
{
    BOOL bRet = FALSE;

    if (!CUIThreadWindow::GetUIThreadWindow())
    {
        CUIThreadWindow::InitUIThreadWindow();
    }

    do 
    {
        CEngineConfigure config;
        bRet = config.Init(this);
        if (!bRet)
        {
            break;
        }

    } while (0);

    return bRet;
}

void __stdcall Engine::CEngine::Uninit( void )
{
    StopScanThread();
    StopFixThread();

    int nCount = m_itemContainer.GetCount();
    for (int i = 0; i < nCount; ++i)
    {
        IItem* pItem = m_itemContainer.GetItem(i);
        if (pItem)
        {
            pItem->Destroy();
        }
    }
    m_itemContainer.Clear();
}

void Engine::CEngine::StopScanThread()
{
    CancelScan();

    if (m_hThreadScan)
    {
        if (WAIT_TIMEOUT == WaitForSingleObject(m_hThreadScan, 2000))
        {
            TerminateThread(m_hThreadScan, -1);
        }
        CloseHandle(m_hThreadScan);
        m_hThreadScan = NULL;
    }

    if (m_hEventStopScan)
    {
        CloseHandle(m_hEventStopScan);
        m_hEventStopScan  = NULL;
    }
}

void Engine::CEngine::StopFixThread()
{
    CancelFix();

    if (m_hThreadFix)
    {
        if (WAIT_TIMEOUT == WaitForSingleObject(m_hThreadFix, 2000))
        {
            TerminateThread(m_hThreadFix, -1);
        }
        CloseHandle(m_hThreadFix);
        m_hThreadFix = NULL;
    }

    if (m_hEventStopFix)
    {
        CloseHandle(m_hEventStopFix);
        m_hEventStopFix  = NULL;
    }
}

int  __stdcall CEngine::GetItemCount( void )
{
    return m_itemContainer.GetCount();
}

IItem * __stdcall CEngine::GetItem( int nIdx )
{
    return m_itemContainer.GetItem(nIdx);
}

void Engine::CEngine::AddItem( IItem *pItem )
{
    m_itemContainer.AddItem(pItem);
}

//“Ï≤Ω…®√Ë
void __stdcall CEngine::ScanAsync( IScanCallback *pScanCallback )  
{
    if (m_hThreadScan)
    {
        return;
    }

    InterlockedExchange(&m_bCancelScan, FALSE);

    m_ScanCallbackUI.SetCallback(pScanCallback);

    ATLASSERT(m_hEventStopScan == NULL);
    m_hEventStopScan = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (m_hEventStopScan == NULL)
        return;
		
    ATLASSERT(m_hThreadScan == NULL);
    m_hThreadScan = (HANDLE)_beginthreadex(NULL, 0, ThreadProcScan, this, 0, NULL);
    if (m_hThreadScan == NULL)
        return;
}

void __stdcall CEngine::CancelScan()
{
    InterlockedExchange(&m_bCancelScan, TRUE);

    if (m_hEventStopScan)
    {
        SetEvent(m_hEventStopScan);
    }
}

unsigned int __stdcall Engine::CEngine::ThreadProcScan( LPVOID lpParam )
{
    Engine::CEngine* pThis = static_cast<Engine::CEngine*>(lpParam);
    pThis->ScanThread();
    return 0;
}

void CEngine::ScanThread(void)
{
    m_ScanCallbackUI.OnScanStart();

    int nCount = m_itemContainer.GetCount();
    for (int i = 0; i < nCount; ++i)
    {
        if (m_bCancelScan)
        {
            break;
        }

        IItem *pItem = m_itemContainer.GetItem(i);
        if (pItem)
        {
            int nProgress = (i+1)*100/nCount;
            m_ScanCallbackUI.OnScanProgress(nProgress);

            pItem->Scan(&m_ScanCallbackUI);
        }
    }

    m_ScanCallbackUI.OnScanEnd();

    if (m_hThreadScan)
    {
        CloseHandle(m_hThreadScan);
        m_hThreadScan = NULL;
    }

    if (m_hEventStopScan)
    {
        CloseHandle(m_hEventStopScan);
        m_hEventStopScan  = NULL;
    }
}

void __stdcall CEngine::FixOneAsync(IItem* pItem, IFixCallback *pFixCallback)
{
    if (m_hThreadFix)
    {
        return;
    }

    if (NULL == pItem)
    {
        ATLASSERT(FALSE);
        return;
    }

    InterlockedExchange(&m_bCancelFix, FALSE);

    m_FixCallbackUI.SetCallback(pFixCallback);

    ATLASSERT(m_hEventStopFix == NULL);
    m_hEventStopFix = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (m_hEventStopFix == NULL)
        return;

    ATLASSERT(m_hThreadFix == NULL);
    ThreadInfo<void, IItem, void>* pThreadInfo = new ThreadInfo<void, IItem, void>();
    pThreadInfo->pT = this;
    pThreadInfo->a1 = pItem;
    pThreadInfo->a2 = NULL;
    m_hThreadFix = (HANDLE)_beginthreadex(NULL, 0, ThreadProcFixOne, pThreadInfo, 0, NULL);
    if (m_hThreadFix == NULL)
        return;
}

unsigned int __stdcall Engine::CEngine::ThreadProcFixOne( LPVOID lpParam )
{
    ThreadInfo<void, IItem, void>* pThreadInfo = (ThreadInfo<void, IItem, void>*)(lpParam);
    Engine::CEngine* pThis = (Engine::CEngine*)(pThreadInfo->pT);
    IItem* pItem = (IItem*)pThreadInfo->a1;
    delete pThreadInfo;

    pThis->FixOneThread(pItem);
    return 0;
}

void Engine::CEngine::FixOneThread( IItem* pItem )
{
    m_FixCallbackUI.OnFixStart();

    if (pItem)
    {
        pItem->Fix(&m_FixCallbackUI);
    }

    m_FixCallbackUI.OnFixEnd();

    if (m_hThreadFix)
    {
        CloseHandle(m_hThreadFix);
        m_hThreadFix = NULL;
    }

    if (m_hEventStopFix)
    {
        CloseHandle(m_hEventStopFix);
        m_hEventStopFix  = NULL;
    }
}

void __stdcall CEngine::FixAllAsync( IFixCallback *pFixCallback )
{
    if (m_hThreadFix)
    {
        return;
    }

    InterlockedExchange(&m_bCancelFix, FALSE);

    m_FixCallbackUI.SetCallback(pFixCallback);

    ATLASSERT(m_hEventStopFix == NULL);
    m_hEventStopFix = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (m_hEventStopFix == NULL)
        return;

    ATLASSERT(m_hThreadFix == NULL);
    m_hThreadFix = (HANDLE)_beginthreadex(NULL, 0, ThreadProcFix, this, 0, NULL);
    if (m_hThreadFix == NULL)
        return;
}

unsigned int __stdcall Engine::CEngine::ThreadProcFix( LPVOID lpParam )
{
    Engine::CEngine* pThis = static_cast<Engine::CEngine*>(lpParam);
    pThis->FixThread();
    return 0;
}

void CEngine::FixThread(void)
{
    m_FixCallbackUI.OnFixStart();

    int nCount = m_itemContainer.GetCount();
    for (int i = nCount -1; i >= 0; --i)
    {
        if (IsNeedStopFix(0))
        {
            break;
        }

        IItem *pItem = m_itemContainer.GetItem(i);
        if (pItem && pItem->HasProblem())
        {
            pItem->Fix(&m_FixCallbackUI);
        }
    }

    m_FixCallbackUI.OnFixEnd();

    if (m_hThreadFix)
    {
        CloseHandle(m_hThreadFix);
        m_hThreadFix = NULL;
    }

    if (m_hEventStopFix)
    {
        CloseHandle(m_hEventStopFix);
        m_hEventStopFix  = NULL;
    }
}

void __stdcall CEngine::CancelFix()
{
    InterlockedExchange(&m_bCancelFix, TRUE);

    if (m_hEventStopFix)
    {
        SetEvent(m_hEventStopFix);
    }

    int nCount = m_itemContainer.GetCount();
    for (int i = 0; i < nCount; ++i)
    {
        IItem *pItem = m_itemContainer.GetItem(i);
        if (pItem)
        {
            pItem->CancelFix();
        }
    }
}

BOOL Engine::CEngine::IsNeedStopScan( DWORD dwTimeout )
{
    if (m_hEventStopScan)
    {
        return (WAIT_TIMEOUT != WaitForSingleObject(m_hEventStopScan, dwTimeout));
    }
    return TRUE;
}

BOOL Engine::CEngine::IsNeedStopFix( DWORD dwTimeout )
{
    if (m_hEventStopFix)
    {
        return (WAIT_TIMEOUT != WaitForSingleObject(m_hEventStopFix, dwTimeout));
    }
    return TRUE;
}

