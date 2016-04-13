#pragma once

#include "ISoftwareProxy.h"

#include <boost/shared_ptr.hpp>


class ISoftwareMgrWrapper
{
public:
	virtual SoftwareProxyPtr GetSofteware(const WCHAR * pszIDS) = 0;
	virtual SoftwareProxyPtr CreateSofteware(const WCHAR * pszIDS) = 0;
	virtual void Download(const WCHAR * szUrls) = 0;
	virtual IAnimationPtr CreateAnimation(const WCHAR * szUrl, long *pID) = 0;

	virtual void GetDownMgrIconPos(LONG & x, long & y) = 0;
	virtual HWND GetMainWnd(void) = 0;
	virtual BOOL CheckDownDir(ULONGLONG llTotalSize) = 0;
};

typedef boost::shared_ptr<ISoftwareMgrWrapper> SoftwareMgrWrapperPtr;