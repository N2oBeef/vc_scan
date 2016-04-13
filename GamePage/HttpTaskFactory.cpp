#include "StdAfx.h"
#include "HttpTaskFactory.h"

CHttpTaskFactory::CHttpTaskFactory( void )
: m_bInit(FALSE)
{

}

CHttpTaskFactory::~CHttpTaskFactory(void)
{
}

IHttpTask* CHttpTaskFactory::CreateTask()
{
    Init360Net();
    return m_httpTaskWrapper.CreateTask();
}

void CHttpTaskFactory::Init360Net( void )
{
    if (m_bInit)
        return;

    WCHAR szComputerZDir[MAX_PATH] = {};
    GetModuleFileName(g_hModule, szComputerZDir, MAX_PATH);
    PathRemoveFileSpec(szComputerZDir);
    PathRemoveFileSpec(szComputerZDir);

    WCHAR szNet[MAX_PATH] = {};
    wcscpy_s(szNet, MAX_PATH, szComputerZDir);
    PathAppend(szNet, L"DrvMgr\\360NetUL.dll");

    if (!VerifySignHelper::Has360Sign(szNet))
    {
        return;
    }

    m_bInit = TRUE;
    m_httpTaskWrapper.Init(szNet);
}
