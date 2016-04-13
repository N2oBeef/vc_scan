#pragma once

#include "Base/SingletonT.h"

class CConfigure
	: public CSingletonT<CConfigure>
{
	SINGLETON(CConfigure);
private:
	CConfigure();

public:
	~CConfigure();
	
	//是否体检过
	BOOL HasExamined(void);
	void SetExamined(void);

	//是否还有未修复的问题
	int  GetUnfixProblem(void);
	void SetUnfixProblem(int nCount);

	void CpuScore(int nScore);
	void GpuScore(int nScore);
	void MemSize(int nM);

	int CpuScore(void)
	{
		return m_nCpuScore;
	}

	int GpuScore(void)
	{
		return m_nGpuScore;
	}
	int MemSize(void)
	{
		return m_nMemMSize;
	}

	void CpuRank(int nPercent);
	int GetCpuRank(void);

	void GpuRank(int nPercent);
	int GetGpuRank(void);

	void MemRank(int nPercent);
	int GetMemRank(void);


private:
	BOOL m_bExamined;
	int  m_nUnfixProblem;

	CString m_szConfigFile;

	int m_nCpuScore;
	int m_nGpuScore;
	int m_nMemMSize; //MB

	int m_nCpuPercent;
	int m_nGpuPercent;
	int m_nMemPercent;
};