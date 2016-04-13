#include "StdAfx.h"
#include "UIThreadWindow.h"

CUIThreadWindow * CUIThreadWindow::m_pUIThreadWindow = NULL;

CUIThreadWindow::CUIThreadWindow(void)
{
}

CUIThreadWindow::~CUIThreadWindow(void)
{
}

CUIThreadWindow * CUIThreadWindow::GetUIThreadWindow( void )
{
	return m_pUIThreadWindow;
}

LRESULT CUIThreadWindow::OnSwitchToUIThreadMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
	ISimpleBind * pBind = reinterpret_cast<ISimpleBind *>(wParam);
	if(pBind == NULL) return FALSE;

	pBind->SimpleInvolk();

	return TRUE;
}


LRESULT CUIThreadWindow::OnAsyncToUIThreadMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
	ISimpleBind * pBind = reinterpret_cast<ISimpleBind *>(wParam);
	if(pBind == NULL) return FALSE;

	pBind->SimpleInvolk();
	
	delete pBind;
	pBind = NULL;

	return TRUE;
}


BOOL CUIThreadWindow::InitUIThreadWindow( void )
{
	m_pUIThreadWindow = new CUIThreadWindow;
	if(m_pUIThreadWindow == NULL) return NULL;

	HWND hWnd = m_pUIThreadWindow->Create(NULL, NULL, NULL, WS_POPUP);

	return hWnd != NULL;
}

void RunAsUIThread( ISimpleBind * pSimpleBind )
{
	CUIThreadWindow * pWnd = CUIThreadWindow::GetUIThreadWindow();
	if(pWnd == NULL) return;

	pWnd->SendMessage(WM_SWITCH_TO_UI_THREAD, (WPARAM)pSimpleBind);
}

void AsyncToUIThread( ISimpleBind * pSimpleBind )
{
	CUIThreadWindow * pWnd = CUIThreadWindow::GetUIThreadWindow();
	if(pWnd == NULL) return;
	
	pWnd->PostMessage(WM_ASYNC_TO_UI_THREAD, (WPARAM)pSimpleBind);
}
