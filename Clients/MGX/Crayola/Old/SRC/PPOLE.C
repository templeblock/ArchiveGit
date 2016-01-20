//®FD1¯®PL1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#ifdef _MAC
#define _WIN32REG		// includes winreg.h while in windows.h
#endif

#define OLEMAIN
#include <windows.h>
#include <shellapi.h>
#include "data.h"
#include "ppole.h"
#include "id.h"
#include "routines.h" // only for dbg() prototype

// Static prototypes
static void FAR PASCAL PPOLERegisterApplication( HINSTANCE hInstance, LPSTR lpDocClass);
static BOOL RegKeyValue( BOOL bObjectClass, LPSTR lpString, LPSTR lpValue );
static BOOL RegKeyExists( BOOL bObjectClass, LPSTR lpString );

typedef OLESTATUS (CALLBACK* OLE1_OPENPROC)(LPOLESERVER, LHSERVERDOC, OLE_LPCSTR, LPOLESERVERDOC FAR*);
typedef OLESTATUS (CALLBACK* OLE1_CREATEPROC)(LPOLESERVER, LHSERVERDOC, OLE_LPCSTR, OLE_LPCSTR, LPOLESERVERDOC FAR*);
typedef OLESTATUS (CALLBACK* OLE1_CREATEFROMTEMPLATEPROC)(LPOLESERVER, LHSERVERDOC, OLE_LPCSTR, OLE_LPCSTR, OLE_LPCSTR, LPOLESERVERDOC FAR*);
typedef OLESTATUS (CALLBACK* OLE1_EDITPROC)(LPOLESERVER, LHSERVERDOC, OLE_LPCSTR, OLE_LPCSTR, LPOLESERVERDOC FAR*);
typedef OLESTATUS (CALLBACK* OLE1_EXITPROC)(LPOLESERVER);
typedef OLESTATUS (CALLBACK* OLE1_RELEASEPROC)(LPOLESERVER);
typedef OLESTATUS (CALLBACK* OLE1_EXECUTEPROC)(LPOLESERVER, HGLOBAL);

typedef OLESTATUS (CALLBACK* OLE1_SAVE)(LPOLESERVERDOC);
typedef OLESTATUS (CALLBACK* OLE1_CLOSE)(LPOLESERVERDOC);
typedef OLESTATUS (CALLBACK* OLE1_SETHOSTNAMES)(LPOLESERVERDOC, OLE_LPCSTR, OLE_LPCSTR);
typedef OLESTATUS (CALLBACK* OLE1_SETDOCDIMENSIONS)(LPOLESERVERDOC, OLE_CONST RECT FAR*);
typedef OLESTATUS (CALLBACK* OLE1_GETOBJECT)(LPOLESERVERDOC, OLE_LPCSTR, LPOLEOBJECT FAR*, LPOLECLIENT);
typedef OLESTATUS (CALLBACK* OLE1_RELEASE)(LPOLESERVERDOC);
typedef OLESTATUS (CALLBACK* OLE1_SETCOLORSCHEME)(LPOLESERVERDOC, OLE_CONST LOGPALETTE FAR*);
typedef OLESTATUS (CALLBACK* OLE1_EXECUTE)(LPOLESERVERDOC, HGLOBAL);

typedef void FAR* (CALLBACK* OLE1_QUERYPROTOCOL)(LPOLEOBJECT, OLE_LPCSTR);
typedef OLESTATUS (CALLBACK* OLE1_RELEASEOBJ)(LPOLEOBJECT);
typedef OLESTATUS (CALLBACK* OLE1_SHOW)(LPOLEOBJECT, BOOL);
typedef OLESTATUS (CALLBACK* OLE1_DOVERB)(LPOLEOBJECT, UINT, BOOL, BOOL);
typedef OLESTATUS (CALLBACK* OLE1_GETDATA)(LPOLEOBJECT, OLECLIPFORMAT, HANDLE FAR*);
typedef OLESTATUS (CALLBACK* OLE1_SETDATA)(LPOLEOBJECT, OLECLIPFORMAT, HANDLE);
typedef OLESTATUS (CALLBACK* OLE1_SETTARGETDEVICE)(LPOLEOBJECT, HGLOBAL);
typedef OLESTATUS (CALLBACK* OLE1_SETBOUNDS)(LPOLEOBJECT, OLE_CONST RECT FAR*);
typedef OLECLIPFORMAT (CALLBACK* OLE1_ENUMFORMATS)(LPOLEOBJECT, OLECLIPFORMAT);
typedef OLESTATUS (CALLBACK* OLE1_SETOBJCOLORSCHEME)(LPOLEOBJECT, OLE_CONST LOGPALETTE FAR*);

