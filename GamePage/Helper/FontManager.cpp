#include "stdafx.h"
#include "FontManager.h"

#define FONT_MS_YA_HEI	0x0001
#define FONT_TAHOMA		0x0010
#define FONT_SONG		0x0100
#define FONT_MASK		0x0111

#define FONT_GEN		1
#define FONT_BLOD		2
#define FONT_UNDERLINE	3
#define FONT_BLOD_13	4
#define FONT_GEN_13		5
#define FONT_ITALIC		6

CFontManager * CFontManager::s_pThemeMgr = NULL;

int CALLBACK EnumFontFamProc_ThemeMgr(
							 ENUMLOGFONT *lpelf,    // logical-font data
							 NEWTEXTMETRIC *lpntm,  // physical-font data
							 DWORD FontType,        // type of font
							 LPARAM lParam          // application-defined data
							 )
{
	if( StrCmpW( L"微软雅黑" /* 微软雅黑 */, (wchar_t*)lpelf->elfFullName) == 0 )
	{
		FontManager_Instance()->SetSystemFont(
			FontManager_Instance()->GetSystemFont() | FONT_MS_YA_HEI
			);
	}
	if( StrCmpW(L"Tahoma", (wchar_t*)lpelf->elfFullName) == 0 )
	{
		FontManager_Instance()->SetSystemFont(
			FontManager_Instance()->GetSystemFont() | FONT_TAHOMA
			);
	}
	if( StrCmpW( L"宋体" /* 宋体 */, (wchar_t*)lpelf->elfFullName) == 0 ||
		StrCmpW(_T("新細明體"), (wchar_t*)lpelf->elfFullName) == 0 )
	{
		FontManager_Instance()->SetSystemFont(
			FontManager_Instance()->GetSystemFont() | FONT_SONG
			);
	}
	
	//我们需要的标识位都已经设置，可以中断这次枚举了
	if( FontManager_Instance()->GetSystemFont() == FONT_MASK )
		return 0;
	
	return 1;
}


CFontManager::CFontManager() : m_nSystemFonts(0)
{

}

CFontManager::~CFontManager()
{

}

void CFontManager::Init()
{
	HDC	 dc = ::GetDC(NULL);
	if(IsWindowsVistaLater())
	{
		EnumFontFamilies(dc, NULL, (FONTENUMPROC)EnumFontFamProc_ThemeMgr, (LPARAM)this);
	}
	else
	{
		m_nSystemFonts |= FONT_SONG;
	}

	if( (m_nSystemFonts & FONT_SONG) == FONT_SONG )
	{
		m_strFontName = _T("宋体");
	}

	if( (m_nSystemFonts & FONT_TAHOMA) == FONT_TAHOMA )
	{
		m_strFontName = _T("Tahoma");
	}
	if( (m_nSystemFonts & FONT_MS_YA_HEI) == FONT_MS_YA_HEI )
	{
		m_strFontName = _T("微软雅黑");
	}

	::ReleaseDC(NULL, dc);
	
	m_sDefaltFont.CreateFont( -12 ,  0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
        GB2312_CHARSET, OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
        DEFAULT_PITCH|FF_SWISS, m_strFontName);
}

BOOL CFontManager::IsWindowsVistaLater()
{
	OSVERSIONINFOEX osvi;  
    BOOL bOsVersionInfoEx;  
	
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));  
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);  

    bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi);  

    // win7的系统版本为NT6.1   
    if (  osvi.dwMajorVersion >= 6 )
    {  
        return true;
    }  
    else  
    {  
        return false;  
    }  	
}
