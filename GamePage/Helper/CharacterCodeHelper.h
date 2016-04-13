#ifndef _____CHARACTERCODEHELPER____
#define _____CHARACTERCODEHELPER____

#include <string>
using namespace std;

#define DISALLOW_EVIL_CONSTRUCTORS(TypeName)    \
	TypeName(const TypeName&);                    \
	void operator=(const TypeName&)


CString UrlDecode(LPCTSTR url);
CString UrlEncode(const CString& strUnicode);
string URLDecodeEx(const string &sIn);
string URLEncodeEx(const string &sIn);
class Utf8
{
public:
	explicit Utf8(const wchar_t* str)
	{
		m_pChar = NULL;
		m_nLen  = 0;
		*this = str;
	}

	explicit Utf8() {
		m_pChar = NULL;
		m_nLen  = 0;
	}

	~Utf8() {
		if (m_pChar)
		{
			delete m_pChar;
		}
		m_pChar = NULL;
		m_nLen  = 0;
	}
	

	inline Utf8& operator=(const wchar_t* str)
	{
		m_nLen = WideCharToMultiByte(CP_UTF8, 0, str, (int)wcslen(str), NULL, 0, NULL, NULL);
		m_nLen++;
		m_pChar = new char[m_nLen]; //static_cast<char*>(_alloca(m_nLen));
		memset(m_pChar,0,m_nLen);

		WideCharToMultiByte(CP_UTF8, 0, str, (int)wcslen(str), m_pChar, m_nLen, NULL, NULL);

		return *this;
	}

	/*inline operator const std::string&() const
	{
		return str_;
	}*/

	inline const char* AsSz() const
	{
		return m_pChar;
	}

	// Deprecated
	/*inline const std::string& AsString() const
	{
		return str_;
	}*/

	// Deprecated
	inline int Len8() const
	{
		return m_nLen;
	}

private:
	DISALLOW_EVIL_CONSTRUCTORS(Utf8);
	char* m_pChar;
	int   m_nLen;
	//std::string str_;
};

class Utf16
{
public:
	Utf16(const char* pData)
	{
		m_nLen = MultiByteToWideChar(CP_UTF8, 0, pData, -1, NULL, 0);
		m_nLen++;
		str_ = new wchar_t[m_nLen];
		memset(str_,0,sizeof(wchar_t)*m_nLen);
		MultiByteToWideChar(CP_UTF8, 0, pData, -1, str_, m_nLen);
	}

	~Utf16()
	{
		delete str_;
	}
	inline const wchar_t* AsWz() const
	{
		return str_;
	}
	
	inline int Len(void)
	{
		return m_nLen;
	}
private:
	DISALLOW_EVIL_CONSTRUCTORS(Utf16);
	wchar_t *str_;
	int m_nLen;
};


class AnsnToUtf16
{
public:
	AnsnToUtf16(const char* pData)
	{
		m_nLen = MultiByteToWideChar(CP_ACP, 0, pData, -1, NULL, 0);
		m_nLen++;
		str_ = new wchar_t[m_nLen];
		memset(str_,0,sizeof(wchar_t)*m_nLen);
		MultiByteToWideChar(CP_ACP, 0, pData, -1, str_, m_nLen);
	}

	~AnsnToUtf16()
	{
		delete str_;
	}
	inline const wchar_t* AsWz() const
	{
		return str_;
	}

	inline int Len(void)
	{
		return m_nLen;
	}
private:
	DISALLOW_EVIL_CONSTRUCTORS(AnsnToUtf16);
	wchar_t *str_;
	int m_nLen;
};
#endif

