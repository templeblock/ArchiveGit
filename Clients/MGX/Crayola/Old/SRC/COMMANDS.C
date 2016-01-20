//®FD1¯®PL1¯®TP0¯®BT0¯®RM255¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"
#include "ppole.h"
#include "gallery.h"

extern HINSTANCE hInstAstral;
extern HWND	hWndAstral;
extern HWND	hClientAstral;

extern SAVE Save;


BOOL bPaintAppActive = FALSE;
BOOL bHexBrush = NO;            // used by hexcircle brush with virt keys
/***********************************************************************/
long DoCommand( HWND hWindow, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
FNAME szFileName;
LPFRAME lpFrame;
LPOBJECT lpObject, lpBase;
RECT rect, rMask, rTemp, rAll;
HWND hWnd;
STRING szString;
int i, idFileType, x, y, dx, dy;
BOOL fDoZoom, bPrint, fHasZoom;
LPSTR lpszCmdLine;
HGLOBAL hMem;
ITEMID idDataType;
DWORD dwReturn;
int NewShape;
UINT wID;

SoundStartID( wParam, NO/*bLoop*/, NULL/*hInstance*/ );
/*
	WIN16:	ID          = wParam
			ctrl handle = LOWORD(lParam)
			notify code = HIWORD(lParam)

	WIN32:	ID          = LOWORD(wParam)
			ctrl handle = lParam
			notify code = HIWORD(wParam)
*/

#ifdef WIN32
	wID = LOWORD(wParam);
#else
	wID = wParam;
#endif

switch (wID)
	{
	case IDM_EXIT:
	DeactivateTool();
	PostMessage( hWndAstral, WM_CLOSE, 0, 0L);
    Delay (750);
	break;

	case IDM_CLOSE:
	DeactivateTool();
	/* Check to see if the image needs to be saved */
	if ( !ConfirmClose(NO,NO) )
		break;
	CloseImage( NO, lpImage ); // Close the active image
	break;

	case IDM_ABOUT:
	/* Bring up the modal 'About' dialog box */
	AstralDlg( NO|2, hInstAstral, hWindow, IDD_ABOUT, DlgAboutProc);
	break;

	case IDM_NEXTPAGE:
	if ( !lpImage )
		break;
	DeactivateTool();
	SendMessage( lpImage->hWnd, WM_SYSCOMMAND, SC_NEXTWINDOW, 0L );
	break;

	case IDM_NEW:
	DeactivateTool();
	SoundStartResource( "magic01", NO, NULL );
	AstralCursor( IDC_WAIT );
	New( Control.NewWidth, Control.NewHeight, Control.NewResolution,
		Control.NewDepth );
	AstralCursor( NULL );
	break;

	case IDM_OPEN:
	DeactivateTool();
	/* Bring up the modal 'File Open' box */
	if ( !(dwReturn = DoOpenDlg( hWindow, IDD_OPEN,
		Save.FileType, szFileName, NO )) )
		break;
	idFileType = LOWORD( dwReturn );
	Save.FileType = idFileType;
	PutDefaultInt( "FileType", idFileType - IDN_TIFF );
	AstralImageLoad( idFileType, szFileName, MAYBE, YES );
	break;

	case IDM_ALTOPEN:
	DeactivateTool();
	// Command line sent from a second instance prior to it closing
	/* If the caller passed in a file name, try to load it or print it */
	if ( !(lpszCmdLine = (LPSTR)lParam) )
		break;
	HandleCommandLine( hWindow, lpszCmdLine, &bPrint );
	break;

	case IDM_SAVE:
	DeactivateTool();
	HandleSaveCommand(hWindow, wID, YES);
	break;

	case IDM_SAVEAS:
	DeactivateTool();
	HandleSaveCommand(hWindow, wID, YES);
	break;

	case IDM_SAVESPECIAL:
	DeactivateTool();
	HandleSaveCommand(hWindow, wID, YES);
	break;

	case IDM_SAVEWALLPAPER:
	DeactivateTool();
	SaveWallpaper( "CRAYOLA", NO/*fTiled*/ );
	break;

	case IDM_REVERT:
	if ( !lpImage )
		break;
	DeactivateTool();
	if ( lpImage->fUntitled ) // If the picture doesn't have a name, get out
		break;
	if ( ImgChanged(lpImage) )
		{
		/* Check to see if its OK to trash changes */
		if ( AstralOKCancel(IDS_OKTOREVERT,
				(LPSTR)lpImage->CurFile) == IDCANCEL )
			break;
		}
	lstrcpy( szFileName, lpImage->CurFile );
	idFileType = lpImage->FileType;
	fDoZoom = IsZoomed(lpImage->hWnd);
	fHasZoom = Window.fHasZoom;
	CloseImage( NO, lpImage ); // Close the active image
	if ( !AstralImageLoad( idFileType, szFileName, fDoZoom, YES ) )
		break;
	break;

	case IDM_RECALLIMAGE0:
	case IDM_RECALLIMAGE1:
	case IDM_RECALLIMAGE2:
	case IDM_RECALLIMAGE3:
	case IDM_RECALLIMAGE4:
	case IDM_RECALLIMAGE5:
	case IDM_RECALLIMAGE6:
	case IDM_RECALLIMAGE7:
	case IDM_RECALLIMAGE8:
	case IDM_RECALLIMAGE9:
	DeactivateTool();
	GetMenuString( GetMenu(hWindow), wID,
		szFileName, sizeof(szFileName), MF_BYCOMMAND );
	i = 0;
	while ( szFileName[i] && szFileName[i] != ' ' )
		i++; // Skip over the numeric id in the string (3. junk.tif)
	lstrcpy(szString, &szFileName[i+1]);
	GetRecallFileName(szString);
	AstralImageLoad( NULL, szString, MAYBE, YES );
	break;

	case IDM_PRINT:
	if ( !lpImage )
		break;
	DeactivateTool();
	AstralCursor( IDC_WAIT );
	DoPrintSizeInits();
	SoundStartResource( "print", YES, NULL );
	PrintFile( hWindow, filename(lpImage->CurFile), YES, lpImage, NULL );
	SoundStop();
	AstralCursor( NULL );
	break;

	case IDM_PRINTSETUP:
	/* Bring up the setup dialog box for the active printer */
	AstralDlg( NO|2, hInstAstral, hWindow, IDD_PRINTSETUP,
		DlgPrintSetupProc);
	break;

	case IDC_SOUND:
	SoundToggle();
	break;

	case IDM_UNDO:
	if ( !lpImage )
		break;
	DeactivateTool();
	ImgEditUndo(lpImage, YES, NO);
	break;

	case IDM_CUT:
	case IDM_COPY:
	if ( !lpImage )
		break;
	DeactivateTool();

	// Create the clipboard files from the image
	ProgressBegin(1, IDS_PROGCOPY);
	if ( !ImgWriteClipOut( lpImage, NULL, NULL, &rMask, lpImage->DataType ) )
		{
		ProgressEnd();
		Message(IDS_EMEMALLOC);
		break;
		}
	ProgressEnd();

	OpenClipboard(hWindow);
	EmptyClipboard();

	// Passing a NULL data handle in SetClipboardData() means that
	// the data will be requested in a WM_RENDERFORMAT message
	hMem = ConstructObject( lpImage,
		IsRectEmpty(&rMask)? (LPRECT)NULL : (LPRECT)&rMask );
	if ( pOLE )
		{ // Any data put on before Native will become staticly copied
		SetClipboardData( pOLE->cfNative, NULL );
		SetClipboardData( pOLE->cfOwnerLink, hMem );
		}
	SetClipboardData( CF_DIB, NULL );
	if ( Control.DoPicture )
		SetClipboardData( CF_METAFILEPICT, NULL );
	if ( Control.DoBitmap )
		SetClipboardData( CF_BITMAP, NULL );
	SetClipboardData( CF_PALETTE, NULL );
	SetClipboardData( Control.cfImage, NULL );
	if ( pOLE && wID == IDM_COPY && !lpImage->fUntitled )
		{ // ObjectLink is retrieved during a Paste Link...
		SetClipboardData( pOLE->cfObjectLink, hMem );
		}
	CloseClipboard();
	if ( wID == IDM_COPY )
		break;

	// else fall through to IDM_DELETE

	case IDM_DELETE:
	if ( !lpImage )
		break;
	{
	COLORINFO ColorInfo;

	DeactivateTool();
	ColorInfo.gray   = 255;
	SetColorInfo( &ColorInfo, &ColorInfo, CS_GRAY );
	TintFill( lpImage, &ColorInfo, 255, MM_NORMAL,
		wID == IDM_CUT ? IDS_UNDOCUT : IDS_UNDODELETE );
	}
	break;

	case IDM_PASTE:
	if ( !lpImage )
		break;

	case IDM_PASTEASNEW:
	if ( !OpenClipboard(hWndAstral) )
		{
		Message( IDS_ECLIPOPEN );
		break;
		}
	if ( !IsClipboardFormatAvailable(CF_DIB) &&
	     !IsClipboardFormatAvailable(CF_BITMAP) &&
	     !IsClipboardFormatAvailable(Control.cfImage) )
		{
		Message( IDS_ECLIPOPEN );
		CloseClipboard();
		break;
		}
	CloseClipboard();

	DeactivateTool();
	// First put clipboard contents into a file(s)
	AstralCursor(IDC_WAIT);
	if ( !PasteFromClipboard( hWindow, (wID == IDM_PASTE) /*fNeedMask*/ ) )
		{
		AstralCursor(NULL);
		Message(IDS_EMEMALLOC);
		break;
		}
	if ( wID == IDM_PASTE )
		{
		ProgressBegin(1, IDS_PROGPASTECLIP);
		TransformObjectsStart( YES/*fNewObject*/ );
        if ( ImgCreateClipInObject( lpImage, NO ) )
			;
		ProgressEnd();
		}
	else
	if ( wID == IDM_PASTEASNEW )
		{
		if ( lpFrame = AstralFrameLoad( Names.PasteImageFile, -1,
			 &idDataType, &idFileType) )
			{
			if ( NewImageWindow(
				NULL,			// lpOldFrame
				NULL,			// Name
				lpFrame, 		// lpNewFrame
				idFileType,		// lpImage->FileType
				idDataType,		// lpImage->DataType
				FALSE,			// New view?
				IMG_DOCUMENT,	// lpImage->DocumentType
				NULL,			// lpImage->ImageName
				MAYBE
				) )
					lpImage->fChanged = YES;
			}
		}
	break;

	case IDM_ESCAPE:
	if (!lpImage)
		break;
	if (lpImage->hWnd == hZoomWindow)
		break;
	if ( Tool.bActive && Tool.lpToolProc )
		DestroyProc( lpImage->hWnd, 1L );
	break;

	case IDM_SIZEUP:
	case IDM_SIZEDOWN:
	if (!Retouch.hBrush)
		break;				 
	SetFocus( hWindow ); // Take focus away from any controls
	if ( Retouch.BrushShape == IDC_BRUSHCUSTOM )
		break;
	if ( wID == IDM_SIZEUP )
    {
    	if( bHexBrush && ( Retouch.BrushSize % 2 == 0 ) && ( Retouch.BrushSize > 6 ))
    		Retouch.BrushSize += 2;
        else        
    		Retouch.BrushSize++;
    }
	else	
    {
    	if( bHexBrush && ( Retouch.BrushSize % 2 == 0 ) && ( Retouch.BrushSize > 6 )) 
    		Retouch.BrushSize -= 2;
        else        
    		Retouch.BrushSize--;
    }
	if ( Retouch.BrushSize < 1 || Retouch.BrushSize > MAX_BRUSH_SIZE )
		{
		Retouch.BrushSize = bound( Retouch.BrushSize, 1,MAX_BRUSH_SIZE);
		MessageBeep(0);
		break;
		}
	if (lpImage)
		DisplayBrush(0, 0, 0, OFF);
	SetMgxBrushSize(Retouch.hBrush, Retouch.BrushSize);
	if (lpImage && Window.hCursor == Window.hNullCursor)
		DisplayBrush(lpImage->hWnd, 32767, 32767, ON);
	if ( Tool.hRibbon )
		SetSlide( Tool.hRibbon, IDC_BRUSHSIZE, Retouch.BrushSize );
	break;

	case IDM_SHAPEUP:
	case IDM_SHAPEDOWN:
	if (!Retouch.hBrush)
		break;
	SetFocus( hWindow ); // Take focus away from any controls
	NewShape = Retouch.BrushShape;
	if ( wID == IDM_SHAPEUP )
		NewShape++;
	else	NewShape--;
	if ( NewShape > IDC_BRUSHCUSTOM )
		NewShape = IDC_BRUSHCIRCLE;
	if ( NewShape < IDC_BRUSHCIRCLE )
		NewShape = IDC_BRUSHCUSTOM;
	if (lpImage)
	 	DisplayBrush(0, 0, 0, OFF);
	if (!SetMgxBrushShape(Retouch.hBrush, NewShape, Names.CustomBrush))
		{
		NewShape = IDC_BRUSHCIRCLE;
		SetMgxBrushShape(Retouch.hBrush, NewShape, Names.CustomBrush);
		}
	Retouch.BrushShape = NewShape;
	if (lpImage && Window.hCursor == Window.hNullCursor)
	 	DisplayBrush(lpImage->hWnd, 32767, 32767, ON);
	if ( Tool.hRibbon )
		{
		CheckComboItem( Tool.hRibbon, IDC_BRUSHSHAPE, IDC_BRUSHCIRCLE,
			IDC_BRUSHCUSTOMNEW, Retouch.BrushShape );
		SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
		}
	break;

	case IDM_MOVEUP:
	case IDM_MOVEDOWN:
	case IDM_MOVELEFT:
	case IDM_MOVERIGHT:
	if (!lpImage)
		break;
	lpBase = ImgGetBase(lpImage);
	if (ImgGetSelObject(lpImage, NULL) == lpBase)
		break;
	dx = dy = 0;
	if (wID == IDM_MOVEUP)
		--dy;
	if (wID == IDM_MOVEDOWN)
		++dy;
	if (wID == IDM_MOVELEFT)
		--dx;
	if (wID == IDM_MOVERIGHT)
		++dx;
	lpObject = NULL;
	while (lpObject = ImgGetSelObject(lpImage, lpObject))
		{
		rect = lpObject->rObject;
		OffsetRect(&rect, dx, dy);
		if (!AstralIntersectRect(&rTemp, &lpBase->rObject, &rect))
			break;
		}
	if (lpObject)
		break;
	AstralSetRectEmpty(&rAll);
	lpObject = NULL;
	while (lpObject = ImgGetSelObject(lpImage, lpObject))
		{
		rect = lpObject->rObject;
		OffsetRect(&lpObject->rObject, dx, dy);
		AstralUnionRect(&rAll, &rAll, &lpObject->rObject);
		if (!lpObject->Pixmap.fNewFrame &&
				EqualRect(&rect, &lpObject->rUndoObject))
			lpObject->rUndoObject = lpObject->rObject;
		UpdateImage(&rect, TRUE);
		UpdateImage(&lpObject->rObject, TRUE);
		}
	if (wID == IDM_MOVEUP)
		{
		x = (rAll.left + rAll.right)/2;
		y = rAll.top;
		}
	else
	if (wID == IDM_MOVEDOWN)
		{
		x = (rAll.left + rAll.right)/2;
		y = rAll.bottom;
		}
	else
	if (wID == IDM_MOVELEFT)
		{
		x = rAll.left;
		y = (rAll.top + rAll.bottom)/2;
		}
	else
	if (wID == IDM_MOVERIGHT)
		{
		x = rAll.right;
		y = (rAll.top + rAll.bottom)/2;
		}
	File2Display(&x, &y);
	AutoScroll(lpImage->hWnd, x, y);
	AstralUpdateWindow(lpImage->hWnd);
	DisplayInfo(-1, &rAll);
	break;

	case IDM_LASTTOOL:
	DeactivateTool();
	if ( Tool.idLast && (hWnd = AstralDlgGet(IDD_MAIN)) )
		SendMessage( hWnd, WM_COMMAND, Tool.idLast, 2L);
	break;

	case IDM_PREF:
	// prevent problems if running animations and they change
	// the wave mix dll setting in preferences
	StopAnimation();
	AstralDlg( NO|2, hInstAstral, hWindow, IDD_PREF, DlgPrefProc );
	break;

	case IDC_VIEWLAST: // duplicate of function in the view ribbon
	if ( !lpImage )
		break;
	RevertLastView();
	break;

//	case IDC_VIEWFULL: // duplicate of function in the view ribbon
//	if ( !lpImage )
//		break;
//	AstralDlg( NO, hInstAstral, hWindow, IDD_VIEWFULL, DlgFullScreenViewProc );
//	break;

	case IDC_VIEWALL: // duplicate of function in the view ribbon
	if ( !lpImage )
		break;
	ViewAll();
	break;

	case IDC_ZOOMIN: // duplicate of function in the view ribbon
	if ( !lpImage )
		break;

	if (!lpImage->lpDisplay)
		break;

	if (!lpImage->lpDisplay->ViewPercentage)
		break;

	x = ( lpImage->lpDisplay->FileRect.left
			+ lpImage->lpDisplay->FileRect.right ) / 2;
	y = ( lpImage->lpDisplay->FileRect.top
			+ lpImage->lpDisplay->FileRect.bottom ) / 2;
	SaveLastView();
	Zoom(x,y, +100, YES, ( View.ZoomWindow ^ CONTROL ) );
	break;

	case IDC_ZOOMOUT: // duplicate of function in the view ribbon
	if ( !lpImage )
		break;

	if (!lpImage->lpDisplay)
		break;

	if (!lpImage->lpDisplay->ViewPercentage)
		break;

	x = ( lpImage->lpDisplay->FileRect.left
			+ lpImage->lpDisplay->FileRect.right ) / 2;
	y = ( lpImage->lpDisplay->FileRect.top
			+ lpImage->lpDisplay->FileRect.bottom ) / 2;
	SaveLastView();
	Zoom(x,y, -100, YES,( View.ZoomWindow ^ CONTROL ) );
	break;

	case IDM_HELP:
	Control.Hints = !Control.Hints;
	PutDefInt (Control.Hints,Control.Hints);
	break;

	default:
	return( FALSE );
	}

return( TRUE );
}


