#include "stdafx.h"
#include "ScanCallbackOnUI.h"

#include "Base/UIThreadWindow.h"

using namespace Engine;
Engine::CScanCallbackOnUI::CScanCallbackOnUI()
: m_pScanCallback(NULL)
{

}

Engine::CScanCallbackOnUI::~CScanCallbackOnUI()
{

}

void __stdcall Engine::CScanCallbackOnUI::OnScanStart( void )
{
	RunAsUIThread(&simple_bind(&Engine::CScanCallbackOnUI::OnScanStartOnUI, this));
}

void __stdcall Engine::CScanCallbackOnUI::OnScanItemStart( IItem *pItem )
{
	RunAsUIThread(&simple_bind(&Engine::CScanCallbackOnUI::OnScanItemStartOnUI, this,pItem));
}

void __stdcall Engine::CScanCallbackOnUI::OnScanItemEnd( IItem *pItem )
{
	RunAsUIThread(&simple_bind(&Engine::CScanCallbackOnUI::OnScanItemEndOnUI, this,pItem));
}

void __stdcall Engine::CScanCallbackOnUI::OnScanProgress( int nProgress )
{
	RunAsUIThread(&simple_bind(&Engine::CScanCallbackOnUI::OnScanProgressOnUI, this,nProgress));
}

void __stdcall Engine::CScanCallbackOnUI::OnScanEnd( void )
{
	RunAsUIThread(&simple_bind(&Engine::CScanCallbackOnUI::OnScanEndOnUI, this));
}

//////////////////////////////////////////////////////////////////////////
void  Engine::CScanCallbackOnUI::OnScanStartOnUI( void )
{
	if (m_pScanCallback)
	{
		m_pScanCallback->OnScanStart();
	}
}

void Engine::CScanCallbackOnUI::OnScanItemStartOnUI( IItem *pItem )
{
	if (m_pScanCallback)
	{
		m_pScanCallback->OnScanItemStart(pItem);
	}
}

void Engine::CScanCallbackOnUI::OnScanItemEndOnUI( IItem *pItem )
{
	if (m_pScanCallback)
	{
		m_pScanCallback->OnScanItemEnd(pItem);
	}
}

void Engine::CScanCallbackOnUI::OnScanProgressOnUI( int nProgress )
{
	if (m_pScanCallback)
	{
		m_pScanCallback->OnScanProgress(nProgress);
	}
}

void Engine::CScanCallbackOnUI::OnScanEndOnUI( void )
{
	if (m_pScanCallback)
	{
		m_pScanCallback->OnScanEnd();
	}
}

