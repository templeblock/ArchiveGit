#include "windows.h"
#include "proto.h"
#include "mmsystem.h"
#include "memory.h"

LOCAL HPALETTE Load(HMMIO hmmio);
LOCAL BOOL Save(HMMIO hmmio, HPALETTE hPal);
LOCAL int DoCreateIdentityPalette( LPPALETTEENTRY palPalEntry, int nColors );

//************************************************************************
// Clear the System Palette so that we can ensure an identity palette
//************************************************************************
void ClearSystemPalette(void)
//************************************************************************
{
	HPALETTE hBlackPal, hOldPal;
	HDC hDC;
	int i;
	struct
		{
		WORD Version;
		WORD NumberOfEntries;
		PALETTEENTRY aEntries[256];
		} Palette = { 0x300, 256 };
	
	if ( !(hDC = GetDC(NULL)) )
		return;
	
	// Reset everything in the system palette to black
	for (i = 0; i < 256; i++)
		{
		Palette.aEntries[i].peRed	= 0;
		Palette.aEntries[i].peGreen = 0;
		Palette.aEntries[i].peBlue	= 0;
		Palette.aEntries[i].peFlags = PC_NOCOLLAPSE;
		}
	
	// Create, select, realize, deselect, and delete the palette
	if ( hBlackPal = CreatePalette( (LPLOGPALETTE)&Palette ) )
		{
		hOldPal = SelectPalette( hDC, hBlackPal, FALSE/*bProtectPhysicalPal*/ );
		RealizePalette( hDC );
	
		// JMM - Don't know why it is necessary to do this for the old palette
		//SelectPalette( hDC, hOldPal, TRUE/*bProtectPhysicalPal*/ );
		//RealizePalette( hDC );
	
		SelectPalette( hDC, hOldPal, TRUE/*bProtectPhysicalPal*/ );
		DeleteObject( hBlackPal );
		}
	
	ReleaseDC( NULL, hDC );
}

//************************************************************************
HPALETTE CreateIdentityPalette( LPRGBQUAD pColorTable, int nColors )
//************************************************************************
{
	if ( !pColorTable )
		return( NULL );
	
	LPLOGPALETTE lpPalette;
	if ( !(lpPalette = (LPLOGPALETTE)Alloc(sizeof(LOGPALETTE)+ (256 * sizeof(PALETTEENTRY)))) )
		return(NULL);
	
	lpPalette->palVersion = 0x300;
	for ( int i = 0; i < nColors; ++i)
	{
		lpPalette->palPalEntry[i].peRed = pColorTable[i].rgbRed;
		lpPalette->palPalEntry[i].peGreen = pColorTable[i].rgbGreen;
		lpPalette->palPalEntry[i].peBlue = pColorTable[i].rgbBlue;
		lpPalette->palPalEntry[i].peFlags = 0;
	}
	lpPalette->palNumEntries = DoCreateIdentityPalette(lpPalette->palPalEntry, nColors);
	HPALETTE hPal = CreatePalette(lpPalette);
	FreeUp(lpPalette);
	return( hPal );
}

//************************************************************************
BOOL MakeIdentityPalette( HPALETTE hPal )
//************************************************************************
{
	UINT iColors;
	GetObject(hPal, 2, (LPSTR)&iColors);
	if (iColors <= 0)
		return(FALSE);
	
	LPPALETTEENTRY lpEntry;
	if ( !(lpEntry = (LPPALETTEENTRY)Alloc(256 * sizeof(PALETTEENTRY))) )
		return(FALSE);
	
	GetPaletteEntries(hPal, 0, iColors, lpEntry);
	if (!(iColors = DoCreateIdentityPalette(lpEntry, iColors)))
	{
		FreeUp(lpEntry);
		return(FALSE);
	}
	SetPaletteEntries(hPal, 0, iColors, lpEntry);
	FreeUp(lpEntry);
	return(TRUE);
}

