#pragma once

typedef enum _PanelID
{
	UNKNOWN_PANEL = -1,
	STARTUP_PANEL = 0,
	SCANNING_PANEL,
	GAMES_PANEL,
	PANEL_ALL
}PANEL_ID;

class IPanel;
class IPage
{
public:
	virtual PANEL_ID __stdcall GetCurrentPanelID( void ) = 0;
	virtual IPanel * __stdcall GetCurrentPanel( void ) = 0;
	virtual void __stdcall SetCurrentPanel( PANEL_ID panelID) = 0;
	virtual HWND __stdcall GetPageWnd(void) = 0;
};
