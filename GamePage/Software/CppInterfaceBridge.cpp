#include "stdafx.h"
#include "CppInterfaceBridge.h"

#include "Browser/DocExternalDispath.h"
#include "Browser/InvokeBoostFunction.h"
#include "Helper/CharacterCodeHelper.h"
#include "json/JsonHelper.h"
#include "Utils.h"
#include "Config/Configure.h"

#include <SoftMgr\SoftMgrDefine.h>
#include "Software\SoftwareMgrWrapper.h"

#include <comutil.h>

#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
using namespace boost;
using namespace std;

#include "json/cJSON.h"
#include "json/JsonHelper.h"
using namespace JsonHelper;

struct CAutoDeleteJson
{
	CAutoDeleteJson(cJSON *pJsonIn)
		: pJson(pJsonIn){}
	~CAutoDeleteJson(){
		if (pJson)
		{
			cJSON_Delete(pJson);
			pJson = NULL;
		}
	}

	cJSON *pJson;
};

CCppInterfaceBridge::CCppInterfaceBridge()
: m_pSoftwareMgrWrapper(NULL)
{
}

CCppInterfaceBridge::~CCppInterfaceBridge()
{
	
}

void CCppInterfaceBridge::InitInvoke( CDocExternalDispath *pDispath )
{
	if (pDispath == NULL) return;

	pDispath->AddInvokeCall(boost::make_shared<CInvokeBoostFunction>(L"Install", bind(&CCppInterfaceBridge::Install, this, _1,_2)));
	pDispath->AddInvokeCall(boost::make_shared<CInvokeBoostFunction>(L"Download", bind(&CCppInterfaceBridge::Download, this, _1,_2)));
	pDispath->AddInvokeCall(boost::make_shared<CInvokeBoostFunction>(L"FlyIcon", bind(&CCppInterfaceBridge::FlyIcon, this, _1,_2)));
	pDispath->AddInvokeCall(boost::make_shared<CInvokeBoostFunction>(L"GetStatus", bind(&CCppInterfaceBridge::GetStatus, this, _1,_2)));
	pDispath->AddInvokeCall(boost::make_shared<CInvokeBoostFunction>(L"OnWebReady", bind(&CCppInterfaceBridge::OnWebReady, this, _1,_2)));
	pDispath->AddInvokeCall(boost::make_shared<CInvokeBoostFunction>(L"GetSystemInfo", bind(&CCppInterfaceBridge::GetSystemInfo, this, _1,_2)));
	pDispath->AddInvokeCall(boost::make_shared<CInvokeBoostFunction>(L"SetBenchRank", bind(&CCppInterfaceBridge::SetBenchRank, this, _1,_2)));
	pDispath->AddInvokeCall(boost::make_shared<CInvokeBoostFunction>(L"Examine", bind(&CCppInterfaceBridge::Examine, this, _1,_2)));
	pDispath->AddInvokeCall(boost::make_shared<CInvokeBoostFunction>(L"GetProblem", bind(&CCppInterfaceBridge::GetProblem, this, _1,_2)));
}

