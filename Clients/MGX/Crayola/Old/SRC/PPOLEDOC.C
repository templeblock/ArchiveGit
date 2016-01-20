/*
 * PPOLEDOC.C
 *
 * Contains all callback functions in the OLESERVERDOCVTBL struture:
 *      DocClose
 *      DocGetObject
 *      DocExecute
 *      DocRelease
 *      DocSave
 *      DocSetColorScheme
 *      DocSetDocDimensions
 *      DocSetHostNames
 *
 * Copyright(c) Microsoft Corp. 1992 All Rights Reserved
 *
 */

#include <windows.h>
#include "id.h"
#include "data.h"
#include "ppole.h"
#include "routines.h"

extern HWND hWndAstral;

/*
 * DocClose
 *
 * Purpose:
 *  Instructs the server to unconditionally close the document.  OLESVR
 *  calls DocClose when the client initiates a shutdown.  OLESVR always
 *  calls this function before calling ServerRelease.
 *
 * Parameters:
 *  lpImageDoc      LPIMAGE identifying the document affected.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
OLESTATUS FAR PASCAL EXPORT DocClose(LPIMAGE lpImageDoc)
/************************************************************************/
{
#ifdef USEOLE
OLESTATUS       os;

//dbg("{DClose %lx", lpImageDoc);dbg(0);

if ( !ImageIsDoc( lpImageDoc ) )
	{
//	dbg("}DClose err; bad image=%lx", lpImageDoc );dbg(0);
	return( OLE_ERROR_GENERIC );
	}

/*
 * Take no action to notify user--Client will take care of that.
 *
 * 1.   Call OleRevokeServerDoc; resources are freed when OLESVR
 *      calls DocRelease.
 * 2.   Return the return value of OleRevokeServerDoc, which will
 *      generally be OLE_OK.
 */

os = OleRevokeServerDoc( lpImageDoc->lhDoc );
//dbg("}DClose RevDoc(lhDoc=%lx)-os=%d", os, lpImageDoc->lhDoc );dbg(0);
return( os );

#else
return OLE_OK;
#endif // #ifdef USEOLE
}


/*
 * DocRelease
 *
 * Purpose:
 *  Notifies the server when a revoked document may be destroyed.
 *
 * Parameters:
 *  lpImageDoc      LPIMAGE identifying the document affected.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
OLESTATUS FAR PASCAL EXPORT DocRelease(LPIMAGE lpImageDoc)
/************************************************************************/
{
#ifdef USEOLE
//dbg("{DRel lpImageDoc=%lx", lpImageDoc);dbg(0);

if ( !ImageIsDoc( lpImageDoc ) )
	{
//	dbg("}DRel err: bad image=%lx", lpImageDoc );dbg(0);
	return( OLE_ERROR_GENERIC );
	}

/*
 * 1.   Free any resources allocated for this document, but
 *      DO NOT free the OLESERVERDOC structure itself.  This could
 *      include freeing objects, however.
 * 2.   Set a flag to indicate that Release has been called.
 * 3.   Return OLE_OK if successful, OLE_ERROR_GENERIC otherwise.
 */
// Release any object stored in this document.
if ( lpImageDoc->pObj )
	FreeUp( (LPTR)lpImageDoc->pObj );
lpImageDoc->pObj = NULL;
lpImageDoc->fRelease = TRUE;

//dbg("}DRel OK");dbg(0);

#endif // #ifdef USEOLE
return( OLE_OK );
}


