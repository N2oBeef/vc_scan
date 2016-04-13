// GamePage.h : Declaration of the CGamePage

#pragma once
#include "resource.h"       // main symbols

#include "GamePage_i.h"
#include "_IGamePageEvents_CP.h"
#include "_IPageEntryEvents_CP.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

#include "MainWnd.h"


// CGamePage

class ATL_NO_VTABLE CGamePage :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CGamePage, &CLSID_GamePage>,
	public IConnectionPointContainerImpl<CGamePage>,
	public CProxy_IGamePageEvents<CGamePage>,
	public IDispatchImpl<IGamePage, &IID_IGamePage, &LIBID_GamePageLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public CProxy_IPageEntryEvents<CGamePage>
{
public:
	CGamePage()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_GAMEPAGE1)


BEGIN_COM_MAP(CGamePage)
	COM_INTERFACE_ENTRY(IGamePage)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IPageEntry)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CGamePage)
	CONNECTION_POINT_ENTRY(__uuidof(_IGamePageEvents))
	CONNECTION_POINT_ENTRY(__uuidof(_IPageEntryEvents))
END_CONNECTION_POINT_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreatePage( 
		/* [in] */ OLE_HANDLE hParentWnd,
		/* [in] */ LONG nX,
		/* [in] */ LONG nY,
		/* [in] */ LONG nWidth,
		/* [in] */ LONG nHeight,
		/* [retval][out] */ OLE_HANDLE *hWnd);

	virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnActive( void);

	virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnDeactive( void);

	virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DispatchCommand( 
		/* [in] */ BSTR bstrCmd,
		/* [in] */ LONG lCode,
		LONG lSubCode,
		/* [retval][out] */ LONG *pProcessed);

	virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnPreClose( 
		/* [retval][out] */ LONG *pCanClose);

	virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsBusy( 
		/* [retval][out] */ LONG *pIsBusy);

	virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DestroyPage( void);

	virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWnd( 
		/* [retval][out] */ OLE_HANDLE *pChildWnd);

	virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetParentWnd( 
		/* [retval][out] */ OLE_HANDLE *pParentWnd);

	virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Call( 
		/* [in] */ BSTR bstrCmd,
		/* [in] */ LONG lCode,
		/* [in] */ LONG lSubCode,
		/* [retval][out] */ LONG *pResult);

	static void InitSite(void);

private:
	void Init();
	void Init360Base(void);

private:
	HWND      m_hParentWnd;
	CMainWnd  m_MainWnd;

	CGdiplusInitializer m_gdiInitializer;

	BOOL m_bPageActive;
};

OBJECT_ENTRY_AUTO(__uuidof(GamePage), CGamePage)