//返回大于0真正开始下载，返回0没开始下载
void CCppInterfaceBridge::Install( DISPPARAMS *pDispParams,VARIANT *pVarResult )
{
	LOG(L"call Install");

	if (pDispParams == NULL) 
	{
		return SetRetValue(0, pVarResult);
	}

	if (!m_pSoftwareMgrWrapper) 
	{
		return SetRetValue(0, pVarResult);
	}

	string szJson = GetJsonParam(pDispParams);
	CAutoDeleteJson jsonObj(cJSON_Parse(szJson.c_str()));
	if(jsonObj.pJson == NULL) 
	{
		return SetRetValue(0, pVarResult);
	}
	
	ULONGLONG llTotalSize = GetTotalSize(jsonObj.pJson);
	if (!m_pSoftwareMgrWrapper->CheckDownDir(llTotalSize))
	{
		LOG(L"Select directory failed, do not install");
		return SetRetValue(0, pVarResult);
	}
	
	int nRet = 0;
	int nCount = cJSON_GetArraySize(jsonObj.pJson);
	for (int n = 0; n < nCount; ++n)
	{
		cJSON *pItem = cJSON_GetArrayItem(jsonObj.pJson, n);
		if (pItem == NULL) continue;

		CString szIds = JsonHelper::GetStringW(pItem, "ids");
		if (szIds.IsEmpty()) continue;
		
		LOG(L"Js Call Install %s", szIds);
	
		FlyIconImp(pItem);
	
		SoftwareProxyPtr spSoftwareProxy = m_pSoftwareMgrWrapper->GetSofteware((LPCWSTR)szIds);
		if (spSoftwareProxy)
		{
			spSoftwareProxy->Install();
		}
		else
		{
			spSoftwareProxy = m_pSoftwareMgrWrapper->CreateSofteware((LPCWSTR)szIds);
			if (!spSoftwareProxy) continue;
			
			//reg
			REG_INFO reginfo;
			GetRegInfo(pItem, reginfo);
			for (size_t i = 0; i < reginfo.size(); ++i)
			{
				spSoftwareProxy->AddReg(
					(OLE_HANDLE)reginfo[i].hKey, 
					_bstr_t(reginfo[i].szSubKey), 
					_bstr_t(reginfo[i].szValue), 
					_bstr_t(reginfo[i].szExe));
			}

			std::string szName = JsonHelper::GetString(pItem, "soft_name", "");
			std::string szDecode = URLDecodeEx(szName.c_str());
			Utf16 u16(szDecode.c_str());

			spSoftwareProxy->SetName(_bstr_t(u16.AsWz()));

			CString szValue = JsonHelper::GetStringW(pItem, "icon");
			spSoftwareProxy->SetIconUrl(_bstr_t(szValue));

			szValue = JsonHelper::GetStringW(pItem, "360softmgrid");
			spSoftwareProxy->Set360SoftMgrId(_bstr_t(szValue));

			CString szValue1 = JsonHelper::GetStringW(pItem, "md5");
			szValue = JsonHelper::GetStringW(pItem, "down_url");
			spSoftwareProxy->SetDownUrl(_bstr_t(szValue), _bstr_t(szValue1));

			szValue = JsonHelper::GetStringW(pItem, "param");
			spSoftwareProxy->SetRunParam(_bstr_t(szValue));
			
			//urlencoded
			string szRaw = JsonHelper::GetString(pItem, "signer_key", "");
			szValue = ConvertToWString(szRaw);
			spSoftwareProxy->SetSigner(_bstr_t(szValue));

			int nType = JsonHelper::GetInt(pItem, "type", INSTALL_PACKAGE_TYPE_EXE);
			spSoftwareProxy->SetDownFileType(nType);
			
			int nCat = JsonHelper::GetInt(pItem, "cat", 0); //默认是个软件
			spSoftwareProxy->IsGamePatch(nCat == 0);
			spSoftwareProxy->Install();
		}
		nRet++;
	}
	
	SetRetValue(nRet, pVarResult);
}

ULONGLONG CCppInterfaceBridge::GetTotalSize(cJSON *pJson)
{
	if (pJson == NULL) return 0;
	
	ULONGLONG llSize = 0;
	int nCount = cJSON_GetArraySize(pJson);
	for (int n = 0; n < nCount; ++n)
	{
		cJSON *pItem = cJSON_GetArrayItem(pJson, n);
		if (pItem == NULL) continue;

		string szSize = JsonHelper::GetString(pItem, "file_size", "");
		float fSize = atof(szSize.c_str());
		llSize += (fSize * 1024*1024);
	}

	return llSize;
}


void CCppInterfaceBridge::Download( DISPPARAMS *pDispParams,VARIANT *pVarResult )
{
	LOG(L"call Download");

	if (pDispParams == NULL) return;
	if (!m_pSoftwareMgrWrapper) return;

	string szPram = GetJsonParam(pDispParams);
	if (szPram.empty()) return;
	CAutoDeleteJson jsonObj(cJSON_Parse(szPram.c_str()));
	if(jsonObj.pJson == NULL) return;
	
	CString szUrls;
	int nCount = cJSON_GetArraySize(jsonObj.pJson);
	for (int n = 0; n < nCount; ++n)
	{
		cJSON *pItem = cJSON_GetArrayItem(jsonObj.pJson, n);
		if (pItem == NULL) continue;

		CString szUrl = JsonHelper::GetStringW(pItem, "icon");
		if (!szUrl.IsEmpty())
		{
			szUrls += szUrl;
			szUrls += L";";
		}
	}

	m_pSoftwareMgrWrapper->Download(szUrls);	
}

