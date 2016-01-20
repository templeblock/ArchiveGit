//®FD1¯®PL1¯®TP0¯®BT0¯
/*
 * PPOLESVR.C
 *
 * Contains all callback functions in the OLESERVERVTBL struture:
 *      ServerCreateDoc
 *      ServerCreateDocFromTemplate
 *      ServerOpenDoc
 *      ServerEditDoc
 *      ServerExecute
 *      ServerExit
 *      ServerRelease
 *
 * Also includes the constructor function PServerAllocate.
 *
 * Copyright(c) Microsoft Corp. 1992 All Rights Reserved
 *
 */

#include <windows.h>
#include "types.h"
#include "id.h"
#include "routines.h" // only for dbg() prototype
#include "data.h" // for lpImage
#include "ppole.h"

extern HINSTANCE hInstAstral;
extern HWND hWndAstral;
extern SAVE Save;

/*
 * ServerCreateDoc
 *
 * Purpose:
 *  Creates a new document of the class in pszClass which will be
 *  embedded in a client application document whose name is in
 *  pszDoc.
 *
 * Parameters:
 *  pSvr            LPIMAGESERVER to structure identifying the server.
 *  lhDoc           LHSERVERDOC identifying the document.
 *  pszClass        LPSTR specifying the class of document to create.
 *  pszDoc          LPSTR specifying the document for use in window titles.
 *			Format is:  Client%ClientFile%ClientDocument[num]#num
 *  ppServerDoc     LPLPOLESERVERDOC in which to store a pointer
 *                  to the OLESERVERDOC structure for this new document.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
OLESTATUS FAR PASCAL EXPORT ServerCreateDoc(LPIMAGESERVER pSvr, LHSERVERDOC lhDoc,
                                  LPSTR pszClass, LPSTR pszDoc,
                                  LPLPOLESERVERDOC ppServerDoc)
/************************************************************************/
{
#ifdef USEOLE
OLESTATUS os;
int i;
LPSTR lp;
DWORD dwReturn;
FNAME szFileName;
ITEMID idFileType;

//dbg("{SCreateD pSvr=%lx lhDoc=%lx, Class=%ls Doc=%ls", pSvr, lhDoc, pszClass, pszDoc);dbg(0);

/*
 * 1.   Create a document of the specified class.
 * 2.   Allocate and initialize an OLESERVERDOC structure.
 * 3.   Store lhDoc in the OLESERVERDOC structure.
 * 4.   Pass back a pointer to the new OLESERVERDOC structure in ppServerDoc.
 * 5.   Return OLE_OK if successful, OLE_ERROR_NEW otherwise.
 */

/* Bring up the modal 'File Open' box */
os = OleBlockServer( pOLE->lhSvr ); // Block since we'll enter a message loop
dwReturn = DoOpenDlg( hWndAstral, IDD_OPEN,
	Save.FileType, szFileName, NO );
do { os = OleUnblockServer( pOLE->lhSvr, &i ); } while( i ); // Unblock

if ( !dwReturn )
	{
//	dbg("}SCreateD err; cancel open");dbg(0);
	return( OLE_ERROR_NEW );
	}

idFileType = LOWORD( dwReturn );
Save.FileType = idFileType;
PutDefaultInt( "FileType", idFileType - IDN_TIFF );

pOLE->InMethod++;
AstralImageLoad( idFileType, szFileName, MAYBE, NO );
pOLE->InMethod--;

if ( !lpImage || lpImage->lhDoc ) // If no new/unreg'd document was created...
	{
//	dbg("}SCreateD err");dbg(0);
	return( OLE_ERROR_NEW );
	}

lpImage->lhDoc = lhDoc;
lpImage->fOwnedByClient = YES;
lpImage->fUntitled = YES;
lpImage->fChanged = YES;
if ( lp = lstrfind( pszDoc, "%" ) )
    if ( lp = lstrfind( lp+1, "%" ) )
	lstrcpy( lpImage->CurFile, lp+1 );
if ( lp = lstrfind( lpImage->CurFile, "#" ) )
	*lp = '\0';
*ppServerDoc = (LPOLESERVERDOC)lpImage;
//dbg("}SCreateD lpImage=%lx", lpImage);dbg(0);

#endif // #ifdef USEOLE
return( OLE_OK );
}


