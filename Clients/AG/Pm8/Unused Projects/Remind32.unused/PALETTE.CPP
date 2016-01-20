/*
// $Workfile: PALETTE.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:27p $
//
// Copyright © 1997 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/PALETTE.CPP $
// 
// 1     3/03/99 6:27p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:34   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:55:58   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "palette.h"

// Our special PrintMaster palette.
CPalette *pOurPal = NULL;

const RGBQUAD* GetIdentityPalette(void)
{
	// The identity pallete

	static const RGBQUAD BASED_CODE IdentityPalette[256] =
	{
		//  B    G    R
		{   0,   0,   0 },
		{   0,   0, 128 },
		{   0, 128,   0 },
		{   0, 128, 128 },
		{ 128,   0,   0 },
		{ 128,   0, 128 },
		{ 128, 128,   0 },
		{ 192, 192, 192 },
		{ 192, 220, 192 },
		{ 240, 202, 166 },
		{   4,   4,   4 },
		{   8,   8,   8 },
		{  12,  12,  12 },
		{  17,  17,  17 },
		{  22,  22,  22 },
		{  28,  28,  28 },
		{  34,  34,  34 },
		{  41,  41,  41 },
		{  85,  85,  85 },
		{  77,  77,  77 },
		{  66,  66,  66 },
		{  57,  57,  57 },
		{ 129, 129, 129 },
		{   0,   0, 129 },
		{   0, 129,   0 },
		{   0, 129, 129 },
		{ 129,   0,   0 },
		{ 129,   0, 129 },
		{ 129, 129,   0 },
		{   0,   0,  51 },
		{   0,   0, 102 },
		{   0,   0, 153 },
		{   0,   0, 204 },
		{   0,  51,   0 },
		{   0,  51,  51 },
		{   0,  51, 102 },
		{   0,  51, 153 },
		{   0,  51, 204 },
		{   0,  51, 255 },
		{   0, 102,   0 },
		{   0, 102,  51 },
		{   0, 102, 102 },
		{   0, 102, 153 },
		{   0, 102, 204 },
		{   0, 102, 255 },
		{   0, 153,   0 },
		{   0, 153,  51 },
		{   0, 153, 102 },
		{   0, 153, 153 },
		{   0, 153, 204 },
		{   0, 153, 255 },
		{   0, 204,   0 },
		{   0, 204,  51 },
		{   0, 204, 102 },
		{   0, 204, 153 },
		{   0, 204, 204 },
		{   0, 204, 255 },
		{   0, 255, 102 },
		{   0, 255, 153 },
		{   0, 255, 204 },
		{  51,   0,   0 },
		{  51,   0,  51 },
		{  51,   0, 102 },
		{  51,   0, 153 },
		{  51,   0, 204 },
		{  51,   0, 255 },
		{  51,  51,   0 },
		{  51,  51,  51 },
		{  51,  51, 102 },
		{  51,  51, 153 },
		{  51,  51, 204 },
		{  51,  51, 255 },
		{  51, 102,   0 },
		{  51, 102,  51 },
		{  51, 102, 102 },
		{  51, 102, 153 },
		{  51, 102, 204 },
		{  51, 102, 255 },
		{  51, 153,   0 },
		{  51, 153,  51 },
		{  51, 153, 102 },
		{  51, 153, 153 },
		{  51, 153, 204 },
		{  51, 153, 255 },
		{  51, 204,   0 },
		{  51, 204,  51 },
		{  51, 204, 102 },
		{  51, 204, 153 },
		{  51, 204, 204 },
		{  51, 204, 255 },
		{  51, 255,  51 },
		{  51, 255, 102 },
		{  51, 255, 153 },
		{  51, 255, 204 },
		{  51, 255, 255 },
		{ 102,   0,   0 },
		{ 102,   0,  51 },
		{ 102,   0, 102 },
		{ 102,   0, 153 },
		{ 102,   0, 204 },
		{ 102,   0, 255 },
		{ 102,  51,   0 },
		{ 102,  51,  51 },
		{ 102,  51, 102 },
		{ 102,  51, 153 },
		{ 102,  51, 204 },
		{ 102,  51, 255 },
		{ 102, 102,   0 },
		{ 102, 102,  51 },
		{ 102, 102, 102 },
		{ 102, 102, 153 },
		{ 102, 102, 204 },
		{ 102, 153,   0 },
		{ 102, 153,  51 },
		{ 102, 153, 102 },
		{ 102, 153, 153 },
		{ 102, 153, 204 },
		{ 102, 153, 255 },
		{ 102, 204,   0 },
		{ 102, 204,  51 },
		{ 102, 204, 153 },
		{ 102, 204, 204 },
		{ 102, 204, 255 },
		{ 102, 255,   0 },
		{ 102, 255,  51 },
		{ 102, 255, 153 },
		{ 102, 255, 204 },
		{ 204,   0, 255 },
		{ 255,   0, 204 },
		{ 153, 153,   0 },
		{ 153,  51, 153 },
		{ 153,   0, 153 },
		{ 153,   0, 204 },
		{ 153,   0,   0 },
		{ 153,  51,  51 },
		{ 153,   0, 102 },
		{ 153,  51, 204 },
		{ 153,   0, 255 },
		{ 153, 102,   0 },
		{ 153, 102,  51 },
		{ 153,  51, 102 },
		{ 153, 102, 153 },
		{ 153, 102, 204 },
		{ 153,  51, 255 },
		{ 153, 153,  51 },
		{ 153, 153, 102 },
		{ 153, 153, 153 },
		{ 153, 153, 204 },
		{ 153, 153, 255 },
		{ 153, 204,   0 },
		{ 153, 204,  51 },
		{ 102, 204, 102 },
		{ 153, 204, 153 },
		{ 153, 204, 204 },
		{ 153, 204, 255 },
		{ 153, 255,   0 },
		{ 153, 255,  51 },
		{ 153, 204, 102 },
		{ 153, 255, 153 },
		{ 153, 255, 204 },
		{ 153, 255, 255 },
		{ 204,   0,   0 },
		{ 153,   0,  51 },
		{ 204,   0, 102 },
		{ 204,   0, 153 },
		{ 204,   0, 204 },
		{ 153,  51,   0 },
		{ 204,  51,  51 },
		{ 204,  51, 102 },
		{ 204,  51, 153 },
		{ 204,  51, 204 },
		{ 204,  51, 255 },
		{ 204, 102,   0 },
		{ 204, 102,  51 },
		{ 153, 102, 102 },
		{ 204, 102, 153 },
		{ 204, 102, 204 },
		{ 153, 102, 255 },
		{ 204, 153,   0 },
		{ 204, 153,  51 },
		{ 204, 153, 102 },
		{ 204, 153, 153 },
		{ 204, 153, 204 },
		{ 204, 153, 255 },
		{ 204, 204,   0 },
		{ 204, 204,  51 },
		{ 204, 204, 102 },
		{ 204, 204, 153 },
		{ 204, 204, 204 },
		{ 204, 204, 255 },
		{ 204, 255,   0 },
		{ 204, 255,  51 },
		{ 153, 255, 102 },
		{ 204, 255, 153 },
		{ 204, 255, 204 },
		{ 204, 255, 255 },
		{ 204,   0,  51 },
		{ 255,   0, 102 },
		{ 255,   0, 153 },
		{ 204,  51,   0 },
		{ 255,  51,  51 },
		{ 255,  51, 102 },
		{ 255,  51, 153 },
		{ 255,  51, 204 },
		{ 255,  51, 255 },
		{ 255, 102,   0 },
		{ 255, 102,  51 },
		{ 204, 102, 102 },
		{ 255, 102, 153 },
		{ 255, 102, 204 },
		{ 204, 102, 255 },
		{ 255, 153,   0 },
		{ 255, 153,  51 },
		{ 255, 153, 102 },
		{ 255, 153, 153 },
		{ 255, 153, 204 },
		{ 255, 153, 255 },
		{ 255, 204,   0 },
		{ 255, 204,  51 },
		{ 255, 204, 102 },
		{ 255, 204, 153 },
		{ 255, 204, 204 },
		{ 255, 204, 255 },
		{ 255, 255,  51 },
		{ 204, 255, 102 },
		{ 255, 255, 153 },
		{ 255, 255, 204 },
		{ 102, 102, 255 },
		{ 102, 255, 102 },
		{ 102, 255, 255 },
		{ 255, 102, 102 },
		{ 255, 102, 255 },
		{ 255, 255, 102 },
		{ 193, 193, 193 },
		{  95,  95,  95 },
		{ 119, 119, 119 },
		{ 134, 134, 134 },
		{ 150, 150, 150 },
		{ 203, 203, 203 },
		{ 178, 178, 178 },
		{ 215, 215, 215 },
		{ 221, 221, 221 },
		{ 227, 227, 227 },
		{ 234, 234, 234 },
		{ 241, 241, 241 },
		{ 248, 248, 248 },
		{ 240, 251, 255 },
		{ 164, 160, 160 },
		{ 128, 128, 128 },
		{   0,   0, 255 },
		{   0, 255,   0 },
		{   0, 255, 255 },
		{ 255,   0,   0 },
		{ 255,   0, 255 },
		{ 255, 255,   0 },
		{ 255, 255, 255 }
	};
	return IdentityPalette;
}

CPalette* CreateIdentityPalette(void)
{
	BOOL fSuccess = FALSE;
	
	HPALETTE hPalette = NULL;
	HPALETTE hOldPalette = NULL;
	CPalette* pPalette = NULL;
	HDC hDC = NULL;
	int i;
	const RGBQUAD* IdentityPalette = GetIdentityPalette();

	// A structure to hold the palette being built.
		
	struct
	{
		WORD wVersion;
		WORD wNumberOfEntries;
		PALETTEENTRY Entries[256];
	} Palette =
	{
		0x300,
		256
	};
	
	TRY
	{
		// Get the screen DC, it will be used to retrieve information
		// about the static colors.
			
		hDC = GetDC(NULL);
			
		if (hDC != NULL)
		{
			// First, clear the system palette to black.
				
			for (i = 0; i < 256; i++)
			{
				Palette.Entries[i].peRed = 0;
				Palette.Entries[i].peGreen = 0;
				Palette.Entries[i].peBlue = 0;
				Palette.Entries[i].peFlags = PC_NOCOLLAPSE;
			}
			
			// Create, select, realize, deselect, and delete the palette.
			
			if (hPalette = CreatePalette((LOGPALETTE *)&Palette))
			{
				hOldPalette = SelectPalette(hDC, hPalette, FALSE);
						
				if (hOldPalette != NULL)
				{
					RealizePalette(hDC);
					SelectPalette(hDC, hOldPalette, FALSE);
				}
						
				DeleteObject(hPalette);
				hPalette = NULL;
			}
			
			// Get the current system palette and fill in the non static
			// colors with the colors from our identity palette.
			// If the system palette does not exist, do not fill in any
			// system colors.

			int nStaticColors = 0;

			if (GetSystemPaletteEntries(hDC, 0, 256, Palette.Entries) != 0)
			{
				nStaticColors = GetDeviceCaps(hDC, NUMCOLORS)/2;

				if (nStaticColors > 20/2)
				{
				/*
				// We clip the static colors at twenty. This means we get our
				// 236 entries in the palette. Since we have knocked off some
				// static colors, the mapping is no longer an identity mapping.
				// NOTE: This *may* be PrintMaster-specific.
				*/
					nStaticColors = 20/2;
				}
			}

			for (i = 0; i < nStaticColors; i++)
			{
				Palette.Entries[i].peFlags = 0;
			}

			for (; i < 256-nStaticColors; i++)
			{
				Palette.Entries[i].peRed = IdentityPalette[i].rgbRed;
				Palette.Entries[i].peGreen = IdentityPalette[i].rgbGreen;
				Palette.Entries[i].peBlue = IdentityPalette[i].rgbBlue;
				Palette.Entries[i].peFlags = PC_NOCOLLAPSE;
			}
				
			for (; i < 256; i++)
			{
				Palette.Entries[i].peFlags = 0;
			}
				
			// Create the palette.
				
			pPalette = new CPalette;
						
			if (pPalette != NULL)
			{
				fSuccess = pPalette->CreatePalette((LOGPALETTE *)&Palette);
					
				if (!fSuccess)
				{
					delete pPalette;
					pPalette = NULL;
				}
			}
			
			ReleaseDC(NULL, hDC);
			hDC = NULL;
		}
	}
	CATCH_ALL(e)
	{
		fSuccess = FALSE;
		
		if (hOldPalette != NULL)
		{
			SelectPalette(hDC, hOldPalette, FALSE);
			DeleteObject(hOldPalette);
			hOldPalette = NULL;
		}
		
		if (hPalette != NULL)
		{
			DeleteObject(hPalette);
			hPalette = NULL;
		}
		
		if (hDC != NULL)
		{
			ReleaseDC(NULL, hDC);
		}
		
		delete pPalette;
		pPalette = NULL;
	}
	END_CATCH_ALL
	
	ASSERT(fSuccess);
	
	return pPalette;
}

void initialize_palette(void)
{
	pOurPal = CreateIdentityPalette();
}

void free_palette(void)
{
	if (pOurPal != NULL)
	{
		delete pOurPal;
	}
}