/*
 * PPOLEInstanceInit
 *
 * Purpose:
 *  Handles OLE-server specific initialziation:
 *      1.  Register clipboard formats for "Native", "OwnerLink",
 *          and "ObjectLink."
 *      2.  Initialize VTBLs for server, document, and object.
 *      3.  Register the server with OLESVR.
 *      4.  Parse the command line to determine initial window state.
 *
 * Parameters:
 *  hInstance       HANDLE of the application instance.
 *  pszCmds         LPSTR to command line string.
 *  lpwCmdShow      LPWORD initial ShowWindow command passed to WinMain.
 *
 * Return Value:
 *  BOOL            FALSE if an error occurred, otherwise TRUE and
 *                  lpwCmdShow is returned with a valid ShowWindow command
 *                  for the initial state of the window.
 */

/************************************************************************/
BOOL FAR PASCAL PPOLEInstanceInit(HINSTANCE hInstance, LPSTR pszCmds,
	LPWORD lpwCmdShow)
/************************************************************************/
{
#ifdef USEOLE
OLESTATUS os;
LPSTR lpEmbed;
BOOL bDoOLE;

if ( pOLE ) // OLE server already allocated
	return( YES );

// 0. Register the application into the Windows Registration database
PPOLERegisterApplication( hInstance, OBJECT_CLASS );

if ( !Control.Retail )
	return( YES ); // If not retail, we're done - NO OLE

if ( (bDoOLE = GetDefaultInt( "OLE", -1 )) < 0 )
	PutDefaultInt( "OLE", YES ); // First time default is YES
else
if ( !bDoOLE )
	return( YES ); // If OLE turned off, we're done - NO OLE

if ( !(pOLE = (LPIMAGESERVER)Alloc( sizeof(IMAGESERVER) )) )
	return( FALSE );

clr( (LPTR)pOLE, sizeof(IMAGESERVER) );

// 1. Register clipboard formats.
if ( !(pOLE->cfNative     = RegisterClipboardFormat("Native")) ||
     !(pOLE->cfOwnerLink  = RegisterClipboardFormat("OwnerLink")) ||
     !(pOLE->cfObjectLink = RegisterClipboardFormat("ObjectLink")) )
	return( FALSE );

// 2. Initialize the method tables.
if ( !PPOLEVtblInitServer(   hInstance, &pOLE->vtblSvr ) ||
     !PPOLEVtblInitDocument( hInstance, &pOLE->vtblDoc ) ||
     !PPOLEVtblInitObject(   hInstance, &pOLE->vtblObj ) )
	return( FALSE );

// 3. Initialize the server variables
pOLE->pvtbl = &pOLE->vtblSvr;
pOLE->fRelease = TRUE; // Indicate a released state.

// 4. Register the server application with OLESVR.DLL
if ( os = OleRegisterServer( OBJECT_CLASS, (LPOLESERVER)pOLE, &pOLE->lhSvr,
	hInstance, OLE_SERVER_SINGLE ) )
	{
//	dbg("RegSvr err os=%d", os );dbg(0);
	return( FALSE );
	}

//dbg("RegSvr pOLE=%lx", pOLE);dbg(0);

os = OleBlockServer( pOLE->lhSvr );
//dbg("blocking = %d",os);

// 5. Modify the CmdShow flag if we are being started by a client
pOLE->fStartedByClient = NO;
if ( (lpEmbed = lstrfind( pszCmds, "Embedding" )) &&
     (lpEmbed == pszCmds || lpEmbed == (pszCmds+1)) )
	{
	pOLE->fStartedByClient = YES;
	*lpwCmdShow = SW_SHOWMINNOACTIVE; // SW_HIDE;
	}

#endif // #ifdef USEOLE

//dbg("Started by client = %d", pOLE->fStartedByClient );dbg(0);
return( TRUE );
}


