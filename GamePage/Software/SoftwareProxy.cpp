#include "stdafx.h"
#include "SoftwareProxy.h"
#include "IJsInterfaceBridge.h"
#include "ICppInterfaceBridge.h"
#include "SoftwareMgrWrapper.h"

#include <SoftMgr/SoftMgrDefine.h>

CSoftwareProxy::CSoftwareProxy(ISoftwareExPtr spSoftwareEx)
:m_spSoftwareEx(spSoftwareEx)
, m_bUnadvise(FALSE)
, m_bIsGamePatch(FALSE)
{

}

CSoftwareProxy::~CSoftwareProxy()
{
	if (m_spSoftwareEx && m_bUnadvise)
	{
		DispEventUnadvise(m_spSoftwareEx);
	}
}

STDMETHODIMP_(BOOL) CSoftwareProxy::IsInstalled( void )
{
	if (!m_spSoftwareEx) return 0;

	long lRet = m_spSoftwareEx->IsInstalled();
	return lRet > 0;
}

STDMETHODIMP_(int) CSoftwareProxy::Install( void )
{
	if (!m_spSoftwareEx) return 0;
	
	m_spSoftwareEx->SetIntegerValue(
		SOFTWARE_PROPERTY_CATEGORY, 
		m_bIsGamePatch ? SOFTWARE_CATEGORY_SOFT : SOFTWARE_CATEGORY_GAME);

	m_spSoftwareEx->SetIntegerValue(
		SOFTWARE_PROPERTY_AFTER_DOWN_ACTION, 
		m_bIsGamePatch ? AFTER_DOWNLOAD_ACTION_INSTALL : AFTER_DOWNLOAD_ACTION_OPEN_FOLDER);
	
	CString szDownDir = CSoftwareMgrWrapper::GetInstance()->GetDownloadDir();
	m_spSoftwareEx->SetStringValue(SOFTWARE_PROPERTY_DOWNDIR, _bstr_t(szDownDir));

	return m_spSoftwareEx->Install();
}

STDMETHODIMP CSoftwareProxy::SetDownFileType( long lType )
{
	if (!m_spSoftwareEx) return 0;

	return m_spSoftwareEx->SetDownFileType(lType);
}

STDMETHODIMP CSoftwareProxy::SetName( BSTR bstrName )
{
	if (!m_spSoftwareEx) return 0;

	return m_spSoftwareEx->SetName(bstrName);
}

STDMETHODIMP CSoftwareProxy::SetDesc( BSTR bstrDesc )
{
	if (!m_spSoftwareEx) return 0;

	return m_spSoftwareEx->SetDesc(bstrDesc);
}

STDMETHODIMP CSoftwareProxy::SetIconUrl( BSTR bstrIconUrl )
{
	if (!m_spSoftwareEx) return 0;

	return m_spSoftwareEx->SetIconUrl(bstrIconUrl);
}

STDMETHODIMP CSoftwareProxy::Set360SoftMgrId( BSTR bstr360Id )
{
	if (!m_spSoftwareEx) return 0;

	return m_spSoftwareEx->Set360SoftMgrId(bstr360Id);
}

STDMETHODIMP CSoftwareProxy::SetDownUrl( BSTR bstrUrl, BSTR bstrMd5 )
{
	if (!m_spSoftwareEx) return 0;

	return m_spSoftwareEx->SetDownUrl(bstrUrl, bstrMd5);
}

STDMETHODIMP CSoftwareProxy::SetRunParam( BSTR bstrParam )
{
	if (!m_spSoftwareEx) return 0;

	return m_spSoftwareEx->SetRunParam(bstrParam);
}

STDMETHODIMP CSoftwareProxy::AddReg( OLE_HANDLE hKEY, BSTR bstrSubKey, BSTR bstrValue, BSTR bstrExe )
{
	if (!m_spSoftwareEx) return 0;

	return m_spSoftwareEx->AddReg(hKEY, bstrSubKey,bstrValue, bstrExe);
}

