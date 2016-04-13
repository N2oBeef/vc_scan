#pragma once

#include <vector>
#include "IInvoke.h"

#include "InvokeBoostFunction.h"

#include <boost/shared_ptr.hpp>
typedef boost::shared_ptr<IInvoke> InvokePtr;

class CDocExternalDispath 
	: public IDispatch
{
public:
	CDocExternalDispath();
	~CDocExternalDispath();

	virtual ULONG STDMETHODCALLTYPE AddRef( void) throw();
	virtual ULONG STDMETHODCALLTYPE Release( void) throw();
	STDMETHOD(QueryInterface)(REFIID, void**) throw() ;

	HRESULT STDMETHODCALLTYPE GetTypeInfoCount( UINT *pctinfo);
	HRESULT STDMETHODCALLTYPE GetTypeInfo( UINT iTInfo,LCID lcid, ITypeInfo **ppTInfo);
	HRESULT STDMETHODCALLTYPE GetIDsOfNames( REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);

	HRESULT STDMETHODCALLTYPE Invoke( DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,
							VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);

public:
	void AddInvokeCall(InvokePtr invokePtr);

private:
	std::vector<InvokePtr> m_Invokes;
	typedef std::vector<InvokePtr>::iterator InvokeItor;
};

