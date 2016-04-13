#pragma once
#include "Item.h"
#include <vector>

namespace Engine
{
    class CItemDirectX : public CItem
    {
        friend class CEngineConfigure;

    public:
        CItemDirectX(void);
        ~CItemDirectX(void);

    protected:
        virtual BOOL __stdcall CheckProblem();

    protected:
        CString m_strKeyword;
        std::vector<CString> m_vecNotFoundFile;
    };
}