//************************************************************************
LOCAL int DoCreateIdentityPalette( LPPALETTEENTRY palPalEntry, int nColors )
//************************************************************************
{
	int i, nEntries;
	
	if ( !palPalEntry )
		return( 0 );
	
	HDC hdc;
	if ( !(hdc = GetDC(NULL)) )
		return( 0 );
	
	// For SYSPAL_NOSTATIC, just copy the color table into
	// a PALETTEENTRY array and replace the first and last entries
	// with black and white	   
	if (GetDeviceCaps(hdc, RASTERCAPS) & RC_PALETTE)
		{
		nEntries = 256;
		if (GetSystemPaletteUse(hdc) == SYSPAL_NOSTATIC)
			{
			if ( nColors > 254 )
				nColors = 254;
	
			// The first is black
			palPalEntry[ 0 ].peRed	= 0;
			palPalEntry[ 0 ].peGreen = 0;
			palPalEntry[ 0 ].peBlue	= 0;
			palPalEntry[ 0 ].peFlags = 0;
	
			// Fill in the palette with the given values, marking each as PC_NOCOLLAPSE
			for (i = 0; i < nColors; i++)
				palPalEntry[i+1].peFlags = PC_NOCOLLAPSE;
	
			// Mark any remaining entries PC_NOCOLLAPSE
			for (; i < 254; ++i)
				palPalEntry[i+1].peFlags = PC_NOCOLLAPSE;
	
			// The last entry is white
			palPalEntry[255].peRed	= 255;
			palPalEntry[255].peGreen = 255;
			palPalEntry[255].peBlue	= 255;
			palPalEntry[255].peFlags = 0;
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
	
			GetSystemPaletteEntries(hdc, 0, nStaticColors, &palPalEntry[0]);
			GetSystemPaletteEntries(hdc, 256-nStaticColors, nStaticColors, &palPalEntry[256-nStaticColors]);
	
			// Set the peFlags of the lower static colors to zero
			for (i=0; i<nStaticColors; i++)
				palPalEntry[i].peFlags = 0;
	
			// Fill in the entries from the given color table
			for (; i<nColors; i++)
				palPalEntry[i].peFlags = PC_NOCOLLAPSE;
	
			// Mark any empty entries as PC_NOCOLLAPSE
			for (; i<256 - nStaticColors; i++)
				palPalEntry[i].peFlags = PC_NOCOLLAPSE;
	
			// Set the peFlags of the upper static colors to zero
			for (i = 256 - nStaticColors; i<256; i++)
				palPalEntry[i].peFlags = 0;
			}
		}
	else
		{
		nEntries = nColors;
		for (i = 0; i < nColors; i++)
			palPalEntry[i].peFlags = 0;
		}
	
	ReleaseDC(NULL, hdc);
	
	return( nEntries );
}

//***********************************************************************
HPALETTE CopySystemPalette(BYTE peFlags)
//***********************************************************************
{
	#define NUMENTRIES 256

	LPLOGPALETTE lpPal;
	LPPALETTEENTRY lpEntry;
	PALETTEENTRY   TempEntries[256+1];
	HWND hWnd;
	HDC hDC;
	int i, n;
	
	lpPal = (LPLOGPALETTE)TempEntries;
	lpPal->palVersion = 0x300;
	lpPal->palNumEntries = 256;
	lpEntry = lpPal->palPalEntry;

	if ( !(hWnd = GetDesktopWindow()) )
		return( NULL );

	if ( !(hDC = GetDC( hWnd )) )
		return( NULL );
						  
	if ((GetDeviceCaps(hDC, RASTERCAPS) & RC_PALETTE) == 0)
	{
		ReleaseDC(hWnd, hDC);
		return(NULL);
	}

	if (peFlags != PC_EXPLICIT)
		n = GetSystemPaletteEntries( hDC, 0, NUMENTRIES, lpEntry );
	else
		n = NUMENTRIES;

	ReleaseDC( hWnd, hDC );

	if ( n != NUMENTRIES )
		return( NULL );

	if (peFlags == PC_EXPLICIT)
	{
		for ( i=0; i<NUMENTRIES; i++ )
		{
			lpEntry[i].peRed = i;
			lpEntry[i].peRed = 0;
			lpEntry[i].peRed = 0;
			lpEntry[i].peFlags = PC_EXPLICIT;
		}
	}
	else
	{
		for ( i=0; i<NUMENTRIES; i++ )
		{
			lpEntry[i].peFlags = peFlags;
		}
	}

	return( CreatePalette( lpPal ) );
}


//***********************************************************************
HPALETTE CopyPalette( HPALETTE hPal )
//***********************************************************************
{
	LPLOGPALETTE lpPal;
	LPPALETTEENTRY lpEntry;
	PALETTEENTRY TempEntries[256+1];
	short iNumEntries;
	
	GetObject(hPal, 2, (LPSTR)&iNumEntries);
	if ( !iNumEntries )
		return( NULL );
	lpPal = (LPLOGPALETTE)TempEntries;
	lpPal->palVersion = 0x300;
	lpPal->palNumEntries = iNumEntries;
	lpEntry = lpPal->palPalEntry;
	GetPaletteEntries(hPal, 0, iNumEntries, lpEntry);
	return( CreatePalette( lpPal ) );
}

