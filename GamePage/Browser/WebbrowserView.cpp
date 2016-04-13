#include "StdAfx.h"

#include <strsafe.h>
#include "WebbrowserView.h"

CWebbrowserView::CWebbrowserView(void)
: m_bError(FALSE)
{

}

CWebbrowserView::~CWebbrowserView(void)
{
}

BOOL CWebbrowserView::PreTranslateMessage( MSG* pMsg )
{
	// This was stolen from an SDI app using a form view.  
	//  
	// Pass keyboard messages along to the child window that has the focus.  
	// When the browser has the focus, the message is passed to its containing  
	// CAxWindow, which lets the control handle the message.  
	if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&  
		(pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))  
		return FALSE; 

	// give HTML page a chance to translate this message

	if (!::IsWindow(m_hWnd)) return FALSE;
	
	BOOL bRet = FALSE;
	if(pMsg->hwnd == m_hWnd || IsChild(pMsg->hwnd))
		bRet = (BOOL)SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);

	return bRet;
}

LRESULT CWebbrowserView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);

	CComPtr<IWebBrowser2> spWebBrowser2;
	HRESULT hRet = QueryControl(IID_IWebBrowser2, (void**)&spWebBrowser2);
	if (hRet != S_OK)
	{
		return -1;		
	}
	if(spWebBrowser2)
	{
		spWebBrowser2->put_Silent(VARIANT_TRUE);
		DispEventAdvise(spWebBrowser2,&DIID_DWebBrowserEvents2);
	}
	
	CComPtr<IAxWinAmbientDispatch> spHost;
	hRet = QueryHost(IID_IAxWinAmbientDispatch, (void**)&spHost);
	if (hRet != S_OK)
	{
		return -1;		
	}
	if(SUCCEEDED(hRet))
	{
		hRet = spHost->put_AllowContextMenu(VARIANT_FALSE);
		ATLASSERT(SUCCEEDED(hRet));
		
		const DWORD _DOCHOSTUIFLAG_THEME = 0x40000;
		hRet = spHost->put_DocHostFlags(DOCHOSTUIFLAG_NO3DBORDER | _DOCHOSTUIFLAG_THEME|DOCHOSTUIFLAG_SCROLL_NO);
		ATLASSERT(SUCCEEDED(hRet));
	}
	
	return 0;
}

void CWebbrowserView::SetExternalDispath( IDispatch *pDispath )
{
	if (pDispath == NULL) return;
	
	CComPtr<IAxWinHostWindow> spHostWnd;
	HRESULT hRet = QueryHost(__uuidof(IAxWinHostWindow), (void**)&spHostWnd);
	if (hRet != S_OK)
	{
		return ;		
	}

	if(SUCCEEDED(hRet))
	{
		hRet = spHostWnd->SetExternalDispatch(pDispath);
		ATLASSERT(SUCCEEDED(hRet));
	}
}

void CWebbrowserView::NavigateUrl(LPCWSTR lpUrl)
{
	CComPtr<IWebBrowser2> spWebBrowser2;	

	HRESULT hRet = QueryControl(IID_IWebBrowser2, (void**)&spWebBrowser2);

	if (hRet != S_OK)
	{
		return ;		
	}
	if(spWebBrowser2)
	{
		CComVariant vtURL((LPCWSTR)lpUrl);
		CComVariant vt;
		spWebBrowser2->Navigate2(&vtURL,&vt,&vt,&vt,&vt);

		m_bError = FALSE;
	}
}

void __stdcall CWebbrowserView::OnNavigateComplete2( IDispatch* pDisp, VARIANT* pvURL )
{
}

void __stdcall CWebbrowserView::OnDocumentComplete( IDispatch* pDisp, VARIANT* pvURL )
{
	LOG(L"OnDocumentComplete, success = %d", !m_bError);

	if (m_loadCompleteCallback)
	{
		m_loadCompleteCallback(!m_bError);
	}
}

CComPtr<IHTMLWindow2> CWebbrowserView::GetHtmlWindow()
{
	if(m_spHtmlWindow != NULL) return m_spHtmlWindow;

	CComPtr<IWebBrowser2> spWebBrowser2;
	CComPtr<IDispatch>			spDispDoc;
	
	HRESULT hRet = QueryControl(IID_IWebBrowser2, (void**)&spWebBrowser2);
	if(FAILED(hRet)) return NULL;

	hRet = spWebBrowser2->get_Document(&spDispDoc);
	if(FAILED(hRet)) return NULL;

	hRet = spDispDoc ->QueryInterface(IID_IHTMLDocument2,(void**)&m_spHtmlDoc);
	if(FAILED(hRet)) return NULL;

	hRet = m_spHtmlDoc->get_parentWindow(&m_spHtmlWindow);
	if(FAILED(hRet)) return NULL;

	return m_spHtmlWindow;
}

