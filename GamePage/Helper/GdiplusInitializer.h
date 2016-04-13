#pragma once

#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

class CGdiplusInitializer
{
public:
    CGdiplusInitializer(void);
    ~CGdiplusInitializer(void);

	void Init(void);
	void Uninit(void);

private:
    ULONG_PTR m_gdiplusToken;
};