//***********************************************************************
HPALETTE CreateCustomPalette( LPRGB lpRGBmap, int iEntries )
//***********************************************************************
{
	LPLOGPALETTE lpPal;
	PALETTEENTRY TempEntries[256+1];
	lpPal = (LPLOGPALETTE)TempEntries;
	lpPal->palVersion = 0x300;
	lpPal->palNumEntries = iEntries;

	LPPALETTEENTRY lpEntry = lpPal->palPalEntry;

	LPRGB lpRGB = lpRGBmap;
	for ( int i = 0; i < iEntries; ++i )
	{
		lpEntry->peRed	 = lpRGB->red;
		lpEntry->peGreen = lpRGB->green;
		lpEntry->peBlue	 = lpRGB->blue;
		lpEntry->peFlags = PC_RESERVED;
		lpEntry++;
		lpRGB++;
	}
	return( CreatePalette( lpPal ) );
}

//***********************************************************************
HPALETTE CreateCustomPalette( LPRGBQUAD lpRGBmap, int iEntries )
//***********************************************************************
{
	LPLOGPALETTE lpPal;
	PALETTEENTRY TempEntries[256+1];
	lpPal = (LPLOGPALETTE)TempEntries;
	lpPal->palVersion = 0x300;
	lpPal->palNumEntries = iEntries;

	LPPALETTEENTRY lpEntry = lpPal->palPalEntry;

	LPRGBQUAD lpRGB = lpRGBmap;
	for ( int i = 0; i < iEntries; ++i )
	{
		lpEntry->peRed	 = lpRGB->rgbRed;
		lpEntry->peGreen = lpRGB->rgbGreen;
		lpEntry->peBlue	 = lpRGB->rgbBlue;
		lpEntry->peFlags = 0;
		lpEntry++;
		lpRGB++;
	}
	return( CreatePalette( lpPal ) );
}

/***********************************************************************/
BOOL ComparePalettes(HPALETTE hPal1, HPALETTE hPal2)
/***********************************************************************/
{
	LPPALETTEENTRY lpEntry1, lpEntry2;
	lpEntry1 = (LPPALETTEENTRY)Alloc(sizeof(PALETTEENTRY)*256);
	lpEntry2 = (LPPALETTEENTRY)Alloc(sizeof(PALETTEENTRY)*256);
	if (!lpEntry1 || !lpEntry2)
	{
		if (lpEntry1)
			FreeUp(lpEntry1);
		if (lpEntry2)
			FreeUp(lpEntry2);
		return(FALSE);
	}

	int n1 = GetPaletteEntries(hPal1, 0, 256, lpEntry1);
	int n2 = GetPaletteEntries(hPal2, 0, 256, lpEntry2);
	if (n1 != n2)
		return(FALSE);
	for ( int i = 0; i < n1; ++i )
	{
		if (lpEntry1[i].peRed != lpEntry2[i].peRed)
			break;
		if (lpEntry1[i].peGreen != lpEntry2[i].peGreen)
			break;
		if (lpEntry1[i].peBlue != lpEntry2[i].peBlue)
			break;
	}
	FreeUp(lpEntry1);
	FreeUp(lpEntry2);
	return(i == n1);
}

// Load a palette from a named file.
/***********************************************************************/
HPALETTE LoadPalette(LPCTSTR lpFileName)
/***********************************************************************/
{
	HFILE fh;
	HPALETTE hPal;
    HMMIO hmmio;
    MMIOINFO info;

    // Try to open the file for read access.
	if ( (fh = _lopen(lpFileName, OF_READ)) == HFILE_ERROR )
		return(NULL);
    _fmemset(&info, 0, sizeof(info));
    info.adwInfo[0] = fh;
    hmmio = mmioOpen(NULL,
                     &info,
                     MMIO_READ | MMIO_ALLOCBUF);
    if (!hmmio)
    {
		#ifdef _DEBUG
        Print("mmioOpen failed");
		#endif
        return NULL;
    }
    hPal = Load(hmmio);
    mmioClose(hmmio, MMIO_FHOPEN);
    _lclose(fh);
    return hPal;
}

// Save a palette to a named file
/***********************************************************************/
BOOL SavePalette(LPCTSTR lpFileName, HPALETTE hPal)
/***********************************************************************/
{
	HFILE fh;
    HMMIO hmmio;
    MMIOINFO info;

    // Try to open the file for read access.
	if ( (fh = _lcreat(lpFileName, 0)) == HFILE_ERROR )
	{
		#ifdef _DEBUG
		Print("Error creating file '%s'", lpFileName);
		#endif
		return(FALSE);
	}

    memset(&info, 0, sizeof(info));
    info.adwInfo[0] = fh;
    hmmio = mmioOpen(NULL,
                     &info,
                     MMIO_WRITE | MMIO_CREATE | MMIO_ALLOCBUF);
    if (!hmmio)
    {
		#ifdef _DEBUG
        Print("mmioOpen failed");
		#endif
		_lclose(fh);
        return FALSE;
    }
    BOOL bResult = Save(hmmio, hPal);
    mmioClose(hmmio, MMIO_FHOPEN);
	_lclose(fh);
    return bResult;
}

