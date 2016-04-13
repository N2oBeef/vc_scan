#pragma once

#include "IEngine.h"
#include "Base/XThread.h"

#include "Engine.h"

class CEngineConfigure
{
public:
	CEngineConfigure();
	~CEngineConfigure();

	BOOL Init(Engine::CEngine *pEngine);

private:
	void InitItems(void);

private:
	Engine::CEngine *m_pEngine;
};