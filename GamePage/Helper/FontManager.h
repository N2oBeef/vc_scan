#if !defined(AFX_THEMEMANAGER_H__EDA64472_767E_43EF_8795_D30F9C9BE082__INCLUDED_)
#define AFX_THEMEMANAGER_H__EDA64472_767E_43EF_8795_D30F9C9BE082__INCLUDED_

#define FontManager_Instance() CFontManager::GetInstance()

class CFontManager
{
public:
	static CFontManager* GetInstance()
	{
		
		if (s_pThemeMgr == NULL)
		{
			s_pThemeMgr  = new CFontManager();
		}
		return s_pThemeMgr;
	}
	static void DestroyInstance(void)
	{
		if (s_pThemeMgr)
		{
			delete s_pThemeMgr;
			s_pThemeMgr = NULL;
		}
	}

	virtual ~CFontManager();

	

private:
	CFontManager();
	CFontManager( const CFontManager& );
	void operator=(const CFontManager& );

public:
	void Init();

	const CString& GetDefaultFontName(){ return m_strFontName; }
	HFONT GetDefaultFont(){ return m_sDefaltFont; }

	const void SetSystemFont(WORD nFont){ m_nSystemFonts = nFont; }
	const WORD GetSystemFont(){ return m_nSystemFonts; }

	BOOL IsWindowsVistaLater();

private:
	CFont   m_sDefaltFont;
	CString m_strFontName;
	WORD	m_nSystemFonts;

	static CFontManager *s_pThemeMgr;
};

#endif
