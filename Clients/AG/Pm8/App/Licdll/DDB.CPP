/*
// $Workfile: DDB.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/DDB.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:19:44   Fred
// Initial revision.
// 
//    Rev 1.0   20 May 1997 10:41:36   johno
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
// $Log: /PM8/App/LICDLL/DDB.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   08 Sep 1996 09:40:26   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 13:35:36   FRED
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 09:52:06   FRED
//  
// 
//    Rev 1.3   03 Jan 1996 12:56:22   JAY
// Round 2
// 
//    Rev 1.2   21 Dec 1995 10:26:58   JAY
//  
// 
//    Rev 1.1   21 Dec 1995 09:57:18   JAY
//  
// 
//    Rev 1.0   19 Dec 1995 15:27:54   JAY
// Initial revision.
// 
//    Rev 1.1   04 Dec 1995 12:40:50   JAY
//  
// 
//    Rev 1.0   01 Dec 1995 12:50:04   JAY
// Initial revision.
// 
//    Rev 1.0   05 Nov 1994 09:21:58   FRED
// Initial revision.
// 
//    Rev 1.0   23 Sep 1994 12:29:36   FRED
// Initial revision.
*/ 

#pragma warning(disable:4100)
#pragma warning(disable:4505)
#pragma warning(disable:4704)

#ifndef NOSTRICT
#define STRICT
#endif

#include "stdafx.h"
#include "ddb.h"

CDDB::CDDB(										// build a CDDB object from a DIB in a resource file
	HINSTANCE hInstance,							// instance conatining DIB resource
	LPCSTR lpResource,							// DIB resource identifier
	LPCSTR wResourceType,						// type of resource, usually RT_BITMAP
	HDC hDC,											// device context to use for call to CreateDIBitmap()
	BOOL fUseDIBPalette)							// flag indicating if DIB bitmap should be used for conversion
{
	Release(TRUE);
	
	ConstructDDBFromResource(				// build a CDDB object from a DIB in a resource file
		hInstance,									// instance conatining DIB resource
		lpResource,									// DIB resource identifier
		wResourceType,								// type of resource, usually RT_BITMAP
		hDC,											// device context to use for call to CreateDIBitmap()
		fUseDIBPalette);							// flag indicating if DIB bitmap should be used for conversion
}
	
CDDB::CDDB(										// build a CDDB object from a DIB in a resource file
	HINSTANCE hInstance,							// instance conatining DIB resource
	LPCSTR lpResource1,							// DIB resource identifier for 1 bit/pixel DIB
	LPCSTR lpResource4,							// DIB resource identifier for 4 bit/pixel DIB
	LPCSTR lpResource8,							// DIB resource identifier for 8 bit/pixel DIB
	LPCSTR lpResource24,							// DIB resource identifier for 24 bit/pixel DIB
	LPCSTR wResourceType,						// type of resource, usually RT_BITMAP
	HDC hDC,											// device context to use for call to CreateDIBitmap()
	BOOL fUseDIBPalette)							// flag indicating if DIB bitmap should be used for conversion
{
	Release(TRUE);
	
	/*
	// Get the best resource choice based on the color depth of the device context.
	*/
	
	LPSTR lpResource;
	
	if ((lpResource = (LPSTR)ChooseColor(hDC, lpResource1, lpResource4, lpResource8, lpResource24)) != NULL)
	{
		ConstructDDBFromResource(			// build a CDDB object from a DIB in a resource file
			hInstance,								// instance conatining DIB resource
			lpResource,								// DIB resource identifier
			wResourceType,							// type of resource, usually RT_BITMAP
			hDC,										// device context to use for call to CreateDIBitmap()
			fUseDIBPalette);						// flag indicating if DIB bitmap should be used for conversion
	}
}
	
CDDB::CDDB(										// build a CDDB object from a DIB in a file
	LPSTR lpszFileName,							// name of file containing DIB data
	HDC hDC,											// device context to use for call to CreateDIBitmap()
	BOOL fUseDIBPalette,							// flag indicating if DIB bitmap should be used for conversion
	HPALETTE hPalette /*=NULL*/)
{
	Release(TRUE);
	
	ConstructDDBFromFile(					// build a CDDB object from a DIB in a file
		lpszFileName,								// name of file containing DIB data
		hDC,											// device context to use for call to CreateDIBitmap()
		fUseDIBPalette,							// flag indicating if DIB bitmap should be used for conversion
		hPalette);
}

