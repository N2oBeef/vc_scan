#pragma once

#include "InvokeBase.h"

#include <boost/function.hpp>
typedef boost::function<void (DISPPARAMS *pDispParams,VARIANT *pVarResult)> DispathBridgeFunction;

class CInvokeBoostFunction
	: public CInvokeBase
{
public:
	CInvokeBoostFunction(const CString & szFunName, DispathBridgeFunction funBridge)
		: CInvokeBase(szFunName)
		, m_funBridge(funBridge)
	{

	}
	virtual void Execute(DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo, UINT *puArgErr)
	{
		if (m_funBridge)
		{
			m_funBridge(pDispParams, pVarResult);
		}
	}

private:
	DispathBridgeFunction m_funBridge;
};
