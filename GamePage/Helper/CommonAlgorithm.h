#pragma once

class CCommonAlgorithm
{
public:
	static BOOL MakeBuffMd5(LPVOID pBuf,int nBufLen,BYTE md5[16]);
	static BOOL MakeFileMd5(LPCWSTR pFile,BYTE md5[16]);
	static BOOL ConvertMD5SumToChar(BYTE md5[16],LPTSTR pBuf,int nCount);
	static BOOL GetFileVer(LPCTSTR lpFileFullPath, LPWSTR lpVer, int nVerlen);
	static LONGLONG GetFileLength( const CString & szFilename );
	static BOOL ConvertMD5CharToByte( BYTE *pMd5, int nLen, LPTSTR pBuf, int nCount );
};
