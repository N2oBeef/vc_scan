#pragma once

#include "simple_bind.h"

void RunAsUIThread( ISimpleBind * pSimpleBind );
void AsyncToUIThread( ISimpleBind * pSimpleBind );

#define WM_SWITCH_TO_UI_THREAD (WM_USER + 1978)
#define WM_ASYNC_TO_UI_THREAD (WM_USER + 1979)

class CUIThreadWindow : public CWindowImpl<CUIThreadWindow>
{
public:
	CUIThreadWindow(void);
	virtual ~CUIThreadWindow(void);
	static BOOL InitUIThreadWindow(void);
	static CUIThreadWindow * GetUIThreadWindow(void);

	BEGIN_MSG_MAP_EX(CUIThreadWindow)
		MESSAGE_HANDLER( WM_SWITCH_TO_UI_THREAD, OnSwitchToUIThreadMessage )
		MESSAGE_HANDLER( WM_ASYNC_TO_UI_THREAD, OnAsyncToUIThreadMessage )
		END_MSG_MAP();

private:
	LRESULT OnSwitchToUIThreadMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );
	LRESULT OnAsyncToUIThreadMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );

private:
	static CUIThreadWindow * m_pUIThreadWindow;
};


