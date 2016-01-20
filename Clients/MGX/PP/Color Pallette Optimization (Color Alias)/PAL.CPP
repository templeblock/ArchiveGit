#include "stdafx.h"
#include "pal.h"

//************************************************************************
BOOL CreateIdentityMap( LPRGBQUAD pColorTable, int nColors )
//************************************************************************
{
int i;
LPLOGPALETTE lpPalette;
HDC hdc;

if ( !pColorTable )
	return( FALSE );

if ( !(hdc = GetDC(NULL)) )
	return( FALSE );

lpPalette = (LPLOGPALETTE)malloc(sizeof(LOGPALETTE)+ (256 * sizeof(PALETTEENTRY)));
if (!lpPalette)
	{
	ReleaseDC(NULL, hdc);
	return(FALSE);
	}
lpPalette->palVersion = 0x300;

// For SYSPAL_NOSTATIC, just copy the color table into
// a PALETTEENTRY array and replace the first and last entries
// with black and white    
if (GetDeviceCaps(hdc, RASTERCAPS) & RC_PALETTE)
	{
	lpPalette->palNumEntries = 256;
	if (GetSystemPaletteUse(hdc) == SYSPAL_NOSTATIC)
		{
		if ( nColors > 254 )
			nColors = 254;

	    // The first is black
	    lpPalette->palPalEntry[ 0 ].peRed   = 0;
	    lpPalette->palPalEntry[ 0 ].peGreen = 0;
	    lpPalette->palPalEntry[ 0 ].peBlue  = 0;
	    lpPalette->palPalEntry[ 0 ].peFlags = 0;

	    // Fill in the palette with the given values, marking each
	    // as PC_NOCOLLAPSE
	    for (i = 0; i < nColors; i++)
		    {
		    lpPalette->palPalEntry[i+1].peRed   = pColorTable[i].rgbRed;
		    lpPalette->palPalEntry[i+1].peGreen = pColorTable[i].rgbGreen;
		    lpPalette->palPalEntry[i+1].peBlue  = pColorTable[i].rgbBlue;
		    lpPalette->palPalEntry[i+1].peFlags = PC_NOCOLLAPSE;
		    }

	    // Mark any remaining entries PC_NOCOLLAPSE
	    for (; i < 254; ++i)
		    lpPalette->palPalEntry[i+1].peFlags = PC_NOCOLLAPSE;

	    // The last entry is white
	    lpPalette->palPalEntry[255].peRed   = 255;
	    lpPalette->palPalEntry[255].peGreen = 255;
	    lpPalette->palPalEntry[255].peBlue  = 255;
	    lpPalette->palPalEntry[255].peFlags = 0;
		}
	else
		// For SYSPAL_STATIC, get the twenty static colors into
		// the array, then fill in the empty spaces with the
		// given color table
		{
	    int nStaticColors;

	    // Get the static colors
	    nStaticColors = GetDeviceCaps(hdc, NUMCOLORS);
	    nStaticColors = nStaticColors / 2;

		if ( nColors > 256 )
			nColors = 256;
	    nColors -= nStaticColors;

	    GetSystemPaletteEntries(hdc, 0, 256, lpPalette->palPalEntry);

	    // Set the peFlags of the lower static colors to zero
	    for (i=0; i<nStaticColors; i++)
	    	lpPalette->palPalEntry[i].peFlags = 0;

	    // Fill in the entries from the given color table
	    for (; i<nColors; i++)
	    	{
		    lpPalette->palPalEntry[i].peRed   = pColorTable[i].rgbRed;
		    lpPalette->palPalEntry[i].peGreen = pColorTable[i].rgbGreen;
		    lpPalette->palPalEntry[i].peBlue  = pColorTable[i].rgbBlue;
		    lpPalette->palPalEntry[i].peFlags = PC_NOCOLLAPSE;
	    	}

	    // Mark any empty entries as PC_NOCOLLAPSE
	    for (; i<256 - nStaticColors; i++)
	    	lpPalette->palPalEntry[i].peFlags = PC_NOCOLLAPSE;

	    // Set the peFlags of the upper static colors to zero
	    for (i = 256 - nStaticColors; i<256; i++)
		    lpPalette->palPalEntry[i].peFlags = 0;
		}
	}
else
	{
	lpPalette->palNumEntries = nColors;
	for (i = 0; i < nColors; i++)
	    {
	    lpPalette->palPalEntry[i].peRed   = pColorTable[i].rgbRed;
	    lpPalette->palPalEntry[i].peGreen = pColorTable[i].rgbGreen;
	    lpPalette->palPalEntry[i].peBlue  = pColorTable[i].rgbBlue;
	    lpPalette->palPalEntry[i].peFlags = 0;
	    }
	}


ReleaseDC(NULL, hdc);

for (i = 0; i < lpPalette->palNumEntries; ++i)
	{
	pColorTable[i].rgbRed = lpPalette->palPalEntry[i].peRed;
	pColorTable[i].rgbGreen = lpPalette->palPalEntry[i].peGreen;
	pColorTable[i].rgbBlue = lpPalette->palPalEntry[i].peBlue; 
	pColorTable[i].rgbReserved = 0;
	}

free(lpPalette);
return( TRUE );
}
