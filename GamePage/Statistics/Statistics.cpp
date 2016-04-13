// 360SafeInterface.cpp: implementation of the CStatistic class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <WinInet.h>
#include <IPTypes.h>
#include <iphlpapi.h>
#include <miniui/Utils/SimpleThread.h>

#ifdef _WIN64
#include <BaseModule/360MID/360MID_x64.h>
#else
#include <BaseModule/360MID/360MID.h>
#endif // _WIN64

#include "Statistics.h"
#include "../Helper/CommonAlgorithm.h"
#include "../Helper/CharacterCodeHelper.h"
#include "../HttpTaskFactory.h"

#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "Version.lib")

#define STATS_URL L"http://s1.ludashi.com/url2?pid={pid}&mid={mid}&appver={appver}&modver={modver}&type={type}&action={action}"

//Key 按照字母顺序排列
#define SIGN_PARAM L"action={action}#appver={appver}#mid={mid}#modver={modver}#pid={pid}#type={type}#a53c4acbca539e77e0c17e52a2371fe2"
#define SIGN_STATS_URL L"http://www.ludashi.com/stat/pc.php?pid={pid}&type={type}&action={action}&appver={appver}&modver={modver}&mid={mid}&sign_name=pc&sign={sign}"

//#define SIGN_PARAM_EX L"action={action}#appver={appver}#mid={mid}#modver={modver}#pid={pid}#type={type}#value={value}#a53c4acbca539e77e0c17e52a2371fe2"
//#define SIGN_STATS_URL_EX L"http://www.ludashi.com/stat/pc.php?pid={pid}&type={type}&action={action}&appver={appver}&modver={modver}&mid={mid}&value={value}&sign_name=pc&sign={sign}"

#define APP_NAME _T("ComputerZ_CN.exe")

#define URL_PID         L"{pid}"
#define URL_MID         L"{mid}"
#define URL_APP_VER     L"{appver}"
#define URL_MOD_VER     L"{modver}"
#define URL_TYPE        L"{type}"
#define URL_ACTION      L"{action}"
#define URL_SIGN        L"{sign}"
#define URL_VALUE       L"{value}"

//    功能               type              action

typedef struct _DownloadItem
{
	enum { MAX_URL_LEN = 2048 };
	enum { MAX_FILE_LEN = _MAX_PATH * 2 };

    _DownloadItem(CStatistic* pThis, LPCWSTR pszUrl, LPCWSTR pszFilename, BOOL bNeedDelete, StatUrlSendStateCallback stateCallback) 
        : m_pThis(pThis)
        , m_bNeedDelete(bNeedDelete)
        , m_stateCallback(stateCallback)
    {
        memset(m_szUrl, 0, sizeof(m_szUrl));
        if(pszUrl)
        {
            StringCchCopy(m_szUrl, _countof(m_szUrl), pszUrl);
        }

        memset(m_szFilename, 0, sizeof(m_szFilename));
        if(pszFilename)
        {
            StringCchCopy(m_szFilename, _countof(m_szFilename), pszFilename);
        }
    }

	CStatistic * m_pThis;
	WCHAR m_szUrl[MAX_URL_LEN];
	WCHAR m_szFilename[MAX_FILE_LEN];
	BOOL m_bNeedDelete;
    StatUrlSendStateCallback m_stateCallback;
} DOWNLOADITEM;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStatistic::CStatistic()
{
}

unsigned int WINAPI CStatistic::DownloadThreadProc(LPVOID lpParam)
{
    DOWNLOADITEM * pItem = reinterpret_cast<DOWNLOADITEM *>(lpParam);
    if(pItem == NULL) return 0;	

    do
    {		
        CStatistic * pThis = pItem->m_pThis;
        if(pThis == NULL) break;

        if(pItem->m_szUrl[0] == _T('\0')) break;

        if(pThis->HaveInternetConnection())
        {
            //LOG(L"[CStatistic] connection url:%s", pItem->m_szUrl);
            pThis->InternalDownloadFile(pItem->m_szUrl, pItem->m_szFilename, pItem->m_stateCallback);
        }
        else
        {
            //LOG(L"[CStatistic] no connection url:%s", pItem->m_szUrl);
        }
    }while(0);

    if(pItem->m_bNeedDelete)
    {
        delete pItem;
    }

    return 0;
}