HRESULT __stdcall CSoftwareProxy::OnEvent( LONG lEventId, LONG lCode, BSTR lStatus, IUnknown * pUnknown )
{
	if (!m_spJsBridge) return S_OK;

	long lFireEventId = -1;
	long lFireCode = 1;

	//start install
	if (lEventId == SOFTMGRINST_NOTIFY_ACTION_TASK_BEGIN_DOWNLOAD)
	{
		lFireEventId = EVENT_ID_DOWNLOAD_START;
	}

	//download end
	if (lEventId == SOFTMGRINST_NOTIFY_ACTION_TASK_END_DOWNLOAD)
	{
		lFireEventId = EVENT_ID_DOWNLOAD_END;
		if (lCode == DOWNLOAD_STATUS_SUCCESS ||
			lCode == INSTALL_STATUS_SUCCESS)
		{
			lFireCode = 1;
		}
		else
		{
			lFireCode = 0;
		}
	}
	
	//取消下载
	_bstr_t bstr;
	if (lEventId == SOFTMGRINST_NOTIFY_ACTION_TASK_CANCEL_DOWNLOAD)
	{
		lFireEventId = EVENT_ID_DELETE;
		bstr  = lStatus;

		//删除了软件项目，
		m_spSoftwareEx = NULL;
	}
	else
	{
		if (!m_spSoftwareEx) return 0;

		bstr = m_spSoftwareEx->GetIDS();
	}
	
	//等待安装到安装完成
	if (lEventId >= SOFTMGRINST_NOTIFY_ACTION_TASK_WAIT_FOR_INSTALL &&
		lEventId <= SOFTMGRINST_NOTIFY_ACTION_TASK_END_INSTALL)
	{
		lFireEventId = EVENT_ID_DOWNLOAD_END;
	}

	if (lFireEventId > 0)
	{
		m_spJsBridge->OnEvent(lFireEventId, lFireCode, bstr, L"");
	}

	return S_OK;
}

HRESULT __stdcall CSoftwareProxy::OnProgress( LONGLONG llCurrentLen, LONGLONG llTotalLen )
{
	if (!m_spSoftwareEx) return 0;

	_bstr_t bstr = m_spSoftwareEx->GetIDS();

	m_spJsBridge->OnProgress(bstr, llTotalLen, llCurrentLen, 1);

	return S_OK;
}

void CSoftwareProxy::SetJsInterfaceBridge( JsInterfaceBridgePtr spBridge)
{
	ATLASSERT(spBridge);
	if (spBridge)
	{
		m_spJsBridge = spBridge;
	}
}

STDMETHODIMP CSoftwareProxy::SetSigner( BSTR bstrSigner )
{
	if (!m_spSoftwareEx) return 0;

	return m_spSoftwareEx->SetSigner(bstrSigner);
}

void CSoftwareProxy::AdviseEvent( void )
{
	if (!m_spSoftwareEx) return;
	
	DispEventAdvise(m_spSoftwareEx);
	m_bUnadvise = TRUE;	
}

STDMETHODIMP_(void) CSoftwareProxy::GetIDS( BSTR* bstr )
{
	if (m_spSoftwareEx && bstr)
	{
		*bstr = m_spSoftwareEx->GetIDS();
	}
}

STDMETHODIMP_(int) CSoftwareProxy::GetStatus( void )
{
	if (!m_spSoftwareEx) return SOFTWARE_STATUS_NOT_INSTALL;
	
	LONG lStatus = 0;
	LONG lStatusCode = 0;
	m_spSoftwareEx->GetStatus(&lStatus, &lStatusCode);
	
	//download end
	if (lStatus == SOFTMGRINST_NOTIFY_ACTION_TASK_END_DOWNLOAD)
	{
		//下载失败
		if (lStatusCode != DOWNLOAD_STATUS_SUCCESS && lStatusCode != INSTALL_STATUS_SUCCESS)
		{
			return SOFTWARE_STATUS_DOWNLOAD_FAILED;
		}
	}

	if (lStatus >= SOFTMGRINST_NOTIFY_ACTION_TASK_END_DOWNLOAD &&
		lStatus <= SOFTMGRINST_NOTIFY_ACTION_TASK_END_INSTALL)
	{
		return SOFTWARE_STATUS_INSTALLED;
	}

	//下载中
	if (lStatus >= SOFTMGRINST_NOTIFY_ACTION_TASK_PREPARE_DOWNLOAD &&
		lStatus <= SOFTMGRINST_NOTIFY_ACTION_TASK_CANCEL_DOWNLOAD)
	{
		//取消了，
		if (lStatus == SOFTMGRINST_NOTIFY_ACTION_TASK_CANCEL_DOWNLOAD)
		{
			return SOFTWARE_STATUS_NOT_INSTALL;
		}
		return SOFTWARE_STATUS_DOWNLOADING;
	}

	return SOFTWARE_STATUS_NOT_INSTALL;
}

STDMETHODIMP CSoftwareProxy::IsGamePatch( BOOL bisPatch )
{
	m_bIsGamePatch = bisPatch;
	return S_OK;
}
