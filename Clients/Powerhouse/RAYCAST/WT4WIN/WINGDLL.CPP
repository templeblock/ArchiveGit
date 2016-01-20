/////////////////////////////////////////////////////////////////////////////
// WINGDLL.CPP
//
// Interface class for WinG.
//
// Uses LoadLibrary and GetProcAddress to populate the class.
//
/////////////////////////////////////////////////////////////////////////////

#include <wing.h>          //#includes WINDOWS.H
#include "wingdll.h"

WinGdll::WinGdll()
{
   m_bLoaded = FALSE;
}

WinGdll::~WinGdll()
{
   if (m_bLoaded)
      FreeLibrary (m_hLib);
   m_bLoaded = FALSE;
}

BOOL WinGdll::Load()
{
#if defined(WIN32) || defined(_WIN32)
	m_hLib = LoadLibrary("wing32.dll"); 
	if( m_hLib == NULL ) {
		return FALSE;
	}
#else
	m_hLib = LoadLibrary("wing.dll"); 
	if( m_hLib < HINSTANCE_ERROR ) {
		return FALSE;
	}
#endif

      //link the DLL's functions to our function pointers.

   pCreateDC = (wingCreateDC) GetProcAddress (m_hLib, "WinGCreateDC");
	if (pCreateDC == NULL) {
		FreeLibrary (m_hLib);
		return FALSE;
	}

   pRecommendDIBFormat = (wingRecommendDIBFormat)
                        GetProcAddress (m_hLib, "WinGRecommendDIBFormat");
	if (pRecommendDIBFormat == NULL) {
		FreeLibrary (m_hLib);
		return FALSE;
	}

   pCreateBitmap = (wingCreateBitmap) 
                        GetProcAddress (m_hLib, "WinGCreateBitmap");
	if (pCreateBitmap == NULL) {
		FreeLibrary (m_hLib);
		return FALSE;
	}

   pGetDIBPointer = (wingGetDIBPointer) 
                        GetProcAddress (m_hLib, "WinGGetDIBPointer");
	if (pGetDIBPointer == NULL) {
		FreeLibrary (m_hLib);
		return FALSE;
	}

   pGetDIBColorTable = (wingGetDIBColorTable)
                        GetProcAddress (m_hLib, "WinGGetDIBColorTable");
	if (pGetDIBColorTable == NULL) {
		FreeLibrary (m_hLib);
		return FALSE;
	}

   pSetDIBColorTable = (wingSetDIBColorTable)
                        GetProcAddress (m_hLib, "WinGSetDIBColorTable");
	if (pSetDIBColorTable == NULL) {
		FreeLibrary (m_hLib);
		return FALSE;
	}


   pCreateHalftonePalette = (wingCreateHalftonePalette)
                        GetProcAddress (m_hLib, "WinGCreateHalftonePalette");
	if (pCreateHalftonePalette == NULL) {
		FreeLibrary (m_hLib);
		return FALSE;
	}

   pCreateHalftoneBrush = (wingCreateHalftoneBrush)
                        GetProcAddress (m_hLib, "WinGCreateHalftoneBrush");
	if (pCreateHalftoneBrush == NULL) {
		FreeLibrary (m_hLib);
		return FALSE;
	}

   pBitBlt = (wingBitBlt) GetProcAddress (m_hLib, "WinGBitBlt");
	if (pBitBlt == NULL) {
		FreeLibrary (m_hLib);
		return FALSE;
	}

   pStretchBlt = (wingStretchBlt) GetProcAddress (m_hLib, "WinGStretchBlt");
	if (pStretchBlt == NULL) {
		FreeLibrary (m_hLib);
		return FALSE;
	}

   m_bLoaded = TRUE;       //ready for business!
   return TRUE;
}

BOOL WinGdll::Free()
{
   if (m_bLoaded)
		FreeLibrary (m_hLib);
   m_bLoaded = FALSE;
   return TRUE;
}