/*
 * ServerCreateDocFromTemplate
 *
 * Purpose:
 *  Creates a new document initialized with the data in a specified
 *  file.  The new document is opened for editing and is embedded
 *  within the client when the server is updated or closed.
 *
 * Parameters:
 *  pSvr            LPIMAGESERVER to structure identifying the server.
 *  lhDoc           LHSERVERDOC identifying the document.
 *  pszClass        LPSTR specifying the class of document to create.
 *  pszDoc          LPSTR to the permanent name of the document to open.
 *  pszTemplate     LPSTR to the file to use as a template.
 *  ppServerDoc     LPLPOLESERVERDOC in which to store a pointer
 *                  to the OLESERVERDOC structure for this new document.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
OLESTATUS FAR PASCAL EXPORT ServerCreateDocFromTemplate(
                                LPIMAGESERVER pSvr, LHSERVERDOC lhDoc,
                                LPSTR pszClass, LPSTR pszDoc, LPSTR pszTemplate,
                                LPLPOLESERVERDOC ppServerDoc)
/************************************************************************/
{
#ifdef USEOLE
LPSTR lp;

//dbg("{STemplateD pSvr=%lx lhDoc=%lx, Class=%ls Doc=%ls", pSvr, lhDoc, pszClass, pszDoc);dbg(0);

/*
 * 1.   Create a document of the specified class.
 * 2.   Read the contents of the specified file and initialize the document.
 * 3.   Allocate and initialize an OLESERVERDOC structure.
 * 4.   Store lhDoc in the OLESERVERDOC structure.
 * 5.   Store a pointer to the new OLESERVERDOC structure in ppServerDoc.
 * 6.   Return OLE_OK if successful, OLE_ERROR_TEMPLATE otherwise.
 */

pOLE->InMethod++;
AstralImageLoad( IDN_TIFF, pszTemplate, MAYBE, NO );
pOLE->InMethod++;

if ( !lpImage || lpImage->lhDoc ) // If no new/unreg'd document was created...
	{
//	dbg("}STemplateD err");dbg(0);
	return( OLE_ERROR_TEMPLATE );
	}

lpImage->lhDoc = lhDoc;
lpImage->fOwnedByClient = YES;
lpImage->fUntitled = YES;
lpImage->fChanged = YES;
if ( lp = lstrfind( pszDoc, "%" ) )
    if ( lp = lstrfind( lp+1, "%" ) )
	lstrcpy( lpImage->CurFile, lp+1 );
if ( lp = lstrfind( lpImage->CurFile, "#" ) )
	*lp = '\0';
*ppServerDoc = (LPOLESERVERDOC)lpImage;
//dbg("}STemplateD lpImage=%lx", lpImage);dbg(0);

#endif // #ifdef USEOLE
return( OLE_OK );
}


/*
 * ServerOpenDoc
 *
 * Purpose:
 *  Opens an exiting file (document) and prepares the document for
 *  editing, generally done when a user double-clicks a linked
 *  object in the client.  Note that the server is hidden at this
 *  point.
 *
 * Parameters:
 *  pSvr            LPIMAGESERVER to structure identifying the server.
 *  lhDoc           LHSERVERDOC identifying the document.
 *  pszDoc          LPSTR to the permanent name of the document to
 *                  be opened.
 *  ppServerDoc     LPLPOLESERVERDOC in which to store a pointer
 *                  to the OLESERVERDOC structure for this new document.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
OLESTATUS FAR PASCAL EXPORT ServerOpenDoc(LPIMAGESERVER pSvr, LHSERVERDOC lhDoc,
                                LPSTR pszDoc, LPLPOLESERVERDOC ppServerDoc)
/************************************************************************/
{
#ifdef USEOLE
//dbg("{SOpenD pSvr=%lx lhDoc=%lx, Doc=%ls", pSvr, lhDoc, pszDoc);dbg(0);

/*
 * 1.   Create a document of the specified class.
 * 2.   Read the contents of the specified file and initialize the document.
 * 3.   Store lhDoc in the OLESERVERDOC structure.
 * 4.   Store a pointer to the new OLESERVERDOC structure in ppServerDoc.
 * 5.   Return OLE_OK if successful, OLE_ERROR_OPEN otherwise.
 */

pOLE->InMethod++;
AstralImageLoad( IDN_TIFF, pszDoc, MAYBE, NO );
pOLE->InMethod--;

if ( !lpImage || lpImage->lhDoc ) // If no new/unreg'd document was created...
	{
//	dbg("}SOpenD err");dbg(0);
	return( OLE_ERROR_OPEN );
	}

lpImage->lhDoc = lhDoc;
lpImage->fOwnedByClient = NO; // It's a linked file
*ppServerDoc = (LPOLESERVERDOC)lpImage;
//dbg("}SOpenD lpImage=%lx", lpImage);dbg(0);

#endif // #ifdef USEOLE
return( OLE_OK );
}


