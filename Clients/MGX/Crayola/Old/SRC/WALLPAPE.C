// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "windows.h"
#include "id.h"
#include "routines.h"

/************************************************************

 	SaveWallpaper

	  	Saves the current picture into the specified file.
		Installs it as the system wallpaper (uses name given).
		Sets the INI tiled flag to the passed value

		returns FALSE if it could not save wallpaper.
************************************************************/
BOOL SaveWallpaper( LPSTR lpName, BOOL fTiled )
{
WORD idDataType;
STRING szSaveAs;
LPFRAME lpFrame;
int nBits, nPlanes, nDepth, nOutWidth, nOutHeight;

if ( !lpImage )
	return(FALSE);
if ( !(lpFrame = ImgGetBaseEditFrame(lpImage)) )
	return(FALSE);

AstralCursor( IDC_WAIT );	/* Put up the wait cursor */

nBits = GetDeviceCaps( Window.hDC, BITSPIXEL );
nPlanes = GetDeviceCaps( Window.hDC, PLANES );
nDepth = nBits * nPlanes;
nOutWidth = GetDeviceCaps( Window.hDC,HORZRES);
nOutHeight = GetDeviceCaps( Window.hDC,VERTRES);

// decide on proper depth
if ( FrameType(lpFrame) == FDT_LINEART )
	idDataType = IDC_SAVELA;
else
if (nDepth <= 4)
	{
  	if( FrameType(lpFrame) == FDT_GRAYSCALE )
		idDataType = IDC_SAVESP;
	else
		idDataType = IDC_SAVE4BITCOLOR;
	}
else
if( nDepth <= 8 || !Control.Save24BitWallpaper)
	{
	if ( FrameType(lpFrame) == FDT_GRAYSCALE )
		idDataType = IDC_SAVECT;
	else
		idDataType = IDC_SAVE8BITCOLOR;
	}
else
	{
	if ( FrameType(lpFrame) == FDT_GRAYSCALE )
		idDataType = IDC_SAVECT;
	else
		idDataType = IDC_SAVE24BITCOLOR;
	}

// Save file into the Windows directory
GetWindowsDirectory(szSaveAs, sizeof(FNAME));
FixPath( szSaveAs );
lstrcat( szSaveAs, filename(lpName) );
stripext( szSaveAs );
lstrcat( szSaveAs, ".BMP" );

if ( !AstralImageSave( IDN_BMP, idDataType, szSaveAs ) )
	goto ErrorExit;

// Make szSaveAs the current wallpaper and apply
WriteProfileString( "Desktop", "TileWallPaper", (fTiled?"1":"0") );
SystemParametersInfo( SPI_SETDESKWALLPAPER, 0, filename(szSaveAs),
	SPIF_UPDATEINIFILE );
AstralCursor( NULL ); /* Revert back to the old cursor */
return (TRUE);

ErrorExit:

AstralCursor( NULL ); /* Revert back to the old cursor */
return(FALSE);
}
