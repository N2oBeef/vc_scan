#include "stdafx.h"
#include "ScanPanel.h"
#include <interface/IPage.h>
#include "List/ListItem.h"
#include "Config/Configure.h"
#include "Software/SoftwareMgrWrapper.h"

#include <boost/make_shared.hpp>
#include <boost/bind.hpp>

CScanPanel::CScanPanel(void)
: m_nProblemCount(0)
, m_nTotalCount(0)
, m_nBtnScanFixFunction(FUCNTION_SCAN)
, m_dwCookGpu(0)
, m_dwCookCpu(0)
, m_dwCookMem(0)
{
	m_nPanelID = SCANNING_PANEL;	
}

HRESULT CScanPanel::OnBtnCancel( WORD wNotifyCode, WORD wID, HWND hWndCtl, Isite* site )
{
    switch(m_nBtnScanFixFunction)
	{
	case FUCNTION_SCAN:
		break;
	case FUNCTION_CANCEL_SCAN:
		break;
	case FUNCTION_ONE_KEY_FIX:
		OnBtnFixAll();
		break;
	case FUNCTION_CANCEL_ONEKEY_FIX:
		OnBtnCancelFixAll();		
		break;
	case FUNCTION_CANCLE_SINGLE_FIX:
		OnBtnCancelSingleFix();
		break;
	}
	
	return S_OK;
}

void CScanPanel::InitEvent( void )
{
	if (m_spPanelSite == NULL) return;
	
	DWORD cookie = 0;
	
	// 开始扫描按钮
	IsitePtr spBtnSite = m_spPanelSite->Getitem_by_cookie(IDC_BTN_CANCEL, true);
	ATLASSERT(spBtnSite);
	if (spBtnSite)
	{
		AtlAdvise(spBtnSite, (Isite_events*)this, DIID_Isite_events, &cookie);
	}

	AtlAdvise(m_spBtnMoreGames, (Isite_events*)this, DIID_Isite_events, &cookie);	

	Istatic_sitePtr spCpuPercent = CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_CPU_PERCENT);
	ATLASSERT(spCpuPercent);
	AtlAdvise(spCpuPercent, (Isite_events*)this, DIID_Isite_events, &m_dwCookCpu);

	spCpuPercent = CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_GPU_PERCENT);
	ATLASSERT(spCpuPercent);
	AtlAdvise(spCpuPercent, (Isite_events*)this, DIID_Isite_events, &m_dwCookGpu);

	spCpuPercent = CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_MEM_PERCENT);
	ATLASSERT(spCpuPercent);
	AtlAdvise(spCpuPercent, (Isite_events*)this, DIID_Isite_events, &m_dwCookMem);
}

void  __stdcall CScanPanel::OnPanelActivate( const CRect &rct )
{
	BOOL bFirstCreate = (m_spPanelSite == NULL);

	CPanelBase::OnPanelActivate(rct);

	// 事件只能挂接一次
	if (bFirstCreate)
	{	
		InitCtrl();
		m_siteRes.Init(m_szXmlFile);

		InitEvent();
		UpdateBenchRank();
		
		Engine::CEngine::GetInstance()->Init();
	}
	
	if (m_nBtnScanFixFunction == FUCNTION_SCAN )
	{
		Engine::CEngine::GetInstance()->ScanAsync(this);
		m_nBtnScanFixFunction = FUNCTION_CANCEL_SCAN;
	}
}

