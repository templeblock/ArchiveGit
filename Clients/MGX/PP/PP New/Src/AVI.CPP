// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "pp.h"
#include <mmsystem.h>
#include "ppafx.h"
#include "id.h"
#include "aviutil.h"
#include "aviintf.h"

// Static prototypes
BOOL WINPROC EXPORT DlgAviProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
static BOOL mciSend( LPTSTR lpCommand, LPTSTR lpReturn, int iReturnSize );
static BOOL AviInit(void);
static BOOL AviEnd(void);
static int AviTotalFrames(void);
static int AviCurrentFrame(void);
static BOOL AviOpen(HWND hWnd, LPTSTR lpFileName);
static BOOL AviClose(HWND hWnd);
static BOOL AviPosition(HWND hWnd);
static void AviPlay(HWND hWnd, ITEMID wDirection);
static void AviSeek(HWND hWnd, int nFrame);
static void AviStep(HWND hWnd, ITEMID wDirection);
static int AviGetFrameCount(LPTSTR lpFileName);

// Static data
static BOOL fPlaying;	// Play flag: TRUE == playing, FALSE == paused
static BOOL fSilent;
static LPFRAME lpFrame;
static HWND hWndCallBack;
static int nLastFrame;
static RECT rcMovie;	// the rect where the movie is positioned
						// for QT/W this is the movie rect, for AVI
						// this is the location of the playback window

typedef TCHAR MCISTRING[255];

//***********************************************************************
//	if frameNum = -1 then will prompt user
//	AVIOptions.AVIPosition is the frame read
//***********************************************************************
LPFRAME avirdr( LPTSTR ifile, int outdepth, LPINT lpDataType, int frameNum )
//***********************************************************************
{
	int nBits;
	HDC hDC;
	BOOL bPaletteColor;
	
	fSilent = NO;
	lstrcpy( Names.Saved, ifile );
	
	if (frameNum < 0)
	{
		// prompt user
		if ( !AstralDlg( NO, PictPubApp.GetResourceHandle(), MessageParent(), IDD_AVI, DlgAviProc ) )
			return( NO );
		frameNum = AVIOptions.AVIPosition;
	}
	if (!LoadAviDLL())
		return(NULL);
	// open frame
	AstralCursor(IDC_WAIT);
	lpFrame = aviGetFrame(ifile, frameNum);
//	lpFrame = AviReadFrame( ifile,-1, NULL, frameNum );
	AVIOptions.AVIPosition = frameNum;
	AstralCursor(NULL);
	
	if ( !lpFrame )
		goto ExitError;
	
	switch( FrameType( lpFrame ) )
	{
		case FDT_LINEART :
			*lpDataType = IDC_SAVELA;
		break;
	
		case FDT_GRAYSCALE :
			*lpDataType = IDC_SAVECT;
		break;
	
		case FDT_RGBCOLOR :
			if ( !(hDC = GetDC(NULL)) )
				bPaletteColor = NO;
			else
			{
				nBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
				bPaletteColor = ( nBits <= 8 );
				ReleaseDC( NULL, hDC );
			}
			if (bPaletteColor)
				*lpDataType = IDC_SAVE8BITCOLOR;
			else
				*lpDataType = IDC_SAVE24BITCOLOR;
		break;
	
		case FDT_CMYKCOLOR :
			*lpDataType = IDC_SAVE32BITCOLOR;
		break;
	}
	FreAviDLL();
	return( lpFrame );

ExitError:
	FreAviDLL();
	return(NULL);
}


//***********************************************************************
//	lpBaseFile is the file which the rest of the video comes from
//	Note AVIOptions must be filled in.
//***********************************************************************
int aviwrt( LPTSTR ifile, LPOBJECT lpObject, LPFRAME lpFrame, LPRECT lpRect, int flag, LPTSTR lpBaseFile)
//***********************************************************************
{
	if (!LoadAviDLL())
		return(-1);
	if (!lpBaseFile)
	{
		// create new AVI file
		if(!aviNewFile(ifile, lpFrame, lpObject))
			goto ExitError;
	}
	else
	{
		if(!aviInstertFrame(lpBaseFile, 
							ifile, 
							AVIOptions.iAVIPosType, 
							AVIOptions.AVIPosition, 
							AVIOptions.iAVISizingType == 1/*AVI_CLIP*/, 
							lpFrame, 
							lpObject))
			goto ExitError;
	}
	FreAviDLL();
	return(0);

ExitError:
	FreAviDLL();
	return(-1);
}


