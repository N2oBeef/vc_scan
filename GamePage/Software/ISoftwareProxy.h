#pragma once

#include <boost/shared_ptr.hpp>

class ISoftwareProxy
{
public:
	STDMETHOD_(BOOL, IsInstalled)(void) PURE;
	STDMETHOD_(int, Install)(void) PURE;
	STDMETHOD_(void, GetIDS)(BSTR* bstr) PURE;
	STDMETHOD(SetDownFileType)(long lType) PURE;
	STDMETHOD(SetName)(BSTR bstrName) PURE;
	STDMETHOD(SetDesc)(BSTR bstrDesc) PURE;
	STDMETHOD(SetIconUrl)(BSTR bstrIconUrl) PURE;
	STDMETHOD(Set360SoftMgrId)(BSTR bstr360Id) PURE;
	STDMETHOD(SetDownUrl)(BSTR bstrUrl, BSTR bstrMd5) PURE;
	STDMETHOD(SetRunParam)(BSTR bstrParam) PURE;
	STDMETHOD(SetSigner)(BSTR bstrSigner) PURE;
	STDMETHOD(AddReg)(OLE_HANDLE hKEY, BSTR bstrSubKey, BSTR bstrValue, BSTR bstrExe) PURE;
	STDMETHOD_(int, GetStatus)(void) PURE;
	STDMETHOD(IsGamePatch)(BOOL bisPatch) PURE;
};

typedef boost::shared_ptr<ISoftwareProxy> SoftwareProxyPtr;

