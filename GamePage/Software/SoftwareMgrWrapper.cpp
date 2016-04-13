#include "stdafx.h"
#include "SoftwareMgrWrapper.h"
#include "JsInterfaceBridge.h"
#include "SoftwareProxy.h"
#include "Utils.h"

#include <BaseModule/Common/ComObjectCreator.h>
#include <boost/make_shared.hpp>
#include <ShlObj.h>

CSoftwareMgrWrapper::CSoftwareMgrWrapper()
: m_hSoftMgrDll(NULL)
, m_localReady(FALSE)
, m_hMainWnd(NULL)
{

}

CSoftwareMgrWrapper::~CSoftwareMgrWrapper()
{
	if (m_spSoftMgr)
	{
		DispEventUnadvise(m_spSoftMgr);
	}
}

void CSoftwareMgrWrapper::SetJsInterfaceBidge( CComPtr<IDispatch> & spScript )
{
	if (!m_spJsInterfaceBridge)
	{
		m_spJsInterfaceBridge = boost::make_shared<CJsInterfaceBidge>(spScript);
	}
	else
	{
		m_spJsInterfaceBridge->JsInterface(spScript);
	}

	if (m_localReady)
	{
		FireDownloadingReadyEvent();
	}
}

void CSoftwareMgrWrapper::InitSoftwareMgr(HWND hMainWnd)
{
	if (m_spSoftMgr) return;

	m_hMainWnd = hMainWnd;
	if (!m_hSoftMgrDll)
	{
		InitDll();
	}

	SmartObjCreator(m_spSoftMgr, m_hSoftMgrDll).CreateInstance(__uuidof(SoftwareMgr));	
	ATLASSERT(m_spSoftMgr);

	DispEventAdvise(m_spSoftMgr);

	if (m_spSoftMgr)
	{
		m_spSoftMgr->Init(SOFTWARE_TYPE_DOWNING);
	}

	m_cppInterfaceBridge.InitInvoke(&m_externalDispath);
	m_cppInterfaceBridge.SetSoftwareMgrWrapper(this);
}

void CSoftwareMgrWrapper::InitDll( void )
{
	if (m_hSoftMgrDll) return;

	WCHAR szDll[MAX_PATH] = {};
	GetModuleFileName(g_hModule, szDll, MAX_PATH);
	PathRemoveFileSpec(szDll);
	PathRemoveFileSpec(szDll);
	PathAppend(szDll, L"SoftMgr/SoftMgr.dll");

#ifndef _DEBUG
#ifndef DISABLE_RELEASE_CHECK_SIGN
	if (!VerifySignHelper::Has360Sign_Prompt(szDll))
	{
		return;
	}
#endif
#endif

	m_hSoftMgrDll = ::LoadLibrary(szDll);
	ATLASSERT(m_hSoftMgrDll);
}

IAnimationPtr CSoftwareMgrWrapper::CreateAnimation( const WCHAR *  szUrl , long *pID)
{
	if (szUrl == NULL || pID == NULL) return NULL;
	
	if (!m_hSoftMgrDll)
	{
		InitDll();
	}
	
	IAnimationPtr spAnimation;
	SmartObjCreator(spAnimation, m_hSoftMgrDll).CreateInstance(__uuidof(Animation));	
	ATLASSERT(spAnimation);
	
	CString szFile = CUtils::GetIconPath(szUrl);
	*pID = spAnimation->Create(_bstr_t(szFile));

	return spAnimation;
}


HRESULT __stdcall CSoftwareMgrWrapper::OnEvent( LONG lEventId, LONG lCode, BSTR lStatus, IUnknown *pUnknown )
{
	if (lEventId == SOFTWAREMGR_EVENT_INIT_DOWNLOADING_READY && !m_localReady)
	{
		m_localReady = TRUE;
		
		LOG(L"EVENT_ID_LOCAL_CONFIG_READY");
		FireDownloadingReadyEvent();
	}

	if (!m_spJsInterfaceBridge) return S_OK;
	
	if (lEventId == SOFTWAREMGR_EVENT_ADD_TO_DOWNLOADING_SOFTWARE)
	{
		m_spJsInterfaceBridge->OnEvent(EVENT_ID_DOWNLOAD_START, 0, lStatus, NULL);
	}

	if (lEventId == SOFTWAREMGR_EVENT_DELETE_DOWNLOADING_SOFTWARE)
	{
		m_spJsInterfaceBridge->OnEvent(EVENT_ID_DELETE, 0, lStatus, NULL);
	}

	return S_OK;
}

