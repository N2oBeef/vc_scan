// dllmain.h : Declaration of module class.

class CGamePageModule : public CAtlDllModuleT< CGamePageModule >
{
public :
	DECLARE_LIBID(LIBID_GamePageLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_GAMEPAGE, "{94958D95-AC19-4FAC-B8B0-D001D5AF09AE}")
};

extern class CGamePageModule _AtlModule;