CDDB::CDDB(										// build a CDDB object from a DIB in a file
	LPSTR lpszFileName1,							// name of file containing 1 bit/pixel DIB data
	LPSTR lpszFileName4,							// name of file containing 4 bit/pixel DIB data
	LPSTR lpszFileName8,							// name of file containing 8 bit/pixel DIB data
	LPSTR lpszFileName24,						// name of file containing 24 bit/pixel DIB data
	HDC hDC,											// device context to use for call to CreateDIBitmap()
	BOOL fUseDIBPalette)							// flag indicating if DIB bitmap should be used for conversion
{
	Release(TRUE);
	
	/*
	// Get the best resource choice based on the color depth of the device context.
	*/
	
	LPSTR lpszFileName;
	
	if ((lpszFileName = (LPSTR)ChooseColor(hDC, lpszFileName1, lpszFileName4, lpszFileName8, lpszFileName24)) != NULL)
	{
		ConstructDDBFromFile(				// build a CDDB object from a DIB in a file
			lpszFileName,							// name of file containing DIB data
			hDC,										// device context to use for call to CreateDIBitmap()
			fUseDIBPalette);						// flag indicating if DIB bitmap should be used for conversion
	}
}

CDDB::CDDB(										// build a CDDB object from a DIB in a file
	LPBITMAPINFOHEADER lpHeader,				// pointer to BITMAPINFOHEADER structure
	HDC hDC,											// device context to use for call to CreateDIBitmap()
	BOOL fUseDIBPalette,							// flag indicating if DIB bitmap should be used for conversion
	HPALETTE hPalette) /*=NULL*/				// Palette to use
{
	Release(TRUE);
	
	ConstructDDB(
		lpHeader,									// pointer to DIB data
		hDC,											// device context to use for call to CreateDIBitmap()
		fUseDIBPalette,							// flag indicating if DIB bitmap should be used for conversion
		hPalette);
}

CDDB::~CDDB(void)								// destroy a CDDB object
{
	Release();
}

HBITMAP CDDB::GetBitmap(void)				// return the DDB handle
{
	return m_hBitmap;
}

HPALETTE CDDB::GetPalette(void)			// return the palette handle
{
	return m_hPalette;
}

BOOL CDDB::IsValid(void)					// check if the object is valid
{
	return (m_hBitmap != NULL) && (m_hPalette != NULL);
}
	
LPCSTR CDDB::ChooseColor(					// determine which best match for color depth
	HDC hDC,											// device context to use to determine number of colors
	LPCSTR lpResource1,							// choice for 1 bit/pixel
	LPCSTR lpResource4,							// choice for 4 bit/pixel
	LPCSTR lpResource8,							// choice for 8 bit/pixel
	LPCSTR lpResource24)							// choice for 24 bit/pixel
{
	LPCSTR lpResource[4];
	
	lpResource[0] = lpResource1;
	lpResource[1] = lpResource4;
	lpResource[2] = lpResource8;
	lpResource[3] = lpResource24;
	
	/*
	// Compute the index of the desired pointer.
	*/
	
	UINT wIndex;
	
	switch (GetDeviceCaps(hDC, BITSPIXEL)*GetDeviceCaps(hDC, PLANES))
	{
		case 0:
		case 1:	wIndex = 0;	break;
		case 4:	wIndex = 1;	break;
		case 8:	wIndex = 2;	break;
		case 24:	wIndex = 3;	break;
		default:	wIndex = 2;	break;
	}
	
	/*
	// If the desired pointer is not available, search down.
	*/
	
	if (lpResource[wIndex] == NULL)
	{
		int i;
	
		for (i = wIndex-1; i >= 0; i--)
		{
			if (lpResource[i] != NULL)
			{
				wIndex = i;
				break;
			}
		}
	}
		
	/*
	// If the desired pointer is still not available, search up.
	*/
	
	if (lpResource[wIndex] == NULL)
	{
		int i;
	
		for (i = wIndex+1; i < sizeof(lpResource)/sizeof(lpResource[0]); i++)
		{
			if (lpResource[i] != NULL)
			{
				wIndex = i;
				break;
			}
		}
	}

	/*
	// Return the pointer or NULL if none was found.
	*/
		
	return lpResource[wIndex];
}
	
UINT CDDB::PaletteEntryCount(				// determine the number of palette entries in a DIB
	LPBITMAPINFOHEADER lpHeader)				// pointer to BITMAPINFOHEADER structure
{
	UINT wNumberOfPaletteEntries = 0;
		
	if (lpHeader->biClrUsed != 0)
	{
		wNumberOfPaletteEntries = (UINT)(lpHeader->biClrUsed);
	}
			
	else
	{
		switch (lpHeader->biBitCount)
		{
			case 1:	wNumberOfPaletteEntries = 2;		break;
			case 4:	wNumberOfPaletteEntries = 16;		break;
			case 8:	wNumberOfPaletteEntries = 256;	break;
		}
	}
	
	return wNumberOfPaletteEntries;
}

