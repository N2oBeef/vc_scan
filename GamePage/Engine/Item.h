#pragma once

#include "IEngine.h"
#include "Base/XThread.h"
#include "Software/ICppInterfaceBridge.h"
#include <boost/thread.hpp>
#include "Net/IHttpTask.h"
#include <SoftMgr/SoftMgrDefine.h>

namespace Engine
{
    class CItem
        : public Engine::IItem
        , public IHttpTaskCallback
    {
        friend class CEngineConfigure;

    public:
        CItem();
        virtual ~CItem();

    public:
        virtual void __stdcall Destroy();

        virtual void __stdcall GetName(WCHAR * pszName, int nMaxCount);

        //是否有问题
        virtual BOOL __stdcall HasProblem(void); 

        //方法供引擎内部使用
        virtual void __stdcall Scan(IScanCallback *pCallback);
        virtual void __stdcall CancelScan(void);

        //方法供引擎内部使用
        virtual void __stdcall Fix(IFixCallback *pFixCallback);
        virtual void __stdcall CancelFix(void);

    protected:
        virtual BOOL __stdcall CheckProblem();

        void FixThread();

        virtual void _stdcall OnHttpStatus(IHttpTask *pTask, HTTP_INFO &inf);

        BOOL DoPrepareDownload();
        BOOL DoStartDownload();
        BOOL DoStartInstall();
        BOOL DoCheckInstall();

        BOOL SyncWait(DWORD dwTimeout);
        void SyncNotify();

        void RequestSoftMgrInfoBy360SoftMgrId();
        void ProcessResultSoftMgrInfoBy360SoftMgrId( const char* pHttpData );
        void SoftMgrInfoBy360SoftMgrIdCallback(CString strUrl);
        CString GetParamBySoftMgrInfoUrl(const CString& str360SoftMgrUrl, const CString& strKey, const CString& strDefaultValue);

        BOOL AsyncDownloadFile(const CString& strUrl, const CString& strDownloadPath);
        CString GetFileNameByHttpUrl( const CString& strHttpUrl );
        void ProcessDownloadFileResult( const HTTP_INFO &inf );
        void ProcessDownloadFileProgress( const HTTP_INFO &inf );

        BOOL CheckFileIsValid(const CString& strFilePath);
        BOOL CheckSigner(CString & szSigner, const CString & szFilePath);

    protected:
        IFixCallback* m_pFixCallback;
        volatile LONG m_bCancelFix;

        BOOL m_bHasProblem;
        CString m_szName;

        CString m_strId;
        CString m_str360SoftMgrId;
        CString m_strUrl;
        CString m_strMD5;
        REG_INFO m_RegInfo;
        INSTALL_PACKAGE_TYPE m_installPackageType;
        CString m_strRunPath;
        CString m_strRunParam;
        CString m_strSigner;

        boost::shared_ptr<IHttpTask> m_spHttpTaskSoftMgrInfoBy360SoftMgrId;
        boost::shared_ptr<IHttpTask> m_spHttpTaskDownload;

        HANDLE m_hSyncEvent;
        CString m_str360SoftMgrUrl;
        CString m_strP2PID;
        CString m_strUrlBy360SoftMgr;

        CString m_strDownloadFilePath;
    };
}