HRESULT CWebbrowserView::SetLocation( float fLongitude,float fLatitude )
{
	GetHtmlWindow();
	if(m_spHtmlWindow)
	{
		WCHAR szParam[MAX_PATH];
		ZeroMemory(szParam,MAX_PATH);
		//StringCchPrintf(szParam,MAX_PATH,L"JsGetParams();");
		//CString strJs(szParam);
		CComBSTR bstrJs = SysAllocString(L"setCurLocation(0.1, 0.5);");
		CComBSTR bstrlan = SysAllocString(L"javascript");
		VARIANT varRet;
		varRet.vt = VT_EMPTY;
		HRESULT hr = m_spHtmlWindow->execScript(bstrJs, bstrlan, &varRet);
		return hr;
	}
	return S_FALSE;
}

BOOL CWebbrowserView::GetJScript( CComPtr<IDispatch>& spDisp )
{
	if (!m_spHtmlDoc)
	{
		GetHtmlWindow();

		KillHtmlScrollBars(m_spHtmlDoc);
	}
	ATLASSERT(m_spHtmlDoc);
	HRESULT hr = m_spHtmlDoc->get_Script(&spDisp);
	ATLASSERT(SUCCEEDED(hr));
	return SUCCEEDED(hr);
}

BOOL CWebbrowserView::CallJScript(const CString &strFunc, const CStringVector& paramArray,CComVariant* pVarResult)
{
	CComPtr<IDispatch> spScript;
	if(!GetJScript(spScript))
	{
		return FALSE;
	}

	CComBSTR bstrMember(strFunc);
	DISPID dispid = NULL;
	HRESULT hr = spScript->GetIDsOfNames(IID_NULL,&bstrMember,1,
		LOCALE_SYSTEM_DEFAULT,&dispid);
	
	ATLASSERT(SUCCEEDED(hr));
	if(FAILED(hr)) return FALSE;

	const size_t arraySize = paramArray.size();

	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof dispparams);
	dispparams.cArgs = arraySize;
	dispparams.rgvarg = new VARIANT[dispparams.cArgs];

	for( size_t i = 0; i < arraySize; i++)
	{
		CComBSTR bstr = paramArray[arraySize - 1 - i]; // back reading
		bstr.CopyTo(&dispparams.rgvarg[i].bstrVal);
		dispparams.rgvarg[i].vt = VT_BSTR;
	}
	dispparams.cNamedArgs = 0;

	EXCEPINFO excepInfo;
	memset(&excepInfo, 0, sizeof excepInfo);
	CComVariant vaResult;
	UINT nArgErr = (UINT)-1;  // initialize to invalid arg

	hr = spScript->Invoke(dispid,IID_NULL,0,
		DISPATCH_METHOD,&dispparams,&vaResult,&excepInfo,&nArgErr);

	delete [] dispparams.rgvarg;

	if(FAILED(hr))
	{
		return FALSE;
	}

	if(pVarResult)
	{
		*pVarResult = vaResult;
	}
	
	return TRUE;
}

void CWebbrowserView::TestJsFunction()
{
	CComVariant retValue;
	CStringVector vect;

	CString sz;
	sz.Format(L"%f", 3.1415);

	vect.push_back(sz);
	vect.push_back(L"鲁大师");
	vect.push_back(L"ludashi");
	            
	CallJScript(L"JsGetParams", vect, &retValue);

}

void CWebbrowserView::SetLoadCompleteCallback( LoadCallback callback )
{
	m_loadCompleteCallback = callback;
}

void __stdcall CWebbrowserView::OnNavigateEror( IDispatch *pDisp, VARIANT* pvURL, VARIANT* pvFrame, VARIANT* pvStatusCode, VARIANT_BOOL* pvbCancel )
{
	LOG(L"OnNavigateEror", !m_bError);

	m_bError = TRUE;
	if (m_loadCompleteCallback)
	{
		m_loadCompleteCallback(FALSE);
	}
}


void CWebbrowserView::Refresh( void )
{
	CComPtr<IWebBrowser2> spWebBrowser2;	

	HRESULT hRet = QueryControl(IID_IWebBrowser2, (void**)&spWebBrowser2);
	if (spWebBrowser2)
	{
		spWebBrowser2->Refresh();
	}
}


BOOL CWebbrowserView::KillHtmlScrollBars(LPDISPATCH pdispDoc)
{
	CComQIPtr<IHTMLDocument2>piDoc(pdispDoc);
	CComPtr<IHTMLElement>piElem;
	CComPtr<IHTMLBodyElement>piBody;

	if (!piDoc) return FALSE;

	piDoc->get_body(&piElem);

	if (!piElem) return  FALSE;

	piElem->QueryInterface(&piBody);

	if (!piBody) return FALSE;

	return SUCCEEDED(piBody->put_scroll(CComBSTR(_T("no"))));
}
