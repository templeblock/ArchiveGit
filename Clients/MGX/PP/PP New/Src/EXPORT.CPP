// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "export.h"

static int iExportXSize, iExportYSize, iExportDepth, iExportResolution;
LPTR lpExportBuffer = NULL;

/************************************************************************/

BOOL FAR EXPORT ExportClockCursor(int part, int whole, BOOL bEscapable)
{
	return(AstralClockCursor(part, whole, bEscapable));
}

/************************************************************************/

LPTR FAR EXPORT ExportDataPointer(int x, int y, BOOL bModify)
{
	LPIMAGE lpImage = GetActiveImage();
	int iPixelCount = iExportXSize - x;
	

	if (x <= iExportXSize || (y <= iExportYSize))
		ImgGetLine( lpImage, NULL, x, y, iPixelCount, lpExportBuffer );

	return(lpExportBuffer);
}

/************************************************************************/

BOOL ExportImageDLL( LPSTR lpExportFile )
{
	HMODULE   hModule;
	FARPROC  lpClockCursor = NULL;
	FARPROC  lpDataPointer = NULL;
	EXPORTPROC    lpStartProc;
	EXPORTPROC    lpProcessProc;
	EXPORTPROC    lpEndProc;
	LPFRAME  lpFrame;

	EXPPARMS parms;
	BOOL     fSuccess = FALSE;
	LPIMAGE lpImage;

	lpImage = GetActiveImage();

	if (!lpImage)
		goto Exit;

	lpFrame = ImgGetBaseEditFrame(lpImage);

	if (!lpFrame)
		goto Exit;

	if (!(hModule = (HMODULE)AstralLoadLibrary(lpExportFile)))
		goto Exit;

	lpStartProc   = (EXPORTPROC)GetProcAddress(hModule, "_ExportStart");
	lpProcessProc = (EXPORTPROC)GetProcAddress(hModule, "_ExportProcess");
	lpEndProc     = (EXPORTPROC)GetProcAddress(hModule, "_ExportEnd");

	if ( !lpStartProc || !lpProcessProc || !lpEndProc )
		goto Exit;

	iExportXSize = FrameXSize( lpFrame );
	iExportYSize = FrameYSize( lpFrame );
	iExportDepth = FrameDepth( lpFrame );
	iExportResolution = FrameResolution( lpFrame );

	lpClockCursor = MakeProcInstance( (FARPROC)ExportClockCursor, PictPubApp.Get_hInstAstral() );
	lpDataPointer = MakeProcInstance( (FARPROC)ExportDataPointer, PictPubApp.Get_hInstAstral() );

	AllocLines( &lpExportBuffer, 1, iExportXSize, max(1, iExportDepth));

	parms.hInstance      = PictPubApp.Get_hInstAstral();
	parms.hWnd           = GetActiveWindow();
	parms.lpszFileName   = lpImage->CurFile;
	parms.XSize          = iExportXSize;
	parms.YSize          = iExportYSize;
	parms.Depth          = iExportDepth;
	parms.Resolution     = iExportResolution;
	parms.StatusCallback = (LPROC)lpClockCursor;
	parms.DataPointer    = (LPTRPROC)lpDataPointer;

	if (!lpClockCursor || !lpDataPointer || !lpExportBuffer)
		goto Exit;


	if ((*lpStartProc)((LPEXPPARMS)&parms))
	{
		MessageStatus( IDS_PLEASEWAIT );
		if ((*lpProcessProc)((LPEXPPARMS)&parms))
		{
			fSuccess = TRUE;
			(*lpEndProc)((LPEXPPARMS)&parms);
		}
	}

Exit:

	if (lpClockCursor)
		FreeProcInstance((FARPROC)lpClockCursor);

	if (lpDataPointer)
		FreeProcInstance((FARPROC)lpDataPointer);

	if (lpExportBuffer)
		FreeUp(lpExportBuffer);

	lpExportBuffer = NULL;

	if (hModule)
		FreeLibrary(hModule);

	MessageStatus( 0 );

	return(fSuccess);
}

/************************************************************************/

BOOL ExportImage( HWND hWnd, ITEMID idItem, ITEMID idIniSection )
{
	HINSTANCE hInstance;
	STRING szAppName, szIniSection;
	STRING szExportName;
	BOOL bSuccess = FALSE;

	// Get the name of export filter
	if ( !(GetMenuString( GetMenu(hWnd), idItem, szAppName, sizeof(szAppName),
   	MF_BYCOMMAND )) ) {
		return( FALSE );
	}

	// Get the name of the INI section to read
	hInstance = GetWindowInstance( hWnd );
	if ( !AstralStrEx( idIniSection, szIniSection, sizeof(szIniSection) )) {
		return( FALSE );
	}

	// Get the module file name for the application
	// Put it in a staic string so the callback has access to it
	if ( !(GetProfileString( szIniSection, szAppName, "", szExportName,
		sizeof(szExportName) )) ) {
		return( FALSE );
	}

	bSuccess = ExportImageDLL( (LPSTR)szExportName );

	return( bSuccess );
}