/***********************************************************************/
int DoAviSave( LPIMAGE lpImage, int idDataType, LPTSTR lpFileName )
/***********************************************************************/
{
	LPFRAME lpFrame, lpCombinedFrame;
	LPOBJECT lpObject;
	FNAME szTempName;
	RECT rArea;
	int ret;
	
	if (!lpImage)
		return(FALSE);
	lpFrame = ImgGetBaseEditFrame(lpImage);
	if (!lpFrame)
		return(FALSE);
	lpObject = ImgGetBase(lpImage);
	
	if (ImgCountObjects( lpImage ) <= 1)
		lpObject = NULL;

	
	// check destination
	if (!FileExists(lpFileName))  
	{
		// we are just saving a new file
		ret = aviwrt( lpFileName, lpObject, lpFrame, NULL, 0, NULL);
		if (ret < 0)
			return(FALSE);
		goto DoneWriting;			
	}
	
	// we are adding to an existing file
	AVIOptions.AVILastPosition = AviGetFrameCount(lpFileName);
	if (!AVIOptions.AVILastPosition)
	{
		Message(IDS_EREAD, lpFileName);
		return(FALSE);
	}
	if ( lpImage->DocumentType == IMG_AVIDOC)
		AVIOptions.AVIPosition = Min(lpImage->Element, AVIOptions.AVILastPosition);
	else 
		AVIOptions.AVIPosition = AVIOptions.AVIPosition;
	if (!SetupSaveOptions(IDN_AVI, NO))
		return(FALSE);
	// copy lpFileName to a temp file so we can read from it (also so it can be retored)
	// make sure we have write access to destination file
	if (!FilePermission(lpFileName,  2 /*WRITE*/))  // write access?
	{
		Message(IDS_EWRITEPROTECT, lpFileName);
		return(FALSE);
	}
	// create temporary file name
	lstrcpy(szTempName, lpFileName);
	stripfile(szTempName);
	lstrcat(szTempName, _T("pp"));
	if (!GetTempFilename(szTempName, extension(lpFileName)))
	{
		Message(IDS_ETEMPFILE);
		return(FALSE);
	}
	// temp name creation was successful
	if (RenameFile(lpFileName, szTempName))
	{
		Message(IDS_ETEMPFILE);
		return(FALSE);
	}
	ret = aviwrt(lpFileName,  lpObject, lpFrame, NULL, 0, szTempName);
	if (ret >= 0)  // successful write, bag temp file
		FileDelete(szTempName); // don't care if error
	else		// unsuccessful write, rename temp to original
	{
		if (RenameFile(szTempName, lpFileName)) // do rename
			FileDelete(szTempName);	// error, bag temp file
		return(FALSE);
	}
DoneWriting:
	if(Browser.CreateThumbnailOnSave)
	{
		if (lpObject)
		{
			rArea.left = rArea.top = 0;
			rArea.right = rArea.bottom = 63;
			lpCombinedFrame = ImgCopyFrame(lpImage,NULL,&rArea,NO,NO);
		}
		else
			lpCombinedFrame = lpFrame;
		
		// UpdateThumbnail does scale down
		if (lpCombinedFrame)
			UpdateThumbnail(lpFileName,lpCombinedFrame,
				FrameXSize(lpFrame), 
				FrameYSize(lpFrame),
				idDataType,	IDN_AVI);
	}
	SetImgChanged(lpImage, NO);
	return(TRUE);
return(FALSE);
}


