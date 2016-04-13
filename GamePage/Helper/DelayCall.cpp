#include "StdAfx.h"
#include "DelayCall.h"


UINT CDelayCall::Start( int mseconds , delayFunc_CB func, PVOID param /*= NULL*/, BOOL bLoop /*= TRUE*/)
{
	UINT idEvent = SetTimer( NULL, 2468 , mseconds ,DelayTimerFunc);
	DWORD err = GetLastError();

	DelayParam dp;
	dp.delayCb = func;
	dp.param = param;
	dp.bLoop = bLoop;

	DelayParamIterator iter = m_mapFunc.find( idEvent);
	if ( iter != m_mapFunc.end())
	{
		m_mapFunc[idEvent] = dp;
	}
	else
	{
		m_mapFunc.insert( std::make_pair(idEvent , dp));
	}
	
	return idEvent;
}

void CDelayCall::Stop(UINT &idEvent)
{
	BOOL bret = KillTimer( NULL, idEvent);
	m_mapFunc.erase(idEvent);
	idEvent = 0;
}

VOID _stdcall DelayTimerFunc( HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	CDelayCall::DelayParamMap & paramMap = CDelayCall::GetInstance()->m_mapFunc;
	if (paramMap.find(idEvent) == paramMap.end())
	{
		return;
	}
	
	DelayParam dp = CDelayCall::GetInstance()->m_mapFunc[idEvent];
	dp.delayCb( dp.param, idEvent);

	if( dp.bLoop == FALSE)
	{
		CDelayCall::GetInstance()->Stop(idEvent);
	}
}