BOOL CStatistic::InternalDownloadFile( LPCWSTR lpszUrl, LPCWSTR lpszLocalFile, StatUrlSendStateCallback stateCallback)
{
    BOOL bResult = FALSE;

    if(lpszUrl != NULL && *lpszUrl != L'\0') 
    {	
        if(lpszLocalFile == NULL || * lpszLocalFile == L'\0')
        {
            TCHAR szFile[MAX_PATH * 2]={0};
            if (SUCCEEDED(URLDownloadToCacheFile(NULL, lpszUrl, szFile, MAX_PATH, 0, NULL)))
            {
                //LOG(L"[CStatistic] connection success url:%s", lpszUrl);
                DeleteFile(szFile);
                bResult = TRUE;
            }
            else
            {
                //LOG(L"[CStatistic] connection failed:%d, url:%s", GetLastError(), lpszUrl);
            }
        }
        else
        {
            if(SUCCEEDED(URLDownloadToFile(NULL, lpszUrl, lpszLocalFile, 0, NULL)))
            {
                bResult = TRUE;
            }
        }
    }

    if (stateCallback)
    {
        stateCallback(bResult);
    }

    return bResult;
}

void _stdcall CStatistic::OnHttpStatus( IHttpTask *pTask, HTTP_INFO &inf )
{
    if(inf.nStatus == eHttpSuccess || inf.nStatus == eHttpFail)
    {
        if (pTask)
        {
            pTask->Release();
        }
    }
}

// 下载一个文件
STDMETHODIMP_(BOOL) CStatistic::DownloadFile(
    LPCWSTR lpszUrl, 
    LPCWSTR lpszLocalFile, 
    BOOL bUseMultiThread, 
    WPARAM wParam,
    StatUrlSendStateCallback stateCallback)
{
    if(lpszUrl == NULL)
        return FALSE;

    //if(bUseMultiThread)	// 使用多线程
    //{
    //    _DownloadItem * pItem = new _DownloadItem(this, lpszUrl, lpszLocalFile, TRUE, stateCallback);
    //    CSimpleThread simplethread(DownloadThreadProc, pItem);
    //}
    //else				// 使用单线程
    //{
    //    return InternalDownloadFile(lpszUrl, lpszLocalFile, stateCallback);
    //}

    IHttpTask* pHttpTask = CHttpTaskFactory::GetInstance()->CreateTask();
    if (pHttpTask)
    {
        pHttpTask->SetCallback(this);
        pHttpTask->AddHeader("Connection", "Close");
        pHttpTask->Start(eHttpGet, lpszUrl);
        return TRUE;
    }

    return FALSE;
}

STDMETHODIMP_(BOOL) CStatistic::HaveInternetConnection()
{
    DWORD dwFlag;

    //	如果函数返回FALSE,则肯定没有连接
    if( InternetGetConnectedState(&dwFlag,0) )
    {
        //	必须有下列任意一种连接，才认为是有连接
        //	因为INTERNET_CONNECTION_CONFIGURED被置位也可能是没有连接
        if( (dwFlag&INTERNET_CONNECTION_LAN) ||
            (dwFlag&INTERNET_CONNECTION_MODEM) ||
            (dwFlag&INTERNET_CONNECTION_PROXY) )
            return TRUE;
    }

    return IsNetGatewayExists();
}

