#include "StdAfx.h"
#include "ItemMVCRT.h"
#include "Utils.h"

namespace Engine
{
    CItemMVCRT::CItemMVCRT(void)
    {
    }

    CItemMVCRT::~CItemMVCRT(void)
    {
    }

    BOOL __stdcall CItemMVCRT::CheckProblem()
    {
        return !CUtils::IsInstalled2(m_RegInfo);
    }
}