void __stdcall CScanPanel::UpdateBenchRank(void)
{
	int nLevel = CConfigure::GetInstance()->GetCpuRank();
	Istatic_sitePtr spCpuLevel = CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_CPU_LEVEL);
	ATLASSERT(spCpuLevel);
	CString szLevle = GetLevel(nLevel);
	spCpuLevel->set_text(_bstr_t(szLevle));
	spCpuLevel->repaint(FALSE);
	
	CString szPercen;
	CString szShow;
	Istatic_sitePtr spCpuPercent = CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_CPU_PERCENT);
	ATLASSERT(spCpuPercent);
	if (nLevel > 0)
	{
		szPercen.Format(L"%d%%", nLevel);
		szShow.Format(m_siteRes.LoadStringById(L"IDS_PERFORMANCE_PERCENT"), szPercen);
		AtlUnadvise(spCpuPercent, DIID_Isite_events, m_dwCookCpu);
	}
	else
	{
		szShow = m_siteRes.LoadStringById(L"IDS_PERFORMANCE_NO_SCORE");
	}
	spCpuPercent->set_text(_bstr_t(szShow));
	spCpuPercent->repaint(FALSE);

	nLevel = CConfigure::GetInstance()->GetGpuRank();
	Istatic_sitePtr spGpuLevel = CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_GPU_LEVEL);
	ATLASSERT(spGpuLevel);
	szLevle = GetLevel(nLevel);
	spGpuLevel->set_text(_bstr_t(szLevle));
	spGpuLevel->repaint(FALSE);
	Istatic_sitePtr spGpuPercent = CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_GPU_PERCENT);
	ATLASSERT(spGpuPercent);

	if (nLevel > 0)
	{
		szPercen.Format(L"%d%%", nLevel);
		szShow.Format(m_siteRes.LoadStringById(L"IDS_PERFORMANCE_PERCENT"), szPercen);
		AtlUnadvise(spGpuPercent, DIID_Isite_events, m_dwCookGpu);
	}
	else
	{
		szShow = m_siteRes.LoadStringById(L"IDS_PERFORMANCE_NO_SCORE");
	}
	spGpuPercent->set_text(_bstr_t(szShow));
	spGpuPercent->repaint(FALSE);


	nLevel = CConfigure::GetInstance()->GetMemRank();
	Istatic_sitePtr spMemLevel = CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_MEM_LEVEL);
	ATLASSERT(spGpuLevel);
	szLevle = GetLevel(nLevel);
	spMemLevel->set_text(_bstr_t(szLevle));
	spMemLevel->repaint(FALSE);

	Istatic_sitePtr spMemPercent = CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_MEM_PERCENT);
	ATLASSERT(spMemPercent);

	if (nLevel > 0)
	{
		szPercen.Format(L"%d%%", nLevel);
		szShow.Format(m_siteRes.LoadStringById(L"IDS_PERFORMANCE_PERCENT"), szPercen);
		AtlUnadvise(spMemPercent, DIID_Isite_events, m_dwCookMem);
	}
	else
	{
		szShow = m_siteRes.LoadStringById(L"IDS_PERFORMANCE_NO_SCORE");
	}
	spMemPercent->set_text(_bstr_t(szShow));
	spMemPercent->repaint(FALSE);
}

CString CScanPanel::GetLevel(int nPercen)
{
	if (nPercen >= 70)
	{
		return m_siteRes.LoadStringById(L"IDS_GOOD");
	}
	if (nPercen >= 30)
	{
		return m_siteRes.LoadStringById(L"IDS_BETTER");
	}
	if (nPercen == 0)
	{
		return m_siteRes.LoadStringById(L"IDS_NO_SCORE");
	}
	return m_siteRes.LoadStringById(L"IDS_SO_SO");
}

void CScanPanel::InitCtrl( void )
{
	Irich_list_sitePtr spList =CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_RICH_LIST);
	ATLASSERT(spList);
	m_list.Init(spList);

	m_spProgress = CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_PROGRESS);
	ATLASSERT(m_spProgress);
	m_spProgress->set_value(0);
	m_rctScanProgress = m_spProgress->get_bound();
	m_rctFixProgress = m_rctScanProgress;
	m_rctFixProgress.OffsetRect(0, -19);

	m_spListHead = CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_LIST_HEAD);
	ATLASSERT(m_spListHead);

	m_spFirstLine = CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_TITLE_FIRST);
	ATLASSERT(m_spFirstLine);
	m_rctFirstLine = m_spFirstLine->get_bound();

	m_spSecondLine = CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_TITLE_SECOND);'
	ATLASSERT(m_spSecondLine);
	m_rctSecondLine = m_spSecondLine->get_bound();

	m_spScanFixBtn = CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_BTN_CANCEL);
	ATLASSERT(m_spScanFixBtn);
	m_rctBtnCancel = m_spScanFixBtn->get_bound();
	
	m_rctBtnFix = m_rctBtnCancel;
	m_rctBtnFix.left = m_rctBtnFix.right - 134;
	m_rctBtnFix.bottom = m_rctBtnFix.top + 40;

	m_spBtnMoreGames = CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_BTN_MORE_GAMES);
	ATLASSERT(m_spBtnMoreGames);

	m_spPicRotating = CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_PIC_ROTATE);
	ATLASSERT(m_spPicRotating);
	CSiteUIHelper::HideSiteUI(m_spPicRotating);
}

