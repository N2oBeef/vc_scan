#include "stdafx.h"
#include "hash.h"
#include "sha1.h"
#include "XFile.h"
#include <math.h>

CStringA Digest2String(unsigned const  char*pDigest, int nDigest)
{
	//BYTE *pBYTE = (BYTE *)pDigest;
	CStringA ret;
	char szTemp[8];
	for(int i=0; i<nDigest; i++)
	{
		sprintf(szTemp, "%02X", pDigest[i]);
		ret += szTemp;
	}
	
	return ret;
}

DWORD calc_360_part_size(__int64 filesz)
{
	if (filesz <= 2 * 1024 * 1024)  return 16 * 1024;

	if (filesz >= (UINT64)4 * 1024 * 1024 * 1024)  return 512 * 1024;

	float f = sqrt( (float)filesz/4096);
	unsigned short n = (unsigned short)f;
	int i = 3;
	while(n && i < 16)
	{
		if( n & (0x8000>>i))    return (0x8000 >> i) * 1024;
		i++;
	}

	return 16 * 1024;
}

BOOL calc_hash(const TCHAR *file_path, CStringA &Calced_hash, CStringA &Calced_Fullhash, CStringA &filesize)
{
	Calced_hash = "";

	BYTE hash[20] = {0};

	std::vector<char> part_hash;

	__int64 file_size = XFile::file_get_size(file_path);
	if(file_size == 0)
		return FALSE;
	DWORD part_size = calc_360_part_size(file_size);
	if(part_size == 0)return FALSE;

	XFileEx hFile;
	hFile.OpenRead(file_path);

	if(! hFile.IsValid())
	{
		return FALSE;
	}

	int part_count = int(file_size / part_size);
	if( (file_size % part_size) != 0)part_count ++;

	std::vector<BYTE> buf;
	buf.resize(part_size);
	BYTE *part_buf = &buf[0];

	part_hash.resize(part_count * 20);

	BOOL success = TRUE;

	for(int i=0;i<part_count;i++)
	{
		// 可能 nHave 小于 part_size
		int nHave = hFile.Read(part_buf, part_size);

		if(nHave <= 0)
		{
			success = FALSE;
			break;
		}

		hash_function::CSha1::HashBuffer(part_buf, nHave, (BYTE *)&part_hash[i * 20]);
	}

	hash_function::CSha1::HashBuffer(&part_hash[0], part_count * 20, hash);

	hFile.Close();

	Calced_hash = Digest2String(hash, sizeof(hash));
	Calced_Fullhash = Digest2String((unsigned char *)&part_hash[0], part_hash.size());

	filesize.Format("%I64d", file_size);
	return TRUE;
}

CStringA GetFileP2PID(LPCTSTR szFileName)
{
	CStringA strHash, strFullhash, strFilesize;

	calc_hash(szFileName, strHash, strFullhash, strFilesize);

	return strHash;
}
