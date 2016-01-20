//®FD1¯®PL1¯®TP0¯®BT0¯
#ifndef __ppole
#define __ppole

#ifndef _INC_OLE
//#include <ole.h>
#endif

// Verbs
#define OBJVERB_EDIT    0
#define OBJVERB_PLAY    1
#define OBJVERB_MAX     1

#define OBJECT_CLASS _T("BitmapImage")

/*
 * Function prototypes, organized by source file.  Any small definition
 * required by only one source file is also included here under the
 * same heading.
 */

//typedef LPOLESERVER    FAR *LPLPOLESERVER;
typedef LPOLESERVERDOC FAR *LPLPOLESERVERDOC;
typedef LPOLEOBJECT    FAR *LPLPOLEOBJECT;

typedef LPVOID (FAR PASCAL *LPVOIDPROC) (LPOLEOBJECT, LPTSTR);

typedef struct _object
    {
    LPOLEOBJECTVTBL     pvtbl;          // Standard
    BOOL                Dummy;
    LPOLECLIENT         pClient;        // Necessary for notifications
    LPIMAGE 		lpImageDoc;	// The parent document
    STRING		szItem;		// The object string (%d,%d)-(%d,%d)
    } IMAGEOBJECT;

typedef IMAGEOBJECT FAR *LPIMAGEOBJECT;
#define CBIMAGEOBJECT sizeof(IMAGEOBJECT)

/*
 * Server data structure holding any OLE-specific variables.
 */
typedef struct _imageserver
    {
    LPOLESERVERVTBL     pvtbl;          // OLE Standard
    LHSERVER            lhSvr;          // Required by OleRegisterServer
    BOOL                fRelease;       // Flag to watch if we need to wait
    BOOL		fStartedByClient;

    OLESERVERVTBL       vtblSvr;
    OLESERVERDOCVTBL    vtblDoc;
    OLEOBJECTVTBL       vtblObj;

    WORD                cfNative;
    WORD                cfOwnerLink;
    WORD                cfObjectLink;

    BOOL		InMethod;	// Are we inside of a method call?
    } IMAGESERVER;

typedef IMAGESERVER FAR *LPIMAGESERVER;

/*
 * STDTARGETDEVICE structure for ObjSetTargetDevice.
 */
typedef struct _target
    { // Offsets are pointers into the data field.
    WORD    deviceNameOffset;   // szDeviceName
    WORD    driverNameOffset;   // szDriverName
    WORD    portNameOffset;     // szPortName
    WORD    extDevModeOffset;   // DEVMODE structure
    WORD    extDevModeSize;     // Size of the DEVMODE struct, for versioning.
    WORD    environmentOffset;  // Current MS-DOS environment, in a string
    WORD    environmentSize;    // Length of environment string.
    BYTE    data[1];
    } STDTARGETDEVICE;

typedef STDTARGETDEVICE FAR * LPSTDTARGETDEVICE;
#define CBSTDTARGETDEVICE sizeof(STDTARGETDEVICE)

/*
 * Function prototypes for OLE-Specific functions.
 */
// Source file: ppole.c
BOOL FAR PASCAL PPOLEInstanceInit(HINSTANCE hInstance, LPTSTR pszCmds, LPINT lpwCmdShow);
void FAR PASCAL PPOLERegisterApplication( HINSTANCE hInstance, LPTSTR lpDocClass );
BOOL FAR PASCAL PPOLEExit(void);
BOOL FAR PASCAL PPOLERegisterDocument( HWND hWnd );
BOOL FAR PASCAL PPOLERevokeDocument( HWND hWnd );
void FAR PASCAL PPOLEClientNotify(LPIMAGEOBJECT pObj, WORD wMsg);
BOOL FAR PASCAL PPOLEReleaseWait( BOOL FAR *pf );
BOOL FAR PASCAL PPOLEVtblInitServer(HINSTANCE hInst, LPOLESERVERVTBL pvt);
void FAR PASCAL PPOLEVtblFreeServer(LPOLESERVERVTBL pvt);
BOOL FAR PASCAL PPOLEVtblInitDocument(HINSTANCE hInst, LPOLESERVERDOCVTBL pvt);
void FAR PASCAL PPOLEVtblFreeDocument(LPOLESERVERDOCVTBL pvt);
BOOL FAR PASCAL PPOLEVtblInitObject(HINSTANCE hInst, LPOLEOBJECTVTBL pvt);
void FAR PASCAL PPOLEVtblFreeObject(LPOLEOBJECTVTBL pvt);