void CScanPanel::AddToList(Engine::IItem *pEngineItem)
{
	boost::shared_ptr<CListItem> spItem = boost::make_shared<CListItem>(m_list.GetTemplateItem());
	spItem->Init(pEngineItem, &m_siteRes);
	spItem->SetList(&m_list);
	spItem->FixSingleItemFunction(boost::bind(&CScanPanel::OnBtnFixSingleItem, this, _1));
	
	if (pEngineItem->HasProblem())
	{
		m_list.InsertItem(0, spItem); 
	}
	else
	{
		m_list.InsertItem(-1, spItem);
	}

	m_list.UpdateLayout();
}

//////////////////////////////////////////////////////////////////////////
//

void __stdcall CScanPanel::OnScanStart( void )
{   
	m_list.Clear();
	CSiteUIHelper::ShowSiteUI(m_spProgress);
	m_spProgress->set_bound(&m_rctScanProgress, FALSE);
	m_spPanelSite->repaint(FALSE);
	m_nTotalCount = 0;
	m_nProblemCount = 0;

	//title
	CString szFisrtLine = m_siteRes.LoadStringById(L"IDS_SCAN_TITLE");
	m_spFirstLine->set_text(_bstr_t(szFisrtLine));
	m_spFirstLine->set_bound(&m_rctFirstLine, FALSE);
	m_spFirstLine->repaint(FALSE);

	CSiteUIHelper::DisableSiteUI(m_spBtnMoreGames);

	SetBtnCancel(TRUE);
}

void __stdcall CScanPanel::OnScanItemStart( Engine::IItem *pItem )
{
	if (pItem == NULL) return;
	
	WCHAR szName[MAX_PATH] = {};
	pItem->GetName(szName, MAX_PATH);

	CString szNameShow;
	CString szRaw = m_siteRes.LoadStringById(L"IDS_SANNING_NAME_FTM");
	szNameShow.Format(szRaw, szName);
	m_spSecondLine->set_text(_bstr_t(szNameShow));
	m_spSecondLine->repaint(FALSE);
}

void __stdcall CScanPanel::OnScanItemEnd( Engine::IItem *pItem )
{
	LOG(L"OnScanItemEnd");

	if (pItem == NULL) return;

	AddToList(pItem);
	m_nTotalCount++;

	if (pItem->HasProblem())
	{
		m_nProblemCount++;

		if (m_nProblemCount == 1)
		{
			UpdateStatusIcon(TRUE);
		}
	}

	UpdateListHead();
}

void CScanPanel::UpdateStatusIcon(BOOL bError)
{
	Ipicture_sitePtr spTitleIcon = CSiteUIHelper::FindSitePtr(m_spPanelSite, IDC_PIC_STATUS);
	ATLASSERT(spTitleIcon);
	spTitleIcon->load_image_by_id(m_siteRes.GetResId(bError ? L"IDI_START_STAT_ERROR" : L"IDI_START_STAT_NORMAL"));
	spTitleIcon->repaint(FALSE);
}

void CScanPanel::UpdateListHead(void)
{
	CString szListHead;
	CString szListHeadFmt = m_siteRes.LoadStringById(m_nProblemCount ? L"IDS_SCAN_LIST_HEAD_PROBLEM" : L"IDS_SCAN_LIST_HEAD_OK");
	if (m_nProblemCount)
	{
		szListHead.Format(szListHeadFmt, m_nTotalCount, m_nProblemCount);
	}
	else
	{
		szListHead.Format(szListHeadFmt, m_nTotalCount);
	}
	m_spListHead->set_text(_bstr_t(szListHead));
	m_spListHead->repaint(FALSE);
}

void __stdcall CScanPanel::OnScanProgress( int nProgress )
{
	if (m_spProgress)
	{
		m_spProgress_->set_value(nProgress);
		m_spProgress->repaint(FALSE);
	}
	LOG(L"OnScanProgress: %d", nProgress);
}

