// 通用HTTP上传下载封装
//
// 支持特性：HTTP/HTTPS，表单提交上传，Cookies，自定义HTTP头，代理认证与密码保存，
//           网络异常处理，重定向，重试，无文件大小的下载，防卡死，诊断日志。
//           支持vc9/vc6


#pragma once
#include <vector>
#include <windows.h>
#include <cassert>

#define ASSERT(expr) assert(expr)


enum HttpStatus
{
	eHttpWait		= 0,			// 排队或未开始的状态
	eHttpRunning	= 1,			// 正在运行
	eHttpSuccess	= 2,			// 成功
	eHttpFail		= 3,			// 失败
};

enum HttpMethod
{
	eHttpGet		= 0,			// GET
	eHttpPostSimple	= 1,			// POST 简单数据 -- form-urlencoded
	eHttpPost		= 2,			// POST 表单数据 -- multipart/form-data
	eHttpPostRaw	= 3,			// POST 原始数据 -- octet-stream
};
struct HTTP_INFO
{
	HTTP_INFO()
	{
		memset(this, 0, sizeof(HTTP_INFO));
	}
	
	DWORD		nID;				// 本任务id
	HttpStatus 	nStatus;			// 状态

	DWORD		dwHttpCode;			// HTTP 状态码，供调试
	DWORD		dwErrCode;			// 错误码

	const char	*pHttpData;			// 下载到内存模式的数据，支持二进制，尾部有\0便于外界字符串分析。这个\0不算在dwHttpDataLen内
	const wchar_t*pHttpFile;		// 下载到文件模式的文件路径
	DWORD		dwHttpDataLen;		// 收到的数据大小
	BOOL		bDataCompress;		// 收到的数据是否是压缩的格式(gzip)

	const char *pHeaderSend;		// 原始请求头，供调试
	const char *pHeaderRecv;		// 原始响应头，供调试

	DWORD		dwTime;				// 已用时间, 毫秒
	DWORD		dwAvgSpeed;			// 平均速度 KB/s
	char		ip[64];				// 连接的ip


	const char *szUrl;				// 传入的参数，供调试
	HttpMethod	nMethod;

	DWORD		dwHttpDataCntLen;	// 收到数据的预期大小(类似content-length)
	DWORD		dwProgress;			// 进度(0~100)
	DWORD		dwCurSpeed;			// 瞬时速度 KB/s


	char		reserved[1024];
};

// 回调。注意回调函数是从内部网络线程内回调的，外界请考虑线程同步的问题
// 不使用回调也可以，还可以走查询模式
class IHttpTask;

class IHttpTaskCallback
{
public:
	// 状态回调
	virtual void _stdcall OnHttpStatus(IHttpTask *pTask, HTTP_INFO &inf) = 0;
};

#define IHTTP_ERR_USER_CANCEL	50001			// 用户取消 -- 无结果之前调用删除（现在无回调了）
#define IHTTP_ERR_TIMEOUT		50002			// 超时

class IHttpTask
{
public:
	// 设置回调指针。不用此函数表示不回调。传入NULL也是取消回调
	virtual void SetCallback(IHttpTaskCallback *pCallback) = 0;

	// 开始执行请求（非阻塞）。注意设置请求头等参数应该在调用本函数前设置完。
	virtual BOOL Start(HttpMethod nMethod, const char *szUrl) = 0;
	virtual BOOL Start(HttpMethod nMethod, const wchar_t *szUrl) = 0;

	// 暂停并删除任务（非阻塞），删除任务后，不要再用此指针。
	virtual BOOL Release() = 0;
	
	// 添加自定义HTTP头。默认User-Agent是IE7
	// AddHeader("Cookie", "GP=1; FG=1; USERID=77992fd2f44dfb5701d6286e; LPA=2");
	// AddHeader("User-Agent", "uploader/1.1");
	// AddHeader("Referer", "http://www.360.cn/")
	virtual BOOL AddHeader(const wchar_t *szName, const wchar_t *szValue) = 0;
	virtual BOOL AddHeader(const char *szName, const char *szValue) = 0;

	// POST 提交时，增加字符串字段
	virtual BOOL PostStr(const wchar_t *szName, const wchar_t *szValue) = 0;
	virtual BOOL PostStr(const char *szName, const char *szValue) = 0;
	
