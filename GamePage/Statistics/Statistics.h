#pragma once

#include "../Base/SingletonT.h"
#include <boost/function.hpp>
#include "../Net/IHttpTask.h"

typedef boost::function<void(BOOL)> StatUrlSendStateCallback;

class CStatistic
    : public CSingletonT<CStatistic>
    , public IHttpTaskCallback
    
{
    SINGLETON(CStatistic);

private:
    CStatistic();

public:
    // 发送一个统计URL
    STDMETHOD_(BOOL, SendStatUrlEx)(LPCWSTR lpszType, LPCWSTR lpszAction, LPCWSTR lpszExtra = NULL, BOOL bUseThread = TRUE, StatUrlSendStateCallback stateCallback = NULL);
    // 发送带校验的统计URL
    STDMETHOD_(BOOL, SendStatUrlExWithSigned)(LPCWSTR lpszType, LPCWSTR lpszAction, BOOL bUseThread = TRUE, StatUrlSendStateCallback stateCallback = NULL);
    // 发送一个统计URL
    STDMETHOD_(BOOL, SendStatUrl)(LPCWSTR lpszUrl, BOOL bUseThread = TRUE, StatUrlSendStateCallback stateCallback = NULL);

protected:
    // 下载一个文件
    STDMETHOD_(BOOL, DownloadFile)(
        LPCWSTR lpszUrl, 
        LPCWSTR lpszLocalFile, 
        BOOL bUseMultiThread, 
        WPARAM wParam,
        StatUrlSendStateCallback stateCallback);

    // 输出版本格式为：%d.%d.%d.%d
    STDMETHOD_(BOOL, GetFileVer)(LPWSTR lpFileFullPath, LPWSTR lpVer, int nVerlen);

    // 获取MID
    STDMETHOD_(BOOL, GetMID)(
        LPWSTR lpszBuffer, int nMaxCharCount /* 注意，不是缓冲区长度，而是字符个数 */);

    CString GetPID();

public:
    // 是否存在网络连接
    STDMETHOD_(BOOL, HaveInternetConnection)(void);

    // 是否存在网关
    STDMETHOD_(BOOL, IsNetGatewayExists)(void);

private:
    // static 
    static unsigned int WINAPI DownloadThreadProc(LPVOID lpParam);

    void _stdcall OnHttpStatus(IHttpTask *pTask, HTTP_INFO &inf);

private:
    void ReplacePlaceholders(CString & szUrl, const CString & szType, const CString & szAction);

    // 下载一个文件
    BOOL InternalDownloadFile(LPCWSTR lpszUrl, LPCWSTR lpszLocalFile, StatUrlSendStateCallback stateCallback);

private:
    CString m_szPID;
    CString m_szMID;
    CString m_szAppVer;
    CString m_szModVer;
};