/***********************************************************************/
BOOL WINPROC EXPORT DlgAviProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	HWND hWnd;
	int i;
	static BOOL bOpen;
	
	hWndCallBack = hDlg;
	hWnd = GetDlgItem( hDlg, IDC_AVI_PREVIEW );
	switch (msg)
	    {
	    case WM_INITDIALOG:
		if ( !AviInit() )
		{
			AstralDlgEnd( hDlg, FALSE );
			break;
		}
		if ( !AviOpen( hWnd, Names.Saved ) )
		{
			AviEnd();
			AstralDlgEnd( hDlg, FALSE );
			break;
		}
		bOpen = YES;
		nLastFrame = AviTotalFrames();
		CenterPopup( hDlg );
		InitSlide( hDlg, IDC_AVI_FRAMES, 1, 1, Max(nLastFrame,2) );
		SetDlgItemInt( hDlg, IDC_AVI_LASTFRAME, nLastFrame, NO );
		SetDlgItemInt( hDlg, IDC_AVI_FRAME, 1, NO );
		SetWindowWord( GetDlgItem(hDlg,IDC_AVI_PLAY), GWW_ICONID, IDC_AVI_RPLAY);
		fPlaying = FALSE;
		break;
	
		case WM_SIZE:
		if ( bOpen )
			AviPosition( hWnd );	// re-center the movie
		return( FALSE );
	
		case WM_DESTROY:
		if (bOpen)
		{
			AviClose( hWnd );
			AviEnd();
			bOpen = NO;
		}
		return( FALSE );
	
		case MM_MCINOTIFY:
		// This is where we check the status of an AVI
		// movie that might have been playing.	We do
		// the play with MCI_NOTIFY on so we should get
		// a MCI_NOTIFY_SUCCESSFUL if the play
		// completes on it's own.
		switch(wParam)
		{
			case MCI_NOTIFY_SUCCESSFUL:
			// the play finished, let's rewind and clear our flag.
			fPlaying = FALSE;
			mciSend( _T("seek mov to start"), NULL, 0 );
			SetWindowWord( GetDlgItem(hDlg,IDC_AVI_PLAY), GWW_ICONID, IDC_AVI_RPLAY);
			AstralControlRepaint( hDlg, IDC_AVI_PLAY );
			i = AviCurrentFrame()+1;
			SetSlide( hDlg, IDC_AVI_FRAMES, i );
			SetDlgItemInt( hDlg, IDC_AVI_FRAME, i, NO );
			return( FALSE );
		}
		break;
	
	    case WM_SETCURSOR:
		return( SetupCursor( ( HWND )wParam, lParam, IDD_AVI ) );
	
	    case WM_CTLCOLOR:
	//		return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
	//		HIWORD(lParam) ) );
			return(1);
	
	    case WM_COMMAND:
		switch (wParam)
		    {
			case IDC_AVI_CONFIGURE:
				mciSend( _T("configure mov"), NULL, 0 );
	//			if ( bOpen )
	//				AviPosition( hWnd );	// re-center the movie
				break;
	
		    case IDC_AVI_FRAMES:
				if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
					break;
				i = HandleSlide( hDlg, wParam, lParam, NULL );
				AviSeek( hWnd, i-1 );
			break;
	
			case IDC_AVI_HOME:
				AviSeek( hWnd, 0 );
				break;
	
			case IDC_AVI_END:
				AviSeek( hWnd, nLastFrame-1 );
				break;
	
			case IDC_AVI_PLAY:
			case IDC_AVI_RPLAY:
				AviPlay( hWnd, wParam );
				break;
	
			case IDC_AVI_STEP:
			case IDC_AVI_RSTEP:
				AviStep( hWnd, wParam );
				break;
	
		    case IDOK:
				AVIOptions.AVIPosition = AviCurrentFrame();
				AviClose( hWnd );
				AviEnd();
				bOpen = NO;
				AstralCursor(IDC_WAIT);
//				lpFrame = AviReadFrame(Names.Saved, -1, NULL, AVIOptions.AVIPosition );
				AstralCursor(NULL);
				AstralDlgEnd( hDlg, TRUE );
				return(TRUE);
			break;
	
		    case IDCANCEL:
				lpFrame = NULL;
				AstralDlgEnd( hDlg, FALSE );
			break;
	
		    default:
				return( FALSE );
		    }
	
		SetWindowWord( GetDlgItem(hDlg,IDC_AVI_PLAY), GWW_ICONID,
			( !fPlaying ? IDC_AVI_RPLAY : IDC_AVI_STOP ) );
		AstralControlRepaint( hDlg, IDC_AVI_PLAY );
		i = AviCurrentFrame()+1;
		SetSlide( hDlg, IDC_AVI_FRAMES, i );
		SetDlgItemInt( hDlg, IDC_AVI_FRAME, i, NO );
		break;
	
	    default:
		return( FALSE );
	    }
	
	return( TRUE );
}


//************************************************************************
LPFRAME AviReadFrame( LPTSTR lpFileName, int outdepth, LPINT lpDataType, int nFrame )
//************************************************************************
{
	LPFRAME lpFrame;
	
	if (!LoadAviDLL())
		return(NULL);
	lpFrame = aviGetFrame(lpFileName, nFrame);
	FreAviDLL();
	return( lpFrame );
}

//************************************************************************
static BOOL mciSend( LPTSTR lpCommand, LPTSTR lpReturn, int iReturnSize )
//************************************************************************
{
	DWORD dw;
	MCISTRING szBuffer;
	BOOL bOldState;
	
	bOldState = AstralCursorEnable(NO);
#ifdef CALFIX
	dw = mciSendString( lpCommand, lpReturn, iReturnSize, hWndCallBack );
#endif
	AstralCursorEnable(bOldState);
	if (dw)
	{
		if ( !fSilent )
		{
#ifdef CALFIX
			mciGetErrorString( dw, szBuffer, sizeof(szBuffer) );
#endif
			Print( "%ls\nCommand: %ls", (LPTR)szBuffer, lpCommand );
		}
		return( FALSE );
	}
	return( TRUE );
}

