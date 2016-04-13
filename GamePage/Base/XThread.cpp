#include "stdafx.h"
#include "XThread.h"

BOOL CXThread::Init(IThreadCallback* pCallback)
{
	HRESULT hRet = m_ThreadImpl.Initialize();
	if(FAILED(hRet)) return FALSE;

	BOOL bRet = m_Event.Create(NULL, FALSE, FALSE, NULL);
	ATLASSERT(bRet);
	if(!bRet) return FALSE;

	m_ThreadImpl.AddHandle(m_Event, this, 0);
	m_pCallback = pCallback;

	InterlockedExchange(&m_bInit, 1);
	return TRUE;
}

VOID CXThread::UnInit()
{
	m_pCallback = NULL;
	InterlockedExchange(&m_bInit, 0);
}

BOOL CXThread::Start(LONG lParam, IThreadCallback* pCallback)
{
	if(IsBusy()) return FALSE;

	if(InterlockedCompareExchange(&m_bInit, 0, 0) == 0)
	{
		if(!Init(pCallback)) return FALSE;
	}

	InterlockedExchange(&m_lParam, lParam);
	m_Event.Set();

	return TRUE;
}

BOOL CXThread::Exit(DWORD dwMill)
{
	m_ThreadImpl.Shutdown(dwMill);
	return TRUE;
}

BOOL CXThread::IsBusy()
{
	return (InterlockedCompareExchange(&m_bRunning, 1, 1) == 1);
}


HRESULT CXThread::Execute(DWORD_PTR dwParam, HANDLE hObject)
{
	DWORD dwThreadID = ::GetCurrentThreadId();
	::CoInitialize(NULL);

	DWORD dwBegin = GetTickCount();
	InterlockedExchange(&m_bRunning, 1);
	ATLTRACE(_T("Thread Enter [%d]*********\n"), dwThreadID);

	if(NULL != m_pCallback)
	{
		m_pCallback->ThreadImpl((LPVOID)m_lParam);
	}

	ATLTRACE(_T("Thread Leave [%d][%d]*********\n"), dwThreadID, GetTickCount() - dwBegin);
	InterlockedExchange(&m_bRunning, 0);

	::CoUninitialize();
	return S_OK;
}

HRESULT CXThread::CloseHandle(HANDLE hHandle)
{
	return S_OK;
}
