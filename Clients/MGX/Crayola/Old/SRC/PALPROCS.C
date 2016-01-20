//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

// local storage values that need to be saved in pp.ini
static LPPALETTE lpPaletteList;	// Pointer to list of palettes
static int iActiveEntry; // Index of active palette entry

#define PALETTE_NAME "Crayon"

/***********************************************************************/
void PaletteInit(HWND hDlg)
/***********************************************************************/
{
LPPALETTE lpPalette;
FNAME szFileName;
LPPALETTE lpHeadPalette;

if ( LookupExtFile( PALETTE_NAME, szFileName, IDN_PALETTE ) )
	lpPalette = ReadPalette( szFileName,YES );
else
	lpPalette = NULL;
if ( !lpPalette )
	lpPalette = GetPalette(lpPaletteList, NULL, 0/*iPalette*/);
lpHeadPalette = lpPalette;
if (lpPaletteList && lpPaletteList != lpHeadPalette)
	FreeUpPalette(lpPaletteList);
lpPalette = GetPalette(lpHeadPalette, NULL, 0/*iPalette*/);
lpPaletteList = lpHeadPalette;
iActiveEntry = 0;
}


/***********************************************************************/
BOOL PaletteColor( int iEntry, LPCOLORINFO lpColorInfo )
/***********************************************************************/
{
LPPALETTE lpPalette;

if ( !(lpPalette = GetPalette(lpPaletteList, NULL, 0/*iPalette*/)) )
	return( FALSE );
if ( iEntry >= lpPalette->iColors )
	return( FALSE );
*lpColorInfo = lpPalette->lpColorInfo[iEntry];
return( TRUE );
}

/***********************************************************************/
void PaletteSelect( int iEntry, WORD wOption )
/***********************************************************************/
{
STRING szString;
LPPALETTE lpPalette;

if ( !(lpPalette = GetPalette(lpPaletteList, NULL, 0/*iPalette*/)) )
	return;
if ( iEntry >= lpPalette->iColors )
	iEntry = lpPalette->iColors - 1;
if ( iEntry < 0 )
	iEntry = 0;
if ( wOption != 3 ) // user is not browsing over color
	iActiveEntry = iEntry;
GetPaletteEntryLabel(lpPaletteList, 0/*iPalette*/, iEntry, (LPTR)szString);
ColorStatus(&lpPalette->lpColorInfo[iEntry], 0L, szString, YES);
if (wOption == 0)
	SetActiveColor(&lpPalette->lpColorInfo[iActiveEntry],
		lpPalette->lpColorInfo[iActiveEntry].ColorSpace, FALSE );
else
if (wOption == 1)
	SetAlternateColor(&lpPalette->lpColorInfo[iActiveEntry],
		lpPalette->lpColorInfo[iActiveEntry].ColorSpace, FALSE );
}


/***********************************************************************/
void PaletteClose( void )
/***********************************************************************/
{
if ( !lpPaletteList )
	return;

// FreeUp all palettes in list
FreeUpPalette(lpPaletteList);
lpPaletteList = NULL;
}
