// ®FD1¯®PL1¯®TP0¯®BT0¯®RM255¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "twain.h"
#include "dca_glue.h"
#include "dca_acq.h"
#include "plugin.h"
#include "browser.h"
#include "svrdoc.h"
#include "svritem.h"
#include "paledit.h"
#include "cmdundo.h"
#include "ctxhelp.h"
#include "ppimport.h"

extern WPARAM MDIFrame_wParam;
extern LPARAM MDIFrame_lParam;

/***********************************************************************/
long DoCommand( HWND hWindow, UINT idCmd, HWND hControl, UINT codeNotify )
/***********************************************************************/
{
FNAME       szFileName;
BOOL        bPrint;
LPTSTR      lpszCmdLine;
LPIMAGE     lpImage;

lpImage = GetActiveImage();

//if ( lpImage && idCmd >= PS_FIRST_PLUGIN && idCmd <= PS_LAST_PLUGIN )
if( idCmd >= PS_FIRST_PLUGIN && idCmd <= PS_LAST_PLUGIN )
    {
    DeactivateTool();
    return( HandlePluginCommand( lpImage, MDIFrame_wParam, MDIFrame_lParam ));
    }

//if ( lpImage && idCmd >= PS_FIRST_ADOBEPLUGIN && idCmd <= PS_LAST_ADOBEPLUGIN )
if( idCmd >= PS_FIRST_ADOBEPLUGIN && idCmd <= PS_LAST_ADOBEPLUGIN )
    {
    DeactivateTool();
    return( HandleAdobePluginCommand( lpImage, idCmd ));
    }
	
// handle messages which should be passed to the ribbons
if (WITHIN(idCmd, IDM_FIRST_RIBBONMENU, IDM_LAST_RIBBONMENU))
{
	if(Tool.hRibbon) 
		SEND_WM_COMMAND(Tool.hRibbon, idCmd, hControl, codeNotify);
}

switch (idCmd)
    {
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
	 lstrcpy(szFileName, Control.RecallImage[idCmd - IDM_RECALLIMAGE0]);
    if( OPTION3 && Control.Retail )
        DoLowResLoad( 0, szFileName );
    else
    if( OPTION1 && Control.Retail )
        PictPubApp.OpenDocumentFileImmediate( szFileName, OM_FASTBITS );
    else
        AstralImageLoadBG( 0, szFileName, YES );
    break;

	case IDM_ALTOPEN:
    DeactivateTool();
    // Command line sent from a second instance prior to it closing
    /* If the caller passed in a file name, try to load it or print it */
/*  
    if ( !(lpszCmdLine = (LPTR)lParam) )
        break;
*/
    HandleCommandLine( hWindow, lpszCmdLine, &bPrint );
	break;

    case PS_SERVICE_ABORT_BEGIN:
    case PS_SERVICE_ABORT_CHECK:
    case PS_SERVICE_ABORT_END:
    case PS_SERVICE_ABORT_BEGIN_NOESC:
    case PS_SERVICE_BUF_DELETE:
    case PS_SERVICE_GET_IMAGE_INFO:
    case PS_SERVICE_PRIVATE_INI:
    case PS_SERVICE_BUF_ALLOC:
    case PS_SERVICE_BUF_LOCK1:
    case PS_SERVICE_BUF_UNLOCK1:
    case PS_SERVICE_BUF_NEXTLINE:
    case PS_SERVICE_BUF_LINEPTR:
    case PS_SERVICE_BUF_LINE_TABLE:
    return( HandlePluginMessage( lpImage, MDIFrame_wParam, MDIFrame_lParam ));
    break;

    case IDM_RUN0:
    case IDM_RUN1:
    case IDM_RUN2:
    case IDM_RUN3:
    case IDM_RUN4:
    case IDM_RUN5:
    case IDM_RUN6:
    case IDM_RUN7:
    case IDM_RUN8:
    case IDM_RUN9:
    RunTask( hWindow, idCmd, IDS_RUNTASKLIST );
    break;

    case IDM_CLOSEIT:
    DeactivateTool();
    PictPubApp.CloseDocumentFile(lpImage);
    break;

    default:
    return( FALSE );
    }

return( TRUE );
}

