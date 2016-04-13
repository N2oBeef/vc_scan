#pragma once

//c++ call javascript interface
#include <boost/shared_ptr.hpp>

typedef enum _tagEVENT_ID
{
	EVENT_ID_LOCAL_CONFIG_READY = 1, // 本地配置加载完成，网页调用接口获取安装状态
	EVENT_ID_DOWNLOAD_START,         // 开始下载软件，szParam1为下载软件的ids
	EVENT_ID_DOWNLOAD_END,           // 下载完成，nCode为下载状态，大于0下载成功，否则失败，szParam1为下载软件的ids
	EVENT_ID_DELETE,                 // 从下载器里面删掉了，szParam1为下载软件的ids
	EVENT_ID_REFRESH_RANK,           // 更新跑分的排名
}EVENT_ID;

class IJsInterfaceBridge
{
public:
	virtual void OnEvent(int nEventId, int nCode, WCHAR* szParam1, WCHAR *szParam2) = 0;
	virtual void OnProgress(WCHAR * ids, LONGLONG lTotalSize, LONGLONG lDownloadSize, int nStatus) = 0;
	virtual void JsInterface(CComPtr<IDispatch> & spScript) = 0;
};

typedef boost::shared_ptr<IJsInterfaceBridge> JsInterfaceBridgePtr;