SoftwareProxyPtr CSoftwareMgrWrapper::GetSofteware( const WCHAR * pszIDS )
{
	if (pszIDS == NULL || !m_spSoftMgr) return SoftwareProxyPtr();
	
	CComBSTR bstrIDSGet(pszIDS);
	SoftProxyItor it = m_softContainer.begin();
	SoftProxyItor itEnt = m_softContainer.end();
	for (; it != itEnt; ++it)
	{
		SoftwareProxyPtr spSoftProxy = *it;
		if (spSoftProxy)
		{
			CComBSTR bstrIDS;
			spSoftProxy->GetIDS(&bstrIDS);
			if (bstrIDS == bstrIDSGet)
			{
				return spSoftProxy;
			}
		}
	}
	
	//本地没有创建的，先查询下现在的列表有了没
	ISoftwareExPtr spSoftwareEx = m_spSoftMgr->GetSoftwareByIDS(pszIDS);
	if (!spSoftwareEx) return SoftwareProxyPtr();

	boost::shared_ptr<CSoftwareProxy> spSoft =  boost::make_shared<CSoftwareProxy>(spSoftwareEx);
	spSoft->SetJsInterfaceBridge(m_spJsInterfaceBridge);
	spSoft->AdviseEvent();
	m_softContainer.push_back(spSoft);

	return spSoft;
}

void CSoftwareMgrWrapper::Download(const WCHAR * szUrls )
{
	if (szUrls == NULL || !m_spSoftMgr) return;
	
	m_spSoftMgr->Download(_bstr_t(szUrls));
}

SoftwareProxyPtr CSoftwareMgrWrapper::CreateSofteware( const WCHAR * pszIDS )
{
	if (pszIDS == NULL) return SoftwareProxyPtr();
	
	ISoftwareExPtr spSoftwareEx = m_spSoftMgr->CreateSoftware(pszIDS);
	if (!spSoftwareEx) return SoftwareProxyPtr();

	boost::shared_ptr<CSoftwareProxy> spSoft =  boost::make_shared<CSoftwareProxy>(spSoftwareEx);
	spSoft->SetJsInterfaceBridge(m_spJsInterfaceBridge);
	spSoft->AdviseEvent();
	m_softContainer.push_back(spSoft);

	return spSoft;
}

void CSoftwareMgrWrapper::GetDownMgrIconPos( LONG & x, long & y )
{
	x = m_ptDownMgrIcon.x;
	y = m_ptDownMgrIcon.y;
}

HWND CSoftwareMgrWrapper::GetMainWnd( void )
{
	return m_hMainWnd;	
}

void CSoftwareMgrWrapper::AdviseDownloadingSofteEvent(void)
{
	ATLASSERT(m_spSoftMgr);

	ISoftwareIteratorPtr spIterator = m_spSoftMgr->GetSoftwareIterator(SOFTWARE_TYPE_DOWNING);
	if (!spIterator) return;

	for (spIterator->First(); spIterator->HasNext(); spIterator->Next())
	{
		ISoftwareExPtr spSoftEx = spIterator->GetCurrentSoftware();
		if (!spSoftEx) continue;
		
		_bstr_t bstrIDS = spSoftEx->GetIDS();
		LOG(L"Init CSoftwareProxy of ids: %s", CA2W(_com_util::ConvertBSTRToString(bstrIDS)));
		boost::shared_ptr<CSoftwareProxy> spSoftProxy =  boost::make_shared<CSoftwareProxy>(spSoftEx);
		spSoftProxy->SetJsInterfaceBridge(m_spJsInterfaceBridge);
		spSoftProxy->AdviseEvent();
		m_softContainer.push_back(spSoftProxy);
	}
}