	// POST 提交时，增加二进制字段
	// szFilename 为可选字段，不为空则为本字段增加 filename 属性
	// szContentType 为可选字段，不为空则为本字段增加 Content-Type 属性
	virtual BOOL PostBin(const char *szName, const BYTE *pData, DWORD dwDataLen, const char *szFilename, const char *szContentType) = 0;
	virtual BOOL PostBin(const wchar_t *szName, const BYTE *pData, DWORD dwDataLen, const wchar_t *szFilename, const wchar_t *szContentType) = 0;

	// 支持下载到文件，对于下载超大文件的产品适用，如果设置了此参数，pHttpData就是空的
	virtual void SetFilename(const wchar_t *lpszFilename) = 0;

	// 设置参数。对于能识别的参数，返回TRUE
	// SetParam("timeout", 15); // 设置超时。超时是指无数据交互，不是整个请求的最长执行时间. 默认30秒
	// SetParam("retry", 2);	// 设置失败自动重试。默认不重试。只有重试完毕才产生callback
	// SetParam("zlib", 1);		// 设置向服务器post时，对内容做zlib的compress2()压缩
	virtual BOOL SetParam(const char *szName, DWORD dwValue) = 0;
	virtual BOOL SetParam(const char *szName, const char *szValue) = 0;
	
	// 获取状态。如果用回调模式，则无需调用此函数，所需的变量在callback的参数有
	virtual BOOL GetStatus(HTTP_INFO &st) = 0;	

	// 获取一条响应头。如果有多行的比如Set-Cookie，dwIndex从0开始查起，直到返回FALSE表示没有了
	virtual BOOL GetRespHeader(const char *szName, char *szValue, DWORD &sizeInChars, DWORD dwIndex) = 0;
	virtual BOOL GetRespHeader(const wchar_t *szName, wchar_t *szValue, DWORD &sizeInChars, DWORD dwIndex) = 0;

	// 获取本任务ID，创建任务后可立即拿到ID。对于同一个IHttpTaskManager内的任务ID序号是唯一的，从1开始
	virtual DWORD GetID() = 0;
};

class IHttpTaskManager
{
public:
	virtual IHttpTask* CreateTask() = 0;

	// 设置选项。对于能识别的字段，返回TRUE
	// SetParam("speed_limit_up", 100);			// 上传限速，单位KB/s，默认0表示不限。只对本管理器有效。
	// SetParam("speed_limit_down", 100);		// 下载限速，单位KB/s，默认0表示不限。
	virtual BOOL SetParam(const char *szName, DWORD dwValue) = 0;
	virtual BOOL SetParam(const char *szName, const char *szValue) = 0;

	// 获取状态。对于能识别的字段，返回TRUE
	// GetParam("speed_up", dwValue);			// 获取当前总上传速度。指一个管理器的总速度
	// GetParam("speed_down", dwValue);			// 获取当前总下载速度
	virtual BOOL GetParam(const char *szName, DWORD *dwValue) = 0;
	virtual BOOL GetParam(const char *szName, const char *szValue, int nValueLen) = 0;
};

// 初始化模块
extern "C"  IHttpTaskManager* __stdcall HttpInit();
typedef IHttpTaskManager* ( __stdcall *pFUNC_HttpInit )();

// 反初始化模块
extern "C"  BOOL __stdcall HttpUninit(IHttpTaskManager* pMgr);
typedef BOOL ( __stdcall *pFUNC_HttpUninit )(IHttpTaskManager* pMgr);

// 以下zlib导出接口，参数含义与返回值与zlib一致。唯一区别是compress采用最高压缩比
extern "C"  int __stdcall zlib_uncompress (BYTE *dest, unsigned long *destLen, const BYTE *source, unsigned long sourceLen);
typedef int ( __stdcall *pFUNC_zlib_uncompress )(BYTE *dest, unsigned long *destLen, const BYTE *source, unsigned long sourceLen);

extern "C"  int __stdcall zlib_compress (BYTE *dest, unsigned long *destLen, const BYTE *source, unsigned long sourceLen);
typedef int ( __stdcall *pFUNC_zlib_compress )(BYTE *dest, unsigned long *destLen, const BYTE *source, unsigned long sourceLen);

extern "C"  unsigned long __stdcall zlib_compressBound (unsigned long sourceLen);
typedef unsigned long ( __stdcall *pFUNC_zlib_compressBound )(unsigned long sourceLen);