/************************************************************************/
void FAR PASCAL PPOLERegisterApplication( HINSTANCE hInstance, LPSTR lpDocClass)
{
#ifdef USEOLE
STRING szCommand;
FNAME szFileName;
LPSTR lp;

/*
 * Add various strings to the registration database:
 *  1.  English-readable class name.
 *  2.  Simple class name.
 *  3.  Full path to the server executable.
 *  4.  Full path to the handler DLL, if available.
 *  5.  Verbs
 *  6.  Any formats supported through OleSetData and OleRequestData.
 *  7.  Full application path if the server supports StdExecute.
 */

GetModuleFileName( hInstance, szFileName, sizeof(FNAME)-1 );
Lowercase( szFileName );

if ( !Control.Retail )
	goto Part2;

RegKeyValue( YES, "protocol\\StdFileEditing\\RequestDataFormats", "CF_DIB,CF_BITMAP" );
RegKeyValue( YES, "protocol\\StdFileEditing\\SetDataFormats", "CF_DIB,CF_BITMAP" );
RegKeyValue( YES, "protocol\\StdFileEditing\\verb\\0", "Edit" );
RegKeyValue( YES, "protocol\\StdFileEditing\\server",  szFileName );
RegKeyValue( YES, "protocol\\StdFileExecute\\server",  szFileName );

Part2:
lstrcpy( szCommand, szFileName ); lstrcat( szCommand, " /p %1" );
RegKeyValue( YES, "shell\\print\\command", szCommand );
lstrcpy( szCommand, szFileName ); lstrcat( szCommand, " %1" );
RegKeyValue( YES, "shell\\open\\command", szCommand );
if ( AstralStr( IDS_APPNAMEOLE, &lp ) )
	RegKeyValue( YES, NULL, lp );

if (  !RegKeyExists( NO, ".ART" ) )
	RegKeyValue( NO, ".ART", OBJECT_CLASS );

#endif // #ifdef USEOLE	
}


/*
 * PPOLEExit
 *
 * Purpose:
 *  Handles the File/Exit menu command, verifying dirty files as necessary
 *  and revoking the server.
 *
 * Parameters:
 *  void
 *
 * Return Value:
 *  BOOL            TRUE if the application can exit, FALSE otherwise.
 */

/************************************************************************/
BOOL FAR PASCAL PPOLEExit(void)
/************************************************************************/
{
#ifdef USEOLE
BOOL fRet;
OLESTATUS os;
LHSERVER lhSvr;

if ( !pOLE )
	return( FALSE );

//dbg("{Exit");dbg(0);

lhSvr = pOLE->lhSvr;
pOLE->lhSvr = 0L;
if ( (os = OleRevokeServer( lhSvr )) == OLE_WAIT_FOR_RELEASE )
	{
	pOLE->fRelease = FALSE;
	PPOLEReleaseWait( &pOLE->fRelease );
	fRet = TRUE;
	}
else	fRet = (OLE_OK == os);

PPOLEVtblFreeServer(   &pOLE->vtblSvr );
PPOLEVtblFreeDocument( &pOLE->vtblDoc );
PPOLEVtblFreeObject(   &pOLE->vtblObj );

//dbg("}Exit revoke os=%d fRet=%d", os, fRet);dbg(0);
return( fRet );

#else
return TRUE;
#endif // #ifdef USEOLE
}


/************************************************************************/
BOOL FAR PASCAL PPOLERegisterDocument( HWND hWnd )
/************************************************************************/
{
#ifdef USEOLE
OLESTATUS os;
LPIMAGE lpImageToRegister;

if ( !pOLE )
	return( FALSE );

//dbg("{RegDoc hWnd=%d", hWnd);dbg(0);

if ( !(lpImageToRegister = (LPIMAGE)GetWindowLong( hWnd, GWL_IMAGEPTR )) )
	{
//	dbg("}RegDoc err; NULL image");dbg(0);
	return( FALSE );
	}

if ( lpImageToRegister->lhDoc )
	{
//	dbg("}RegDoc TRUE; Already reg");dbg(0);
	return( TRUE );
	}

lpImageToRegister->pvtbl = &pOLE->vtblDoc;

if ( pOLE->InMethod )
	{
//	dbg("}RegDoc TRUE; In method; no reg");dbg(0);
	return( TRUE );
	}

// Register the document
if ( os = OleRegisterServerDoc( pOLE->lhSvr, lpImageToRegister->CurFile,
	(LPOLESERVERDOC)lpImageToRegister, &lpImageToRegister->lhDoc ) )
	{ // Must revoke on any error.
//	dbg("}RegDoc FALSE reg-os=%d", os);dbg(0);
//	OleRevokeServer( pOLE->lhSvr );
	return( FALSE );
	}

#endif // #ifdef USEOLE

//dbg("}RegDoc TRUE");dbg(0);
return( TRUE );
}