void CSoftwareMgrWrapper::FireDownloadingReadyEvent( void )
{
	if (m_spJsInterfaceBridge)
	{
		LOG(L"Fire downloading software items ready event");

		m_spJsInterfaceBridge->OnEvent(EVENT_ID_LOCAL_CONFIG_READY, 0, NULL, NULL);	

		AdviseDownloadingSofteEvent();
	}
}

void CSoftwareMgrWrapper::SetWebReadyCallback( SimpleFunction func )
{
	m_cppInterfaceBridge.SetWebReadyCallback(func);
}

void CSoftwareMgrWrapper::RefreshBenchRank(void)
{
	if (m_spJsInterfaceBridge)
	{
		m_spJsInterfaceBridge->OnEvent(EVENT_ID_REFRESH_RANK, 0, NULL, NULL);
	}
}

static ULONGLONG s_llCheckTotalSize = 0;
static BOOL      s_llSelectValid = FALSE;
BOOL CSoftwareMgrWrapper::CheckDownDir( ULONGLONG llTotalSize )
{
	s_llCheckTotalSize = llTotalSize;
	s_llSelectValid = TRUE;
	
	CString szPath = CUtils::GetCurrentDownloadPath();
	ULARGE_INTEGER lgFree = {0};
	GetDiskFreeSpaceEx(szPath, NULL, NULL, &lgFree);
	if (lgFree.QuadPart <= s_llCheckTotalSize)
	{
		s_llSelectValid = FALSE;
		SelectDownDir();
	}

	return s_llSelectValid;
}

void SelectedFileChange(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	TCHAR szCurr[MAX_PATH] = { 0 };
	SHGetPathFromIDList((LPCITEMIDLIST)lParam,szCurr);
	if(wcslen(szCurr) == 0)
	{
		::SendMessage(hwnd, BFFM_ENABLEOK, NULL,FALSE);
	}

	if(wcslen(szCurr) >= 2 && szCurr[1] == ':'){
		TCHAR szDiskRoot[4] = { 0 };
		szDiskRoot[0] = szCurr[0];
		szDiskRoot[1] = (TCHAR)':';
		szDiskRoot[2] = (TCHAR)'\\';
		int nType = GetDriveType( szDiskRoot );
		if(nType != DRIVE_FIXED)
		{
			::SendMessage(hwnd, BFFM_ENABLEOK, NULL,FALSE);
		}

		ULARGE_INTEGER lgFree = {0};
		GetDiskFreeSpaceEx(szCurr, NULL, NULL, &lgFree);
		if (lgFree.QuadPart <= s_llCheckTotalSize)
		{
			::SendMessage(hwnd, BFFM_ENABLEOK, NULL,FALSE);
		}
	}
}
int CALLBACK BrowseCallbackProcImpl(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch(uMsg)
	{
	case BFFM_INITIALIZED:
		break;
	case BFFM_SELCHANGED:    //选择路径变化，
		{
			SelectedFileChange(hwnd, uMsg, lParam, lpData);
		}
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CSoftwareMgrWrapper::SelectDownDir(void)
{
	BROWSEINFO bi;
	WCHAR szPath[4096] = {};

	bi.hwndOwner = m_hMainWnd;
	bi.lpszTitle = L"请选择游戏下载目录";
	bi.ulFlags = BIF_USENEWUI | BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
	bi.iImage = 0;
	bi.lParam = 0;
	bi.lpfn = BrowseCallbackProcImpl;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath;
	bi.lParam = reinterpret_cast<LPARAM>(szPath);

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

	if(pidl == NULL) return 0;

	TCHAR szNewPath[4096] = {};

	if(SHGetPathFromIDList(pidl, szNewPath))
	{
		m_szDownLoadDir = szNewPath;
		s_llSelectValid = !m_szDownLoadDir.IsEmpty();

		CUtils::SetCurrentDownloadPath(m_szDownLoadDir);
	}

	return 0;
}

const CString & CSoftwareMgrWrapper::GetDownloadDir( void )
{
	if (m_szDownLoadDir.IsEmpty())
	{
		m_szDownLoadDir = CUtils::GetCurrentDownloadPath();
	}
	return m_szDownLoadDir;
}