// 这个类用于方便的调用
class CHttpTaskWrapper
{
public:
	CHttpTaskWrapper()
	{
		m_hMoudle	= NULL;
		m_HttpInit = NULL;
		m_HttpUninit = NULL;
		m_pTaskManager = 0;
	}
	~CHttpTaskWrapper()
	{
		UnInit();
	}

	IHttpTask* CreateTask()
	{
		if(m_pTaskManager)
        {
            IHttpTask* pTask = m_pTaskManager->CreateTask();
            if (pTask)
            {
                DWORD dwValue = 0;
                pTask->SetParam("ungzip", dwValue);
                pTask->SetParam("gzip", dwValue);            // 告诉上传模块，所有PostBin的二进制数据，在网络发送时做压缩
            }
            return pTask;
        }
		else
			return NULL;
	}

	BOOL   Init(LPCTSTR dllName)
	{
		if (m_hMoudle == 0)
		{
			m_hMoudle = LoadLibrary(dllName); //_SIGN_OK_
			if (m_hMoudle == NULL)
			{
				return FALSE;
			}

			m_HttpInit = (pFUNC_HttpInit)GetProcAddress(m_hMoudle,"HttpInit");
			m_HttpUninit = (pFUNC_HttpUninit)GetProcAddress(m_hMoudle,"HttpUninit");
			m_zlib_uncompress = (pFUNC_zlib_uncompress)GetProcAddress(m_hMoudle,"zlib_uncompress");
			m_zlib_compress = (pFUNC_zlib_compress)GetProcAddress(m_hMoudle,"zlib_compress");
			m_zlib_compressBound = (pFUNC_zlib_compressBound)GetProcAddress(m_hMoudle,"zlib_compressBound");

			// 内部初始化
			if(m_HttpInit)
				m_pTaskManager = m_HttpInit();
		}

		return (m_HttpUninit && m_HttpInit && m_pTaskManager);
	}
	BOOL	UnInit()
	{
		// 内部反初始化
		if(m_HttpUninit)
			m_HttpUninit(m_pTaskManager);

		if (m_hMoudle)
		{
			m_HttpInit = NULL;
			m_HttpUninit = NULL;
			m_zlib_uncompress = NULL;

			// 靠引用计数去真正释放dll
			FreeLibrary(m_hMoudle);
			m_hMoudle = NULL;
		}

		return TRUE;
	}

	// 对zlib解压做个简单封装
	int UnCompressZlib(const BYTE *source, unsigned long nSourceLen, std::vector<BYTE>& vecDest)
	{
#define Z_OK            0
#define Z_BUF_ERROR    (-5)

		ASSERT(nSourceLen > 0);
		if (nSourceLen <= 0)
		{
			return -1;
		}
		int nRet = -1;
		for (unsigned long nDestLen = nSourceLen * 2; ; nDestLen = nDestLen * 2)
		{
			vecDest.resize(nDestLen);
			nRet = zlib_uncompress(&vecDest[0], &nDestLen, source, nSourceLen);
			if (nRet != Z_BUF_ERROR)
			{
				break;
			}
		}
		return nRet;
	}

	int zlib_uncompress(BYTE *dest, unsigned long *destLen, const BYTE *source, unsigned long sourceLen)
	{
		if(m_zlib_uncompress)
			return m_zlib_uncompress(dest, destLen, source, sourceLen);
		else
			return -1;
	}

	int zlib_compress(BYTE *dest, unsigned long *destLen, const BYTE *source, unsigned long sourceLen)
	{
		if(m_zlib_compress)
			return m_zlib_compress(dest, destLen, source, sourceLen);
		else
			return -1;
	}

	unsigned long zlib_compressBound(unsigned long sourceLen)
	{
		if(m_zlib_compressBound)
			return m_zlib_compressBound(sourceLen);
		else
			return 0;
	}

private:
	HMODULE					m_hMoudle;
	pFUNC_HttpInit			m_HttpInit;
	pFUNC_HttpUninit		m_HttpUninit;
	IHttpTaskManager*		m_pTaskManager;
	pFUNC_zlib_uncompress	m_zlib_uncompress;
	pFUNC_zlib_compress		m_zlib_compress;
	pFUNC_zlib_compressBound	m_zlib_compressBound;
};
