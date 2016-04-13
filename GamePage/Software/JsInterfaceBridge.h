#pragma once

#include "IJsInterfaceBridge.h"

class CJsInterfaceBidge
	: public IJsInterfaceBridge
{
public:
	CJsInterfaceBidge(CComPtr<IDispatch> spScript);
	~CJsInterfaceBidge();

public:
	void OnEvent(int nEventId, int nCode, WCHAR* szParam1, WCHAR *szParam2);
	void OnProgress(WCHAR *ids, LONGLONG lTotalSize, LONGLONG lDownloadSize, int nStatus);
	virtual void JsInterface(CComPtr<IDispatch> & spScript);

private:
	BOOL CallJs(const CString & szFunName, DISPPARAMS &dispparams);

private:
	CComPtr<IDispatch> m_spScript;
};