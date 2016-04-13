#pragma once

//interface for javascript to call
#include <boost/function.hpp>
typedef boost::function<void(void)> SimpleFunction;

typedef enum _tagSofewareStatus
{
	SOFTWARE_STATUS_NOT_INSTALL = 1,
	SOFTWARE_STATUS_DOWNLOADING,  
	SOFTWARE_STATUS_DOWNLOAD_FAILED,
	SOFTWARE_STATUS_INSTALLED,
}SOFTWARE_STATUS;

typedef struct {
    HKEY hKey;
    CString szSubKey;
    CString szValue;
    CString szExe;
    BOOL bIsX64;
}REG_GROUP;

typedef std::vector<REG_GROUP> REG_INFO;

class ICppInterfaceBridge
{
public:
	virtual void Install(DISPPARAMS *pDispParams,VARIANT *pVarResult) PURE;
	virtual void Download(DISPPARAMS *pDispParams,VARIANT *pVarResult) PURE;
	virtual void GetStatus(DISPPARAMS *pDispParams,VARIANT *pVarResult) PURE;
	virtual void FlyIcon(DISPPARAMS *pDispParams,VARIANT *pVarResult) PURE;
	virtual void OnWebReady(DISPPARAMS *pDispParams,VARIANT *pVarResult) PURE;
};