/*
 * DocGetObject
 *
 * Purpose:
 *  Requests the server application to create an OLEOBJECT structure.
 *
 * Parameters:
 *  lpImageDoc      LPIMAGE identifying the document affected.
 *  pszItem         LPSTR specifying the name of the item in a document
 *                  for which the object is to be created.  If NULL, then
 *                  the entire document is requested.  This could be a
 *                  range of spreadsheet cells like "R1:C3-R10:C50"
 *  ppObj           LPLPOLEOBJECT at which to store a pointer to the
 *                  OLEOBJECT structure.
 *  pClient         LPOLECLIENT that should be associated with
 *                  the object in order to notify OLECLI when the object
 *                  changes.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
OLESTATUS FAR PASCAL EXPORT DocGetObject(LPIMAGE lpImageDoc, LPSTR lpItem,
		LPLPOLEOBJECT ppObj, LPOLECLIENT pClient)
/************************************************************************/
{
#ifdef USEOLE
LPIMAGEOBJECT pObj;
POINT Points[4];
int nPoints;
STRING szItem;
LPIMAGE lpOldImage;

//dbg("{DGetObj lpImage=%lx Item='%ls'", lpImageDoc, lpItem );dbg(0);

if ( !ImageIsDoc( lpImageDoc ) )
	{
//	dbg("}DGetObj err; bad image=%lx", lpImageDoc );dbg(0);
	return( OLE_ERROR_FORMAT );
	}

/*
 * 1.   Allocate and initialize an OLEOBJECT structure.
 *	lpItem == NULL means the entire document
 * 2.   Store pClient in the object's OLEOBJECT structure for use
 *      in sending notifications to the client.
 * 3.   Store a pointer to the new OLEOBJECT structure in ppObj.
 * 4.   Return OLE_OK if successful, OLE_ERROR_NAME if pszObj is
 *      not recognized, or OLE_ERROR_MEMORY if the object could not
 *      be allocated.
 *
 * This function is called in response to a client calling
 * OleGetObject.
 */

// Allocate one from local FIXED memory.
if ( !(pObj = PObjectAllocate(&pOLE->vtblObj)) )
	{
//	dbg("}DGetObj mem err" );dbg(0);
	return( OLE_ERROR_MEMORY );
	}

// Remember who we created for freeing in DocRelease.
lpImageDoc->pObj = (LPTR)pObj;
if ( lpImageDoc->fOwnedByClient )
	lpItem = NULL; // Embedded data shouldn't have an item string (a mask)
if ( lpItem ) // Save the item string
	lstrcpy( pObj->szItem, lpItem );
else
	lstrcpy( pObj->szItem, "." );

lpOldImage = ImageSet( lpImageDoc );
RemoveMask();
if ( lpItem && *lpItem )
	{
	lstrcpy( szItem, lpItem );
	if ( sscanf( szItem, "(%d,%d)-(%d,%d)", &Points[0].x, &Points[0].y,
		&Points[2].x, &Points[2].y ) > 0 )
		{
		nPoints = 4;
		Points[1].x = Points[2].x;
		Points[1].y = Points[0].y;
		Points[3].x = Points[0].x;
		Points[3].y = Points[2].y;
		SetNewMask( Points, &nPoints, 1, -1, SHAPE_ADD );
		}
	}
ImageSet( lpOldImage );

// Must save this for sending notifications.
pObj->pClient = pClient;
pObj->lpImageDoc = lpImageDoc;

// Pass back the pointer to this object.
*ppObj = (LPOLEOBJECT)pObj;
//dbg("}DGetObj OK pObj=%lx", pObj);dbg(0);

#endif // #ifdef USEOLE
return( OLE_OK );
}


/*
 * DocExecute
 *
 * Purpose:
 *  Passes DDE Execute strings related to the document to the server
 *  application.
 *
 * Parameters:
 *  lpImageDoc      LPIMAGE identifying the document affected.
 *  hCommands       HANDLE to global memory containing DDE execute strings.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
OLESTATUS FAR PASCAL EXPORT DocExecute(LPIMAGE lpImageDoc, HANDLE hCommands)
/************************************************************************/
{
#ifdef USEOLE
//dbg("{DExec lpImage=%lx hCommands=%d", lpImageDoc, hCommands);dbg(0);

if ( !ImageIsDoc( lpImageDoc ) )
	{
//	dbg("}DExec err; bad image=%lx", lpImageDoc );dbg(0);
	return( OLE_ERROR_GENERIC );
	}

/*
 * 1.   Lock the hCommands handle to access the execute strings.
 * 2.   Parse and execute the commands as necessary.
 * 3.   DO NOT FREE hCommands.
 * 4.   Return OLE_OK if successful, OLE_ERROR_COMMAND otherwise.
 */

//dbg("}DExec err" );dbg(0);

#endif // #ifdef USEOLE
return( OLE_ERROR_COMMAND );
}


/*
 * DocSave
 *
 * Purpose:
 *  Instructs the server application to save the document.  If DocSave is
 *  called you are assumed to have a know filename to which you can save
 *  since this method is only used in linking.
 *
 * Parameters:
 *  lpImageDoc      LPIMAGE identifying the document affected.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
OLESTATUS FAR PASCAL EXPORT DocSave(LPIMAGE lpImageDoc)
/************************************************************************/
{
#ifdef USEOLE
//dbg("{DSave lpImage=%lx", lpImageDoc);dbg(0);

if ( !ImageIsDoc( lpImageDoc ) )
	{
//	dbg("}DSave err; bad image=%lx", lpImageDoc );dbg(0);
	return( OLE_ERROR_GENERIC );
	}

/*
 * 1.   Save the document to the known filename.  How you save
 *      documents is application-specific.
 * 2.   Return OLE_OK if the save is successful, OLE_ERROR_GENERIC
 *      otherwise.
 */

ActivateDocument( lpImageDoc->hWnd );
SendMessage( hWndAstral, WM_COMMAND, IDM_SAVE, 0L );
//dbg("}DSave OK" );dbg(0);

#endif // #ifdef USEOLE
return( OLE_OK );
}