// Load a palette from an open MMIO handle.
/***********************************************************************/
LOCAL HPALETTE Load(HMMIO hmmio)
/***********************************************************************/
{
    // Check it's a RIFF PAL file.
    MMCKINFO ckFile;
    ckFile.fccType = mmioFOURCC('P','A','L',' ');
    if (mmioDescend(hmmio,
                    &ckFile,
                    NULL,
                    MMIO_FINDRIFF) != 0) {
		#ifdef _DEBUG
        Print("Not a RIFF or PAL file");
		#endif
        return NULL;
    }
    // Find the 'data' chunk.
    MMCKINFO ckChunk;
    ckChunk.ckid = mmioFOURCC('d','a','t','a');
    if (mmioDescend(hmmio,
                    &ckChunk,
                    &ckFile,
                    MMIO_FINDCHUNK) != 0) {
		#ifdef _DEBUG
        Print("No data chunk in file");
		#endif
        return NULL;
    }
    // Allocate some memory for the data chunk.
    int iSize = (int)ckChunk.cksize;
    void FAR* pdata = (void FAR *)Alloc(iSize);
    if (!pdata) {
		#ifdef _DEBUG
        Print("No mem for data");
		#endif
        return NULL;
    }
    // Read the data chunk.
    if (mmioRead(hmmio,
                 (HPSTR)pdata,
                 iSize) != iSize) {
		#ifdef _DEBUG
        Print("Failed to read data chunk");
		#endif
        FreeUp(pdata);
        return NULL;
    }
    // The data chunk should be a LOGPALETTE structure
    // from which we can create a palette.
    LOGPALETTE FAR * pLogPal = (LOGPALETTE FAR *)pdata;
    if (pLogPal->palVersion != 0x300) {
		#ifdef _DEBUG
        Print("Invalid version number");
		#endif
        FreeUp(pdata);
        return NULL;
    }
    // Get the number of entries.
    int nEntries = pLogPal->palNumEntries;
    if (nEntries <= 0) {
		#ifdef _DEBUG
        Print("No colors in palette");
		#endif
        FreeUp(pdata);
        return NULL;
    }
	return(CreatePalette(pLogPal));
}

// Save a palette to an open MMIO handle.
/***********************************************************************/
LOCAL BOOL Save(HMMIO hmmio, HPALETTE hPal)
/***********************************************************************/
{
    // Create a RIFF chunk for a PAL file.
	short iColors;

    MMCKINFO ckFile;
    ckFile.cksize = 0; // Corrected later.
    ckFile.fccType = mmioFOURCC('P','A','L',' ');
    if (mmioCreateChunk(hmmio,
                        &ckFile,
                        MMIO_CREATERIFF) != 0) {
		#ifdef _DEBUG
        Print("Failed to create RIFF-PAL chunk");
		#endif
        return FALSE;
    }
    // Create the LOGPALETTE data which will become
    // the data chunk.
	GetObject(hPal, 2, (LPSTR)&iColors);
	if (iColors <= 0)
	{
		#ifdef _DEBUG
		Print("No colors in palette");
		#endif
		return(FALSE);
	}
    int iSize = sizeof(LOGPALETTE)
                + (iColors-1) * sizeof(PALETTEENTRY);
    LOGPALETTE FAR* plp = (LOGPALETTE FAR*)Alloc(iSize);
	if (!plp)
	{
		#ifdef _DEBUG
		Print("Out of memory");
		#endif
		return(FALSE);
	}
    _fmemset(plp, 0, iSize);
    plp->palVersion = 0x300;
    plp->palNumEntries = iColors;
    GetPaletteEntries(hPal, 0, iColors, plp->palPalEntry);

    // Create the data chunk.
    MMCKINFO ckData;
    ckData.cksize = iSize; 
    ckData.ckid = mmioFOURCC('d','a','t','a');
    if (mmioCreateChunk(hmmio,
                        &ckData,
                        0) != 0) {
		#ifdef _DEBUG
        Print("Failed to create data chunk");
		#endif
        return FALSE;
    }
    // Write the data chunk.
    if (mmioWrite(hmmio,
                 (HPSTR)plp,
                 iSize) != iSize) {
		#ifdef _DEBUG
        Print("Failed to write data chunk");
		#endif
        FreeUp(plp);
        return FALSE;
    }
    FreeUp(plp);
    // Ascend from the data chunk which will correct the length.
    mmioAscend(hmmio, &ckData, 0);
    // Ascend from the RIFF/PAL chunk.
    mmioAscend(hmmio, &ckFile, 0);

    return TRUE;
}
