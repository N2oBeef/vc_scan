#include "stdafx.h"
#include "JsInterfaceBridge.h"

CJsInterfaceBidge::CJsInterfaceBidge( CComPtr<IDispatch> spScript )
: m_spScript(spScript)
{
	
}

CJsInterfaceBidge::~CJsInterfaceBidge()
{

}

void CJsInterfaceBidge::OnEvent( int nEventId, int nCode, WCHAR* szParam1, WCHAR *szParam2 )
{
	if (!m_spScript) return;

	LOG(L"Call Js OnEvent: eventid = %d, code = %d, param1 = %s, param2 = %s",
		nEventId, nCode, szParam1 ? szParam1 : L"", szParam2 ? szParam2 : L"");
	
	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof dispparams);
	dispparams.cArgs = 4;
	dispparams.rgvarg = new VARIANT[dispparams.cArgs]; //delete on CallJs
	dispparams.cNamedArgs = 0;
	
	dispparams.rgvarg[0].vt = VT_BSTR;
	CComBSTR bstr = szParam2 ? szParam2 : L"";
	bstr.CopyTo(&dispparams.rgvarg[0].bstrVal);
	
	dispparams.rgvarg[1].vt = VT_BSTR;
	bstr = szParam1 ? szParam1 : L"";
	bstr.CopyTo(&dispparams.rgvarg[1].bstrVal);

	dispparams.rgvarg[2].vt = VT_I4;
	dispparams.rgvarg[2].intVal = nCode;

	dispparams.rgvarg[3].vt = VT_I4;
	dispparams.rgvarg[3].intVal = nEventId;
	
	BOOL bRet = CallJs(L"OnEvent", dispparams);
	if (!bRet)
	{
		LOG(L"Call Js OnEvent Failed");
	}
}

void CJsInterfaceBidge::OnProgress( WCHAR *ids, LONGLONG lTotalSize, LONGLONG lDownloadSize, int nStatus )
{
	if (!m_spScript) return;

	LOG(L"Call Js OnProgress: ids = %s, totalsize = %I64d, downsize = %I64d, status = %d",
		ids ? ids : L"", lTotalSize, lDownloadSize, nStatus);

	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof dispparams);
	dispparams.cArgs = 4;
	dispparams.rgvarg = new VARIANT[dispparams.cArgs]; //delete on CallJs
	dispparams.cNamedArgs = 0;
	
	dispparams.rgvarg[0].vt = VT_I4;
	dispparams.rgvarg[0].intVal = nStatus;

	dispparams.rgvarg[1].vt = VT_I8;
	dispparams.rgvarg[1].llVal = lDownloadSize;
	
	dispparams.rgvarg[2].vt = VT_I8;
	dispparams.rgvarg[2].llVal = lTotalSize;

	dispparams.rgvarg[3].vt = VT_BSTR;
	CComBSTR bstr = ids ? ids : L"";
	bstr.CopyTo(&dispparams.rgvarg[3].bstrVal);

	BOOL bRet = CallJs(L"OnProgress", dispparams);
	if (!bRet)
	{
		LOG(L"Call Js OnProgress Failed");
	}
}


BOOL CJsInterfaceBidge::CallJs(const CString & szFunName, DISPPARAMS &dispparams)
{
	if (!m_spScript) return FALSE;

	CComBSTR bstrMember(szFunName);
	DISPID dispid = NULL;
	HRESULT hr = m_spScript->GetIDsOfNames(IID_NULL,&bstrMember,1,
		LOCALE_SYSTEM_DEFAULT,&dispid);

	ATLASSERT(SUCCEEDED(hr));
	if(FAILED(hr)) return FALSE;

	EXCEPINFO excepInfo;
	memset(&excepInfo, 0, sizeof excepInfo);
	CComVariant vaResult;
	UINT nArgErr = (UINT)-1;  // initialize to invalid arg

	hr = m_spScript->Invoke(dispid,IID_NULL,0,
		DISPATCH_METHOD,&dispparams,&vaResult,&excepInfo,&nArgErr);

	delete [] dispparams.rgvarg;

	return SUCCEEDED(hr);
}

void CJsInterfaceBidge::JsInterface( CComPtr<IDispatch> & spScript )
{
	m_spScript = spScript;
}