void CCppInterfaceBridge::FlyIcon( DISPPARAMS *pDispParams,VARIANT *pVarResult )
{
	LOG(L"call FlyIcon");
	if (!m_pSoftwareMgrWrapper) return;
	
	string szPram = GetJsonParam(pDispParams);
	CAutoDeleteJson jsonObj(cJSON_Parse(szPram.c_str()));
	if(jsonObj.pJson == NULL) return;

	CString szIcon = JsonHelper::GetStringW(jsonObj.pJson, "icon");
	CPoint ptSrc;
	ptSrc.x = JsonHelper::GetInt(jsonObj.pJson, "x", 0);
	ptSrc.y = JsonHelper::GetInt(jsonObj.pJson, "y", 0);
	long lId = 0;
	IAnimationPtr spAnimation = m_pSoftwareMgrWrapper->CreateAnimation(szIcon, &lId);
	if (!spAnimation) return;
	
	HWND hMainWnd = m_pSoftwareMgrWrapper->GetMainWnd();
	::ClientToScreen(hMainWnd, &ptSrc);

	CPoint ptDownMgr;
	m_pSoftwareMgrWrapper->GetDownMgrIconPos(ptDownMgr.x, ptDownMgr.y);
	CRect rctWnd ;
	::GetWindowRect(hMainWnd, &rctWnd);
	CPoint ptDst = rctWnd.TopLeft();
	ptDst += ptDownMgr;
	
	spAnimation->Start(ptSrc.x, ptSrc.y, ptDst.x + 10, ptDst.y + 10, lId);
}

BOOL CCppInterfaceBridge::FlyIconImp(cJSON *pJson)
{
	LOG(L"call FlyIcon");
	if (!m_pSoftwareMgrWrapper) return FALSE;
	if (pJson == NULL) return FALSE;
	
	CString szIcon = JsonHelper::GetStringW(pJson, "icon");
	CPoint ptSrc;
	ptSrc.x = JsonHelper::GetInt(pJson, "screen_x", 0);
	ptSrc.y = JsonHelper::GetInt(pJson, "screen_y", 0);
	long lId = 0;
	if (ptSrc.x == 0 || ptSrc.y == 0) return  FALSE;
	
	IAnimationPtr spAnimation = m_pSoftwareMgrWrapper->CreateAnimation(szIcon, &lId);
	if (!spAnimation) return FALSE;

	HWND hMainWnd = m_pSoftwareMgrWrapper->GetMainWnd();
	::ClientToScreen(hMainWnd, &ptSrc);

	CPoint ptDownMgr;
	m_pSoftwareMgrWrapper->GetDownMgrIconPos(ptDownMgr.x, ptDownMgr.y);
	CRect rctWnd ;
	::GetWindowRect(hMainWnd, &rctWnd);
	CPoint ptDst = rctWnd.TopLeft();
	ptDst += ptDownMgr;

	spAnimation->Start(ptSrc.x, ptSrc.y, ptDst.x + 10, ptDst.y + 10, lId);

	return TRUE;
}


void CCppInterfaceBridge::GetStatus( DISPPARAMS *pDispParams,VARIANT *pVarResult )
{
	if (pDispParams == NULL) return;
	
	string szPram = GetJsonParam(pDispParams);
	if (szPram.empty()) return;
	
	CAutoDeleteJson jsonObj(cJSON_Parse(szPram.c_str()));
	if(jsonObj.pJson == NULL) return;
	
	CString szJson;
	int nCount = cJSON_GetArraySize(jsonObj.pJson);
	for (int n = 0; n < nCount; ++n)
	{
		cJSON *pItem = cJSON_GetArrayItem(jsonObj.pJson, n);
		if (pItem == NULL) continue;
		
		CString szIds = JsonHelper::GetStringW(pItem, "ids");
		szIds.Trim();
		if (szIds.IsEmpty()) continue;

		int nRet =	SOFTWARE_STATUS_NOT_INSTALL;
		SoftwareProxyPtr spSoftwareProxy = m_pSoftwareMgrWrapper->GetSofteware(szIds.GetBuffer());
		szIds.ReleaseBuffer();

		REG_INFO reginfo;
		GetRegInfo(pItem, reginfo);
		if (CUtils::IsInstalled(reginfo))
		{
			nRet = SOFTWARE_STATUS_INSTALLED;
		}
		else
		{
			if (spSoftwareProxy)
			{
				nRet = spSoftwareProxy->GetStatus();
			}
		}

		CString szItem;
		szItem.Format(L"\"%s\": %d", szIds, nRet);
		if (szJson.IsEmpty())
		{
			szJson += (L"{" + szItem);
		}
		else
		{
			szJson += (L"," + szItem);
		}
	}
	if (!szJson.IsEmpty())
	{
		szJson += L"}";
	}

 	SetRetValue(szJson, pVarResult);
	LOG(L"GetStatus return :"+ szJson);
}

