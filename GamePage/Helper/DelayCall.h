#pragma once

#include <map>
using namespace std;

typedef void (*delayFunc_CB)(PVOID param, UINT idEvent);

//!定时器回调函数
VOID _stdcall DelayTimerFunc( HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime );

struct DelayParam
{
	delayFunc_CB delayCb;
	PVOID param;
	BOOL bLoop;
};

class CDelayCall
{
protected: 
	virtual ~CDelayCall(){}
	
public:
	static CDelayCall* GetInstance()
	{
		static CDelayCall instance;
		return &instance;
	}

	typedef map<UINT, DelayParam>	DelayParamMap;
	typedef map<UINT, DelayParam>::iterator DelayParamIterator;

	//!启动定时功能，在 mseconds 毫秒后，执行函数fun
	virtual UINT Start( int mseconds , delayFunc_CB func, PVOID param = NULL, BOOL bLoop = TRUE);

	//!停止定时功能
	virtual void Stop(UINT &idEvent);


	DelayParamMap m_mapFunc;
	
};