BOOL CDDB::ConstructDDBFromResource(	// build a CDDB object from a DIB in a resource file
	HINSTANCE hInstance,							// instance conatining DIB resource
	LPCSTR lpResource,							// DIB resource identifier
	LPCSTR wResourceType,						// type of resource, usually RT_BITMAP
	HDC hDC,											// device context to use for call to CreateDIBitmap()
	BOOL fUseDIBPalette)							// flag indicating if DIB bitmap should be used for conversion
{
	/*
	// Find the resource, load it, and lock it.
	*/
		
	HRSRC hrDIB;
		
	if ((hrDIB = FindResource(hInstance, lpResource, wResourceType)) != NULL)
	{
		HGLOBAL hDIB;
					
		if ((hDIB = LoadResource(hInstance, hrDIB)) != NULL)
		{
			LPBITMAPINFOHEADER lpHeader;
			
			if ((lpHeader = (LPBITMAPINFOHEADER)LockResource(hDIB)) != NULL)
			{
				/*
				// Validate the header size.
				*/
					
				if (lpHeader->biSize == sizeof(BITMAPINFOHEADER))
				{
					/*
					// Create the DDB.
					*/
						
					ConstructDDB(			// construct a DDB from the DIB data
						lpHeader,				// pointer to BITMAPINFOHEADER structure
						hDC,						// device context to use for call to CreateDIBitmap()
						fUseDIBPalette);		// flag indicating if DIB bitmap should be used for conversion
				}
					
				UnlockResource(hDIB);
				lpHeader = NULL;
			}
						
			FreeResource(hDIB);
			hDIB = NULL;
		}
	}
	
	return IsValid();
}

BOOL CDDB::ConstructDDBFromFile(		// build a CDDB object from a DIB in a file
	LPSTR lpszFileName,						// name of file containing DIB data
	HDC hDC,										// device context to use for call to CreateDIBitmap()
	BOOL fUseDIBPalette,						// flag indicating if DIB bitmap should be used for conversion
	HPALETTE hPalette)
{
	/*
	// Attempt to open the file and read in the BITMAPFILEHEADER structure.
	*/
		
	OFSTRUCT of;
	HFILE hfDIB;
		
	if ((hfDIB = OpenFile(lpszFileName, &of, OF_READ|OF_SHARE_DENY_WRITE)) != HFILE_ERROR)
	{
		BITMAPFILEHEADER BitmapFileHeader;
			
		if (_lread(hfDIB, &BitmapFileHeader, sizeof(BitmapFileHeader)) == sizeof(BitmapFileHeader))
		{
			/*
			// Validate the header.
			*/
				
			if (BitmapFileHeader.bfType == 0x4d42)
			{
				/*
				// Read in the rest of the file.
				*/
					
				HGLOBAL hDIB;
				long dwDIBSize;
					
				dwDIBSize = BitmapFileHeader.bfSize-sizeof(BitmapFileHeader);
					
				if ((hDIB = GlobalAlloc(GMEM_MOVEABLE, dwDIBSize)) != NULL)
				{
					LPBITMAPINFOHEADER lpHeader;
			
					if ((lpHeader = (LPBITMAPINFOHEADER)GlobalLock(hDIB)) != NULL)
					{
						if (_hread(hfDIB, lpHeader, dwDIBSize) == dwDIBSize)
						{
							/*
							// Validate the header size.
							*/
								
							if (lpHeader->biSize == sizeof(BITMAPINFOHEADER))
							{
								/*
								// Create the DDB.
								*/
									
								ConstructDDB(			// construct a DDB from the DIB data
									lpHeader,				// pointer to BITMAPINFOHEADER structure
									hDC,						// device context to use for call to CreateDIBitmap()
									fUseDIBPalette,		// flag indicating if DIB bitmap should be used for conversion
									hPalette);
							}
						}
							
						GlobalUnlock(hDIB);
						lpHeader = NULL;
					}
						
					GlobalFree(hDIB);
					hDIB = NULL;
				}
			}
		}
			
		_lclose(hfDIB);
		hfDIB = HFILE_ERROR;
	}
	
	return IsValid();
}

