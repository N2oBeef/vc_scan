#pragma once

//////////////////////////////////////////////////////////////////////////
// CriticalSection ¼òµ¥·â×°
//////////////////////////////////////////////////////////////////////////
class CCriticalSection
{
public:
	CCriticalSection()
	{
		::InitializeCriticalSection(&m_critic);
	}
	~CCriticalSection()
	{
		::DeleteCriticalSection(&m_critic);
	}
	operator CRITICAL_SECTION&()
	{
		return m_critic;
	}
	operator CRITICAL_SECTION* ()
	{
		return &m_critic;
	}
private:
	CRITICAL_SECTION m_critic;
};

class CAutoLock
{
public:
	CAutoLock(CRITICAL_SECTION& critic):m_critic(critic)
	{
		::EnterCriticalSection(&m_critic);
	}
	~CAutoLock()
	{
		::LeaveCriticalSection(&m_critic);
	}
private:
	CRITICAL_SECTION& m_critic;
};


//////////////////////////////////////////////////////////////////////////
// Event ·â×°
//////////////////////////////////////////////////////////////////////////

class CSimpleEvent
{
public:
    CSimpleEvent()
        :m_hEvent(NULL)
    {
    }

    CSimpleEvent(const WCHAR * pszName, BOOL bManualReset, BOOL bInitState)
        :m_hEvent(NULL)
    {
        Init(pszName, bManualReset, bInitState);
    }

    void Wait(DWORD dwTimeout)
    {
        if (m_hEvent == NULL) return;
        
        ::WaitForSingleObject(m_hEvent, dwTimeout);
    }
    
    void Signal(void)
    {
        if (m_hEvent)
        {
            ::SetEvent(m_hEvent);
        }
    }

    void Reset(void)
    {
        if (m_hEvent)
        {
            ::ResetEvent(m_hEvent);
        }
    }

private:
    void Init(const WCHAR * pszName, BOOL bManualReset, BOOL bInitState)
    {
        m_hEvent = ::CreateEvent(NULL, bManualReset, bInitState, pszName);
    }

    void Release(void)
    {
        Signal();
        CloseHandle(m_hEvent);
    }

private:
    HANDLE m_hEvent;
};