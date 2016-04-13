#include "StdAfx.h"
#include "InvokeBase.h"

UINT g_uDispachSeed = 1;

//TODO:这个不是线程安全的
UINT AllocateDispachId()
{
	return g_uDispachSeed++;
}

CInvokeBase::CInvokeBase(const CString& strName)
:m_uId(AllocateDispachId())
,m_strName(strName)
{
}

CInvokeBase::~CInvokeBase(void)
{
}

UINT CInvokeBase::GetId()
{
	return m_uId;
}

void CInvokeBase::SetId(UINT uId)
{
	m_uId = uId;
}

const CString& CInvokeBase::GetName()
{
	return m_strName;
}

void CInvokeBase::SetName(const CString& strName)
{
	m_strName = strName;
}

void CInvokeBase::Execute(DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo, UINT *puArgErr)
{

}