void CCppInterfaceBridge::GetRegInfo(cJSON *pItem, REG_INFO & regInfo)
{
	cJSON *pArray = cJSON_GetObjectItem(pItem, "registry_loc");
	if (pArray == NULL) return;
	
	int nCount = cJSON_GetArraySize(pArray);
	for (int n = 0; n < nCount; ++n)
	{
		cJSON *pItem = cJSON_GetArrayItem(pArray, n);
		if (pItem == NULL) continue;
		
		REG_GROUP group;
		CString szValue = JsonHelper::GetStringW(pItem, "hkey");
		
		group.hKey = CUtils::StringToHKEY(szValue);

		//urlencoded 
		string szRaw = JsonHelper::GetString(pItem, "subkey", "");
		group.szSubKey = ConvertToWString(szRaw);

		szRaw = JsonHelper::GetString(pItem, "value", "");
		group.szValue = ConvertToWString(szRaw);

		szRaw = JsonHelper::GetString(pItem, "exe", "");
		group.szExe = ConvertToWString(szRaw);

		regInfo.push_back(group);
	}
}

CString CCppInterfaceBridge::ConvertToWString(const string & szRaw)
{
	string szRawDecode = URLDecodeEx(szRaw.c_str());
	Utf16 u16(szRawDecode.c_str());
	return u16.AsWz();
}

string CCppInterfaceBridge::GetJsonParam(DISPPARAMS *pDispParams)
{
	if (pDispParams == NULL) return "";
	
	//参数反过来
	int nIndx = pDispParams->cArgs - 1;
	ATLASSERT(nIndx >= 0);
	if (nIndx < 0) return "";
	
	ATLASSERT(pDispParams->rgvarg[nIndx].vt == VT_BSTR);
	if (pDispParams->rgvarg[nIndx].vt != VT_BSTR) return "";

	char * pszArg = _com_util::ConvertBSTRToString(pDispParams->rgvarg[nIndx].bstrVal);
	if (pszArg == NULL) return "";

	return pszArg;
}

void CCppInterfaceBridge::SetSoftwareMgrWrapper( ISoftwareMgrWrapper * pSoftwareMgrWrapper )
{
	ATLASSERT(pSoftwareMgrWrapper);
	if(pSoftwareMgrWrapper)
	{
		m_pSoftwareMgrWrapper = pSoftwareMgrWrapper;
	}
}

void CCppInterfaceBridge::OnWebReady( DISPPARAMS *pDispParams,VARIANT *pVarResult )
{
	LOG(L"OnWebReady");

	if (m_webReadyCallback)
	{
		m_webReadyCallback();
	}
}

void CCppInterfaceBridge::SetWebReadyCallback( SimpleFunction func )
{
	m_webReadyCallback = func;
}

void CCppInterfaceBridge::GetSystemInfo( DISPPARAMS *pDispParams,VARIANT *pVarResult )
{
	CConfigure *pCfg = CConfigure::GetInstance();
	CString szJson;
	szJson.Format(L"{\"cpu\": %d,\"gpu\": %d,\"mem\": %d, \"system\":\"%s\", \"is64\":\"%d\"}", 
		pCfg->CpuScore(), 
		pCfg->GpuScore(), 
		pCfg->MemSize(),
		CUtils::GetOsName(),
		CUtils::IsWin64());

	SetRetValue(szJson, pVarResult);
	LOG(L"GetBenchmark return:"+szJson);
}

void CCppInterfaceBridge::SetBenchRank( DISPPARAMS *pDispParams,VARIANT *pVarResult )
{
	string szPram = GetJsonParam(pDispParams);
	if (szPram.empty()) return;

	LOG(L"SetBenchRank: %s", Utf16(szPram.c_str()).AsWz());

	CAutoDeleteJson jsonObj(cJSON_Parse(szPram.c_str()));
	if(jsonObj.pJson == NULL) return;
	
	int nPercent = JsonHelper::GetInt(jsonObj.pJson, "cpu", 0);
	CConfigure::GetInstance()->CpuRank(nPercent);

	nPercent = JsonHelper::GetInt(jsonObj.pJson, "gpu", 0);
	CConfigure::GetInstance()->GpuRank(nPercent);

	nPercent = JsonHelper::GetInt(jsonObj.pJson, "mem", 0);
	CConfigure::GetInstance()->MemRank(nPercent);

	::PostMessage(CSoftwareMgrWrapper::GetInstance()->GetMainWnd(), WM_REFRESH_RANK, 0, 0);
}

void CCppInterfaceBridge::Examine( DISPPARAMS *pDispParams,VARIANT *pVarResult )
{
	::PostMessage(CSoftwareMgrWrapper::GetInstance()->GetMainWnd(), WM_START_EXAMINE, 0, 0);
	LOG(L"CCppInterfaceBridge::Examine");
}

//返回问题个数
void CCppInterfaceBridge::GetProblem( DISPPARAMS *pDispParams,VARIANT *pVarResult )
{
	int nCount = CConfigure::GetInstance()->GetUnfixProblem();
	SetRetValue(nCount, pVarResult);

	LOG(L"CCppInterfaceBridge::GetProblem return: %d", nCount);
}

