#include "stdafx.h"
#include "DocExternalDispath.h"

#include "InvokeBase.h"

CDocExternalDispath::CDocExternalDispath()
{

}

CDocExternalDispath::~CDocExternalDispath()
{
	m_Invokes.clear();
}

ULONG STDMETHODCALLTYPE CDocExternalDispath::AddRef( void ) throw()
{
	return 1;
}

ULONG STDMETHODCALLTYPE CDocExternalDispath::Release( void ) throw()
{
	return 1;
}

HRESULT CDocExternalDispath::QueryInterface(REFIID iid, void** lpvObject)
{
	if (iid == IID_IDispatch)
	{
		AddRef();
		*lpvObject = static_cast<IDispatch*>(this);
		return S_OK;
	}

	return E_NOTIMPL;
}

HRESULT CDocExternalDispath::GetTypeInfoCount( UINT *pctinfo)
{
	*pctinfo = m_Invokes.size();
	return S_OK;
}

HRESULT CDocExternalDispath::GetTypeInfo( UINT iTInfo,LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

HRESULT CDocExternalDispath::GetIDsOfNames( REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId)
{
	UINT uCount = m_Invokes.size();
	for (int i = 0;i <static_cast<int>(uCount); i++)
	{
		InvokePtr &spInvoke = m_Invokes[i];
		if (spInvoke)
		{
			if(lstrcmp(rgszNames[0], spInvoke->GetName()) == 0)
			{
				*rgDispId = spInvoke->GetId();
				break;
			}
		}
	}

	return S_OK;
}

HRESULT CDocExternalDispath::Invoke( DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,
								 VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr)
{
	if (wFlags & DISPATCH_METHOD)
	{
		UINT uCount = m_Invokes.size();
		for (int i = 0;i < static_cast<int>(uCount); i++)
		{
			InvokePtr &spInvoke = m_Invokes[i];
			if (spInvoke)
			{
				if (dispIdMember == spInvoke->GetId())
				{
					spInvoke->Execute(pDispParams,pVarResult,pExcepInfo,puArgErr);
					break;
				}
			}
		}
	}
	return S_OK;
}

void CDocExternalDispath::AddInvokeCall(InvokePtr invokePtr)
{
	if (invokePtr)
	{
		m_Invokes.push_back(invokePtr);
	}
}
