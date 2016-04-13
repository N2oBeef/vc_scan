#pragma once
#include <vector>
#include <atlstr.h>

namespace C7za
{
    BOOL CompressFile( const CString& str7zDllPath, const std::vector<CString>& vecFile, const CString& strArchiveFile, const CString& strPassword, BOOL bAppend );
    BOOL UncompressFile( const CString& str7zDllPath, const CString& strArchiveFile, const CString& strOutDirectory, const CString& strPassword, const CString& strEntryName );
}

class C7zaHelper
{
protected:
    typedef struct tagUncompressFileParameter
    {
        C7zaHelper* pThis;
        CString str7zDllPath;
        CString strArchiveFile;
        CString strOutDirectory;
        CString strPassword;
        CString strEntryName;
    } UncompressFileParameter;

public:
    C7zaHelper();
    ~C7zaHelper();

    BOOL StartUncompressFile( const CString& str7zDllPath, 
        const CString& strArchiveFile, 
        const CString& strOutDirectory, 
        const CString& strPassword, 
        const CString& strEntryName );

    static unsigned int WINAPI ThreadProc(void * pParam);

    BOOL DoUncompressFile(const CString& str7zDllPath, 
        const CString& strArchiveFile, 
        const CString& strOutDirectory, 
        const CString& strPassword, 
        const CString& strEntryName);

private:
    HANDLE m_hThread;
};