// Source file: ppoledoc.c
OLESTATUS FAR PASCAL DocClose(LPIMAGE lpImageDoc);
OLESTATUS FAR PASCAL DocRelease(LPIMAGE lpImageDoc);
OLESTATUS FAR PASCAL DocGetObject(LPIMAGE lpImageDoc, LPTSTR lpItem, LPLPOLEOBJECT ppObj, LPOLECLIENT pClient);
OLESTATUS FAR PASCAL DocExecute(LPIMAGE lpImageDoc, HANDLE hCommands);
OLESTATUS FAR PASCAL DocSave(LPIMAGE lpImageDoc);
OLESTATUS FAR PASCAL DocSetColorScheme(LPIMAGE lpImageDoc, LPLOGPALETTE pPal);
OLESTATUS FAR PASCAL DocSetDocDimensions(LPIMAGE lpImageDoc, LPRECT pRect);
OLESTATUS FAR PASCAL DocSetHostNames(LPIMAGE lpImageDoc, LPTSTR pszClient, LPTSTR pszObj);

// Source file: ppoleobj.c
LPIMAGEOBJECT FAR PASCAL PObjectAllocate(LPOLEOBJECTVTBL pVtblObj);
OLESTATUS FAR PASCAL ObjDoVerb(LPIMAGEOBJECT pObj, WORD iVerb, BOOL fShow, BOOL fFocus);
OLECLIPFORMAT FAR PASCAL ObjEnumFormats(LPIMAGEOBJECT pObj, WORD cf);
OLESTATUS FAR PASCAL ObjGetData(LPIMAGEOBJECT pObj, WORD cf, LPHANDLE phData);
LPVOID FAR PASCAL ObjQueryProtocol(LPIMAGEOBJECT pObj, LPTSTR pszProtocol);
OLESTATUS FAR PASCAL ObjRelease(LPIMAGEOBJECT pObj);
OLESTATUS FAR PASCAL ObjSetBounds(LPIMAGEOBJECT pObj, LPRECT pRect);
OLESTATUS FAR PASCAL ObjSetColorScheme(LPIMAGEOBJECT pObj, LPLOGPALETTE pPal);
OLESTATUS FAR PASCAL ObjSetData(LPIMAGEOBJECT pObj, WORD cf, HANDLE hData);
OLESTATUS FAR PASCAL ObjSetTargetDevice(LPIMAGEOBJECT pObj, HANDLE hData);
OLESTATUS FAR PASCAL ObjShow(LPIMAGEOBJECT pObj, BOOL fTakeFocus);

// Source file: ppolesvr.c
OLESTATUS FAR PASCAL ServerCreateDoc(LPIMAGESERVER pSvr, LHSERVERDOC lhDoc, LPTSTR pszClass, LPTSTR pszDoc, LPLPOLESERVERDOC ppServerDoc);
OLESTATUS FAR PASCAL ServerCreateDocFromTemplate( LPIMAGESERVER pSvr, LHSERVERDOC lhDoc, LPTSTR pszClass, LPTSTR pszDoc, LPTSTR pszTemplate, LPLPOLESERVERDOC ppServerDoc);
OLESTATUS FAR PASCAL ServerOpenDoc(LPIMAGESERVER pSvr, LHSERVERDOC lhDoc, LPTSTR pszDoc, LPLPOLESERVERDOC ppServerDoc);
OLESTATUS FAR PASCAL ServerEditDoc(LPIMAGESERVER pSvr, LHSERVERDOC lhDoc, LPTSTR pszClass, LPTSTR pszDoc, LPLPOLESERVERDOC ppServerDoc);
OLESTATUS FAR PASCAL ServerExecute(LPIMAGESERVER pSvr, HANDLE hCommands);
OLESTATUS FAR PASCAL ServerExit(LPIMAGESERVER pSvr);
OLESTATUS FAR PASCAL ServerRelease(LPIMAGESERVER pSvr);

extern LPIMAGESERVER pOLE;

#endif