/************************************************************************/
BOOL FAR PASCAL PPOLERevokeDocument( HWND hWnd )
/************************************************************************/
{
#ifdef USEOLE
OLESTATUS os;
LPIMAGE lpImageToRevoke;

if ( !pOLE )
	return( FALSE );

//dbg("{RevDoc hWnd=%d", hWnd);dbg(0);

if ( !(lpImageToRevoke = (LPIMAGE)GetWindowLong( hWnd, GWL_IMAGEPTR )) )
	{
//	dbg("}RevDoc FALSE; NULL image");dbg(0);
	return( FALSE );
	}

if ( !lpImageToRevoke->lhDoc )
	{
//	dbg("}RevDoc TRUE; Already rev");dbg(0);
	return( TRUE );
	}

if ( pOLE->InMethod )
	{
//	dbg("}RevDoc TRUE; In method; no rev");dbg(0);
	return( TRUE );
	}

if ( (os = OleRevokeServerDoc( lpImageToRevoke->lhDoc )) == OLE_WAIT_FOR_RELEASE )
	{
	lpImageToRevoke->fRelease = FALSE;
	PPOLEReleaseWait( &lpImageToRevoke->fRelease );
	}

if ( lpImageToRevoke->fOwnedByClient && lpImageToRevoke->fUntitled )
	PPOLEClientNotify( (LPIMAGEOBJECT)lpImageToRevoke->pObj, OLE_CLOSED );

// BE SURE ->lhDoc ISN'T USED AFTER THIS REVOKE
lpImageToRevoke->lhDoc = NULL;
//dbg("}RevDoc OK; lpImage=%lx os=%d", lpImageToRevoke, os );dbg(0);
return( !os );

#else

return TRUE;
#endif // #ifdef USEOLE
}


/*
 * PPOLEClientNotify
 *
 * Purpose:
 *  Performs a direct function call to the single callback in the
 *  client that is communicating with this server application.
 *  This is the only point where there is direct communication
 *  between the two applications.
 *
 * Parameters:
 *  pObj            LPIMAGEOBJECT that contains a pClient pointer to an
 *                  LPOLECLIENT that holds a pointer to the OLECLIENTVTBL
 *                  that holds a pointer to the CallBack function.
 *  wMsg            WORD, message to send, such as OLE_CLOSED.
 *
 * Return Value:
 *  None.
 */

/************************************************************************/
void FAR PASCAL PPOLEClientNotify(LPIMAGEOBJECT pObj, WORD wMsg)
/************************************************************************/
{
#ifdef USEOLE
LPOLECLIENT     pClient;
LPOLECLIENTVTBL pvt;

if ( !pOLE || !pObj )
	return;

if ( !(pClient = pObj->pClient) )
	return;

if ( !(pvt = pClient->lpvtbl) )
	return;

//typedef struct _OLECLIENTVTBL {     /* ocv */
//    int (CALLBACK* CallBack)(LPOLECLIENT, OLE_NOTIFICATION, LPOLEOBJECT);

//dbg("{Notify pObj=%lx wMsg=%d", pObj, wMsg);dbg(0);
(pvt->CallBack)(pClient, (OLE_NOTIFICATION)wMsg, (LPOLEOBJECT)pObj);
//dbg("}Notify OK callback");dbg(0);

#endif // #ifdef USEOLE
}


/*
 * PPOLEReleaseWait
 *
 * Purpose:
 *  Enters a Peek/Translate/Dispatch message loop to process all messages
 *  to the application until a release flag has been sent.  The application
 *  calls this routine whenever it is required to wait until conversations
 *  for OLE have terminated.
 *
 *  Some of the messages processed may be DDE messages (for OLE 1.x) that
 *  are eventually passed to OLESVR which eventually calls the ServerRelease
 *  function (see OLESVR.C).  ServerRelease modifies a BOOL flag indicating
 *  that the server is released.
 *
 *  Therefore we can watch a particular memory location (*pf)
 *  and only exit when that flag is set.
 *
 * Parameters:
 *  pf              BOOL FAR * to the flag to wait on.
 *
 * Return Value:
 *  BOOL            Contents of *pf.
 *
 */