// 是否存在网关
STDMETHODIMP_(BOOL) CStatistic::IsNetGatewayExists(void)
{
    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;

    BOOL bExists = FALSE;
    DWORD dwRetVal = 0;

    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO *) malloc(ulOutBufLen);
    if(pAdapterInfo)
    {
        dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen);
        if(dwRetVal == ERROR_BUFFER_OVERFLOW )
        {
            free(pAdapterInfo);

            pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen);
            if(pAdapterInfo)
                dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen);
        }

        if( dwRetVal == NO_ERROR)
        {
            pAdapter = pAdapterInfo;
            while (pAdapter)
            {
                if(pAdapter->GatewayList.IpAddress.String[0])
                {
                    bExists = TRUE;
                    break;
                }
                pAdapter = pAdapter->Next;
            }
        }

        free(pAdapterInfo);
    }

    return bExists;
}

STDMETHODIMP_(BOOL) CStatistic::GetFileVer(LPWSTR lpFileFullPath, LPWSTR lpVer, int nVerlen)
{
    TCHAR  szVersionBuffer[8192] = L"";
    DWORD dwVerSize = 0;
    DWORD dwHandle = 0;

    if (!lpFileFullPath || !lpVer)
        return FALSE;

    dwVerSize = ::GetFileVersionInfoSize( lpFileFullPath, &dwHandle );
    if( dwVerSize == 0 || dwVerSize > (sizeof(szVersionBuffer)-1) )
        return FALSE;

    if( ::GetFileVersionInfo( lpFileFullPath, 0, dwVerSize, szVersionBuffer) )
    {
        VS_FIXEDFILEINFO * pInfo;
        unsigned int nInfoLen;

        if( VerQueryValue( szVersionBuffer, _T("\\"), (void**)&pInfo, &nInfoLen ) )
        {
            memset( lpVer, 0, nVerlen * sizeof(TCHAR) );
            //_snwprintf( lpVer, nVerlen-1, _T("%d.%d.%d.%d"),
            //    HIWORD( pInfo->dwFileVersionMS ), LOWORD( pInfo->dwFileVersionMS ),
            //    HIWORD( pInfo->dwFileVersionLS ), LOWORD( pInfo->dwFileVersionLS ) );
            StringCchPrintf(lpVer, nVerlen, _T("%d.%d.%d.%d"),
                HIWORD( pInfo->dwFileVersionMS ), LOWORD( pInfo->dwFileVersionMS ),
                HIWORD( pInfo->dwFileVersionLS ), LOWORD( pInfo->dwFileVersionLS ));
            return TRUE;
        }
    }

    return FALSE;
}

STDMETHODIMP_(BOOL) CStatistic::GetMID(LPWSTR lpszBuffer, int nMaxCharCount /* 注意，不是缓冲区长度，而是字符个数 */)
{
    if(lpszBuffer == NULL) 
        return FALSE;

    *lpszBuffer = 0;

    return Get360SafeMID(lpszBuffer, nMaxCharCount);
}

CString CStatistic::GetPID()
{
    CRegKey key;
    LRESULT lRes = key.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Ludashi"), KEY_READ);
    if (lRes == ERROR_SUCCESS)
    {
        TCHAR szValue[MAX_PATH];
        DWORD dwLen = MAX_PATH;
        lRes = key.QueryStringValue(_T("VendorID"), szValue, &dwLen);
        if (lRes == ERROR_SUCCESS)
        {
            return szValue;
        }
    }

    return _T("");
}

