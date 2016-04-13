#pragma once
#include <atlutil.h>
#include <atlsync.h>

interface IThreadCallback
{
	virtual DWORD __stdcall ThreadImpl(LPVOID) = 0;
};

class CXThread : public ATL::IWorkerThreadClient
{
public:
	CXThread():m_bInit(0),m_pCallback(NULL),m_bRunning(0),
	m_lParam(0){}

	BOOL	Start(LONG, IThreadCallback*);
	BOOL	IsBusy();
	BOOL	Exit(DWORD dwMill = 10000);

	HRESULT Execute(DWORD_PTR dwParam, HANDLE hObject);
	HRESULT CloseHandle(HANDLE hHandle);

private:
	BOOL	Init(IThreadCallback*);
	VOID	UnInit();

private:
	volatile LONG	m_bInit;
	volatile LONG	m_bRunning;
	volatile LONG	m_lParam;

	ATL::CEvent	m_Event;
	ATL::CWorkerThread<>	m_ThreadImpl;
	IThreadCallback* m_pCallback;

};