/************************************************************************/
BOOL FAR PASCAL PPOLEReleaseWait( BOOL FAR *pf )
/************************************************************************/
{
#ifdef USEOLE
MSG msg;

//dbg("{RelWait *pf=%d", *pf );dbg(0);

*pf = FALSE;

while ( !*pf )
	{
	/*
	 * We use PeekMessage here to make a point about power
	 * management and ROM Windows--GetMessage, when there's
	 * no more messages, will correctly let the system go into
	 * a low-power idle state.  PeekMessage by itself will not.
	 * If you do background processing in a PeekMessage loop like
	 * this, and there's no background processing to be done,
	 * then you MUST call WaitMessage to duplicate the idle
	 * state like GetMessage.
	 */

	if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
		/*
		 * We will not see WM_QUIT in the middle of the
		 * application since this application MUST call
		 * PostQuitMessage to get it in the queue.  Therefore we
		 * don't even worry about it.
		 */
		TranslateMessage (&msg);
		DispatchMessage (&msg);
		}
	else	{
		/*
		 * If the application has some background processing
		 * to do, it should perform a piece of it here.  Otherwise
		 * you MUST call WaitMessage or you'll screw up ROM-Windows
		 * power-management.
		 */
		WaitMessage();
		}
	}

//dbg("}RelWait *pf=%d", *pf );dbg(0);
return( *pf );

#else
return TRUE;
#endif // #ifdef USEOLE
}


/*
 * PPOLEVtblInitServer
 *
 * Purpose:
 *  Creates procedure instances for all the OLE methods required
 *  by the server library for standard server methods.
 *
 * Parameters:
 *  hInst           HANDLE of the application instance.
 *  pvt             LPOLESERVERVTBL to the VTBL to initialize.
 *
 * Return Value:
 *  None
 *
 * Customization:
 *  None required.
 */


/************************************************************************/
BOOL FAR PASCAL PPOLEVtblInitServer(HINSTANCE hInst, LPOLESERVERVTBL pvt)
/************************************************************************/
{
#ifdef USEOLE
pvt->Create  = (OLE1_CREATEPROC)MakeProcInstance( (FARPROC)ServerCreateDoc, hInst );
pvt->CreateFromTemplate =
			   (OLE1_CREATEFROMTEMPLATEPROC)MakeProcInstance( (FARPROC)ServerCreateDocFromTemplate, hInst );
pvt->Open    = (OLE1_OPENPROC)MakeProcInstance( (FARPROC)ServerOpenDoc, hInst );
pvt->Edit    = (OLE1_EDITPROC)MakeProcInstance( (FARPROC)ServerEditDoc, hInst );
pvt->Execute = (OLE1_EXECUTEPROC)MakeProcInstance( (FARPROC)ServerExecute, hInst );
pvt->Exit    = (OLE1_EXITPROC)MakeProcInstance( (FARPROC)ServerExit, hInst );
pvt->Release = (OLE1_RELEASEPROC)MakeProcInstance( (FARPROC)ServerRelease, hInst );

if ( !(pvt->Create)		||
     !(pvt->CreateFromTemplate) ||
     !(pvt->Edit)		||
     !(pvt->Execute)		||
     !(pvt->Exit)		||
     !(pvt->Open)		||
     !(pvt->Release) )
	return( FALSE );

#endif // #ifdef USEOLE

return( TRUE );
}


/*
 * PPOLEVtblFreeServer
 *
 * Purpose:
 *  Frees all procedure instances in the server VTBL.
 *
 * Parameters:
 *  pvt             LPOLESERVERVTBL to the VTBL to free.
 *
 * Return Value:
 *  none
 *
 * Customization:
 *  None required.
 */

/************************************************************************/
void FAR PASCAL PPOLEVtblFreeServer(LPOLESERVERVTBL pvt)
/************************************************************************/
{
#ifdef USEOLE
FreeProcInstance((FARPROC)pvt->Create);
FreeProcInstance((FARPROC)pvt->CreateFromTemplate);
FreeProcInstance((FARPROC)pvt->Edit);
FreeProcInstance((FARPROC)pvt->Exit);
FreeProcInstance((FARPROC)pvt->Open);
FreeProcInstance((FARPROC)pvt->Release);
#endif // #ifdef USEOLE
}


