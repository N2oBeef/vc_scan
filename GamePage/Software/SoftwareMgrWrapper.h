#pragma once

#include "Base/SingletonT.h"

#include "ISoftwareMgrWrapper.h"
#include "IJsInterfaceBridge.h"
#include "CppInterfaceBridge.h"
#include "Browser/DocExternalDispath.h"

#include <vector>


class CSoftwareMgrWrapper
	: public IDispEventSimpleImpl</*nID =*/ 1, CSoftwareMgrWrapper, &__uuidof(_ISoftwareMgrEvents)>
	, public ISoftwareMgrWrapper
	, public CSingletonT<CSoftwareMgrWrapper>
{
	SINGLETON(CSoftwareMgrWrapper);
	~CSoftwareMgrWrapper();

public:
	CSoftwareMgrWrapper();
	
	BEGIN_SINK_MAP(CSoftwareMgrWrapper)
		SINK_ENTRY_INFO(1, __uuidof(_ISoftwareMgrEvents), 1, OnEvent, &case_event_4_param)
	END_SINK_MAP()

	void InitSoftwareMgr(HWND hMainWnd);
	void SetJsInterfaceBidge( CComPtr<IDispatch> & spScript );
	void SetDownMgrIconPos(long x, long y)
	{
		m_ptDownMgrIcon.x = x;
		m_ptDownMgrIcon.y = y;
	}
	virtual BOOL CheckDownDir(ULONGLONG llTotalSize);

public:
	IDispatch *GeDispatch(void)
	{
		return &m_externalDispath;
	}
	const CString & GetDownloadDir(void);

public:
	void SetWebReadyCallback(SimpleFunction func);

	SoftwareProxyPtr GetSofteware(const WCHAR * pszIDS);
	SoftwareProxyPtr CreateSofteware(const WCHAR * pszIDS);
	void Download(const WCHAR * szUrls);
	IAnimationPtr CreateAnimation(const WCHAR *  szUrl, long *pID);

	void GetDownMgrIconPos(LONG & x, long & y);
	HWND GetMainWnd(void);
	void RefreshBenchRank(void);

private:
	HRESULT __stdcall OnEvent(LONG lEventId, LONG lCode, BSTR lStatus, IUnknown *pUnknown);

	void InitDll(void);
	void AdviseDownloadingSofteEvent(void);
	void FireDownloadingReadyEvent(void);
	LRESULT SelectDownDir(void);

private:
	ISoftwareMgrExPtr m_spSoftMgr;
	JsInterfaceBridgePtr m_spJsInterfaceBridge;

	HMODULE m_hSoftMgrDll;
	BOOL    m_localReady;
	CPoint         m_ptDownMgrIcon;
	HWND    m_hMainWnd;

	typedef std::vector<SoftwareProxyPtr> SoftProxyContainer;
	typedef SoftProxyContainer::iterator  SoftProxyItor;
	SoftProxyContainer m_softContainer;

	CDocExternalDispath m_externalDispath;
	CCppInterfaceBridge  m_cppInterfaceBridge;

	CString              m_szDownLoadDir;
};