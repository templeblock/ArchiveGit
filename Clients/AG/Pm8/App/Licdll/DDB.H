/*
// $Workfile: DDB.H $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/DDB.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:20:18   Fred
// Initial revision.
// 
//    Rev 1.0   20 May 1997 10:40:34   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/
/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/DDB.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   08 Sep 1996 09:40:48   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 13:35:36   FRED
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 09:52:08   FRED
//  
// 
//    Rev 1.3   03 Jan 1996 12:56:30   JAY
// Round 2
// 
//    Rev 1.2   21 Dec 1995 10:26:58   JAY
//  
// 
//    Rev 1.1   21 Dec 1995 09:57:22   JAY
//  
// 
//    Rev 1.0   19 Dec 1995 15:28:46   JAY
// Initial revision.
// 
//    Rev 1.1   04 Dec 1995 12:40:50   JAY
//  
// 
//    Rev 1.0   01 Dec 1995 13:07:22   JAY
// Initial revision.
// 
//    Rev 1.0   05 Nov 1994 09:23:18   FRED
// Initial revision.
// 
//    Rev 1.0   23 Sep 1994 12:29:28   FRED
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
		LPSTR lpszFileName,					// name of file containing DIB data
		HDC hDC,									// device context to use for call to CreateDIBitmap()
		BOOL fUseDIBPalette,					// flag indicating if DIB bitmap should be used for conversion
		HPALETTE hPalette = NULL);
	CDDB(										// build a CDDB object from a DIB in a file
		LPSTR lpszFileName1,					// name of file containing 1 bit/pixel DIB data
		LPSTR lpszFileName4,					// name of file containing 4 bit/pixel DIB data
		LPSTR lpszFileName8,					// name of file containing 8 bit/pixel DIB data
		LPSTR lpszFileName24,				// name of file containing 24 bit/pixel DIB data
		HDC hDC,									// device context to use for call to CreateDIBitmap()
		BOOL fUseDIBPalette);				// flag indicating if DIB bitmap should be used for conversion
	CDDB(										// build a CDDB object from a DIB in memory
		LPBITMAPINFOHEADER lpHeader,		// pointer to BITMAPINFOHEADER structure
		HDC hDC,									// device context to use for call to CreateDIBitmap()
		BOOL fUseDIBPalette,					// flag indicating if DIB bitmap should be used for conversion
		HPALETTE hPalette = NULL);			// Palette to use
// Attributes
public:

// Operations
public:

	HBITMAP GetBitmap(void);			// return the DDB handle
	HPALETTE GetPalette(void);			// return the palette handle
	BOOL IsValid(void);					// check if the object is valid
	void OwnsPalette(BOOL fOwnsPalette)
		{ m_fOwnsPalette = fOwnsPalette; }
	
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
		LPSTR lpszFileName,					// name of file containing DIB data
		HDC hDC,									// device context to use for call to CreateDIBitmap()
		BOOL fUseDIBPalette,					// flag indicating if DIB bitmap should be used for conversion
		HPALETTE hPalette = NULL);			// Palette to use
	BOOL ConstructDDB(					// construct a DDB from the DIB data
		LPBITMAPINFOHEADER lpHeader,		// pointer to BITMAPINFOHEADER structure
		HDC hDC,									// device context to use for call to CreateDIBitmap()
		BOOL fUseDIBPalette,					// flag indicating if DIB bitmap should be used for conversion
		HPALETTE hPalette = NULL);			// Palette to use
	BOOL Release(BOOL fConstructing = FALSE);
	HBITMAP m_hBitmap;
	HPALETTE m_hPalette;
	BOOL m_fOwnsPalette;
};

#endif		// ifndef _DDB_H_
