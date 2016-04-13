#include "stdafx.h"
#include "FixCallbackOnUI.h"

#include "Base/UIThreadWindow.h"

using namespace Engine;
Engine::CFixCallbackOnUI::CFixCallbackOnUI()
: m_pFixCallback(NULL)
{

}

Engine::CFixCallbackOnUI::~CFixCallbackOnUI()
{

}

void __stdcall Engine::CFixCallbackOnUI::OnFixStart( void )
{
    RunAsUIThread(&simple_bind(&Engine::CFixCallbackOnUI::OnFixStartOnUI, this));
}

void __stdcall Engine::CFixCallbackOnUI::OnFixItemStart( IItem *pItem )
{
    RunAsUIThread(&simple_bind(&Engine::CFixCallbackOnUI::OnFixItemStartOnUI, this,pItem));
}

void __stdcall Engine::CFixCallbackOnUI::OnFixItemEnd( IItem *pItem , BOOL bSuccess)
{
    RunAsUIThread(&simple_bind(&Engine::CFixCallbackOnUI::OnFixItemEndOnUI, this,pItem, bSuccess));
}

void __stdcall Engine::CFixCallbackOnUI::OnFixProgress( IItem *pItem, int nProgress )
{
    RunAsUIThread(&simple_bind(&Engine::CFixCallbackOnUI::OnFixProgressOnUI, this,pItem, nProgress));
}

void __stdcall Engine::CFixCallbackOnUI::OnFixEnd( void )
{
    RunAsUIThread(&simple_bind(&Engine::CFixCallbackOnUI::OnFixEndOnUI, this));
}
void __stdcall Engine::CFixCallbackOnUI::OnFixEvent( IItem *pItem, int nEvent )
{
	RunAsUIThread(&simple_bind(&Engine::CFixCallbackOnUI::OnFixEventOnUI, this, pItem, nEvent));
}

//////////////////////////////////////////////////////////////////////////
void  Engine::CFixCallbackOnUI::OnFixStartOnUI( void )
{
    LOG(L"OnFixStart\r\n");

    if (m_pFixCallback)
    {
        m_pFixCallback->OnFixStart();
    }
}

void Engine::CFixCallbackOnUI::OnFixItemStartOnUI( IItem *pItem )
{
    if (pItem)
    {
        WCHAR szName[1024] = {};
        pItem->GetName(szName, _countof(szName));
        LOG(L"OnFixItemStart name:%s\r\n", szName);
    }

    if (m_pFixCallback)
    {
        m_pFixCallback->OnFixItemStart(pItem);
    }
}

void Engine::CFixCallbackOnUI::OnFixItemEndOnUI( IItem *pItem , BOOL bSuccess)
{
    if (pItem)
    {
        WCHAR szName[1024] = {};
        pItem->GetName(szName, _countof(szName));
        LOG(L"OnFixItemEnd name:%s, bSuc:%d\r\n", szName, bSuccess);
    }

    if (m_pFixCallback)
    {
        m_pFixCallback->OnFixItemEnd(pItem, bSuccess);
    }
}

void Engine::CFixCallbackOnUI::OnFixProgressOnUI(IItem *pItem, int nProgress )
{
    if (pItem)
    {
        WCHAR szName[1024] = {};
        pItem->GetName(szName, _countof(szName));
        LOG(L"OnFixProgress name:%s, nProgress:%d\r\n", szName, nProgress);
    }

    if (m_pFixCallback)
    {
        m_pFixCallback->OnFixProgress(pItem, nProgress);
    }
}

void Engine::CFixCallbackOnUI::OnFixEndOnUI( void )
{
    LOG(L"OnFixEnd\r\n");

    if (m_pFixCallback)
    {
        m_pFixCallback->OnFixEnd();
    }
}

void Engine::CFixCallbackOnUI::OnFixEventOnUI( IItem *pItem, int nEvent )
{
    if (pItem)
    {
        WCHAR szName[1024] = {};
        pItem->GetName(szName, _countof(szName));
        LOG(L"OnFixEvent name:%s, nEvent:%d\r\n", szName, nEvent);
    }

    if (m_pFixCallback)
    {
        m_pFixCallback->OnFixEvent(pItem, nEvent);
    }	
}

