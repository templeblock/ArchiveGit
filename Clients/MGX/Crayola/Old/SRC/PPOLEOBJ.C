//®FD1¯®PL1¯®TP0¯®BT0¯
/*
 * PPOLEOBJ.C
 *
 * Contains all callback functions in the OLEOBJECTVTBL struture:
 *      ObjDoVerb
 *      ObjEnumFormats
 *      ObjGetData
 *      ObjQueryProtocol
 *      ObjRelease
 *      ObjSetBounds
 *      ObjSetColorScheme
 *      ObjSetData
 *      ObjSetTargetDevice
 *      ObjShow
 *
 * Copyright(c) Microsoft Corp. 1992 All Rights Reserved
 *
 */

#include <windows.h>
#include "routines.h" // only for dbg() prototype
#include "ppole.h"
#include "data.h" // only for lpImage

extern HWND hWndAstral, hClientAstral;

/*
 * PObjectAllocate
 *
 * Purpose:
 *  Allocates a IMAGEOBJECT structure and sets the defaults in its fields.
 *  Used from DocGetObject.
 *
 * Parameters:
 *  pVtblObj        LPOLESERVERDOCVTBL used to initialize the pvtbl field.
 *
 * Return Value:
 *  LPIMAGEOBJECT  Pointer to the allocated structure in local memory.
 */

/************************************************************************/
LPIMAGEOBJECT FAR PASCAL EXPORT PObjectAllocate(LPOLEOBJECTVTBL pVtblObj)
/************************************************************************/
{
#ifdef USEOLE
LPIMAGEOBJECT  pObj;

if ( !(pObj = (LPIMAGEOBJECT)AllocX(sizeof(IMAGEOBJECT), GMEM_SHARE)) )
	return( FALSE );

//Initialize the object.
clr( (LPTR)pObj, sizeof(IMAGEOBJECT) );
pObj->pvtbl = pVtblObj;
return( pObj );

#else
return NULL;
#endif // #ifdef USEOLE
}


/*
 * ObjDoVerb
 *
 * Purpose:
 *  Performs actions on an object when the user opens an object
 *  according to the verb given.
 *
 * Parameters:
 *  pObj            LPIMAGEOBJECT specifying the object affected.
 *  iVerb           WORD index to the verb chosen, zero based.
 *  fShow           BOOL--TRUE if the application should show
 *                  itself with ShowWindow.  FALSE means no change.
 *  fFocus          BOOL--TRUE if the server should get the focus.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
OLESTATUS FAR PASCAL EXPORT ObjDoVerb(LPIMAGEOBJECT pObj, WORD iVerb,
                               BOOL fShow, BOOL fFocus)
/************************************************************************/
{
#ifdef USEOLE
//dbg("{OVerb pObj=%lx iVerb=%d fShow=%d, fFocus=%d", pObj, iVerb, fShow, fFocus);dbg(0);

/*
 * 1.   Execute the verb.
 *        a.  For a 'Play' verb, a server does not generally show
 *            its window or affect the focus.
 *
 *        b.  For an 'Edit' verb, show the server's window and the
 *            object if fShow is TRUE, and take the focus if fFocus
 *            is TRUE.  An ideal way to accomplish this is to call
 *            the ObjShow method through the OLEOBJECTVTBL since that
 *            method will handle showing the object and taking the
 *            focus itself.
 *
 *        c.  An 'Open' verb is not clearly defined; depending on the
 *            application it may mean the same as 'Play' or 'Edit.'
 *            The Image server, if it had an 'Open' verb, would treat
 *            it like 'Edit.'
 *
 * 2.  Return OLE_OK if the verb was successfully executed, otherwise
 *     OLE_ERROR_DOVERB.
 */

switch (iVerb)
    {
    case OBJVERB_EDIT:
        /*
         * On edit, simply show yourself and expect a SetData.
         * Best strategy is to use the Show method for this
         * object if necessary, reducing redundancy.
         */
        if ( fShow )
		{
		(pObj->pvtbl->Show)((LPOLEOBJECT)pObj, fShow );
//		dbg("}OVerb OK after show" );dbg(0);
		return( OLE_OK );
		}

        //Return OLE_OK
        break;


    case OBJVERB_PLAY:
        //Unsupported at this time.
//	dbg("}OVerb err; play" );dbg(0);
        return( OLE_ERROR_DOVERB );

    default:
//	dbg("}OVerb err; default" );dbg(0);
        return( OLE_ERROR_DOVERB );
    }

//dbg("}OVerb OK unknown" );dbg(0);

#endif // #ifdef USEOLE
return( OLE_OK );
}


