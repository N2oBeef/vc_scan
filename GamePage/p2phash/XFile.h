#ifndef __X_FILE__
#define __X_FILE__

#include <vector>

//#include "XEvent.h"
// 跨平台文件目录操作

namespace XFile
{
	// 读写文件，偏移量是从文件头开始. 返回实际读写的字节数。
	int file_read(CString strFilename, __int64 nMoveOffset, void *pBuf, int nToRead);
	int file_write(CString strFilename, __int64 nMoveOffset, bool bCreate, void *pBuf, int nToWrite, bool bFlush);
	
	// 文本格式
	CStringA file_read_text(CString strFilename);
	int file_read_text_lines(CString strFilename, std::vector<CStringA> &list_line);
	
	// 文件操作
	// 失败也会返回0。注意不能用这个函数来判断文件是否存在。
	__int64  file_get_size(CString strFilename);

#ifdef WIN32
	enum SET_SIZE_ERROR_CODE
	{
		SETSIZE_OK = 0,			// 成功
		SETSIZE_ERROR_DISK_FULL = ERROR_DISK_FULL,	// 磁盘满
		SETSIZE_ERROR_INVALID_NAME = ERROR_INVALID_NAME,	// 非法文件名
		SETSIZE_ERROR_PATH_NOT_FOUND = ERROR_PATH_NOT_FOUND,	// 非法路径名
		SETSIZE_ERROR_SHARING_VIOLATION = ERROR_SHARING_VIOLATION,	// 文件正在被使用
		SETSIZE_ERROR_LOCK_VIOLATION = ERROR_LOCK_VIOLATION,	// 文件正在被使用
		SETSIZE_ERROR_ACCESS_DENIED = ERROR_ACCESS_DENIED,	// 无法写入数据
		SETSIZE_ERROR_USER_CANCEL = -1234,			// 用户取消

	};
#else

	// LINUX 定义
	enum SET_SIZE_ERROR_CODE
	{
		SETSIZE_OK = 0,			// 成功
		SETSIZE_ERROR_DISK_FULL = 1,	// 磁盘满
		SETSIZE_ERROR_INVALID_NAME = 2,	// 非法文件名
		SETSIZE_ERROR_PATH_NOT_FOUND = 3,	// 非法路径名
		SETSIZE_ERROR_SHARING_VIOLATION = 4,	// 文件正在被使用
		SETSIZE_ERROR_LOCK_VIOLATION = 5,	// 文件正在被使用
		SETSIZE_ERROR_ACCESS_DENIED = 6,	// 无法写入数据
		SETSIZE_ERROR_USER_CANCEL = -1234,			// 用户取消
	};

#endif
	// 返回enum SET_SIZE_ERROR_CODE的错误码
	// 如果文件不存在，会重新生成，
	// 如果文件存在，则调整原有的大小，不改写原文件已有的内容
	// hStopEvent 是外界提供的停止事件，可以为NULL
	// pfHashPercent 是外界提供的进度信息，可以为NULL
	//int file_set_size(CString strFilename, __int64 nSize, XEVENT_HANDLE hStopEvent, float *pfHashPercent);



	bool file_exist(CString strFilename);
	bool file_delete(CString strFilename);
	bool file_copy(CString strFrom, CString strTo);
	// 返回错误码，0表示成功
	int  file_move(CString strFrom, CString strTo);


	// 获取文件属性，大小和最后修改时间。windows下时间实质为FILETIME, linux下是 time_t
	bool file_get_attrib(CString strFilename, __int64 &file_size, __int64 &last_write);

	// 获取文件时间的字符串，win下是 FILETIME，linux下是time_t
	// 由于历史原因，p2p共享列表用的是 FILETIME，所以没法修改。
	CStringA file_time_string(__int64 nFiletime);

	// 设置文件最后修改时间。win和linux下都是time_t
	bool file_set_last_modify(CString strFilename, time_t nFiletime);
	
	// 目录操作
	bool dir_create(CString strDir, BOOL bNeedRecursive);

	// 目录正规化。根据操作系统，如果末尾不是 \ / 则加上 \ / 等处理
	bool dir_normalize(CString &strDir);

	bool dir_exist(CString strDir);

	// 路径合并
	// 因为linux和windows的目录斜杠是不同的，所以要这个函数来通用化处理
	// 如果 strFilename 已经是全路径，则直接使用 strFilename 作为结果
	CString dir_make_path (CString strBaseDir, CString strFilename);

	//删除文件夹（包括子文件夹）
	bool dir_delete(CString strDir);
	
};


// 析构时会自动调用Close，但不自动调用Flush()

#if ( defined(WIN32) || defined(WIN64) )
	typedef HANDLE		XFILEHANDLE;
#else
	typedef int			XFILEHANDLE;
#endif





// 当前这个类，只是用来写日志。
// 因为打开模式、共享模式有很多，这里写很麻烦，所以用定制的。
class XFileEx
{
public:
	XFileEx() { m_file = 0; } ;
	~XFileEx(){ Close(); };

	// 注：bUnicodeText 只会在文件是空的情况下写入unicode文件头
	// 外界必须保证 bUnicodeText 和 Write 写入内容的一致性
	bool OpenLog(CString strFilename, BOOL bAppend, BOOL bAutoFlush, BOOL bUnicodeText);

	bool OpenRead(CString strFilename);
	bool OpenReadWrite(CString strFilename);

	bool Seek(__int64 pos);

	// 返回实际读到的字节数，可能比 nBytes 要小。
	int Read(void *buf, int nBytes);

	int Write(const void *buf, int nBytes);

	void Flush();
	void Close();

	bool IsValid();

private:

	XFILEHANDLE m_file;
};


#endif	// __X_FILE__