/*
 * PPOLEVtblInitDocument
 *
 * Purpose:
 *  Creates procedure instances for all the OLE methods required
 *  for document methods.
 *
 * Parameters:
 *  hInst           HANDLE of the application instance.
 *  pvt             LPOLESERVERDOCVTBL to the VTBL to initialize.
 * 
 * Return Value:
 *  None
 *
 * Customization:
 *  None required.
 */


/************************************************************************/
BOOL FAR PASCAL PPOLEVtblInitDocument(HINSTANCE hInst, LPOLESERVERDOCVTBL pvt)
/************************************************************************/
{
#ifdef USEOLE
pvt->Close            = (OLE1_CLOSE)MakeProcInstance( (FARPROC)DocClose, hInst );
pvt->GetObject        = (OLE1_GETOBJECT)MakeProcInstance( (FARPROC)DocGetObject, hInst );
pvt->Execute          = (OLE1_EXECUTE)MakeProcInstance( (FARPROC)DocExecute, hInst );
pvt->Release          = (OLE1_RELEASE)MakeProcInstance( (FARPROC)DocRelease, hInst );
pvt->Save             = (OLE1_SAVE)MakeProcInstance( (FARPROC)DocSave, hInst );
pvt->SetColorScheme   = (OLE1_SETCOLORSCHEME)MakeProcInstance( (FARPROC)DocSetColorScheme, hInst );
pvt->SetDocDimensions = (OLE1_SETDOCDIMENSIONS)MakeProcInstance( (FARPROC)DocSetDocDimensions, hInst );
pvt->SetHostNames     = (OLE1_SETHOSTNAMES)MakeProcInstance( (FARPROC)DocSetHostNames, hInst );


if ( !(pvt->Close)	      ||
     !(pvt->GetObject)	      ||
     !(pvt->Execute)	      ||
     !(pvt->Release)	      ||
     !(pvt->Save)	      ||
     !(pvt->SetColorScheme)   ||
     !(pvt->SetDocDimensions) ||
     !(pvt->SetHostNames) )
	return( FALSE );

#endif // #ifdef USEOLE
return( TRUE );
}


/*
 * PPOLEVtblFreeDocument
 *
 * Purpose:
 *  Frees all procedure instances in the document VTBL.
 *
 * Parameters:
 *  pvt             LPOLESERVERDOCVTBL to the VTBL to free.
 *
 * Return Value:
 *  none
 *
 * Customization:
 *  None required.
 */

/************************************************************************/
void FAR PASCAL PPOLEVtblFreeDocument(LPOLESERVERDOCVTBL pvt)
/************************************************************************/
{
#ifdef USEOLE
FreeProcInstance((FARPROC)pvt->Close);
FreeProcInstance((FARPROC)pvt->GetObject);
FreeProcInstance((FARPROC)pvt->Release);
FreeProcInstance((FARPROC)pvt->Save);
FreeProcInstance((FARPROC)pvt->SetColorScheme);
FreeProcInstance((FARPROC)pvt->SetDocDimensions);
FreeProcInstance((FARPROC)pvt->SetHostNames);
#endif // #ifdef USEOLE
}


/*
 * PPOLEVtblInitObject
 *
 * Purpose:
 *  Creates procedure instances for all the OLE methods required
 *  for object methods.
 *
 * Parameters:
 *  hInst           HANDLE of the application instance.
 *  pvt             LPOLEOBJECTVTBL to free.
 * 
 * Return Value:
 *  None
 *
 * Customization:
 *  Your application might not use global variables for srvrvtbl,
 *  docvtbl, and objvtbl.
 */