/***********************************************************************/
BOOL HandleOpenCommand(HWND hWindow)
/***********************************************************************/
{   
    OPENSTATE   	info;
    int         	i;
    FNAME       	szName;
    LPTSTR       	lpString;
    
    /* Bring up the modal 'File Open' box */
    info.bSaving = FALSE;
    info.szFileSpec[0] = info.szAlbum[0] = info.szPath[0] = _T('\0');
    info.idFileType = 0;
    info.wFileAttr = -1;
    info.idDataType = IDC_SAVELA;
    info.bImported = FALSE;

    if (!AstralStr(IDS_FILEOPEN_TITLE, &lpString))
        lstrcpy(info.szTitle, _T("Browser"));
    else
        lstrcpy(info.szTitle, lpString);
    if (!AstralStr(IDS_FILEOPEN_BUTTON, &lpString))
        lstrcpy(info.szOK, _T("OPEN"));
    else
        lstrcpy(info.szOK, lpString);
    if ( !(DoOpenDlg( hWindow, &info)))
        return(FALSE);
    for (i=0; i<info.numFiles;i++)
    {
        lstrcpy(szName, info.lpFileList[i]);
        if (!PromptForVolume(szName, &info.lpFileListVolumes[i], YES))
            continue;
        if (( info.OpenMode == OM_FASTBITS ) && Control.Retail )
            PictPubApp.OpenDocumentFileImmediate (szName,OM_FASTBITS,
					&info.cmsInfo);
        else 
        if (( info.OpenMode == OM_LOWRES ) && Control.Retail )
            DoLowResLoad( info.idFileType, szName, &info.cmsInfo );
        else
	        AstralImageLoadBG(info.idFileType, szName, TRUE, &info.cmsInfo);
    }
    FreeUp(info.lpFileList);
    return(TRUE);
}

/***********************************************************************/
BOOL HandleImportCommand( HWND hWindow )
/***********************************************************************/
{   
    OPENSTATE   info;
    int         i;
    FNAME       szName;
    LPTSTR       lpString;
    
    // Bring up the modal 'File Import' box 
    info.bSaving = FALSE;
    info.szFileSpec[0] = info.szAlbum[0] = info.szPath[0] = _T('\0');
    info.idFileType = 0;
    info.wFileAttr = -1;
    info.idDataType = IDC_SAVELA;
    info.bImported = TRUE;

    if( !AstralStr( IDS_FILEIMPORT_TITLE, &lpString ))
        lstrcpy(info.szTitle, _T("Import Browser") );
    else
        lstrcpy( info.szTitle, lpString );

    if( !AstralStr( IDS_FILEIMPORT_BUTTON, &lpString ))
        lstrcpy( info.szOK, _T("IMPORT") );
    else
        lstrcpy( info.szOK, lpString );

    if( !( DoOpenDlg( hWindow, &info )))
        return( FALSE );

    for( i=0; i<info.numFiles; i++ )
    {
        lstrcpy( szName, info.lpFileList[i] );

        if( !PromptForVolume( szName, &info.lpFileListVolumes[i], YES ))
            continue;

        if(( info.OpenMode == OM_FASTBITS ) && Control.Retail )
            PictPubApp.OpenDocumentFileImmediate( szName, OM_FASTBITS,
					&info.cmsInfo );
        else 
        if(( info.OpenMode == OM_LOWRES ) && Control.Retail )
            DoLowResLoad( info.idFileType, szName, &info.cmsInfo );
        else
            AstralImageLoadBG( info.idFileType, szName, TRUE, &info.cmsInfo );
    }
    FreeUp( info.lpFileList );
    return( TRUE );
}

