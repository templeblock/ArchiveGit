// PMToolbar.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>
#include "PMToolbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPalette* pOurPal = NULL;
HINSTANCE m_hInstance;

static AFX_EXTENSION_MODULE PMToolbarDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("PMTOOLBAR.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(PMToolbarDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(PMToolbarDLL);
		m_hInstance = hInstance;
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("PMTOOLBAR.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(PMToolbarDLL);
	}
	return 1;   // ok
}

BOOL LoadToolbars(CWnd* pParent)
{
	BOOL bRetVal = FALSE;
	HINSTANCE hCurIns = AfxGetResourceHandle();
	AfxSetResourceHandle(m_hInstance);
   if (m_wndFileBar.Create(pParent, WS_CHILD|CBRS_TOP)
	 && m_wndFormatBar.Create(pParent, WS_CHILD|CBRS_TOP)
	 && m_wndEdit1Bar.Create(pParent, WS_CHILD|CBRS_LEFT)
	 && m_wndEdit2Bar.Create(pParent, WS_CHILD|CBRS_LEFT))
   {
		m_wndEdit1Bar.ShowWindow(SW_SHOW);
		m_wndEdit1Bar.InvalidateRect(NULL);
		m_wndEdit2Bar.ShowWindow(SW_SHOW);
		m_wndEdit2Bar.InvalidateRect(NULL);
		m_wndFileBar.ShowWindow(SW_SHOW);
		m_wndFileBar.InvalidateRect(NULL);
		m_wndFormatBar.ShowWindow(SW_SHOW);
		m_wndFormatBar.InvalidateRect(NULL);
      bRetVal =  TRUE;
   }
	AfxSetResourceHandle(hCurIns);
   return bRetVal;
}


extern "C" BOOL WINAPI LoadToolbar(CWnd* pParent, CPalette* pPalette)
{
	pOurPal = pPalette;
	return LoadToolbars(pParent);
}

extern "C" CToolBar* WINAPI GetToolbar(UINT nID)
{
	switch(nID)
	{
		case TOOLBAR_FORMAT:
			return &m_wndFormatBar;
		case TOOLBAR_FILE:
			return &m_wndFileBar;
		case TOOLBAR_EDIT1:
			return &m_wndEdit1Bar;
		case TOOLBAR_EDIT2:
			return &m_wndEdit2Bar;
		default:
			return NULL;
	}
	return NULL;
}



