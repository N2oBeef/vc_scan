#include "stdafx.h"

#include "GdiplusInitializer.h"

CGdiplusInitializer::CGdiplusInitializer(void)
{
}

CGdiplusInitializer::~CGdiplusInitializer(void)
{
	Uninit();
}

void CGdiplusInitializer::Init( void )
{
	GdiplusStartupInput gdiplusStartupInput = NULL;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

void CGdiplusInitializer::Uninit( void )
{
	GdiplusShutdown(m_gdiplusToken);
	m_gdiplusToken = NULL;
}