/*
 * ObjEnumFormats
 *
 * Purpose:
 *  Requests the server to produce the 'next' supported clipboard
 *  format.  Each format is returned in sequence until the terminator
 *  (a NULL) is returned.
 *
 * Parameters:
 *  pObj            LPIMAGEOBJECT specifying the object affected.
 *  cf              WORD the last clipboard format returned by this
 *                  function.  0 indicates the first.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
OLECLIPFORMAT FAR PASCAL EXPORT ObjEnumFormats(LPIMAGEOBJECT pObj, WORD cf)
/************************************************************************/
{
#ifdef USEOLE
//dbg("{OEnum pObj=%lx wCF=%d", pObj, cf);dbg(0);

/*
 * 1.   Depending on cf, return the 'next' clipboard format in which
 *      the server can render the object's data.
 * 2.   If there are no more supported formats after the format in cf,
 *      return NULL.
 *
 * We must use multiple if's instead of a switch statement because
 * all the cf* values are not constants.
 */

if ( !cf )
	{
//	dbg("}OEnum Native %d", pOLE->cfNative );dbg(0);
	return( pOLE->cfNative );
	}
if ( cf == pOLE->cfNative )
	{
//	dbg("}OEnum Owner %d", pOLE->cfOwnerLink );dbg(0);
	return( pOLE->cfOwnerLink );
	}
if ( cf == pOLE->cfOwnerLink )
	{
//	dbg("}OEnum DIB", CF_DIB );dbg(0);
	return( CF_DIB );
	}
if ( cf == CF_DIB )
	{
//	dbg("}OEnum BITMAP", CF_BITMAP );dbg(0);
	return( CF_BITMAP );
	}
if ( cf == CF_BITMAP )
	{
//	dbg("}OEnum METAFILE", CF_METAFILEPICT );dbg(0);
	return( CF_METAFILEPICT );
	}
if ( cf == CF_METAFILEPICT )
	{
//	dbg("}OEnum ObjLink %d", pOLE->cfObjectLink );dbg(0);
	return( pOLE->cfObjectLink );
	}
if ( cf == pOLE->cfObjectLink )
	{
//	dbg("}OEnum NULL");dbg(0);
	return( NULL );
	}

//dbg("}OEnum (huh?)");dbg(0);

#endif // #ifdef USEOLE
return( NULL );
}


