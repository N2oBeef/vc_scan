#pragma once

#include "IInvoke.h"

UINT AllocateDispachId();

class CInvokeBase 
	: public IInvoke
{
public:
	CInvokeBase(const CString& strName);
	~CInvokeBase(void);

	virtual UINT GetId();
	virtual void SetId(UINT uId);

	virtual const CString& GetName();
	virtual void SetName(const CString& strName);

	virtual void Execute(DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo, UINT *puArgErr);

private:
	UINT m_uId;
	CString m_strName;
};
