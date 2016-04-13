#pragma once

#include "DocExternalDispath.h"
#include <ExDispid.h>
#include <boost/function.hpp>
#include <windef.h>
typedef boost::function<void(BOOL)> LoadCallback;

#include <vector>
typedef std::vector<CString> CStringVector;

__declspec(selectany) _ATL_FUNC_INFO NavigateComplete2Struct = {CC_STDCALL, VT_EMPTY, 2, {VT_DISPATCH, VT_BYREF|VT_VARIANT}};
__declspec(selectany) _ATL_FUNC_INFO NavigateErrorStruct = {CC_STDCALL, VT_EMPTY, 5, {VT_DISPATCH, VT_BYREF|VT_VARIANT, VT_BYREF|VT_VARIANT,VT_BYREF|VT_VARIANT, VT_BYREF|VT_BOOL}};

class CWebbrowserView: 
	public IDispEventSimpleImpl<1,CWebbrowserView,&DIID_DWebBrowserEvents2>,
	public CWindowImpl<CWebbrowserView, CAxWindow>,
	public CMessageFilter
{
public:
	CWebbrowserView(void);
	~CWebbrowserView(void);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_WND_SUPERCLASS(_T("CustomBrowserView"), CAxWindow::GetWndClassName())
	BEGIN_SINK_MAP(CWebbrowserView)
		SINK_ENTRY_INFO(1, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2, OnNavigateComplete2, &NavigateComplete2Struct)
		SINK_ENTRY_INFO(1, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, OnDocumentComplete, &NavigateComplete2Struct)
		SINK_ENTRY_INFO(1, DIID_DWebBrowserEvents2, DISPID_NAVIGATEERROR, OnNavigateEror, &NavigateErrorStruct)
	END_SINK_MAP()
	//////////////////////////////////////////////////////////////////////////

	BEGIN_MSG_MAP(CWebbrowserView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
	END_MSG_MAP()

	//////////////////////////////////////////////////////////////////////////

public:
	void NavigateUrl(LPCWSTR lpUrl);
	void SetExternalDispath(IDispatch *pDispath);
	BOOL GetJScript(CComPtr<IDispatch>& spDisp);
	void SetLoadCompleteCallback(LoadCallback callback);
	void Refresh(void);

private:
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	void __stdcall OnNavigateComplete2(IDispatch* pDisp, VARIANT* pvURL);
	void __stdcall OnDocumentComplete(IDispatch* pDisp, VARIANT* pvURL);
	void __stdcall OnNavigateEror(IDispatch *pDisp, VARIANT* pvURL, VARIANT* pvFrame, VARIANT* pvStatusCode, VARIANT_BOOL* pvbCancel);

private:
	//test
	void TestJsFunction();	
	HRESULT SetLocation(float fLongitude, float fLatitude);
	
	CComPtr<IHTMLWindow2> GetHtmlWindow();
	BOOL CallJScript(const CString& strFunc, const CStringVector& paramArray, CComVariant* pVarResult);
	BOOL KillHtmlScrollBars(LPDISPATCH pdispDoc);
private:
	CComPtr<IHTMLWindow2>   m_spHtmlWindow;
	CComPtr<IHTMLDocument2>	m_spHtmlDoc;
	LoadCallback m_loadCompleteCallback;
	BOOL         m_bError;
};