//************************************************************************
static BOOL AviInit(void)
//************************************************************************
{
	return( mciSend( _T("open avivideo"), NULL, 0 ) );
}

//************************************************************************
static BOOL AviEnd(void)
//************************************************************************
{
	return( mciSend( _T("close avivideo"), NULL, 0 ) );
}

//************************************************************************
static int AviTotalFrames(void)
//************************************************************************
{
	MCISTRING szBuffer;
	
	if ( !mciSend( _T("status mov length"), szBuffer, sizeof(szBuffer) ) )
		return( FALSE );
	
	return( Ascii2Long(szBuffer));
}

//************************************************************************
static int AviCurrentFrame(void)
//************************************************************************
{
	MCISTRING szBuffer;
	
	if ( !mciSend( _T("status mov position"), szBuffer, sizeof(szBuffer) ) )
		return( FALSE );
	
	return( Ascii2Long(szBuffer));
}

//************************************************************************
// open an AVI movie and then handle the initialization to
// show the movie and position it properly.	Keep the movie paused when opened
//************************************************************************
static BOOL AviOpen( HWND hWnd, LPTSTR lpFileName )
//************************************************************************
{
	MCISTRING szBuffer;
	
	// we have a .AVI movie to open, use MCI to open it
	wsprintf( szBuffer, "open %ls alias mov style child parent %d",
		lpFileName, hWnd );
	// try to open the file
	if ( !mciSend( szBuffer, NULL, 0 ) )
		return( FALSE );
	// we opened the file o.k., now set up to play it.
	if ( !mciSend( _T("set mov seek exactly on"), NULL, 0 ) )
		return( FALSE );
	if ( !mciSend( _T("set mov time format frames"), NULL, 0 ) )
		return( FALSE );
	if ( !mciSend( _T("window mov state show"), NULL, 0 ) )
		return( FALSE );
	// now get the movie centered
	AviPosition(hWnd);
	// cause an update to occur
	InvalidateRect(hWnd, NULL, FALSE);
	UpdateWindow(hWnd);
	return( TRUE );
}


//************************************************************************
// Close the movie and anything associated with it.
// clear the <fPlaying> flag
//************************************************************************
static BOOL AviClose( HWND hWnd )
//************************************************************************
{
	if ( !mciSend( _T("close mov"), NULL, 0 ) )
		return( FALSE );
	// cause a total repaint to occur
	InvalidateRect(hWnd, NULL, TRUE);
	UpdateWindow(hWnd);
	fPlaying = FALSE; // can't be playing any longer
	return( TRUE );
}


//************************************************************************
// sets the movie rectange <rcMovie> to be centered within the app's window.
//************************************************************************
static BOOL AviPosition( HWND hWnd )
//************************************************************************
{
	RECT       	rcClient, rcMovieBounds;
	MCISTRING   szBuffer;
	LPTSTR      psz;
	WORD 		wChar;
	
	GetClientRect(hWnd, &rcClient);	// get the parent windows rect
	
	// get the original size of the movie
	if ( !mciSend( _T("where mov source"), szBuffer, sizeof(szBuffer) ) )
		return( FALSE );
	
	SetRectEmpty(&rcMovieBounds);	// zero out movie rect
	psz = szBuffer;	// point to rectangle string returned by where command

	//while (*p == _T(' ')) p++;	// skip over starting spaces
	psz = SkipSpaces( psz );		// skip over starting spaces

	//while (*p != _T(' ')) p++;	// skip over the x (which is 0)
	psz = lStrChr( psz, _T(' ')); 	// skip over the x (which is 0)

	//while (*p == _T(' ')) p++;	// skip over spaces between x and y
	psz = SkipSpaces( psz );		// skip over spaces between x and y

	//while (*p != _T(' ')) p++;	// skip over the y (which is 0)
	psz = lStrChr( psz, _T(' ')); 	// skip over the y (which is 0)

	//while (*p == _T(' ')) p++;	// skip over the spaces between y and width
	psz = SkipSpaces( psz );		// skip over the spaces between y and width
	
	// now find the width
	//for(; *p >= _T('0') && *p <= _T('9'); p++ )
	while( TRUE )
	{
		if( IsDBCSLeadByte(( BYTE )*psz ))
			wChar = *(( LPWORD )psz );
		else
			wChar = *psz;

		if( wChar < _T('0') || wChar > _T('9'))
			break;

		rcMovieBounds.right = ( 10 * rcMovieBounds.right ) + ( wChar - _T('0'));
	}

	//while (*p == _T(' ')) p++;	// skip spaces between width and height
	psz = SkipSpaces( psz );		// skip spaces between width and height

	// now find the height
	//for (; *p >= _T('0') && *p <= _T('9'); p++)
	while( TRUE )
	{
		if( IsDBCSLeadByte(( BYTE )*psz ))
			wChar = *(( LPWORD )psz );
		else
			wChar = *psz;

		if( wChar < _T('0') || wChar > _T('9'))
			break;

		rcMovieBounds.bottom = (10 * rcMovieBounds.bottom) + ( wChar - _T('0'));
	}	

	// figure out where to position the window at
	rcMovie.left = (rcClient.right/2) - (rcMovieBounds.right / 2);
	rcMovie.top = (rcClient.bottom/2) - (rcMovieBounds.bottom / 2);
	rcMovie.right = rcMovie.left + rcMovieBounds.right;
	rcMovie.bottom = rcMovie.top + rcMovieBounds.bottom;
	
	// reposition the playback (child) window
	// get the movie window handle
	if ( !mciSend( _T("status mov window handle"), szBuffer, sizeof(szBuffer) ) )
		return( FALSE );

	MoveWindow( (HWND)Ascii2Long(szBuffer), rcMovie.left, rcMovie.top,
		rcMovieBounds.right, rcMovieBounds.bottom, TRUE );
	return( TRUE );
}