BOOL CDDB::ConstructDDB(					// construct a DDB from the DIB data
	LPBITMAPINFOHEADER lpHeader,			// pointer to BITMAPINFOHEADER structure
	HDC hDC,										// device context to use for call to CreateDIBitmap()
	BOOL fUseDIBPalette,						// flag indicating if DIB bitmap should be used for conversion
	HPALETTE hPalette /*=NULL*/)			// Palette to use
{
	/*
	// Build a palette containing the DIB color table. For 24 bit dibs, a default
	// 256 entry palette is built (RGB)=(8*8*4).
	*/

	if ((m_hPalette = hPalette) == NULL)
	{
		UINT wNumberOfPaletteEntries;

		if ((wNumberOfPaletteEntries = PaletteEntryCount(lpHeader)) == 0)
		{
			if (lpHeader->biBitCount == 24)
			{
				wNumberOfPaletteEntries = 256;
			}
		}
									
		if (wNumberOfPaletteEntries > 0)
		{
			LOGPALETTE FAR *lpPalette;
								
			if ((lpPalette = (LOGPALETTE FAR *)new far char[sizeof(LOGPALETTE)+wNumberOfPaletteEntries*sizeof(PALETTEENTRY)]) != NULL)
			{
				lpPalette->palNumEntries = (WORD)wNumberOfPaletteEntries;
				lpPalette->palVersion    = 0x0300;
					
				if (lpHeader->biBitCount != 24)
				{
					RGBQUAD FAR *lpRGB;
									
					lpRGB = (RGBQUAD FAR *)(((LPBYTE)lpHeader)+(WORD)(lpHeader->biSize));
											
					UINT i;
												
					for (i = 0; i < wNumberOfPaletteEntries; i++)
					{
						lpPalette->palPalEntry[i].peRed   = lpRGB[i].rgbRed;
						lpPalette->palPalEntry[i].peGreen = lpRGB[i].rgbGreen;
						lpPalette->palPalEntry[i].peBlue  = lpRGB[i].rgbBlue;
						lpPalette->palPalEntry[i].peFlags = (BYTE)0;
					}
				}
											
				else
				{
					BYTE Red = 0;
					BYTE Green = 0;
					BYTE Blue = 0;
												
					UINT i;
												
					for (i = 0; i < wNumberOfPaletteEntries; i++)
					{
						lpPalette->palPalEntry[i].peRed   = Red;
						lpPalette->palPalEntry[i].peGreen = Green;
						lpPalette->palPalEntry[i].peBlue  = Blue;
						lpPalette->palPalEntry[i].peFlags = (BYTE)0;
													
						if ((Red += 32) == 0)
						{
							if ((Green += 32) == 0)
							{
								Blue += 64;
							}
						}
					}
				}
					
				m_hPalette = CreatePalette(lpPalette);
											
				delete [] lpPalette;
				lpPalette = NULL;
			}
		}
	}

	/*
	// If the palette was successfully created, then go ahead and create the DDB.
	*/

	if (m_hPalette != NULL)
	{
		/*
		// If the user wants to use the DIB palette to convert the DIB to a DDB, then select it now.
		*/

		HPALETTE hOldPalette = NULL;
									
		if (fUseDIBPalette)
		{
			/*
			// Note: The DIB palette is selected as a background palette. This should
			// make the DIB->DDB conversion work without affecting the system colors.
			*/
			
			if ((hOldPalette = SelectPalette(hDC, m_hPalette, TRUE)) != NULL)
			{
				RealizePalette(hDC);
			}
		}
		
		if ((!fUseDIBPalette) || (hOldPalette != NULL))
		{
			/*
			// Create the DDB.
			*/
			
			m_hBitmap = CreateDIBitmap(
								hDC,
								lpHeader,
								CBM_INIT,
								((LPBYTE)lpHeader)+((UINT)lpHeader->biSize)+(PaletteEntryCount(lpHeader)*sizeof(RGBQUAD)),
								(LPBITMAPINFO)lpHeader,
								DIB_RGB_COLORS);
																	
		}
		
		if (hOldPalette != NULL)
		{
			SelectPalette(hDC, hOldPalette, FALSE);
		}
	}

	return IsValid();
}
	
BOOL CDDB::Release(BOOL fConstructing)
{
	if (fConstructing)
	{
		m_fOwnsPalette = TRUE;
		m_hBitmap = NULL;
		m_hPalette = NULL;
	}
	
	if (m_hBitmap != NULL)
	{
		DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}
	
	if (m_hPalette != NULL)
	{
		if (m_fOwnsPalette)
		{
			DeleteObject(m_hPalette);
		}
		m_hPalette = NULL;
	}
	
	return TRUE;
}
