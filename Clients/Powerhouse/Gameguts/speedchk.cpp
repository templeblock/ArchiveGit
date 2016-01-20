#include "windows.h"
#include "proto.h"


//************************************************************************
void SpeedCheck( LPOFFSCREEN lpOffScreen, PDIB pdib )
//************************************************************************
{
#ifdef UNUSED
RGBQUAD rgb1, rgb2;

if (!lpOffScreen)
	return;

HDC hdc;
if ( !(hdc = GetDC(NULL)) )
	return;

WORD wNumEntries;
GetObject( GetApp()->m_hPal, sizeof(WORD), (LPSTR)&wNumEntries );
PALETTEENTRY sEntries[256], pEntries[256];
GetPaletteEntries( GetApp()->m_hPal, 0, wNumEntries, pEntries );
GetSystemPaletteEntries( hdc, 0, 256, sEntries );
ReleaseDC( NULL, hdc );

if ( wNumEntries != 256 )
	{
	Debug( "GetApp()->m_hPal only has %d entries\n", wNumEntries );
	}

// Compare the app's palette with the system palette
int i;
for ( i=0; i<256; i++ )
	{
	if ( pEntries[i].peRed	 != sEntries[i].peRed	||
		 pEntries[i].peBlue	 != sEntries[i].peBlue	||
		 pEntries[i].peGreen != sEntries[i].peGreen )
//		 pEntries[i].peFlags != sEntries[i].peFlags )
		{
		Debug( "GetApp()->m_hPal compare with syspal; entry %d (%d,%d,%d) (%d,%d,%d)\n", i,
			pEntries[i].peRed, pEntries[i].peGreen, pEntries[i].peBlue,
			sEntries[i].peRed, sEntries[i].peGreen, sEntries[i].peBlue );
//		break;
		}
	}

PDIB pdibr, pdibw;
LPWORD lpIndices;

// Compare the system palette with the WRITABLE offscreen
if ( pdibw = lpOffScreen->GetWritableDIB() )
	{
	RGBQUAD FAR *lprgbQuad = pdibw->GetColors();
	for ( i=0; i<256; i++ )
		{
		rgb1 = lprgbQuad[i];
		if ( rgb1.rgbRed   != sEntries[i].peRed	  ||
			 rgb1.rgbGreen != sEntries[i].peGreen ||
			 rgb1.rgbBlue  != sEntries[i].peBlue )
			{
			Debug( "OS writable compare with syspal; entry %d (%d,%d,%d) (%d,%d,%d)\n", i,
				rgb1.rgbRed, rgb1.rgbGreen, rgb1.rgbBlue,
				sEntries[i].peRed, sEntries[i].peGreen, sEntries[i].peBlue );
//			break;
			}
		}
	}

// Compare the READONLY offscreen with the WRITABLE offscreen
if ( (pdibr = lpOffScreen->GetReadOnlyDIB()) &&
	 (pdibw = lpOffScreen->GetWritableDIB()) )
	{
	RGBQUAD FAR *lprQuad = pdibr->GetColors();
	RGBQUAD FAR *lpwQuad = pdibw->GetColors();
	for ( i=0; i<256; i++ )
		{
		rgb1 = lprQuad[i];
		rgb2 = lpwQuad[i];
		if ( rgb1.rgbRed   != rgb2.rgbRed	||
			 rgb1.rgbGreen != rgb2.rgbGreen ||
			 rgb1.rgbBlue  != rgb2.rgbBlue )
			{
			Debug( "OS readonly compare with OS writable; entry %d (%d,%d,%d) (%d,%d,%d)\n", i,
				rgb1.rgbRed, rgb1.rgbGreen, rgb1.rgbBlue,
				rgb2.rgbRed, rgb2.rgbGreen, rgb2.rgbBlue );
//			break;
			}
		}
	}

// Compare the WRITABLE offscreen with a passed DIB
if ( (pdibw = lpOffScreen->GetWritableDIB()) && pdib )
	{
	RGBQUAD FAR *lpQuad = pdib->GetColors();
	RGBQUAD FAR *lpwQuad = pdibw->GetColors();
	for ( i=0; i<256; i++ )
		{
		rgb1 = lpwQuad[i];
		rgb2 = lpQuad[i];
		if ( rgb1.rgbRed   != rgb2.rgbRed	||
			 rgb1.rgbGreen != rgb2.rgbGreen ||
			 rgb1.rgbBlue  != rgb2.rgbBlue )
			{
			Debug( "OS writable compare with passed dib; entry %d (%d,%d,%d) (%d,%d,%d)\n", i,
				rgb1.rgbRed, rgb1.rgbGreen, rgb1.rgbBlue,
				rgb2.rgbRed, rgb2.rgbGreen, rgb2.rgbBlue );
//			break;
			}
		}
	}

// See if the WRITABLE offscreen is set to indices
if ( pdibw = lpOffScreen->GetWritableDIB() )
	{
	lpIndices = (LPWORD)pdibw->GetColors();
	for ( i=0; i<256; i++ )
		{
		if ( lpIndices[i] != (WORD)i )
			{
//			Debug( "OS writable not indices; entry %d (%d)\n", i, lpIndices[i] );
//			break;
			}
		}
	}

// See if the READONLY offscreen is set to indices
if ( pdibr = lpOffScreen->GetReadOnlyDIB() )
	{
	lpIndices = (LPWORD)pdibr->GetColors();
	for ( i=0; i<256; i++ )
		{
		if ( lpIndices[i] != (WORD)i )
			{
//			Debug( "OS readonly not indices; entry %d (%d)\n", i, lpIndices[i] );
//			break;
			}
		}
	}

#endif
}
