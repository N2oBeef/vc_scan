#include "stdafx.h"
#include "XDefine.h"
#include "XFile.h"


//  warning C4800: “BOOL”: 将值强制为布尔值“true”或“false”(性能警告)
#pragma warning(disable: 4800)

namespace XFile
{


int file_read(CString strFilename, __int64 nMoveOffset, void *pBuf, int nToRead)  //nToRead means 需要读的字节数?
{
	HANDLE hFile = CreateFile(strFilename, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	LARGE_INTEGER offs;
	offs.QuadPart = nMoveOffset;

	if(nToRead == -1)
	{
		LARGE_INTEGER i;
		i.QuadPart = 0;
		i.LowPart = ::GetFileSize(hFile, (LPDWORD)&i.HighPart);

		nToRead = (int)i.QuadPart;
	}

	DWORD dwPtrLow = SetFilePointer(hFile, offs.LowPart, &offs.HighPart, FILE_BEGIN);
	if (dwPtrLow == INVALID_SET_FILE_POINTER)
	{
		DWORD dwLastError = GetLastError();
		if( NO_ERROR != dwLastError )
		{
			CloseHandle(hFile);
			return 0;
		}
	}


	DWORD nHave = 0;	
	ReadFile(hFile, pBuf, nToRead, &nHave, NULL); 
	CloseHandle(hFile);
	return nHave;
}

int file_write(CString strFilename, __int64 nMoveOffset, bool bCreate, void *pBuf, int nToWrite, bool bFlush)
{
	int open_flag = OPEN_ALWAYS;
	if(bCreate) open_flag = CREATE_ALWAYS;

	HANDLE hFile = CreateFile(strFilename, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, open_flag, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)return 0;

	LARGE_INTEGER offs;
	offs.QuadPart = nMoveOffset;

	DWORD dwPtrLow = SetFilePointer(hFile, offs.LowPart, &offs.HighPart, FILE_BEGIN);
	if (dwPtrLow == INVALID_SET_FILE_POINTER)
	{
		DWORD dwLastError = GetLastError();
		if( NO_ERROR != dwLastError )
		{
			CloseHandle(hFile);
			return 0;
		}
	}

	DWORD nHave = 0;
	WriteFile(hFile, pBuf, nToWrite, &nHave, NULL);

	if(bFlush)
		FlushFileBuffers(hFile);

	CloseHandle(hFile);
	return nHave;	
}

bool file_get_attrib(CString strFilename, __int64 &file_size, __int64 &last_write)
{
	WIN32_FILE_ATTRIBUTE_DATA attribFile;
	::ZeroMemory( &attribFile, sizeof(WIN32_FILE_ATTRIBUTE_DATA) );

	BOOL bok = ::GetFileAttributesEx( strFilename, GetFileExInfoStandard, &attribFile );
	if(bok == FALSE)
	{
		return false;
	}

	LARGE_INTEGER lg_file_size;
	lg_file_size.HighPart = attribFile.nFileSizeHigh;
	lg_file_size.LowPart = attribFile.nFileSizeLow;

	memcpy(&last_write, &attribFile.ftLastWriteTime, 8);

	file_size = lg_file_size.QuadPart;

	return true;
}


__int64  file_get_size(CString strFilename)
{
	//注：最快速的写法，不需要打开文件句柄
	WIN32_FILE_ATTRIBUTE_DATA attribFile;
	::ZeroMemory( &attribFile, sizeof(WIN32_FILE_ATTRIBUTE_DATA) );

	BOOL bok = ::GetFileAttributesEx( strFilename, GetFileExInfoStandard, &attribFile );
	if(bok == FALSE)
	{
		return 0;
	}

	LARGE_INTEGER lg_file_size;
	lg_file_size.HighPart = attribFile.nFileSizeHigh;
	lg_file_size.LowPart = attribFile.nFileSizeLow;

	return lg_file_size.QuadPart;

	/*
	HANDLE hFile = CreateFile(strFilename, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)return 0;

	LARGE_INTEGER i;
	i.QuadPart = 0;
	i.LowPart = ::GetFileSize(hFile, (LPDWORD)&i.HighPart);

	CloseHandle(hFile);

	return i.QuadPart;

	*/

/*	
// CRT 函数有些内存泄漏
	int fh = _open(csPath, _O_RDONLY );
	if(fh <= 0)return 0;

	pos = _lseeki64( fh, 0L, SEEK_END);
	_close( fh );
*/

	
}


CStringA file_read_text(CString strFilename)
{
	CStringA content;

	char buf[16*1024+1];
	int nPos = 0, nSize = 0;
	for(;;)
	{
		nSize = file_read(strFilename, nPos, buf, sizeof(buf)-1);
		if(nSize <= 0)break;
		buf[nSize] = 0;
		nPos += nSize;
		content += buf;
	}
	return content;
}

bool file_exist(CString strFilename)
{
//	return ::PathFileExists(strFilename);
	return TRUE;
}

bool file_delete(CString strFilename)
{
	return ::DeleteFile(strFilename);
}
bool file_copy(CString strFrom, CString strTo)
{
	return ::CopyFile(strFrom, strTo, FALSE);
}

int  file_move(CString strFrom, CString strTo)
{
	BOOL bok = ::MoveFile(strFrom, strTo);
	if(bok)
		return 0;
	else
		return ::GetLastError();
}

// 目录创建。
bool dir_create(CString strDir, BOOL bNeedRecursive)
{
	// 对于网络共享目录也支持

	dir_normalize(strDir);

	CString strTmpDir;
	for(int i=0;i<strDir.GetLength();i++)     //每一级的目录都得创建
	{
		strTmpDir += strDir.GetAt(i);
		if(strDir.GetAt(i)== _T('\\'))
			::CreateDirectory(strTmpDir, NULL);
	}

	::CreateDirectory(strDir, NULL);


//	return ::PathIsDirectory( strDir );
	return TRUE;
}

CString dir_make_path(CString strBaseDir, CString strFilename)
{
	// 如果目标文件已经是全路径，则直接返回
	if(strFilename.Find(_T(':')) != -1)			// 盘符
	{
		dir_normalize(strFilename);
		return strFilename;
	}
	
	if(strFilename.Left(2) == _T("\\"))			// 网络共享的路径
	{
		dir_normalize(strFilename);
		return strFilename;
	}

	// 正规化原来的base_dir
	CString strRet = strBaseDir;
	dir_normalize(strRet);

	if(strFilename.Left(1) == '\\') strFilename = strFilename.Mid(1);
	strRet += strFilename;

	return strRet;
}

bool dir_exist(CString strDir)
{
//	return ::PathIsDirectory( strDir );
	return TRUE;
}

bool dir_normalize(CString &strDir)
{
	// 支持网络共享盘符 例如  \\server\share
	// 支持普通路径，如 c:\dir
	// 支持斜杠写错的路径  c:/dir

	// 1. 先替换
	
	strDir.Replace(_T("/"), _T("\\"));
	
	// 2. 确保加上目录

	if(strDir.Right(1) != _T('\\'))
		strDir += _T("\\");

	return true;
}

// 递归删除目录及其子目录，支持删除只读文件
bool dir_delete(CString strDir)
{
	HANDLE hFind;  // file handle
	WIN32_FIND_DATA FindFileData;

	TCHAR DirPath[MAX_PATH];
	TCHAR FileName[MAX_PATH];

	if(strDir.Right(1) != _T("\\"))
		strDir += _T('\\');

	_tcscpy(DirPath,strDir);
	_tcscat(DirPath,_T("*.*"));    // searching all files
	_tcscpy(FileName,strDir);

	hFind = FindFirstFile(DirPath,&FindFileData); // find the first file
	if(hFind == INVALID_HANDLE_VALUE) 
		return FALSE;

	_tcscpy(DirPath,FileName);
	    
	bool bSearch = true;
	while(bSearch) 
	{ // until we finds an entry
		if(FindNextFile(hFind,&FindFileData)) 
		{
			if(_tcscmp(FindFileData.cFileName,_T(".")) == 0 
				|| _tcscmp(FindFileData.cFileName, _T("..")) == 0)
				continue;

			_tcscat(FileName,FindFileData.cFileName);
			if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
			{
				// we have found a directory, recurse
				if(!dir_delete(FileName)) 
				{ 
					FindClose(hFind); 
					return FALSE; // directory couldn't be deleted
				}

				RemoveDirectory(FileName); // remove the empty directory
				_tcscpy(FileName,DirPath);
			}
			else
			{
				if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
				{
					SetFileAttributes(FileName, FILE_ATTRIBUTE_NORMAL);
				}

				if(!DeleteFile(FileName))
				{  // delete the file
					FindClose(hFind); 
					return FALSE; 
				}          

				_tcscpy(FileName,DirPath);
			}
		}
		else 
		{
			if(GetLastError() == ERROR_NO_MORE_FILES) // no more files there
				bSearch = false;
			else 
			{
				// some error occured, close the handle and return FALSE
				FindClose(hFind); 
				return FALSE;
			}

		}

	}
	::FindClose(hFind);  // closing file handle

	return ::RemoveDirectory(strDir);			// remove the empty directory
}



// endof namespace XFile::
};


//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------
bool XFileEx::IsValid()
{
	if(m_file != 0 && m_file != INVALID_HANDLE_VALUE)
		return true;
	else
		return false;
}

void XFileEx::Close()
{
	if(IsValid() == false)
		return;

	::CloseHandle(m_file);
	m_file = 0;
}

void XFileEx::Flush()
{
	if(IsValid() == false)
		return;

	::FlushFileBuffers(m_file);
}

bool XFileEx::OpenLog(CString strFilename, BOOL bAppend, BOOL bAutoFlush, BOOL bUnicodeText)
{
	Close();


	DWORD access_mode = CREATE_ALWAYS;
	if(bAppend)
		access_mode = OPEN_ALWAYS;
	
	DWORD dwFlags = FILE_ATTRIBUTE_NORMAL;
	if(bAutoFlush)
		dwFlags |= FILE_FLAG_WRITE_THROUGH;


	m_file = ::CreateFile(strFilename, GENERIC_WRITE, 
		FILE_SHARE_READ, NULL,
			access_mode, dwFlags, 0);

	if(IsValid() == false)
	{
		DWORD dwErr = GetLastError();
		dwErr = 1;
		return false;
	}

	if(bUnicodeText)
	{
		// unicode 版本，写入
		LARGE_INTEGER file_size;

		file_size.QuadPart = 0;
		file_size.LowPart = ::GetFileSize(m_file, (LPDWORD)&file_size.HighPart);

		if(file_size.QuadPart == 0)
		{
			DWORD tag = 0xFEFF;
			DWORD nHave = 0;
			WriteFile(m_file, &tag, 2, &nHave, NULL);
		}
	}

	if(bAppend)
		SetFilePointer(m_file, 0, 0, FILE_END);

	return true;
}


bool XFileEx::OpenRead(CString strFilename)
{
	Close();

	DWORD dwFlags = FILE_FLAG_RANDOM_ACCESS;

	m_file = ::CreateFile(strFilename, GENERIC_READ, 
		FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, dwFlags, 0);

	return IsValid();
}



bool XFileEx::Seek(__int64 pos)
{
	if(IsValid() == false)
		return false;

	LARGE_INTEGER offs;
	offs.QuadPart = pos;

	DWORD dwPtrLow = SetFilePointer( m_file, offs.LowPart, &offs.HighPart, FILE_BEGIN);
	if (dwPtrLow == INVALID_SET_FILE_POINTER)
	{
		DWORD dwLastError = GetLastError();
		if( NO_ERROR != dwLastError )
		{
			return false;
		}
	}
	
	return true;
}

int XFileEx::Write(const void *buf, int nBytes)
{
	if(IsValid() == false)
		return 0;

	DWORD nHave = 0;
	BOOL bok = ::WriteFile(m_file, buf, nBytes, &nHave, NULL);

	if(bok == FALSE || nHave != nBytes)
	{
		return 0;
	}

	return nBytes;
}


int XFileEx::Read(void *buf, int nBytes)
{
	if(IsValid() == false)
		return 0;

	DWORD nHave = 0;
	BOOL bok = ::ReadFile( m_file, buf, nBytes, &nHave, NULL);
	if(bok)
	{
		return nHave;
	}

	return 0;
}

