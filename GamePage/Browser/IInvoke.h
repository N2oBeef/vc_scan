#pragma once

class IInvoke
{
public:
	virtual UINT GetId() = 0;
	virtual void SetId(UINT nId) = 0;

	virtual const CString& GetName() = 0;
	virtual void SetName(const CString& strName ) = 0;

	virtual void Execute(DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo, UINT *puArgErr) = 0;
};