/*
 * ServerEditDoc
 *
 * Purpose:
 *  Creates a document initialized with data from a subsequent call
 *  to the SetData function.  The object is embedded in the client
 *  application and the server is not visible.
 *
 * Parameters:
 *  pSvr            LPIMAGESERVER to structure identifying the server.
 *  lhDoc           LHSERVERDOC identifying the document.
 *  pszClass        LPSTR specifying the class of document to create.
 *  pszDoc          LPSTR specifying the document for use in window titles.
 *  ppServerDoc     LPLPOLESERVERDOC in which to store a pointer
 *                  to the OLESERVERDOC structure for this new document.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
OLESTATUS FAR PASCAL EXPORT ServerEditDoc(LPIMAGESERVER pSvr, LHSERVERDOC lhDoc,
                                LPSTR pszClass, LPSTR pszDoc,
                                LPLPOLESERVERDOC ppServerDoc)
/************************************************************************/
{
#ifdef USEOLE
//dbg("{SEditD pSvr=%lx lhDoc=%lx, Class=%ls Doc=%ls", pSvr, lhDoc, pszClass, pszDoc);dbg(0);

/*
 * This is the same as Open, but tells the server to expect
 * a SetData call and that the server is initially hidden.
 *
 * 1.   Create a document of the specified class.
 * 2.   Allocate and initialize an OLESERVERDOC structure.
 * 3.   Store lhDoc in the OLESERVERDOC structure.
 * 4.   Store pointer to the new OLESERVERDOC structure in ppServerDoc.
 * 5.   Return OLE_OK if successful, OLE_ERROR_EDIT otherwise.
 */

pOLE->InMethod++;
New( 1 /*pixels*/, 1 /*lines*/, 1 /*Resolution*/, 3 /*Depth*/ );
pOLE->InMethod--;

if ( !lpImage || lpImage->lhDoc ) // If no new/unreg'd document was created...
	{
//	dbg("}SEditD err");dbg(0);
	return( OLE_ERROR_EDIT );
	}

lpImage->lhDoc = lhDoc;
lpImage->fOwnedByClient = YES;
*ppServerDoc = (LPOLESERVERDOC)lpImage;
//dbg("}SEditD lpImage=%lx", lpImage);dbg(0);

#endif // #ifdef USEOLE
return( OLE_OK );
}


/*
 * ServerExecute
 *
 * Purpose:
 *  Handles DDE Execute commands sent from the server library
 *  from the client application.  Not all applications need to support
 *  DDE Execute.
 *
 * Parameters:
 *  pSvr            LPIMAGESERVER identifying the server closing.
 *  hCommands       HANDLE to global memory containing the DDE execute
 *                  string.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 *                  This functions returns OLE_ERROR_COMMAND to indicate
 *                  that it is not implemented.
 */

/************************************************************************/
OLESTATUS FAR PASCAL EXPORT ServerExecute(LPIMAGESERVER pSvr, HANDLE hCommands)
/************************************************************************/
{
#ifdef USEOLE
//dbg("{SExec pSvr=%lx hCmd=%d", pSvr, hCommands);dbg(0);

/*
 * 1.   Lock the hCommands handle to access the execute strings.
 * 2.   Parse and execute the commands as necessary.
 * 3.   DO NOT FREE hCommands.
 * 4.   Return OLE_OK if successful, OLE_ERROR_COMMAND otherwise.
 */

//dbg("}SExec err");dbg(0);
#endif // #ifdef USEOLE
return( OLE_ERROR_COMMAND );
}


