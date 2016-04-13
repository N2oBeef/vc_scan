#include "stdafx.h"
#include "CharacterCodeHelper.h"

CString Utf8ToStringT(LPSTR str)
{
	_ASSERT(str);
	USES_CONVERSION;
	WCHAR *buf;
	int length = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	buf = new WCHAR[length+1];
	ZeroMemory(buf, (length+1) * sizeof(WCHAR));
	MultiByteToWideChar(CP_UTF8, 0, str, -1, buf, length);

	return (CString(W2T(buf)));
}

#define IsHexNum(c) ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))

int GetHexCharValue(char c)
{
	if (c >= '0' && c <= '9')
	{
		return c - '0';
	}

	if (c >= 'A' && c <= 'F')
	{
		return 9 + c - 'A' + 1;
	}

	if (c >= 'a' && c <= 'f')
	{
		return 9 + c - 'a' + 1;
	}
	return 0;
}

char GetHexValue( const char* pBuffer)
{
	if( strlen(pBuffer) >= 2 )
	{
		char c1 = GetHexCharValue(pBuffer[0]) ;
		char c2 = GetHexCharValue(pBuffer[1]) ;
		char c = (char)GetHexCharValue(pBuffer[0]) * 16 + (char)GetHexCharValue(pBuffer[1]);
		return c;
	}
	return 0;
}

#define IsHexNum(c) ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))

CString UrlDecode(LPCTSTR url)
{
	Utf8 utfUrl(url);
	USES_CONVERSION;
	const char* _url = utfUrl.AsSz();

	int i = 0;
	int length = (int)strlen(_url);

	CHAR *buf = new CHAR[length];
	ZeroMemory(buf, length);

	LPSTR p = buf;
	while(i < length)
	{
		if(i <= length -3 && _url[i] == '%' && IsHexNum(_url[i+1]) && IsHexNum(_url[i+2]))
		{
			if(i >= 73)
			{
				int nTest = strlen(buf);
				int k = 0;
				k++;
			}

			*(p++) = GetHexValue(_url + i + 1);
			i += 3;
		}
		else if (i <= length - 5 && _url[i] == '%' && _url[i+1] == 'u' )
		{
			char szUnicode[3] = { 0 };
			WCHAR wUnicode = 0;
			char cHigh = GetHexValue(_url + i + 2);
			char cLow  = GetHexValue(_url + i + 4);
			wUnicode = MAKEWORD(cLow, cHigh);

			CString strVal(wUnicode);
			Utf8 utfValue(strVal);
			for (int j = 0;j < utfValue.Len8() - 1;j++)
			{
				*(p++) = utfValue.AsSz()[j];
			}
			i += 6;
		}
		else
		{
			*(p++) = _url[i++];
		}
	}

	Utf16 utf(buf);
	return utf.AsWz();
}

 CString UrlEncode(const CString& strUnicode)
{
	CString strResult = strUnicode;
	LPCWSTR unicode = T2CW(strUnicode);
	int len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, 0, 0, 0, 0);
	if (len)
	{
		char *utf8 = new char[len + 1];
		char *utf8temp = utf8;
		WideCharToMultiByte(CP_UTF8, 0, unicode, -1, utf8, len + 1, 0, 0);
		utf8[len] = NULL;   
		CString strTemp, strEncodeData; 
		while (*utf8 != 0) 
		{ 
			strTemp.Format(_T("%%%2x"), (BYTE)*utf8); 
			strEncodeData += strTemp; 
			++utf8; 
		} 

		delete []utf8temp;

		strResult = strEncodeData;
	}

	return strResult;
}


 typedef unsigned char BYTE;

 BYTE toHex(const BYTE &x)
 {
	 return x > 9 ? x -10 + 'A': x + '0';
 }

 BYTE fromHex(const BYTE &x)
 {
	 return isdigit(x) ? x-'0' : x-'A'+10;
 }

 //gb2312
 string URLEncodeEx(const string &sIn)
 {
	 string sOut;
	 for( size_t ix = 0; ix < sIn.size(); ix++ )
	 {      
		 BYTE buf[4];
		 memset( buf, 0, 4 );
		 if( isalnum( (BYTE)sIn[ix] ) )
		 {      
			 buf[0] = sIn[ix];
			}
		 //else if ( isspace( (BYTE)sIn[ix] ) ) //貌似把空格编码成%20或者+都可以
		 //{
		 //    buf[0] = '+';
		 //}
		 else
			{
				buf[0] = '%';
				buf[1] = toHex( (BYTE)sIn[ix] >> 4 );
				buf[2] = toHex( (BYTE)sIn[ix] % 16);
			}
		 sOut += (char *)buf;
	 }
	 return sOut;
 };

 string URLDecodeEx(const string &sIn)
 {
	 string sOut;
	 for( size_t ix = 0; ix < sIn.size(); ix++ )
	 {
		 BYTE ch = 0;
		 if(sIn[ix]=='%')
			{
				ch = (fromHex(sIn[ix+1])<<4);
				ch |= fromHex(sIn[ix+2]);
				ix += 2;
			}
		 else if(sIn[ix] == '+')
			{
				ch = ' ';
			}
		 else
			{
				ch = sIn[ix];
			}
		 sOut += (char)ch;
	 }
	 return sOut;
 }