/*
 * ObjGetData
 *
 * Purpose:
 *  Retrieves data from the object in a specified format, where the
 *  server should allocate memory (GlobalAlloc with GMEM_DDESHARE),
 *  fill the memory, and return the handle.
 *
 * Parameters:
 *  pObj            LPIMAGEOBJECT specifying the object affected.
 *  cf              WORD format to return data in
 *  phData          LPHANDLE in which to store the allocated handle.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
OLESTATUS FAR PASCAL EXPORT ObjGetData(LPIMAGEOBJECT pObj, WORD cf, LPHANDLE phData)
/************************************************************************/
{
#ifdef USEOLE
HGLOBAL hMem;
POINT Points[4];
int nPoints;
STRING szItem;
LPIMAGE lpOldImage;
LPTR lpDIB;
DWORD dwValue;
RECT rect;
LPRECT lpRect;
LPFRAME lpFrame;
BOOL bHasMask;

//dbg("{OGet pObj=%lx wCF=%d", pObj, cf);dbg(0);

if ( !ImageIsDoc( pObj->lpImageDoc ) )
	{
//	dbg("}OGet err; bad image=%lx", pObj->lpImageDoc );dbg(0);
	return( OLE_ERROR_FORMAT );
	}

lpOldImage = ImageSet( pObj->lpImageDoc );
RemoveMask();
lstrcpy( szItem, pObj->szItem );
if ( *szItem )
	{
	if ( sscanf( szItem, "(%d,%d)-(%d,%d)", &Points[0].x, &Points[0].y,
		&Points[2].x, &Points[2].y ) == 4 )
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

/*
 * 1.   Allocate the requested data throguh GlobalAlloc (with
 *      GMEM_MOVEABLE and GMEM_DDESHARE).  The exception is data for
 *      CF_BITMAP which uses a call like CreateBitmap.
 * 2.   Lock and fill the memory with the appropriate data.
 * 3.   Unlock the memory and store the handle in *phData.
 * 4.   Return OLE_OK if successful, OLE_ERROR_MEMORY otherwise.
 */

// Set the rectangle to the mask we created above from the client rectangle...
bHasMask = ImgGetMaskRect( lpImage, &rect );

if ( !ImgMultipleObjects( lpImage ) )
	{ // If no objects, use a rectangular area of the base
	lpFrame = ImgGetBaseEditFrame(lpImage);
	lpRect = &rect;
	}
else
	{ // If objects, Combine and copy the object data into a new frame
	if ( !(lpFrame = ImgCopyFrame( lpImage, NULL/*lpBaseObject*/, &rect,
		YES/*bCrop*/, NO/*fSelected*/ )) )
			return( OLE_ERROR_MEMORY );
	lpRect = NULL;
	}

// Return a data handle as requested.
if ( cf == CF_DIB || cf == pOLE->cfNative )
	{
	lpDIB = FrameToDIB( lpFrame, lpRect );
	
#ifdef WIN32
	hMem = GlobalHandle( lpDIB );
#else
	dwValue = GlobalHandle( HIWORD(lpDIB) );
	hMem = (HGLOBAL)LOWORD( dwValue );
#endif	
	GlobalUnlock( hMem );
	if ( !lpRect ) FrameClose( lpFrame );
	}
else
if ( cf == CF_BITMAP )
	{
	hMem = FrameToBitmap( lpFrame, lpRect );
	if ( !lpRect ) FrameClose( lpFrame );
	}
else
if ( cf == CF_METAFILEPICT )
	{
	hMem = FrameToMetafile( lpFrame, lpRect );
	if ( !lpRect ) FrameClose( lpFrame );
	}
else
if ( cf == pOLE->cfObjectLink )
	hMem = ConstructObject( lpImage, (bHasMask? &rect : NULL) );
else
if ( cf == pOLE->cfOwnerLink )
	hMem = ConstructObject( lpImage, (bHasMask? &rect : NULL) );
else
	{
//	dbg("}OGet err cf=%d", cf );dbg(0);
	ImageSet( lpOldImage );
	return( OLE_ERROR_FORMAT );
	}
ImageSet( lpOldImage );

if ( !hMem )
	{
//	dbg("}OGet err; NULL hMem" );dbg(0);
	return( OLE_ERROR_MEMORY );
	}

*phData = hMem;
//dbg("}OGet OK hMem=%d", hMem );dbg(0);

#endif // #ifdef USEOLE
return( OLE_OK );
}


/*
 * ObjQueryProtocol
 *
 * Purpose:
 *  Returns a pointer to an IMAGEOBJECT with the appropriate VTBL for
 *  the protocol, either StdFileEditing or StdExecute.  Returns NULL
 *  if that protocol is not supported.
 *
 * Parameters:
 *  pObj            LPIMAGEOBJECT specifying the object affected.
 *  pszProtocol     LPSTR, the protocol name.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
LPVOID FAR PASCAL EXPORT ObjQueryProtocol(LPIMAGEOBJECT pObj, LPSTR pszProtocol)
/************************************************************************/
{
#ifdef USEOLE
//dbg("{Query pObj=%lx, Prot=%ls", pObj, pszProtocol);dbg(0);

/*
 * 1.   If the protocol in pszProtocol is supported, return a pointer
 *      to an object that contains an appropriate VTBL for that protocol,
 *      such as the pObj passed to this method.
 * 2.   If the protocol is not supported, return NULL.
 */

//Check if OLESVR is asking for "StdFileEditing"
if ( !lstrcmp( pszProtocol, "StdFileEditing" ) )
	{
//	dbg("}Query pObj=%lx", pObj );dbg(0);
	return( (LPVOID)pObj );
	}

//dbg("}Query NULL not std" );dbg(0);

#endif // #ifdef USEOLE
return( NULL );
}


/*
 * ObjRelease
 *
 * Purpose:
 *  Notifies a server that is can free any resources associated with an
 *  object.
 *
 * Parameters:
 *  pObj            LPIMAGEOBJECT specifying the object affected.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
OLESTATUS FAR PASCAL EXPORT ObjRelease(LPIMAGEOBJECT pObj)
/************************************************************************/
{
#ifdef USEOLE
//dbg("{ORel pObj=%lx", pObj);dbg(0);

/*
 * 1.   Free any resources allocated for this object, but
 *      DO NOT free the OLEOBJECT structure itself.
 * 2.   Set a flag to indicate that Release has been called.
 * 3.   NULL any saved LPOLECLIENT stores in the OLEOBJECT structure.
 * 4.   Return OLE_OK if successful, OLE_ERROR_GENERIC otherwise.
 *
 * Do not use the Release method to free the memory containing
 * the object since you still need to use its pClient and fRelease.
 * This method simply notifies the object that no one is using it
 * anymore so we don't try to send notifications.
 */

pObj->pClient = NULL;
//dbg("}ORel OK rel=TRUE");dbg(0);

#endif // #ifdef USEOLE
return( OLE_OK );
}


/*
 * ObjSetBounds
 *
 * Purpose:
 *  Specifies a new boundary rectangle for the object in MM_HIMETRIC
 *  units.  Only useful for embedded objects since a linked object
 *  depends on the file loaded.
 *
 * Parameters:
 *  pObj            LPIMAGEOBJECT specifying the object affected.
 *  pRect           LPRECT containing the new bounds.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
OLESTATUS FAR PASCAL EXPORT ObjSetBounds(LPIMAGEOBJECT pObj, LPRECT pRect)
/************************************************************************/
{
#ifdef USEOLE
//dbg("{Bounds pObj=%lx (%d,%d)-(%d,%d)", pObj, pRect->left, pRect->top, pRect->right, pRect->bottom );dbg(0);

//Unused in OLE1.x
//dbg("}Bounds OK" );dbg(0);

#endif // #ifdef USEOLE
return( OLE_OK );
}


/*
 * ObjSetColorScheme
 *
 * Purpose:
 *  Provides a color scheme that the client application recommends for
 *  rendering graphical data.
 *
 * Parameters:
 *  pObj            LPIMAGEOBJECT specifying the object affected.
 *  pPal            LPLOGPALETTE describing the recommended palette.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
OLESTATUS FAR PASCAL EXPORT ObjSetColorScheme(LPIMAGEOBJECT pObj, LPLOGPALETTE pPal)
/************************************************************************/
{
#ifdef USEOLE
//dbg("{OColor pObj=%lx pPal=%lx", pObj, pPal);dbg(0);

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

//dbg("}Color err");dbg(0);

#endif // #ifdef USEOLE
return( OLE_ERROR_PALETTE );
}


/*
 * ObjSetData
 *
 * Purpose:
 *  Instructs the object to take the given data as current and copy it
 *  to the object's internal data, use when a client opens an embedded
 *  object or if the client calls OleSetData.
 *
 *  *NOTE: This function MUST be supported even if the registration
 *         database does not contain an entry for SetDataFormats for
 *         this server, because the callback is still used when opening
 *         an embedded object for editing.
 *
 * Parameters:
 *  pObj            LPIMAGEOBJECT specifying the object affected.
 *  cf              WORD format to return data in
 *  hData           HANDLE to global memory containing the data.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
OLESTATUS FAR PASCAL EXPORT ObjSetData(LPIMAGEOBJECT pObj, WORD cf, HANDLE hData)
/************************************************************************/
{
#ifdef USEOLE
LPFRAME lpFrame, lpOldFrame;

//dbg("{OSet pObj=%lx wCF=%d h=%d", pObj, cf, hData);dbg(0);

/*
 * 1.   If the data format is not supported, return OLE_ERROR_FORMAT.
 * 2.   Attempt to GlobalLock the memory to get a pointer to the data.
 *      If GlobalLock returns NULL, return OLE_ERROR_MEMORY.
 * 3.   Copy the data to the object identified by pObj.
 * 4.   Unlock and GlobalFree the data handle.  The ObjSetData method
 *      is responsible for the memory.
 * 5.   Return OLE_OK.
 */

// Check if we were given acceptable data.
if ( cf == CF_DIB || cf == pOLE->cfNative )
	{
	LPTR lpData;

	lpData  = (LPTR)GlobalLock( hData );
	lpFrame = DIBToFrame( lpData, TRUE );
	GlobalUnlock( hData );
	}
else
if ( cf == CF_BITMAP )
	lpFrame = BitmapToFrame( NULL/*hDC*/, (HBITMAP)hData, NULL/*hPal*/ );
else
	{
//	dbg("}OSet err cf=%d", cf );dbg(0);
	return( OLE_ERROR_FORMAT );
	}

if ( !lpFrame )
	{
//	dbg("}OSet mem err" );dbg(0);
	return( OLE_ERROR_MEMORY );
	}

pOLE->InMethod = YES;

/* Replace the base frame of the active image and redraw image window */
lpOldFrame = ImgGetBaseEditFrame( lpImage );
ImgGetBase(lpImage)->Pixmap.EditFrame = lpFrame;
SetRect( &ImgGetBase(lpImage)->rObject, 0, 0, FrameXSize(lpFrame)-1, 
	FrameYSize(lpFrame)-1 );
if ( lpOldFrame )
	FrameClose( lpOldFrame );
UpdateImageSize();

pOLE->InMethod = NO;

// Server is responsible for freeing the data.
GlobalUnlock( hData );
GlobalFree( hData );

//dbg("}OSet OK" );dbg(0);

#endif // #ifdef USEOLE
return( OLE_OK );
}


/*
 * ObjSetTargetDevice
 *
 * Purpose:
 *  Communicates information from the client application to the server
 *  about the device on which the object is drawn.
 *
 * Parameters:
 *  pObj            LPIMAGEOBJECT specifying the object affected.
 *  hData           HANDLE containing a STDTARGETDEVICE structure.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
OLESTATUS FAR PASCAL EXPORT ObjSetTargetDevice(LPIMAGEOBJECT pObj, HANDLE hData)
/************************************************************************/
{
#ifdef USEOLE
//dbg("{Target pObj=%lx h=%d", pObj, hData);dbg(0);

/*
 * The server is responsible for freeing the the data handle
 * once it has finished using that data.
 *
 * If !hData, then rendering is necessary for the screen.
 */

if ( hData )
	GlobalFree( hData );

//dbg("}Target OK" );dbg(0);

#endif // #ifdef USEOLE
return( OLE_OK );
}


/*
 * ObjShow
 *
 * Purpose:
 *  Causes the server to show an object, showing the window and
 *  scrolling it's client area if necessary.
 *
 * Parameters:
 *  pObj            LPIMAGEOBJECT specifying the object affected.
 *  fTakeFocus      BOOL:  TRUE if the server should get the focus,
 *                  FALSE if not.
 *
 * Return Value:
 *  OLESTATUS       OLE_OK if all is well, otherwise an OLE_* error value.
 */

/************************************************************************/
OLESTATUS FAR PASCAL EXPORT ObjShow(LPIMAGEOBJECT pObj, BOOL fTakeFocus)
/************************************************************************/
{
#ifdef USEOLE
//dbg("{OShow pObj=%lx fFocus=%d", pObj, fTakeFocus);dbg(0);

/*
 * 1.   Show the application window(s) if not already visible.
 * 2.   Scroll the object identified by pObj into view, if necessary.
 * 3.   Select the object if possible.
 * 4.   If fTakeFocus is TRUE, call SetFocus with the main window handle.
 * 5.   Return OLE_OK if successful, OLE_ERROR_GENERIC otherwise.
 */

//Since we only have one object, we don't care what's in pObj.
ShowWindow( hWndAstral, SW_NORMAL );
if ( fTakeFocus )
	SetFocus( hWndAstral );
SendMessage( hClientAstral, WM_MDIACTIVATE, (WORD)pObj->lpImageDoc->hWnd, 0L );

//PPOLEClientNotify( pObj, OLE_CHANGED );
//dbg("}OShow OK" );dbg(0);

#endif // #ifdef USEOLE
return( OLE_OK );
}