/***********************************************************************/
BOOL HandleCommandLine( HWND hWindow, LPTSTR lpszCmdLine, LPINT lpPrint )
/***********************************************************************/
{
	FNAME    szFileName;
	LPTSTR   lp;
	BOOL     bPassedByClient;
	LPIMAGE  lpImageDoc;
	HWND     hWnd;
	WORD 	 wChar;

	*lpPrint = NO;
	if( !lpszCmdLine )
    	return( TRUE );

	// Check if we have "[/ | -]embedding" and a possible filename.
	// usage: PP ["[-/]embedding"] file1 file2 ...

	//dbg("full command line = '%ls'", lpszCmdLine );

	// See if the command line is being passed by a client
	bPassedByClient = NO;
	if(( lp = lstrfind( lpszCmdLine, _T("embedding") )) &&
   	   ( lp == lpszCmdLine || lp == ( MyCharNext( lpszCmdLine ))))
   	{ 	// Command line passed by a client
		//  dbg(_T("command line passed by client"));
    	bPassedByClient = YES;
    	lpszCmdLine = SkipSpaces( MyCharNextN( lp, 9 )); 	// skip over _T("embedding")
   	}

	while( *lpszCmdLine )		// Process any files and switches on the command line
   	{ 
    	// Skip white space and see if we're done...
    	lpszCmdLine = SkipSpaces( lpszCmdLine );
    	if( !( *lpszCmdLine ))	// If NULL get out
        	break; 

		//  dbg( _T("file='%ls' print=%d"), lpszCmdLine, *lpPrint );dbg(0);

    	// Check for any switches preceeding the file name: only /p for now
    	while( TRUE )	
    	{ 
			if( IsDBCSLeadByte(( BYTE )*lpszCmdLine ))
				wChar = *(( LPWORD )lpszCmdLine );
			else
				wChar = *lpszCmdLine;

			if( wChar != _T('/'))	// if not a switch...
				break;

        	lpszCmdLine = MyCharNext( lpszCmdLine );	// Skip over the slash

        	if( !*lpszCmdLine )		// If NULL get out
            	break; 

			if( IsDBCSLeadByte(( BYTE )*lpszCmdLine ))
				wChar = *(( LPWORD )lpszCmdLine );
			else
				wChar = *lpszCmdLine;

        	if( wChar == _T('p') || wChar == _T('P') )
            	*lpPrint = YES;

        	lpszCmdLine = SkipSpaces( MyCharNext( lpszCmdLine ));
		}

    	// Skip white space and see if we're done...
    	lpszCmdLine = SkipSpaces( lpszCmdLine );
    	if( !( *lpszCmdLine ))		// If NULL get out
        	break; 

    	// Try to zap the space after a single file name
    	if(( lp = lstrfind( lpszCmdLine, _T(" ") )))
        	*lp = _T('\0'); // If we found a space, zap it

    	// Copy the full path name into szFileName
    	if( lStrChr( lpszCmdLine, _T('\\') ))
        	lstrcpy( szFileName, lpszCmdLine );
    	else				// If not a full path name...
   		{ 
        	GetCurrentDir( szFileName, sizeof(FNAME) );
        	FixPath( szFileName );
        	lstrcat( szFileName, lpszCmdLine );
		}

    	// Now we're done with lpszCmdLine, so set it up for the next loop
		// If we had found a space, there might be more file names
    	if( lp )	
        	lpszCmdLine = MyCharNext( lp );
    	else    
			lpszCmdLine += lstrlen( lpszCmdLine ); // Point to nothing

    	// Process szFileName

    	// Special handling of documents passed by a client
    	if ( bPassedByClient )
    	{
			CServerDoc *pDoc = PictPubApp.GetDocument(szFileName);
			if (pDoc)
        	{
            	// It's already open....
            	POSITION ViewPos = pDoc->GetFirstViewPosition();
            	ASSERT(ViewPos!=NULL);
            	CView* pView = pDoc->GetNextView( ViewPos );
            	ASSERT(pView);
            	// NOTE send Message to View's Parent MDIFrame
            	SendMessage( PictPubApp.Get_hClientAstral(), WM_MDIACTIVATE, 
                	(WORD)pView->GetParentFrame()->GetSafeHwnd(), 0L );
            	lpImageDoc->fOwnedByClient = YES;
            	*szFileName = _T('\0'); // Zap it
        	}
    	}

    	// If we have a file name, open it...
    	if ( *szFileName )
			hWnd = AstralImageLoad(  0, szFileName, MAYBE, YES );
    	else    
			hWnd = NULL;

    	// If the printing option was passed, print it and close it
    	if ( hWnd && *lpPrint )
    	{
         	FORWARD_WM_COMMAND(PictPubApp.Get_hWndAstral(), IDM_PRINT, NULL, 0, SendMessage);
        	CloseImage( NO, NULL, NULL );
    	}
	}
	return( TRUE );
}
