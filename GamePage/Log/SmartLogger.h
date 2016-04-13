#if !defined(AFX_SMARTLOGGER_H__C5B9B214_D577_46AD_B212_BB038B21314D__INCLUDED_)
#define AFX_SMARTLOGGER_H__C5B9B214_D577_46AD_B212_BB038B21314D__INCLUDED_

#include <BaseModule\Common\AutoLocker.h>

#include <vector>

class CSmartLogger  
{
public:
	CSmartLogger();
	virtual ~CSmartLogger();
	
	typedef std::vector < CString > _Container;

public:
	void SetLogFileName(const CString & strLogFile);

	int AddLog( const CString & szLine );
	int AddLog( LPCTSTR lpszFormat, va_list argList );

	void Clear( void );
	BOOL SaveToFile( void);
	void InitLog(void);

private:
	LARGE_INTEGER GetFileSize( CString & strFile);
private:
	_BaseModule::CCriticalSection m_cs;
	_Container m_container;
	CString m_strFilePath;

	BOOL   m_bFirstWrite;
	
};

#endif