//************************************************************************
// Play/pause the movie depending on the state of the <fPlaying> flag.
// This function sets the <fPlaying> flag appropriately when done
//************************************************************************
static void AviPlay( HWND hWnd, ITEMID wDirection )
//************************************************************************
{
	if ( !wDirection )
		fPlaying = FALSE;		// !wDirection means PAUSE
	else
		fPlaying = !fPlaying;	// swap the play flag
	
	// play/pause the AVI movie
	if (fPlaying)
	{
		if (wDirection == IDC_AVI_RPLAY)
			mciSend( _T("play mov reverse notify"), NULL, 0 );
		else	
			mciSend( _T("play mov notify"), NULL, 0 );
	}
	else
		mciSend( _T("pause mov"), NULL, 0 );
}


//************************************************************************
// Seek to a desired nFrame in the movie.
// Always stop the play before seeking.
//************************************************************************
static void AviSeek( HWND hWnd, int nFrame )
//************************************************************************
{
	MCISTRING szBuffer;
	
	// first stop the movie from playing if it is playing
	if ( fPlaying )
		AviPlay(hWnd, 0);
	if ( nFrame <= 0 )
		mciSend( _T("seek mov to start"), NULL, 0 );
	else
	if ( nFrame >= nLastFrame )
		mciSend( _T("seek mov to end"), NULL, 0 );
	else
	{
		wsprintf( szBuffer, "seek mov to %d", nFrame );
		mciSend( szBuffer, NULL, 0 );
	}
}

//************************************************************************
// Step forward or reverse in the movie.
// wDirection can be IDC_AVI_STEP (forward) or IDC_AVI_RSTEP (reverse)
// Again, stop the play if one is in progress.
//************************************************************************
static void AviStep( HWND hWnd, ITEMID wDirection )
//************************************************************************
{
	if (fPlaying)
		AviPlay(hWnd, 0);	// turn off the movie
	
	if (wDirection == IDC_AVI_STEP)
	{
		if ( AviCurrentFrame() < nLastFrame )
			mciSend( _T("step mov by 1"), NULL, 0 );
	}
	else
	{
		if ( AviCurrentFrame() > 1 )
			mciSend( _T("step mov reverse by 1"), NULL, 0 );
	}
}


//***********************************************************************
//	Opens the file, reads the length, closes the file
//***********************************************************************
static int AviGetFrameCount( LPTSTR lpFileName )
//***********************************************************************
{
	MCISTRING szBuffer;

	// we have a .AVI movie to open, use MCI to open it
	wsprintf( szBuffer, "open %ls alias mov", lpFileName);
	// try to open the file
	if ( !mciSend( szBuffer, NULL, 0 ) )
		return( FALSE );
	// we opened the file o.k., now get the length.
	if ( !mciSend( _T("status mov length"), szBuffer, sizeof(szBuffer) ) )
		return( FALSE );
	if ( !mciSend( _T("close mov"), NULL, 0 ) )
		return( FALSE );
	return( Ascii2Long(szBuffer) );
}
