#include "stdafx.h"
#include "Configure.h"

CConfigure::CConfigure()
: m_bExamined(FALSE)
, m_nUnfixProblem(0)
, m_nCpuScore(0)
, m_nGpuScore(0)
, m_nMemMSize(0)
, m_nCpuPercent(0)
, m_nGpuPercent(0)
,m_nMemPercent(0)
{
	WCHAR szFile[MAX_PATH] = {};
	GetModuleFileName(g_hModule, szFile, MAX_PATH);
	PathRemoveFileSpec(szFile);
	PathRemoveFileSpec(szFile);
	PathAppend(szFile, L"Computerz.set");

	m_szConfigFile = szFile;
}

CConfigure::~CConfigure()
{
}

BOOL CConfigure::HasExamined( void )
{
	m_bExamined = GetPrivateProfileInt(L"GamePage", L"HasExamined", 0, m_szConfigFile) > 0;
	return m_bExamined;
}

int CConfigure::GetUnfixProblem( void )
{
	m_nUnfixProblem = GetPrivateProfileInt(L"GamePage", L"Problems", 0, m_szConfigFile);

	return m_nUnfixProblem;
}

void CConfigure::SetExamined( void )
{
	m_bExamined = TRUE;
	WritePrivateProfileString(L"GamePage", L"HasExamined", L"1", m_szConfigFile);
}

void CConfigure::SetUnfixProblem( int nCount )
{
	CString szCount;
	szCount.Format(L"%d" ,nCount);
	WritePrivateProfileString(L"GamePage", L"Problems", szCount, m_szConfigFile);
}

void CConfigure::CpuScore( int nScore )
{
	m_nCpuScore = nScore;
}

void CConfigure::GpuScore( int nScore )
{
	m_nGpuScore = nScore;
}

void CConfigure::MemSize( int nM )
{
	m_nMemMSize = nM;
}

void CConfigure::CpuRank( int nPercent )
{
	m_nCpuPercent = nPercent;
}

int CConfigure::GetCpuRank( void )
{
	return m_nCpuPercent;
}

void CConfigure::GpuRank( int nPercent )
{
	m_nGpuPercent = nPercent;
}

int CConfigure::GetGpuRank( void )
{
	return m_nGpuPercent;
}

void CConfigure::MemRank( int nPercent )
{
	m_nMemPercent = nPercent;
}

int CConfigure::GetMemRank( void )
{
	return m_nMemPercent;
}