/*
 * DocSetColorScheme
 *
 * Purpose:
 *  Provides a color scheme that the client application recommends for
 *  rendering graphical data.
 *
 * Parameters:
 *  lpImageDoc      LPIMAGE identifying the document affected.
 *  pPal            LPLOGPALETTE describing the recommended palette.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
OLESTATUS FAR PASCAL EXPORT DocSetColorScheme(LPIMAGE lpImageDoc, LPLOGPALETTE pPal)
/************************************************************************/
{
#ifdef USEOLE
//dbg("{DColor" );dbg(0);

if ( !ImageIsDoc( lpImageDoc ) )
	{
	return( OLE_ERROR_GENERIC );
	}

/*
 * Servers are not required to use this palette.  The LPLOGPALETTE
 * only is a convenient structure to contain the color scheme; IT DOES
 * not REPRESENT A PALETTE IN STRICT TERMS!  Do NOT call CreatePalette
 * and try to realize it.
 *
 * The color scheme contained in the LOGPALETTE structure contains
 * a number of colors where the first color is the suggested foreground,
 * the second the suggested background, then the first HALF of those
 * remaining are suggested fill colors and the last half suggested
 * line colors.  If there are an odd number of colors, give the extra
 * color to the fill colors, that is, there is one less line color than
 * fill colors.
 */

//dbg("}DColor err" );dbg(0);

#endif // #ifdef USEOLE
return( OLE_ERROR_PALETTE );
}


/*
 * DocSetDocDimensions
 *
 * Purpose:
 *  Specifies the rectangle on the target device for which EMBEDDED
 *  objects should be formatted.
 *
 * Parameters:
 *  lpImageDoc      LPIMAGE identifying the document affected.
 *  pRect           LPRECT to the device rectangle in MM_HIMETRIC units.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
OLESTATUS FAR PASCAL EXPORT DocSetDocDimensions(LPIMAGE lpImageDoc, LPRECT pRect)
/************************************************************************/
{
#ifdef USEOLE
//dbg("{DDim lpImage=%lx, (%d,%d)-(%d,%d)", lpImageDoc,
//	pRect->left, pRect->top, pRect->right, pRect->bottom );dbg(0);

if ( !ImageIsDoc( lpImageDoc ) )
	{
//	dbg("}DDim err; bad image=%lx", lpImageDoc );dbg(0);
	return( OLE_ERROR_GENERIC );
	}

/*
 * OLESVR will call this method when the client has resized the
 * object.
 *
 * In this case we try to make the parent window the correct size
 * to just contain the object.
 */

//dbg("}DDim OK" );dbg(0);

#endif // #ifdef USEOLE
return( OLE_OK );
}


/*
 * DocSetHostNames
 *
 * Purpose:
 *  Set names that should be used for window titles, only for
 *  embedded objects; linked objects have their own titles as they
 *  are loaded through the server application's usual file loading
 *  mechanism.
 *
 * Parameters:
 *  lpImageDoc      LPIMAGE identifying the document affected.
 *  pszClient       LPSTR to the name of the client application.
 *  pszObj          LPSTR to the name of the embedded object.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
OLESTATUS FAR PASCAL EXPORT DocSetHostNames(LPIMAGE lpImageDoc, LPSTR pszClient,
                                     LPSTR pszObj)
/************************************************************************/
{
#ifdef USEOLE
//dbg("{DNames lpImage=%lx Client=%ls Object=%ls", lpImageDoc, pszClient, pszObj );dbg(0);

if ( !ImageIsDoc( lpImageDoc ) )
	{
//	dbg("}DNames err; bad image=%lx", lpImageDoc );dbg(0);
	return( OLE_ERROR_GENERIC );
	}

/*
 * 1.   Change the title bar to reflect the embedded state with the
 *      appropriate names.
 * 2.   Change the File menu to reflect the embedded state and the name
 *      of the client application.
 * 3.   Store the object and client names in the OLESERVERDOC structure.
 *      These will be needed later for message boxes where the name of
 *      the client application must be displayed.
 * 4.   Return OLE_OK if successful, OLE_ERROR_GENERIC otherwise.
 */

// Set menus and title

lstrcpy( lpImageDoc->CurFile, pszClient );
lstrcat( lpImageDoc->CurFile, "-" );
lstrcat( lpImageDoc->CurFile, pszObj );
SetTitleBar( lpImageDoc->hWnd );
PPOLEClientNotify( (LPIMAGEOBJECT)lpImageDoc->pObj, OLE_CHANGED );
OleSavedServerDoc( lpImageDoc->lhDoc );
//dbg("}DNames OK" );dbg(0);

#endif // #ifdef USEOLE
return( OLE_OK );
}