/*
 * ServerExit
 *
 * Purpose:
 *  Instructs the server application to close documents and shut down.
 *
 * Parameters:
 *  pSvr            LPIMAGESERVER identifying the server closing.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
OLESTATUS FAR PASCAL EXPORT ServerExit(LPIMAGESERVER pSvr)
/************************************************************************/
{
#ifdef USEOLE
//dbg("{SExit pSvr=%lx", pSvr);dbg(0);

/*
 * 1.   Hide the window to prevent any user interaction.
 * 2.   Call OleRevokeServer.  Ignore an OLE_WAIT_FOR_RELEASE return value.
 * 3.   Perform whatever action is necessary to cause the application
 *      to terminate, such as DestroyWindow.
 * 4.   Return OLE_OK if successful, OLE_ERROR_GENERIC otherwise.
 */

ShowWindow( hWndAstral, SW_HIDE );
pSvr->fRelease = FALSE;
OleRevokeServer( pSvr->lhSvr );
DestroyWindow( hWndAstral );
//dbg("}SExit OK");dbg(0);

#endif // #ifdef USEOLE
return( OLE_OK );
}


/*
 * ServerRelease
 *
 * Purpose:
 *  Notifies a server that all connections to it have been closed and
 *  that the server application can terminate.  This function should
 *  change the state of a global flag that causes any PeekMessage
 *  waiting loop to exit.
 *
 * Parameters:
 *  pSvr            LPIMAGESERVER identifying the server.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
OLESTATUS FAR PASCAL EXPORT ServerRelease(LPIMAGESERVER pSvr)
/************************************************************************/
{
#ifdef USEOLE
//dbg("{SRel pSvr=%lx", pSvr);dbg(0);

//// Close any images that have been released
//for ( i = 0; i < NumDocs(); i++ )
//	{
//	lpImageDoc = (LPIMAGE)GetWindowLong( GetDoc(i), 12 );
//	if ( !lpImageDoc->pObj )
//		{ // If we find an image with a released Document...
//		CloseImage( NO, lpImageDoc );
//		i = -1; // Start the loop over again
//		}
//	}

/*
 * 1.   Set a flag to indicate that Release has been called.
 * 2.   If the application is hidden, we must use ServerRelease to
 *      instruct the application to terminate, by posting a WM_CLOSE
 *      or otherwise effective message, then exit the method with OLE_OK.
 * 3.   Free any resources allocated for this server, like documents,
 *      but DO NOT free the OLESERVER structure itself.
 * 4.   Return OLE_OK if successful, OLE_ERROR_GENERIC otherwise.
 */

pSvr->fRelease = TRUE;

/*
 * If we are invisible when we get ServerRelease, that's a flag
 * to us meaning exit.  Posting a WM_CLOSE takes care of all
 * this.  Note that even though WM_CLOSE processing in IMAGE.C
 * checks for a dirty file and asks to save if necessary, the
 * file cannot be dirty because there has been no chance for
 * the user to make any changes.
 *
 * ServerRelease may be called twice when the server is opened invisible
 * for an update of a client object.  In this case, we'll get
 * ServerRelease once, where we should post a message to terminate the
 * application.  The second time around we need to handle free resources
 * associated with the server.  We detect this through the validity
 * pOLE->lhSvr which we set to 0L in FFileExit before calling
 * OleRevokeServer.  0L in lhSvr signals us that we're in the final
 * revoke and we can free documents.
 *
 */
if ( !IsWindowVisible( hWndAstral ) && pOLE->lhSvr )
	{
	PostMessage( hWndAstral, WM_CLOSE, 0, 0L );
//	dbg("}SRel OK after close");dbg(0);
	return( OLE_OK );
	}

//dbg("}SRel OK");dbg(0);

#endif // #ifdef USEOLE
return( OLE_OK );
}
