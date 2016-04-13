#pragma once
#include "Item.h"

namespace Engine
{
    class CItemMVCRT : public CItem
    {
    public:
        CItemMVCRT(void);
        ~CItemMVCRT(void);

    protected:
        virtual BOOL __stdcall CheckProblem();
    };
}
