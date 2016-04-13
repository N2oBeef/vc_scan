#pragma once

// º∆À„ hash

BOOL calc_hash(const TCHAR *file_path, CStringA &Calced_hash, CStringA &Calced_Fullhash, CStringA &filesize);

CStringA Digest2String(unsigned const  char*pDigest, int nDigest);

CStringA GetFileP2PID(LPCTSTR szFileName);



