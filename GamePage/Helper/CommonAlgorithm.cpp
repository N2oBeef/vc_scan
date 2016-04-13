#include "StdAfx.h"
#include "md5.h"
#include "CommonAlgorithm.h"
#include <WinVer.h>

#pragma comment(lib, "Version.lib")

BOOL CCommonAlgorithm::MakeBuffMd5( LPVOID pBuf,int nBufLen,BYTE md5[16] )
{
	memset(md5, 0, 16);

	if(pBuf == NULL) return FALSE;

	MD5_CTX _MD5;

	memset(md5,0,16);

	MD5Init(&_MD5);
	MD5Update(&_MD5, (BYTE*)pBuf, nBufLen);
	MD5Final( &_MD5);
	memcpy( md5, _MD5.digest, 16);

	return TRUE;
}

BOOL CCommonAlgorithm::MakeFileMd5( LPCWSTR pFile,BYTE md5[16] )
{
	BOOL	bRet	= FALSE;
	HANDLE	hFile	= NULL;
	LPVOID	pBuff	= NULL;
	DWORD	dwLen	= 0;
	HANDLE	hFileMap= NULL;

	memset(md5,0,16);
	if(!pFile) goto _EXIT;

	hFile = ::CreateFile(pFile,FILE_READ_DATA,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);
	if(!hFile || hFile==INVALID_HANDLE_VALUE) goto _EXIT;

	hFileMap = CreateFileMapping(hFile,NULL,PAGE_READONLY,0,0,NULL);
	if (!hFileMap) goto _EXIT;

	pBuff	 = MapViewOfFile(hFileMap,FILE_MAP_READ,0,0,0);
	if (!pBuff) goto _EXIT;	

	dwLen = ::SetFilePointer(hFile,0,NULL,FILE_END);
	::SetFilePointer(hFile,0,NULL,FILE_BEGIN);

	bRet = TRUE;
	MakeBuffMd5(pBuff,dwLen,md5);

_EXIT:
	if(pBuff)
		UnmapViewOfFile(pBuff),pBuff = NULL;

	if(hFileMap)
		CloseHandle(hFileMap),hFileMap = NULL;

	if(hFile)
		CloseHandle(hFile),hFile = NULL;

	return bRet;
}

BOOL CCommonAlgorithm::ConvertMD5CharToByte( BYTE *pMd5, int nLen, LPTSTR pBuf, int nCount )
{
	if (nCount < 32 || nLen < 16) return FALSE;
	if (pBuf == NULL) return FALSE;
	
	_tcslwr_s(pBuf, nCount);
	for( int i=0; i<16; i++ )
	{
		CString sz(pBuf+i*2, 2);
		_stscanf_s(sz, _T("%x"), &pMd5[i]);
	}

	return FALSE;
}

BOOL CCommonAlgorithm::ConvertMD5SumToChar( BYTE md5[16],LPTSTR pBuf,int nCount )
{
	memset(pBuf,0,sizeof(TCHAR)*nCount);
	if(!pBuf || nCount<33)
		return FALSE;

	for( int i=0; i<16; i++ )
		_stprintf_s(pBuf+i*2, 16, _T("%02x"), md5[i]);

	_tcsupr_s(pBuf, nCount);

	return TRUE;
}

BOOL CCommonAlgorithm::GetFileVer(LPCTSTR lpFileFullPath, LPWSTR lpVer, int nVerlen)
{
	TCHAR  szVersionBuffer[8192] = _T("");
	DWORD dwVerSize = 0;
	DWORD dwHandle = 0;

	if (!lpFileFullPath || !lpVer)
		return FALSE;

	dwVerSize = ::GetFileVersionInfoSize( lpFileFullPath, &dwHandle );
	if( dwVerSize == 0 || dwVerSize > (sizeof(szVersionBuffer)-1) )
		return FALSE;

	if( ::GetFileVersionInfo( lpFileFullPath, 0, dwVerSize, szVersionBuffer) )
	{
		VS_FIXEDFILEINFO * pInfo;
		unsigned int nInfoLen;

		if( VerQueryValue( szVersionBuffer, _T("\\"), (void**)&pInfo, &nInfoLen ) )
		{
			memset( lpVer, 0, nVerlen * sizeof(TCHAR) );
			wsprintf( lpVer, _T("%d.%d.%d.%d"),
				HIWORD( pInfo->dwFileVersionMS ), LOWORD( pInfo->dwFileVersionMS ),
				HIWORD( pInfo->dwFileVersionLS ), LOWORD( pInfo->dwFileVersionLS ) );
			return TRUE;
		}
	}

	return FALSE;
}

LONGLONG CCommonAlgorithm::GetFileLength( const CString & szFilename )
{
	WIN32_FILE_ATTRIBUTE_DATA data;
	ZeroMemory(&data, sizeof(data));

	GetFileAttributesEx(szFilename, GetFileExInfoStandard, & data);

	LONGLONG lSize = data.nFileSizeHigh;
	lSize <<= 32;
	lSize |= data.nFileSizeLow;

	return lSize;
}