/***********************************************************************/
BOOL HandleSaveCommand(HWND hWindow, WPARAM wParam, BOOL fCommand)
/***********************************************************************/
{
FNAME szFileName;
//LPSTR lpString;
int idDataType, idOldDataType, idFileType, idOldFileType;
BOOL fChanged, fStatus;
DWORD dwReturn;

if (ImgInMaskEditMode(lpImage))
	{
	ImgSetupMaskEditMode(lpImage);
	UpdateStatusBar(NO, NO, NO, NO);
	}
fStatus = YES;
switch (wParam)
	{
	case IDM_SAVE:
	if ( !lpImage )
		break;
	PPOLEClientNotify( (LPIMAGEOBJECT)lpImage->pObj, OLE_CHANGED );
#ifdef USEOLE
	OleSavedServerDoc( lpImage->lhDoc );
#endif // #ifdef USEOLE
	if ( lpImage->fOwnedByClient )
		{ // If it IS embedded client data, inform the client
//		ImgEditApplyAll( lpImage, YES/*fApplyData*/, YES/*fApplyAlpha*/ );
		break;
		}
	/* Save the image under the current name */
	if ( !lpImage->fUntitled) /* If the picture has a name, save into it */
		{
		if ( !(fStatus = AstralImageSave( lpImage->FileType,
			lpImage->DataType, lpImage->CurFile )) )
			break;
		SetImgChanged(lpImage, NO);
//		ImgEditApplyAll( lpImage, YES/*fApplyData*/, YES/*fApplyAlpha*/ );
		break;
		}
	/* else fall through to SaveAs... */

	case IDM_SAVEAS:
	if ( !lpImage )
		break;
	/* Bring up the modal 'SaveAs' box */
	// save current image format information
	lstrcpy(szFileName, lpImage->CurFile);
	fChanged = lpImage->fChanged;
	idOldDataType = lpImage->DataType;
	idOldFileType = IDN_ART;    // save all as ART files

	// ask user for a name
	if ( !(dwReturn = DoOpenDlg( hWindow, IDD_SAVEAS,
	     lpImage->FileType, szFileName, YES )) )
		{
		fStatus = NO;
		break;
		}

	// save the image in the user specified file type and data type
	idFileType = LOWORD( dwReturn );
	idDataType = HIWORD( dwReturn );

	if ( !(fStatus = AstralImageSave( idFileType, idDataType, szFileName )))
		break;

//	// If it IS NOT embedded client data, inform the client
	PPOLEClientNotify( (LPIMAGEOBJECT)lpImage->pObj, OLE_CHANGED );
	
#ifdef USEOLE
	OleSavedServerDoc( lpImage->lhDoc );
#endif // #ifdef USEOLE
	
	// whenever you save changes become permanent - NOT ANY MORE - tmr
	SetImgChanged(lpImage, NO);
//	ImgEditApplyAll( lpImage, YES/*fApplyData*/, YES/*fApplyAlpha*/ );

	// Add the file name to our recall list
	AddRecallImageName( szFileName );

	if (!fCommand)
		break;

	// If this is a chunked image, don't revert or change name
	if (lpImage->DocumentType == IMG_DOCPIECE)
		break;
	if (ImgInMaskEditMode(lpImage))
		break;

	// if data type or file type did not change, just take new name
	if ( idDataType == idOldDataType && idFileType == idOldFileType )
		{ // it has the same FileType, same DataType
		lpImage->fUntitled = NO;
			lstrcpy( lpImage->CurFile, szFileName );
			/* Force the non-client area (title bar) of image to paint */
				SetTitleBar(lpImage->hWnd);
#ifdef USEOLE		
		// If it IS NOT embedded client data, inform the client
		OleRenameServerDoc( lpImage->lhDoc, lpImage->CurFile);
#endif // #ifdef USEOLE		
		break;
		}

	// see if the user wants to convert to using the new format
	if ( !AstralAffirm(IDS_CONFIRM_CONVERT) )
		{ // the user doesn't want to convert to the new format
		lpImage->fChanged = fChanged; // restore the changed flag
		break;
		}

	// the user wants to convert to the new format
	lpImage->FileType = idFileType;
	lpImage->DataType = idDataType;
	lpImage->fUntitled = NO;
	lstrcpy( lpImage->CurFile, szFileName );
	/* Force the non-client area (title bar) of image to paint */
		SetTitleBar(lpImage->hWnd);
#ifdef USEOLE		
		// If it IS NOT embedded client data, inform the client
		OleRenameServerDoc( lpImage->lhDoc, lpImage->CurFile);
#endif // #ifdef USEOLE		
	DoCommand(hWindow, IDM_REVERT, 0L);
	break;

	case IDM_SAVESPECIAL:
	if ( !lpImage )
		break;
	// save current image format information
	lstrcpy(szFileName, lpImage->CurFile);
	fChanged = lpImage->fChanged;
	idOldDataType = lpImage->DataType;
	idOldFileType = IDN_ART;    // save all as ART files

	if (lpImage->fUntitled)
	{
    	// get the new name for the file
    	GetNextIniFilename ((char FAR *)&szFileName);
    	SaveToIniEntry (szFileName, NULL);
    }
	else
	{
		lstrcpy (szFileName, lpImage->CurFile);
	}
    // save the image as the same type
    idFileType = idOldFileType;
    idDataType = idOldDataType;
	if ( !(fStatus = AstralImageSave( idFileType, idDataType, szFileName )))
		break;

	// whenever you save changes become permanent - NOT ANY MORE - tmr
	SetImgChanged(lpImage, NO);
//	ImgEditApplyAll( lpImage, YES/*fApplyData*/, YES/*fApplyAlpha*/ );

	// Add the file name to our recall list
	AddRecallImageName( szFileName );

	if (!fCommand)
		break;

	// If this is a chunked image, don't revert or change name
	if (lpImage->DocumentType == IMG_DOCPIECE)
		break;
	if (ImgInMaskEditMode(lpImage))
		break;

	// if data type or file type did not change, just take new name
	if ( idDataType == idOldDataType && idFileType == idOldFileType )
		{ // it has the same FileType, same DataType
		lpImage->fUntitled = NO;
			lstrcpy( lpImage->CurFile, szFileName );
			/* Force the non-client area (title bar) of image to paint */
				SetTitleBar(lpImage->hWnd);
#ifdef USEOLE
	OleRenameServerDoc( lpImage->lhDoc, lpImage->CurFile );
#endif // #ifdef USEOLE
		break;
		}

	// see if the user wants to convert to using the new format
	if ( !AstralAffirm(IDS_CONFIRM_CONVERT) )
		{ // the user doesn't want to convert to the new format
		lpImage->fChanged = fChanged; // restore the changed flag
		break;
		}

	// the user wants to convert to the new format
	lpImage->FileType = idFileType;
	lpImage->DataType = idDataType;
	lpImage->fUntitled = NO;
	lstrcpy( lpImage->CurFile, szFileName );
	/* Force the non-client area (title bar) of image to paint */
		SetTitleBar(lpImage->hWnd);
	// If it IS NOT embedded client data, inform the client
#ifdef USEOLE
	OleRenameServerDoc( lpImage->lhDoc, lpImage->CurFile );
#endif // #ifdef USEOLE
	DoCommand(hWindow, IDM_REVERT, 0L);
	break;

	default:
	break;
	}

return( fStatus );
}