void CStatistic::ReplacePlaceholders( CString & szUrl, const CString & szType, const CString & szAction )
{
    if (szUrl.IsEmpty())
    {
        ATLASSERT(FALSE);
        return;
    }

    if (m_szPID.IsEmpty())
    {
        m_szPID = GetPID();
    }

    if (m_szMID.IsEmpty())
    {
        GetMID(m_szMID.GetBufferSetLength(MAX_PATH), MAX_PATH);
    }

    if (m_szAppVer.IsEmpty())
    {
        WCHAR szFile[MAX_PATH] = {};
        GetModuleFileName(g_hModule, szFile, MAX_PATH);
        PathRemoveFileSpec(szFile);
        PathRemoveFileSpec(szFile);
        PathAppend(szFile, APP_NAME);

        GetFileVer(szFile, m_szAppVer.GetBufferSetLength(MAX_PATH), MAX_PATH);
    }

    if (m_szModVer.IsEmpty())
    {
        WCHAR szFile[MAX_PATH] = {};
        GetModuleFileName(g_hModule, szFile, MAX_PATH);

        GetFileVer(szFile, m_szModVer.GetBufferSetLength(MAX_PATH), MAX_PATH);
    }

    if (szUrl.Find(URL_PID) > 0)
    {
        szUrl.Replace(URL_PID, m_szPID);
    }

    if (szUrl.Find(URL_MID) > 0)
    {
        szUrl.Replace(URL_MID, m_szMID);
    }

    if (szUrl.Find(URL_APP_VER) > 0)
    {
        szUrl.Replace(URL_APP_VER, m_szAppVer);
    }

    if (szUrl.Find(URL_MOD_VER) > 0)
    {
        szUrl.Replace(URL_MOD_VER, m_szModVer);
    }

    if (szUrl.Find(URL_TYPE) > 0)
    {
        szUrl.Replace(URL_TYPE, szType);
    }

    if (szUrl.Find(URL_ACTION) > 0)
    {
        szUrl.Replace(URL_ACTION, szAction);
    }
}

STDMETHODIMP_(BOOL) CStatistic::SendStatUrl(LPCWSTR lpszUrl, BOOL bUseThread /*= TRUE*/, StatUrlSendStateCallback stateCallback /*= NULL*/)
{
    if (lpszUrl == NULL) 
        return FALSE;

    CString strUrl = lpszUrl;
    ReplacePlaceholders(strUrl, L"", L"");

    return DownloadFile(strUrl, NULL, bUseThread, 0, stateCallback);
}

STDMETHODIMP_(BOOL) CStatistic::SendStatUrlEx(LPCWSTR lpszType, LPCWSTR lpszAction, LPCWSTR lpszExtra /*= NULL*/, BOOL bUseThread /*= TRUE*/, StatUrlSendStateCallback stateCallback /*= NULL*/)
{
    if (!lpszType || !lpszAction)
        return FALSE;

    CString strUrl = STATS_URL;
    ReplacePlaceholders(strUrl, lpszType, lpszAction);

    if (lpszExtra)
    {
        strUrl += lpszExtra;
    }

    return DownloadFile(strUrl, NULL, bUseThread, 0, stateCallback);
}

STDMETHODIMP_(BOOL) CStatistic::SendStatUrlExWithSigned( LPCWSTR lpszType, LPCWSTR lpszAction, BOOL bUseThread /*= TRUE*/, StatUrlSendStateCallback stateCallback /*= NULL*/)
{
    if (!lpszType || !lpszAction)
        return FALSE;

    CString strUrl = SIGN_STATS_URL;
    ReplacePlaceholders(strUrl, lpszType, lpszAction);

    CString strSignParam = SIGN_PARAM;
    ReplacePlaceholders(strSignParam, lpszType, lpszAction);

    Utf8 u8(strSignParam);
    const char * pszParam = u8.AsSz();
    BYTE md5[16] = {};

    WCHAR szMd5[MAX_PATH] = {};
    CCommonAlgorithm::MakeBuffMd5((LPVOID)pszParam, u8.Len8() - 1, md5);/// -'0'
    CCommonAlgorithm::ConvertMD5SumToChar(md5, szMd5, MAX_PATH);
    _tcslwr_s(szMd5);

    if (strUrl.Find(URL_SIGN) > 0)
    {
        strUrl.Replace(URL_SIGN, szMd5);
    }

    return DownloadFile(strUrl, NULL, bUseThread, 0, stateCallback);
}