void __stdcall CScanPanel::OnScanEnd( void )
{
	LOG(L"OnScanEnd");

	CSiteUIHelper::HideSiteUI(m_spProgress);
	CSiteUIHelper::EnableSiteUI(m_spBtnMoreGames);

	CConfigure::GetInstance()->SetExamined();
	CConfigure::GetInstance()->SetUnfixProblem(m_nProblemCount);

	if (m_nProblemCount > 0)
	{
		SetTopTitleText();
		SetBtnCancel(FALSE);

		m_nBtnScanFixFunction = FUNCTION_ONE_KEY_FIX;
	}
	else
	{
		m_nBtnScanFixFunction = FUCNTION_SCAN;
		m_pPage->SetCurrentPanel(GAMES_PANEL);
	}
}

void CScanPanel::SetTopTitleText(void)
{
	CString szFisrtLine = m_siteRes.LoadStringById(L"IDS_SCAN_FINISH_PROBLEM_TITLE");

	m_spFirstLine->set_text(_bstr_t(szFisrtLine));
	CRect rctFirst = m_rctFirstLine;
	rctFirst.OffsetRect(0, 10);
	m_spFirstLine->set_bound(&rctFirst, FALSE);
	m_spFirstLine->repaint(FALSE);

	CString szSecondLine = m_siteRes.LoadStringById(L"IDS_SCAN_FINISH_PROBLEM_SCOND_LINE");
	m_spSecondLine->set_text(_bstr_t(szSecondLine));
	CRect rctSecond = m_rctSecondLine;
	rctSecond.OffsetRect(0, 10);
	m_spSecondLine->set_bound(&rctSecond, FALSE);
	m_spSecondLine->repaint(FALSE);
}

//
//////////////////////////////////////////////////////////////////////////


HRESULT CScanPanel::OnBtnMoreGames( WORD wNotifyCode, WORD wID, HWND hWndCtl, Isite* site )
{
	m_nBtnScanFixFunction = FUCNTION_SCAN;
	m_pPage->SetCurrentPanel(GAMES_PANEL);

	return 0;
}

void CScanPanel::OnBtnCancelFixAll(void)
{
	Engine::CEngine::GetInstance()->CancelFix();
}

void CScanPanel::OnBtnCancelSingleFix(void)
{
	Engine::CEngine::GetInstance()->CancelFix();
}

void CScanPanel::OnBtnFixAll(void)
{
	m_nBtnScanFixFunction = FUNCTION_CANCEL_ONEKEY_FIX;
	Engine::CEngine::GetInstance()->FixAllAsync(this);
}

void CScanPanel::OnBtnFixSingleItem( IItem *pItem )
{
	ATLASSERT(pItem);

	m_nBtnScanFixFunction = FUNCTION_CANCLE_SINGLE_FIX;
	Engine::IItem *pEnginItem = pItem->GetEngineItem();
	Engine::CEngine::GetInstance()->FixOneAsync(pEnginItem, this);
}

//////////////////////////////////////////////////////////////////////////
//fix callback
void __stdcall CScanPanel::OnFixStart( void )
{
	CSiteUIHelper::ShowSiteUI(m_spPicRotating);

	SetBtnCancel(TRUE);

	CSiteUIHelper::DisableSiteUI(m_spBtnMoreGames);
	CSiteUIHelper::HideSiteUI(m_spSecondLine);
	CSiteUIHelper::ShowSiteUI(m_spProgress);
	m_spProgress->set_value(0);
	m_spProgress->set_bound(&m_rctFixProgress, FALSE);

	UpdateStatusIcon(FALSE);

	m_list.Enable(FALSE);
	m_spPanelSite->repaint(FALSE);
}

void __stdcall CScanPanel::OnFixItemStart( Engine::IItem *pItem )
{
	ItemPtr spListItem = m_list.GetItem(pItem);
	if (spListItem)
	{
		spListItem->OnFixItemStart(pItem);
	}

	CSiteUIHelper::EnableSiteUI(m_spScanFixBtn);

	WCHAR szName[MAX_PATH] = {};
	pItem->GetName(szName, MAX_PATH);

	CString szNameShow;
	CString szRaw = m_siteRes.LoadStringById(L"IDS_FIXING_NAME_FMT");
	szNameShow.Format(szRaw, szName);
	m_spFirstLine->set_text(_bstr_t(szNameShow));
	m_spFirstLine->repaint(FALSE);
}

