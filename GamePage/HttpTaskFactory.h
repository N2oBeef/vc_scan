#pragma once
#include "Base/SingletonT.h"
#include "Net/IHttpTask.h"

struct HttpTaskDeletor 
{
    void operator()(IHttpTask *pTask)
    {
        if (pTask)
        {
            pTask->Release();
        }
    }
};

class CHttpTaskFactory
    : public CSingletonT<CHttpTaskFactory>
{
    SINGLETON(CHttpTaskFactory);

public:
    ~CHttpTaskFactory(void);

    IHttpTask* CreateTask();

protected:
    void Init360Net(void);

protected:
    CHttpTaskWrapper m_httpTaskWrapper;
    BOOL m_bInit;

private:
    CHttpTaskFactory(void);
};
