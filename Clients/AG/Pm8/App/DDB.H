/*
// $Workfile: DDB.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/DDB.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 2     6/29/98 9:34a Mwilson
// changed pointers to string to const pointers
// 
//    Rev 1.0   14 Aug 1997 15:20:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:32   Fred
// Initial revision.
// 
//    Rev 1.3   25 Feb 1997 10:57:10   Jay
// Extra paranoia.
// 
//    Rev 1.2   24 Feb 1997 13:52:02   johno
// Added Detach function
// with option to delete the palette
// 
//    Rev 1.1   15 Aug 1996 12:55:34   Fred
// Remembers resource that was used for DDB
// 
//    Rev 1.0   14 Mar 1996 13:26:56   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:50   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:39:24   JAY
// Reverted. New series.
// 
//    Rev 1.0   15 Jun 1994 18:59:18   FRED
// Initial revision.
*/ 

#ifndef _DDB_H_
#define _DDB_H_

#include <windows.h>

/*
// Class for building a device dependent bitmap (DDB) from a DIB resource.
*/

class far CDDB
{
// Construction
public:
	CDDB(										// build a CDDB object from a DIB in a resource file
		HINSTANCE hInstance,					// instance conatining DIB resource
		LPCSTR lpResource,					// DIB resource identifier
		LPCSTR wResourceType,				// type of resource, usually RT_BITMAP
		HDC hDC,									// device context to use for call to CreateDIBitmap()
		BOOL fUseDIBPalette);				// flag indicating if DIB bitmap should be used for conversion
	CDDB(										// build a CDDB object from a DIB in a resource file
		HINSTANCE hInstance,					// instance conatining DIB resource
		LPCSTR lpResource1,					// DIB resource identifier for 1 bit/pixel DIB
		LPCSTR lpResource4,					// DIB resource identifier for 4 bit/pixel DIB
		LPCSTR lpResource8,					// DIB resource identifier for 8 bit/pixel DIB
		LPCSTR lpResource24,					// DIB resource identifier for 24 bit/pixel DIB
		LPCSTR wResourceType,				// type of resource, usually RT_BITMAP
		HDC hDC,									// device context to use for call to CreateDIBitmap()
		BOOL fUseDIBPalette);				// flag indicating if DIB bitmap should be used for conversion
	CDDB(										// build a CDDB object from a DIB in a file
		LPCSTR lpszFileName,					// name of file containing DIB data
		HDC hDC,									// device context to use for call to CreateDIBitmap()
		BOOL fUseDIBPalette);				// flag indicating if DIB bitmap should be used for conversion
	CDDB(										// build a CDDB object from a DIB in a file
		LPCSTR lpszFileName1,					// name of file containing 1 bit/pixel DIB data
		LPCSTR lpszFileName4,					// name of file containing 4 bit/pixel DIB data
		LPCSTR lpszFileName8,					// name of file containing 8 bit/pixel DIB data
		LPCSTR lpszFileName24,				// name of file containing 24 bit/pixel DIB data
		HDC hDC,									// device context to use for call to CreateDIBitmap()
		BOOL fUseDIBPalette);				// flag indicating if DIB bitmap should be used for conversion

// Attributes
public:

// Operations
public:

	HBITMAP GetBitmap(void);			// return the DDB handle
	HPALETTE GetPalette(void);			// return the palette handle
	BOOL IsValid(void);					// check if the object is valid
	LPCSTR GetResource(void)			// return the resource used to create the DDB
		{ return m_lpResource; }
	
   void
   Detach (BOOL DeletePalette = TRUE)
   {
      HPALETTE    hPalette;
      
      hPalette = GetPalette ();
      Release (TRUE);
      if (hPalette != NULL && DeletePalette == TRUE)
         ::DeleteObject (hPalette);
   }

// Implementation
public:
	virtual ~CDDB(void);					// destroy a CDDB object
	
private:
	LPCSTR ChooseColor(					// determine which best match for color depth
		HDC hDC,									// device context to use to determine number of colors
		LPCSTR lpResource1,					// choice for 1 bit/pixel
		LPCSTR lpResource4,					// choice for 4 bit/pixel
		LPCSTR lpResource8,					// choice for 8 bit/pixel
		LPCSTR lpResource24);				// choice for 24 bit/pixel
	UINT PaletteEntryCount(				// determine the number of palette entries in a DIB
		LPBITMAPINFOHEADER lpHeader);		// pointer to BITMAPINFOHEADER structure
	BOOL ConstructDDBFromResource(	// build a CDDB object from a DIB in a resource file
		HINSTANCE hInstance,					// instance conatining DIB resource
		LPCSTR lpResource,					// DIB resource identifier
		LPCSTR wResourceType,				// type of resource, usually RT_BITMAP
		HDC hDC,									// device context to use for call to CreateDIBitmap()
		BOOL fUseDIBPalette);				// flag indicating if DIB bitmap should be used for conversion
	BOOL ConstructDDBFromFile(			// build a CDDB object from a DIB in a file
		LPCSTR lpszFileName,					// name of file containing DIB data
		HDC hDC,									// device context to use for call to CreateDIBitmap()
		BOOL fUseDIBPalette);				// flag indicating if DIB bitmap should be used for conversion
	BOOL ConstructDDB(					// construct a DDB from the DIB data
		LPBITMAPINFOHEADER lpHeader,		// pointer to BITMAPINFOHEADER structure
		HDC hDC,									// device context to use for call to CreateDIBitmap()
		BOOL fUseDIBPalette);				// flag indicating if DIB bitmap should be used for conversion
	BOOL Release(BOOL fConstructing = FALSE);
	HBITMAP m_hBitmap;
	HPALETTE m_hPalette;
	LPCSTR m_lpResource;
};

#endif		// ifndef _DDB_H_
