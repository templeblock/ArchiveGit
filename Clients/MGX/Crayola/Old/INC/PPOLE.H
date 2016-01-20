//®FD1¯®PL1¯®TP0¯®BT0¯
#ifndef __ppole
#define __ppole

#ifndef _INC_OLE
#include <ole.h>
#endif

// Verbs
#define OBJVERB_EDIT    0
#define OBJVERB_PLAY    1
#define OBJVERB_MAX     1

#define OBJECT_CLASS "CrayolaArt"

/*
 * Function prototypes, organized by source file.  Any small definition
 * required by only one source file is also included here under the
 * same heading.
 */

typedef LPOLESERVER    FAR *LPLPOLESERVER;
typedef LPOLESERVERDOC FAR *LPLPOLESERVERDOC;
typedef LPOLEOBJECT    FAR *LPLPOLEOBJECT;

typedef LPVOID (FAR PASCAL *LPVOIDPROC) (LPOLEOBJECT, LPSTR);

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

#ifdef OLEMAIN
	LPIMAGESERVER pOLE;
#else
	extern LPIMAGESERVER pOLE;
#endif

#endif
