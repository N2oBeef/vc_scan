#pragma once

#include "ICppInterfaceBridge.h"
#include "ISoftwareMgrWrapper.h"

#include <string>

class CDocExternalDispath;
struct cJSON;

class CCppInterfaceBridge
	: public ICppInterfaceBridge
{
public:
	CCppInterfaceBridge();
	~CCppInterfaceBridge();

	void InitInvoke(CDocExternalDispath *pDispath);
	void SetSoftwareMgrWrapper(ISoftwareMgrWrapper * pSoftwareMgrWrapper);
	void SetWebReadyCallback(SimpleFunction func);

public:
	void Install(DISPPARAMS *pDispParams,VARIANT *pVarResult);
	void Download(DISPPARAMS *pDispParams,VARIANT *pVarResult);
	void GetStatus(DISPPARAMS *pDispParams,VARIANT *pVarResult);
	void FlyIcon(DISPPARAMS *pDispParams,VARIANT *pVarResult);
	void OnWebReady(DISPPARAMS *pDispParams,VARIANT *pVarResult);
	void GetSystemInfo(DISPPARAMS *pDispParams,VARIANT *pVarResult);
	void SetBenchRank(DISPPARAMS *pDispParams,VARIANT *pVarResult);
	void Examine(DISPPARAMS *pDispParams,VARIANT *pVarResult);
	void GetProblem(DISPPARAMS *pDispParams,VARIANT *pVarResult);

private:
	template<typename T>
	void SetRetValue( T tRet, VARIANT *pVarResult )
	{
		if (pVarResult == NULL) return;

		CComVariant var(tRet);
		var.Detach(pVarResult);
	}

	std::string GetJsonParam(DISPPARAMS *pDispParams);
	void GetRegInfo(cJSON *pItem, REG_INFO & regInfo);
	CString ConvertToWString(const std::string & szRaw);
	BOOL FlyIconImp(cJSON *pJson);
	ULONGLONG GetTotalSize(cJSON *pJson);
private:
	ISoftwareMgrWrapper *m_pSoftwareMgrWrapper;
	SimpleFunction m_webReadyCallback;
};
