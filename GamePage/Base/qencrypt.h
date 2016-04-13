#ifndef __QENCRYPT_H__
#define __QENCRYPT_H__

#define ENCRYPT_KEY     100

//ÒÆÎ»¼Ä´æÆ÷
//hanyugang@360.cn
#include <string>
#include <stdio.h>

__inline void qencrypt(LPBYTE pBuf, DWORD nLength, int key)
{
	size_t i;
	for (i = 0; i < 64; ++i)
	{
		key = key << 1;
		int k = ((key >> 3) & 1) ^ ((key >> 15) & 1) ^ ((key >> 23) & 1) ^ ((key >> 7) & 1);
		if (1 == k)
			key |= 1;
	}

	size_t charlen = 8;
	for (i = 0; i < nLength; ++i)
	{
		BYTE mkey = 0;
		for (size_t j = 0; j < charlen; ++j)
		{
			if ( 1 == ((key >> 31) & 1) )
				mkey |= 1;
			mkey = mkey << 1;

			key = key << 1;
			int k = ((key >> 3) & 1) ^ ((key >> 15) & 1) ^ ((key >> 23) & 1) ^ ((key >> 7) & 1);
			if (1 == k)
				key = key | 1;
		}
		pBuf[i] ^= mkey;
	}
}

__inline std::string qencrypt(const std::string& buf, int key)
{
    size_t i;
    for (i = 0; i < 64; ++i)
    {
        key = key << 1;
        int k = ((key >> 3) & 1) ^ ((key >> 15) & 1) ^ ((key >> 23) & 1) ^ ((key >> 7) & 1);
        if (1 == k)
            key |= 1;
    }
   
    std::string outbuf = buf;
    size_t buflen = outbuf.size();
    size_t charlen = 8*sizeof(std::string::value_type);
    for (i = 0; i < buflen; ++i)
    {
        std::string::value_type mkey = 0;
        for (size_t j = 0; j < charlen; ++j)
        {
            if ( 1 == ((key >> 31) & 1) )
                mkey |= 1;
            mkey = mkey << 1;

            key = key << 1;
            int k = ((key >> 3) & 1) ^ ((key >> 15) & 1) ^ ((key >> 23) & 1) ^ ((key >> 7) & 1);
            if (1 == k)
                key = key | 1;
        }
        outbuf[i] ^= mkey;
    }
    return outbuf;
}

__inline DWORD qcrc32(LPBYTE pBuf, DWORD nLength)
{
	static unsigned int aPoly= 0x04C10DB7;
	static unsigned int tables[256] = {0};
	static bool         binit = false;
	if(binit == false)
	{
		binit = true;
		for ( size_t i = 0; i < 256; ++i )
		{
			unsigned int nAccum = 0;
			unsigned int nData  = i << 24 ;
			for ( size_t j = 0; j < 8; ++j )
			{
				if ( ( nData ^ nAccum ) & 0x80000000 )
					nAccum = ( nAccum << 1 ) ^ aPoly;
				else
					nAccum <<= 1;
				nData <<= 1;
			}
			tables[i] = nAccum;
		}
	}
	unsigned int nAccum = 0;
	for (size_t i = 0; i < nLength; ++i )
	{
		size_t l = ( ( nAccum >> 24 ) ^ pBuf[i] ) % 256;
		nAccum = ( nAccum << 8 ) ^ tables[l];
	}

	return nAccum;
}

//crc32 
//get buf's crc32 
__inline std::string qcrc32(const std::string& buf)
{
   char phex[9] ={0};
   sprintf(phex,"%08x",qcrc32((PBYTE)buf.c_str(), buf.size()));    
   return std::string(phex);
}
#endif //__QENCRYPT_H__