void __stdcall CScanPanel::OnFixItemEnd( Engine::IItem *pItem , BOOL bSuccess)
{
	ItemPtr spListItem = m_list.GetItem(pItem);
	if (spListItem)
	{
		spListItem->OnFixItemEnd(pItem, bSuccess);
	}

	if (bSuccess)
	{
		m_nProblemCount--;
	}

	UpdateListHead();
}

void __stdcall CScanPanel::OnFixProgress( Engine::IItem *pItem, int nProgress )
{
	//fix
	ItemPtr spListItem = m_list.GetItem(pItem);
	if (spListItem)
	{
		spListItem->OnFixProgress(pItem, nProgress);
	}

	m_spProgress->set_value(nProgress);
	m_spProgress->repaint(FALSE);
}

void __stdcall CScanPanel::OnFixEnd( void )
{
	CSiteUIHelper::HideSiteUI(m_spPicRotating);
	CSiteUIHelper::HideSiteUI(m_spProgress);
	CSiteUIHelper::ShowSiteUI(m_spSecondLine);

	CSiteUIHelper::EnableSiteUI(m_spBtnMoreGames);
	m_list.Enable(TRUE);

	CConfigure::GetInstance()->SetUnfixProblem(m_nProblemCount);
	if (m_nProblemCount <= 0)
	{
		m_nBtnScanFixFunction = FUCNTION_SCAN;
		m_pPage->SetCurrentPanel(GAMES_PANEL);
	}
	else
	{
		SetTopTitleText();
		SetBtnCancel(FALSE);
		UpdateStatusIcon(TRUE);
		m_nBtnScanFixFunction = FUNCTION_ONE_KEY_FIX;
	}

	m_spPanelSite->repaint(FALSE);
}

void __stdcall CScanPanel::OnFixEvent( Engine::IItem *pItem, int nEvent )
{
	if (nEvent == Engine::FIX_EVENT_START_INST)
	{
		CSiteUIHelper::DisableSiteUI(m_spScanFixBtn);
		m_spScanFixBtn->repaint(FALSE);
	}
}

//
//////////////////////////////////////////////////////////////////////////

void CScanPanel::SetBtnCancel( BOOL bCancel )
{
	CSiteUIHelper::EnableSiteUI(m_spScanFixBtn);
	
	if (bCancel)
	{
		unsigned long uImg = m_siteRes.GetResId(L"IDI_BTN_CANCEL");
		m_spScanFixBtn->set_bitmap(uImg);
		m_spScanFixBtn->set_bound(&m_rctBtnCancel, FALSE);
		m_spScanFixBtn->repaint(FALSE);
	}
	else
	{
		unsigned long uImg = m_siteRes.GetResId(L"IDI_BTN_ONE_KEY_FIX");
		m_spScanFixBtn->set_bitmap(uImg);
		m_spScanFixBtn->set_bound(&m_rctBtnFix, FALSE);
		m_spScanFixBtn->repaint(FALSE);
	}

	m_spPanelSite->repaint(FALSE);
}

HRESULT CScanPanel::OnLinkPerformance( WORD wNotifyCode, WORD wID, HWND hWndCtl, Isite* site )
{   
	WCHAR szExe[MAX_PATH] = {};
	GetModuleFileName(NULL, szExe, MAX_PATH);
	
#ifndef _DEBUG
	VerifySignHelper::ShellExecute_qsp(NULL, L"open", szExe, L"/RunHardwarePerformace", NULL, SW_SHOWNORMAL);
#else
	ShellExecute(NULL, L"open", szExe, L"/RunHardwarePerformace", NULL, SW_SHOWNORMAL);
#endif // _DEBUG

	return 0;
}

void __stdcall CScanPanel::UpdateScores( void )
{
	CSoftwareMgrWrapper::GetInstance()->RefreshBenchRank();
}

void __stdcall CScanPanel::UpdateScoresRank( void )
{
	UpdateBenchRank();	
}