/************************************************************************/
BOOL FAR PASCAL PPOLEVtblInitObject(HINSTANCE hInst, LPOLEOBJECTVTBL pvt)
/************************************************************************/
{
#ifdef USEOLE
pvt->QueryProtocol  = (OLE1_QUERYPROTOCOL)MakeProcInstance((FARPROC)ObjQueryProtocol,hInst);
pvt->DoVerb         = (OLE1_DOVERB)MakeProcInstance( (FARPROC)ObjDoVerb, hInst );
pvt->EnumFormats    = (OLE1_ENUMFORMATS)MakeProcInstance( (FARPROC)ObjEnumFormats, hInst );
pvt->GetData        = (OLE1_GETDATA)MakeProcInstance( (FARPROC)ObjGetData, hInst );
pvt->Release        = (OLE1_RELEASEOBJ)MakeProcInstance( (FARPROC)ObjRelease, hInst );
pvt->SetBounds      = (OLE1_SETBOUNDS)MakeProcInstance( (FARPROC)ObjSetBounds, hInst );
pvt->SetColorScheme = (OLE1_SETOBJCOLORSCHEME)MakeProcInstance( (FARPROC)ObjSetColorScheme, hInst );
pvt->SetData        = (OLE1_SETDATA)MakeProcInstance( (FARPROC)ObjSetData, hInst );
pvt->SetTargetDevice= (OLE1_SETTARGETDEVICE)MakeProcInstance( (FARPROC)ObjSetTargetDevice, hInst );
pvt->Show           = (OLE1_SHOW)MakeProcInstance( (FARPROC)ObjShow, hInst );

if ( !(pvt->QueryProtocol)	||
     !(pvt->DoVerb)		||
     !(pvt->EnumFormats)	||
     !(pvt->GetData)		||
     !(pvt->Release)		||
     !(pvt->SetBounds)		||
     !(pvt->SetColorScheme)	||
     !(pvt->SetData)		||
     !(pvt->SetTargetDevice)	||
     !(pvt->Show) )
	return( FALSE );

#endif // #ifdef USEOLE
return( TRUE );
}


/*
 * PPOLEVtblFreeObject
 *
 * Purpose:
 *  Frees all procedure instances in the object VTBL.
 *
 * Parameters:
 *  pvt             LPOLEOBJECTVTBL to the VTBL to free.
 *
 * Return Value:
 *  none
 *
 * Customization:
 *  None required.
 */

/************************************************************************/
void FAR PASCAL PPOLEVtblFreeObject(LPOLEOBJECTVTBL pvt)
/************************************************************************/
{
#ifdef USEOLE
FreeProcInstance((FARPROC)pvt->DoVerb);
FreeProcInstance((FARPROC)pvt->EnumFormats);
FreeProcInstance((FARPROC)pvt->GetData);
FreeProcInstance((FARPROC)pvt->QueryProtocol);
FreeProcInstance((FARPROC)pvt->Release);
FreeProcInstance((FARPROC)pvt->SetBounds);
FreeProcInstance((FARPROC)pvt->SetColorScheme);
FreeProcInstance((FARPROC)pvt->SetData);
FreeProcInstance((FARPROC)pvt->SetTargetDevice);
FreeProcInstance((FARPROC)pvt->Show);
#endif // #ifdef USEOLE
}


/***********************************************************************/
static BOOL RegKeyValue(
/***********************************************************************/
BOOL 	bObjectClass,
LPSTR 	lpSubClass,
LPSTR	lpValue)
{
#ifdef USEOLE
HKEY hRootKey;
STRING szClass;

hRootKey = HKEY_CLASSES_ROOT;

szClass[0] = '\0';
if ( bObjectClass )
	{
	lstrcat( szClass, OBJECT_CLASS );
	if ( lpSubClass )
		lstrcat( szClass, "\\" );
	}
if ( lpSubClass )
	lstrcat( szClass, lpSubClass );

return( !RegSetValue( hRootKey, szClass, REG_SZ, lpValue,
	(DWORD)lstrlen(lpValue)+1 ) );

#else
return TRUE;
#endif // #ifdef USEOLE
}


/***********************************************************************/
static BOOL RegKeyExists(
/***********************************************************************/
BOOL 	bObjectClass,
LPSTR 	lpSubClass)
{
#ifdef USEOLE
HKEY hRootKey;
STRING szClass, szValue;
long lLength;

hRootKey = HKEY_CLASSES_ROOT;

szClass[0] = '\0';
if ( bObjectClass )
	{
	lstrcat( szClass, OBJECT_CLASS );
	if ( lpSubClass )
		lstrcat( szClass, "\\" );
	}
if ( lpSubClass )
	lstrcat( szClass, lpSubClass );

lLength = sizeof(szValue);
return( !RegQueryValue( hRootKey, szClass, szValue, &lLength ) );

#else
return TRUE;
#endif // #ifdef USEOLE
}
