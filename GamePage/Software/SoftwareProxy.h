#pragma once

#include "ISoftwareProxy.h"

#include "IJsInterfaceBridge.h"

class CSoftwareProxy
	: public IDispEventSimpleImpl</*nID =*/ 1, CSoftwareProxy, &__uuidof(_ISoftwareEvents)>
	, public ISoftwareProxy
{
public:
	CSoftwareProxy(ISoftwareExPtr spSoftwareEx);
	~CSoftwareProxy();

	BEGIN_SINK_MAP(CSoftwareProxy)
		SINK_ENTRY_INFO(1, __uuidof(_ISoftwareEvents), 1, OnEvent, &case_event_4_param)
		SINK_ENTRY_INFO(1, __uuidof(_ISoftwareEvents), 2, OnProgress, &case_event_2_param)
	END_SINK_MAP()
	
public:
	void SetJsInterfaceBridge(JsInterfaceBridgePtr spBridge);
	void AdviseEvent(void);

public:
	STDMETHOD_(BOOL, IsInstalled)(void);
	STDMETHOD_(int, Install)(void);
	STDMETHOD_(void, GetIDS)(BSTR* bstr);
	STDMETHOD_(int, GetStatus)(void);

	STDMETHOD(SetDownFileType)(long lType);
	STDMETHOD(SetName)(BSTR bstrName);
	STDMETHOD(SetDesc)(BSTR bstrDesc);
	STDMETHOD(SetIconUrl)(BSTR bstrIconUrl);
	STDMETHOD(Set360SoftMgrId)(BSTR bstr360Id);
	STDMETHOD(SetDownUrl)(BSTR bstrUrl, BSTR bstrMd5);
	STDMETHOD(SetRunParam)(BSTR bstrParam);
	STDMETHOD(SetSigner)(BSTR bstrSigner);
	STDMETHOD(AddReg)(OLE_HANDLE hKEY, BSTR bstrSubKey, BSTR bstrValue, BSTR bstrExe);
	STDMETHOD(IsGamePatch)(BOOL bisPatch);

private:
	HRESULT __stdcall OnEvent( LONG lEventId,  LONG lCode,  BSTR lStatus,  IUnknown * pUnknown);
	HRESULT __stdcall OnProgress( LONGLONG llCurrentLen,  LONGLONG llTotalLen);

private:
	ISoftwareExPtr m_spSoftwareEx;
	JsInterfaceBridgePtr m_spJsBridge;
	BOOL m_bUnadvise;
	BOOL m_bIsGamePatch;
};