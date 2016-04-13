#include "stdafx.h"
#include "Item.h"
#include <boost/bind.hpp>

#include <strsafe.h>
#include "Utils.h"
#include "7z/C7za.h"

#include "HttpTaskFactory.h"
#include "xml/XmlParser.h"

#define GET_SOFT_MGR_INFO_BY_360_SOFT_MGR_ID_URL L"http://q.soft.360.cn/get_download_url.php?type=download_url&soft_ids=%s"
#define PDOWN_PREFIX L"pdown://"
#define LUDASHI_DL_TEMP L"LudashiDL"
#define DOWNLOAD_PROGRESS_PERCENT   (80)

using namespace Engine;

Engine::CItem::CItem()
: m_pFixCallback(NULL)
, m_bCancelFix(FALSE)
, m_bHasProblem(FALSE)
, m_installPackageType(INSTALL_PACKAGE_TYPE_UNKNOWN)
, m_hSyncEvent(NULL)
{
    m_hSyncEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

Engine::CItem::~CItem()
{
    if (m_hSyncEvent)
    {
        CloseHandle(m_hSyncEvent);
        m_hSyncEvent = NULL; 
    }
}

void __stdcall Engine::CItem::Destroy()
{
    delete this;
}

void __stdcall Engine::CItem::GetName( WCHAR * pszName, int nMaxCount )
{
    if (pszName == NULL) return;

    StringCchCopy(pszName, nMaxCount, (LPCWSTR)m_szName);
}

BOOL __stdcall Engine::CItem::HasProblem( void )
{
    return m_bHasProblem;
}

void __stdcall Engine::CItem::Scan( IScanCallback *pCallback)
{
    if (pCallback)
    {
        pCallback->OnScanItemStart(this);
    }

    m_bHasProblem = CheckProblem();

    if (pCallback)
    {
        pCallback->OnScanItemEnd(this);
    }
}

BOOL __stdcall Engine::CItem::CheckProblem()
{
    return !CUtils::IsInstalled(m_RegInfo);
}

void __stdcall Engine::CItem::CancelScan( void )
{
}

void __stdcall Engine::CItem::Fix( IFixCallback *pFixCallback)
{   
    InterlockedExchange(&m_bCancelFix, FALSE);
	
    m_pFixCallback = pFixCallback;
    ATLASSERT(m_pFixCallback != NULL);

    FixThread();
}

void Engine::CItem::FixThread()
{
    BOOL bFixResult = FALSE;

    if (m_pFixCallback)
    {
        m_pFixCallback->OnFixItemStart(this);
    }

    do 
    {
        if (!DoPrepareDownload())
        {
            break;
        }

        if (m_bCancelFix)
        {
            break;
        }

        if (!DoStartDownload())
        {
            break;
        }

        if (m_bCancelFix)
		{
			break;
		}

        if (m_pFixCallback)
        {
            m_pFixCallback->OnFixEvent(this, FIX_EVENT_START_INST);
        }

        if (m_pFixCallback)
        {
            m_pFixCallback->OnFixProgress(this, 90);
        }

		if (!DoStartInstall())
		{
			break;
		}

        if (m_bCancelFix)
        {
            break;
        }

        if (m_pFixCallback)
        {
            m_pFixCallback->OnFixProgress(this, 95);
        }

        if (!DoCheckInstall())
        {
            break;
        }

        if (m_pFixCallback)
        {
            m_pFixCallback->OnFixProgress(this, 100);
        }

        bFixResult = TRUE;

    } while (0);

    if (m_pFixCallback)
    {
        m_bHasProblem = !bFixResult;
        m_pFixCallback->OnFixItemEnd(this, bFixResult);
    }
}

void __stdcall Engine::CItem::CancelFix( void )
{
    InterlockedExchange(&m_bCancelFix, TRUE);

    m_spHttpTaskSoftMgrInfoBy360SoftMgrId.reset();
    m_spHttpTaskDownload.reset();

    SyncNotify();
}

BOOL Engine::CItem::SyncWait( DWORD dwTimeout )
{
    ATLASSERT(m_hSyncEvent != NULL);
    if (NULL == m_hSyncEvent)
    {
        return FALSE;
    }

    DWORD dwRet = WaitForSingleObject(m_hSyncEvent, dwTimeout);
    if(WAIT_OBJECT_0 != dwRet && WAIT_TIMEOUT != dwRet)
    {
        // 退出
        return TRUE;
    }

    // 继续
    return FALSE;
}

void Engine::CItem::SyncNotify()
{
    if (NULL == m_hSyncEvent)
    {
        return;
    }

    //SetEvent(m_hSyncEvent);
    PulseEvent(m_hSyncEvent);
}

void _stdcall Engine::CItem::OnHttpStatus( IHttpTask *pTask, HTTP_INFO &inf )
{
    boost::shared_ptr<IHttpTask> spHttpTaskSoftMgrInfoBy360SoftMgrId = m_spHttpTaskSoftMgrInfoBy360SoftMgrId;
    boost::shared_ptr<IHttpTask> spHttpTaskDownload = m_spHttpTaskDownload;

    if (pTask == spHttpTaskSoftMgrInfoBy360SoftMgrId.get())
    {
        if (inf.nStatus == eHttpRunning || inf.nStatus == eHttpWait) 
            return;

        ProcessResultSoftMgrInfoBy360SoftMgrId(inf.pHttpData);
    }
    else if(pTask == spHttpTaskDownload.get())
    {
        switch(inf.nStatus)
        {
        case eHttpSuccess:
        case eHttpFail:
            {
                ProcessDownloadFileResult(inf);
            }
            break;
        case eHttpRunning:
        case eHttpWait:
            {
                ProcessDownloadFileProgress(inf);
            }
            break;
        }
    }
}

BOOL Engine::CItem::DoPrepareDownload()
{
    BOOL bRet = FALSE;

    do 
    {
        if (!m_str360SoftMgrId.IsEmpty())
        {
            if (m_str360SoftMgrUrl.IsEmpty())
            {
                RequestSoftMgrInfoBy360SoftMgrId();

                if (SyncWait(INFINITE))
                {
                    break;
                }
            }

            if (!m_strUrlBy360SoftMgr.IsEmpty())
            {
                bRet = TRUE;
            }

            break;
        }

        if (!m_strUrl.IsEmpty())
        {
            bRet = TRUE;

            break;
        }

    } while (0);

    return bRet;
}

BOOL Engine::CItem::DoStartDownload()
{
    BOOL bRet = FALSE;

    do 
    {
        CString strUrl;
        CString strFileName;
        if (!m_strUrlBy360SoftMgr.IsEmpty())
        {
            strUrl = m_strUrlBy360SoftMgr;
            strFileName = GetFileNameByHttpUrl(m_strUrlBy360SoftMgr);
        }
        else if(!m_strUrl.IsEmpty())
        {
            strUrl = m_strUrl;
            strFileName = GetFileNameByHttpUrl(m_strUrl);
        }

        if (strFileName.IsEmpty())
        {
            break;
        }

        WCHAR szTmpPath[MAX_PATH] = {};
        DWORD dwSize = MAX_PATH;
        DWORD dwRet = GetTempPath(dwSize, szTmpPath);
        if (!(0 < dwRet && dwRet < MAX_PATH))
        {
            break;
        }

        PathAppend(szTmpPath, LUDASHI_DL_TEMP);

        WCHAR szDownloadPath[MAX_PATH] = {};
        PathCombine(szDownloadPath, szTmpPath, strFileName);

        if (!CheckFileIsValid(szDownloadPath))
        {
            if (!AsyncDownloadFile(strUrl, szDownloadPath))
            {
                break;
            }

            if (SyncWait(INFINITE))
            {
                break;
            }
        }
        else
        {
            m_strDownloadFilePath = szDownloadPath;
        }

        if (m_strDownloadFilePath.IsEmpty())
        {
            break;
        }

        bRet = TRUE;

    } while (0);

    return bRet;
}

BOOL Engine::CItem::DoStartInstall()
{
    BOOL bRet = FALSE;

    do
    {
        if (m_strDownloadFilePath.IsEmpty() || !PathFileExists(m_strDownloadFilePath))
        {
            break;
        }

        CString strRunPath;
        CString strRunParam;

        if(INSTALL_PACKAGE_TYPE_EXE == m_installPackageType)
        {
            strRunPath = m_strDownloadFilePath;
            strRunParam = m_strRunParam;

            bRet = CUtils::RunExeShellExecuteEx(strRunPath, strRunParam, TRUE);
        }
        else if(INSTALL_PACKAGE_TYPE_ZIP == m_installPackageType)
        {
            CString strUnzipPath = m_strDownloadFilePath;
            if (strUnzipPath.ReverseFind('.') != -1)
            {
                PathRemoveExtension(strUnzipPath.GetBuffer());
                strUnzipPath.ReleaseBuffer();

                if (PathFileExists(strUnzipPath))
                {
                    CUtils::RemoveDirectory(strUnzipPath);
                }

                WCHAR sz7zDllPath[MAX_PATH] = {};
                GetModuleFileName(g_hModule, sz7zDllPath, _countof(sz7zDllPath));
                PathRemoveFileSpec(sz7zDllPath);
                PathRemoveFileSpec(sz7zDllPath);
                PathAppend(sz7zDllPath, L"DrvMgr\\7za.dll");

                if (C7za::UncompressFile(sz7zDllPath, m_strDownloadFilePath, strUnzipPath, L"", L""))
                {
                    WCHAR szRunPath[MAX_PATH] = {};
                    PathCombine(szRunPath, strUnzipPath, m_strRunPath);

                    strRunPath = szRunPath;
                    strRunParam = m_strRunParam;

                    bRet = CUtils::RunExeShellExecuteEx(strRunPath, strRunParam, TRUE);
                }

                if (PathFileExists(strUnzipPath))
                {
                    CUtils::RemoveDirectory(strUnzipPath);
                }
            }
        }

    } while(0);

    return bRet;
}

BOOL Engine::CItem::DoCheckInstall()
{
    BOOL bSuccess = FALSE;

    do 
    {
        if (CheckProblem())
        {
            break;
        }

        bSuccess = TRUE;

    } while (0);

    return bSuccess;
}

void Engine::CItem::RequestSoftMgrInfoBy360SoftMgrId()
{
    if (!m_spHttpTaskSoftMgrInfoBy360SoftMgrId)
    {
        IHttpTask* pHttpTask = CHttpTaskFactory::GetInstance()->CreateTask();
        if (pHttpTask)
        {
            m_spHttpTaskSoftMgrInfoBy360SoftMgrId.reset(pHttpTask, HttpTaskDeletor());
            m_spHttpTaskSoftMgrInfoBy360SoftMgrId->SetCallback(this);
        }
    }

    if (m_spHttpTaskSoftMgrInfoBy360SoftMgrId)
    { 
        CString strUrl;
        strUrl.Format(GET_SOFT_MGR_INFO_BY_360_SOFT_MGR_ID_URL, m_str360SoftMgrId);

        m_spHttpTaskSoftMgrInfoBy360SoftMgrId->Start(eHttpGet, strUrl);
    }
}

void Engine::CItem::ProcessResultSoftMgrInfoBy360SoftMgrId( const char* pHttpData )
{
    CString strUrl;

    do 
    {
        TiXmlDocument oDoc;
        oDoc.Parse(pHttpData, 0, TIXML_ENCODING_UTF8);
        if (oDoc.Error())
        {
            LOG(L"Parse Xml SoftMgrInfo Error:%S", oDoc.ErrorDesc());
            ATLASSERT(0);
            break;
        }

        TiXmlHandle docHandle(&oDoc);
        TiXmlElement* pElemRetinfo = docHandle.FirstChild("ret").FirstChild("retinfo").ToElement();
        if (!pElemRetinfo)
        {
            ATLASSERT(0);
            break;
        }

        int code = -1;
        pElemRetinfo->QueryIntAttribute("code", &code);
        if (code != 0)
        {
            ATLASSERT(0);
            break;
        }

        int total = 0;
        pElemRetinfo->QueryIntAttribute("total", &total);
        if (total <= 0)
        {
            ATLASSERT(0);
            break;
        }

        TiXmlElement* pElemSoft = docHandle.FirstChild("ret").FirstChild("softs").FirstChild("soft").ToElement();
        if (!pElemSoft)
        {
            ATLASSERT(0);
            break;
        }

        std::string strValue;

        strValue = XmlParser::GetString(pElemSoft, "durls", "");
        strUrl = CA2W(strValue.c_str(), 936);

    } while (0);

    SoftMgrInfoBy360SoftMgrIdCallback(strUrl);
}

void Engine::CItem::SoftMgrInfoBy360SoftMgrIdCallback( CString strUrl )
{
    m_str360SoftMgrUrl = strUrl;

    m_strUrlBy360SoftMgr = GetParamBySoftMgrInfoUrl(m_str360SoftMgrUrl, L"http://", L"");
    if (!m_strUrlBy360SoftMgr.IsEmpty())
    {
        m_strUrlBy360SoftMgr = L"http://" + m_strUrlBy360SoftMgr;
    }

    m_strP2PID = GetParamBySoftMgrInfoUrl(m_str360SoftMgrUrl, L"p2=", L"");

    SyncNotify();
}

CString Engine::CItem::GetParamBySoftMgrInfoUrl(const CString& str360SoftMgrUrl, const CString& strKey, const CString& strDefaultValue )
{
    CString strRet;

    if (!str360SoftMgrUrl.IsEmpty())
    {
        int nStartPos = str360SoftMgrUrl.Find(strKey);
        if (nStartPos != -1)
        {
            nStartPos += strKey.GetLength();
            int nEndPos = str360SoftMgrUrl.Find(L"|", nStartPos);
            if (nEndPos == -1)
            {
                nEndPos = str360SoftMgrUrl.Find(L";", nStartPos);
            }

            if (nEndPos != -1)
            {
                strRet = str360SoftMgrUrl.Mid(nStartPos, nEndPos - nStartPos);
            }
            else
            {
                strRet = str360SoftMgrUrl.Mid(nStartPos);
            }
        }
        else
        {
            strRet = strDefaultValue;
        }
    }

    return strRet;
}

BOOL Engine::CItem::AsyncDownloadFile(const CString& strUrl, const CString& strDownloadPath)
{
    m_strDownloadFilePath.Empty();

    if (!m_spHttpTaskDownload)
    {
        IHttpTask* pHttpTask = CHttpTaskFactory::GetInstance()->CreateTask();
        if (pHttpTask)
        {
            m_spHttpTaskDownload.reset(pHttpTask, HttpTaskDeletor());
            m_spHttpTaskDownload->SetCallback(this);
        }
    }

    if (m_spHttpTaskDownload)
    {
        m_spHttpTaskDownload->SetFilename(strDownloadPath);
        m_spHttpTaskDownload->Start(eHttpGet, strUrl);
        return TRUE;
    } 

    return FALSE;
}

CString Engine::CItem::GetFileNameByHttpUrl( const CString& strHttpUrl )
{
    CString strFileName;

    int nPos = strHttpUrl.ReverseFind('/');
    if (nPos != -1)
    {
        strFileName = strHttpUrl.Right(strHttpUrl.GetLength() - nPos - 1);
    }

    return strFileName;
}

void Engine::CItem::ProcessDownloadFileResult( const HTTP_INFO &inf )
{
    CString strDownloadFilePath = inf.pHttpFile;

    if (eHttpSuccess == inf.nStatus)
    {
        if(CheckFileIsValid(strDownloadFilePath))
        {
            m_strDownloadFilePath = strDownloadFilePath;
        }
    }

    SyncNotify();
}

void Engine::CItem::ProcessDownloadFileProgress( const HTTP_INFO &inf )
{
    if (m_pFixCallback)
    {
        int nProgress = (inf.dwProgress * DOWNLOAD_PROGRESS_PERCENT) / 100;
        m_pFixCallback->OnFixProgress(this, nProgress);
    }
}

BOOL Engine::CItem::CheckFileIsValid( const CString& strFilePath )
{
    BOOL bIsValid = FALSE;

    __int64 int64RealFileSize = CUtils::GetFileSize(strFilePath);
    if (int64RealFileSize > 0)
    {
        do 
        {
            if (!m_strP2PID.IsEmpty())
            {
                CString strRealFileP2PID = CUtils::CalcFileP2PID(strFilePath);
                if (strRealFileP2PID.CompareNoCase(m_strP2PID) == 0)
                {
                    bIsValid = TRUE;
                    break;
                }

                LOG(L"Check 360SoftMgrHash failed, %s", m_strId);
            }

            if (!m_strSigner.IsEmpty())
            {
                if (CheckSigner(m_strSigner, strFilePath))
                {
                    bIsValid = TRUE;
                    break;
                }

                LOG(L"Check Signer failed, %s", m_strId);
            }

            if(!m_strMD5.IsEmpty())
            {
                CString strRealFileMD5 = CUtils::CalcFileMD5(strFilePath);
                if (strRealFileMD5.CompareNoCase(m_strMD5) == 0)
                {
                    bIsValid = TRUE;
                    break;
                }

                LOG(L"Check MD5 failed, %s", m_strId);
            }

            //不认微软数字签名
            DWORD dwSignType = 0;
            if (VerifySignHelper::Has360Sign(strFilePath, &dwSignType))
            {
                bIsValid = (dwSignType != EmbeddedSign);

                if (!bIsValid)
                {
                    LOG(L"Check 360Sign failed, %s", m_strId);
                }
            }


        } while (0);
    }

    return bIsValid;
}

BOOL Engine::CItem::CheckSigner( CString & szSigner, const CString & szFilePath )
{
    vector<CString> SignerVect;

    CString Tokens = L"||";
    CString resTokenUrl;
    CString resToken;

    int curPos = 0;

    resToken = szSigner.Tokenize(Tokens, curPos);
    while(resToken != "")
    {
        resTokenUrl = resToken;
        SignerVect.push_back(resTokenUrl);
        resToken = szSigner.Tokenize(Tokens, curPos);
    }

    for(size_t i = 0;i < SignerVect.size(); ++i)
    {
        CString & szSign = SignerVect[i];

        if (VerifySignHelper::CheckDigitalSign(szFilePath, szSign))
        {
            return TRUE;
        }
    }

    return FALSE;
}