/***********************************************************************/
BOOL HandleCommandLine(
/***********************************************************************/
HWND 	hWindow,
LPSTR 	lpszCmdLine,
LPINT 	lpPrint)
{
FNAME szFileName;
LPSTR lp;
BOOL bPassedByClient, bGotImage;
LPIMAGE lpImageDoc;
HWND hWnd;
int i;

bPaintAppActive = TRUE;

*lpPrint = NO;
if ( !lpszCmdLine )
	{ // If no command line, bring up an empty image
	SendMessage( hWindow, WM_COMMAND, IDM_NEW, 0L );
	if ( (hWnd = AstralDlgGet(IDD_MAIN)) && !IsWindowVisible( hWnd ) )
		ShowWindow( hWnd, SW_SHOW );
	return( TRUE );
	}

// Check if we have "[/ | -]embedding" and a possible filename.
// usage: PP ["[-/]embedding"] file1 file2 ...

// See if the command line is being passed by a client
bPassedByClient = NO;
if ( (lp = lstrfind( lpszCmdLine, "embedding" )) &&
	 (lp == lpszCmdLine || lp == (lpszCmdLine+1)) )
	{ // Command line passed by a client
	bPassedByClient = YES;
	lpszCmdLine = SkipSpaces( lp + 9 ); // skip over "embedding"
	}

bGotImage = NO;
while ( *lpszCmdLine )
	{ // Process any files and switches on the command line

	// Skip white space and see if we're done...
	lpszCmdLine = SkipSpaces( lpszCmdLine );
	if ( !(*lpszCmdLine ) )
		break; // If NULL get out

	// Check for any switches preceeding the file name: only /p for now
	while ( *lpszCmdLine == '/' )
		{ // a switch...
		lpszCmdLine++; // Skip over the slash
		if ( !*lpszCmdLine )
			break; // If NULL get out
		if ( *lpszCmdLine == 'p' || *lpszCmdLine == 'P' )
			*lpPrint = YES;
		lpszCmdLine++; // Skip the option character
		lpszCmdLine = SkipSpaces( lpszCmdLine );
		}

	// Skip white space and see if we're done...
	lpszCmdLine = SkipSpaces( lpszCmdLine );
	if ( !(*lpszCmdLine ) )
		break; // If NULL get out

	// Try to zap the space after a single file name
	if ( (lp = lstrfind( lpszCmdLine, " " )) )
		*lp = '\0'; // If we found a space, zap it

	// Copy the full path name into szFileName
#ifdef _MAC
	lstrcpy( szFileName, lpszCmdLine );
#else	
	if ( lstrchr( lpszCmdLine, '\\' ) )
		lstrcpy( szFileName, lpszCmdLine );
	else
		{ // If not a full path name...
		GetCurrentDir( szFileName, sizeof(FNAME) );
		FixPath( szFileName );
		lstrcat( szFileName, lpszCmdLine );
		}
#endif

	// Now we're done with lpszCmdLine, so set it up for the next loop
	if ( lp ) // If we had found a space, there might be more file names
			lpszCmdLine = lp + 1;
	else	lpszCmdLine += lstrlen(lpszCmdLine); // Point to nothing

	// Special handling of documents passed by a client
////if ( bPassedByClient )
////	{ // Loop through documents to see if it's already open
		for ( i=0; i<NumDocs(); i++ )
			{
			hWnd = GetDoc(i);
			if ( !(lpImageDoc = (LPIMAGE)GetWindowLong( hWnd, GWL_IMAGEPTR )))
				continue;
			if ( !StringsEqual( lpImageDoc->CurFile, szFileName ) )
				continue;
			// It's already open....
			SendMessage( hClientAstral, WM_MDIACTIVATE, (WPARAM)hWnd, 0L );
			if ( bPassedByClient )
				lpImageDoc->fOwnedByClient = YES;
			szFileName[0] = '\0'; // Zap it
			bGotImage = YES;
			break;
			}
////	}

	// If we have a file name, open it...
	if ( *szFileName )
		{
		if ( AstralImageLoad( NULL, szFileName, MAYBE, YES ) )
			bGotImage = YES;
		}

	// If the printing option was passed, print it and close it
	if ( *lpPrint && lpImage )
		{
		SendMessage(hWndAstral, WM_COMMAND, IDM_PRINT, 0L );
		CloseImage( NO, NULL );
		}
	}

if ( !bGotImage )
	{
	if ( !idCurrentRoom )
		{
	    GoRoom (hInstAstral, -1, FALSE);
		return( FALSE );
		}
	// If no image was opened via the command line, go to the opening screen
	SendMessage( hWindow, WM_COMMAND, IDM_NEW, 0L );
	}

if ( (hWnd = AstralDlgGet(IDD_MAIN)) && !IsWindowVisible( hWnd ) )
	ShowWindow( hWnd, SW_SHOW );

return( TRUE );
}


