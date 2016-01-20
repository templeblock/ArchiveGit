/*=========================================================================*\

	Copyright(c) 1993 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=========================================================================*/

/*=========================================================================*\
	plugin.c
\*=========================================================================*/

#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "plugin.h"    // for LP_PIXXXX typedefs
#pragma pack( 1 )
#include "pitypes.h"
#include "pifilter.h"
#include "piexport.h"
#include "piacquir.h"
#pragma pack()
#include <ctype.h>

/*=========================================================================*/

#define HSRC ((HANDLE)1)
#define HDST ((HANDLE)2)

/*=========================================================================*/

// LOCAL prototypes
LOCAL BOOL GetPluginFileSpec( LPSTR lpPath );
LOCAL BOOL GetAdobePluginFileSpec( LPSTR lpPath );

/*=========================================================================*/

typedef struct _plugin
{
	ITEMID idItem;
	FNAME szDLL;
	WORD wOrdinal;
	STRING szMenuString;
	struct _plugin FAR *lpNext;
} PLUGIN;
typedef PLUGIN FAR *LPPLUGIN;

typedef HANDLE ( CALLBACK * QUERYPROC )( HWND );
typedef BOOL ( CALLBACK * CMDPROC )( HWND, int, HANDLE, HANDLE, DWORD, DWORD );
typedef void ( FAR *LPENTRYPOINTPROC )( 
	short selector,
	FilterRecord FAR *,
	long FAR *,
	short FAR * );

static LPPLUGIN lpPluginList;
static LPFRAME  lpSrcFrame, lpDstFrame;
static RECT rMask;
static HMODULE hFirstModule;
static ITEMID idNextNewItem;
static LPPLUGIN lpPluginListEnd;

/*=========================================================================*/

LOCAL void RefreshMaskedBits( 
	LPFRAME lpSrcFrame,
	LPFRAME lpDstFrame,
	LPMASK  lpMask,
	int xOff,
	int yOff );
LOCAL void ReadBuffer( LPFRAME lpFrame, LPTR lpBuffer, Rect Rect, long lRowBytes,
					   short LoPlane, short HiPlane, BOOL bInvert, int iDepth );
LOCAL void WriteBuffer( LPFRAME lpFrame, LPTR lpBuffer, Rect Rect, long lRowBytes,
					    short LoPlane, short HiPlane, BOOL bInvert, int iDepth );
#ifndef WIN32
LOCAL void CopyToHuge( LPTR lpSrc,	HPTR hpDst,	long lCount, BOOL bInvert, int iDepth );
LOCAL void CopyFromHuge( HPTR hpSrc, LPTR lpDst, long lCount, BOOL bInvert, int iDepth );
#endif
LOCAL void copyfromplane( LPTR lpSrc, LPTR lpDst, int iCount, int iDepth );
LOCAL void notfromplane( LPTR lpSrc, LPTR lpDst, int iCount, int iDepth );
LOCAL void copytoplane( LPTR lpSrc, LPTR lpDst, int iCount, int iDepth );
LOCAL void nottoplane( LPTR lpSrc, LPTR lpDst, int iCount, int iDepth );

#define NOTEQUAL( r1, r2 ) ( r1.top != r2.top || 		\
						     r1.bottom != r2.bottom ||	\
							 r1.left != r2.left ||		\
							 r1.right != r2.right )

/*=========================================================================*/

void FreeUpPlugin()
{
    LPPLUGIN lpPlugin;
    STRING szString;

    // This is a cleanup call; Freeup the plugin list and return
    if( lpPluginList )
    {
	    do	
        {
		    lpPlugin = lpPluginList->lpNext;
		    FreeUp(( LPTR )lpPluginList );
		    lpPluginList = lpPlugin;
        } 
        while( lpPluginList );
    }
    lpPluginList = NULL;
    lpPluginListEnd = NULL;
    idNextNewItem = 0;

    if( hFirstModule )
    {
	    GetModuleFileName( hFirstModule, szString, sizeof( STRING ));
	    FreeLibrary( hFirstModule );
	    FileDelete( szString );
	    hFirstModule = NULL;
    }
}

/*=========================================================================*/

BOOL SetupPluginMenus( HMENU hMenu )
{
HMENU hPopupMenu;
HANDLE hMem;
LP_PIMENUITEM lpPI;
LP_PIMENU lpP;
int nMenuItems;
LPPLUGIN lpPlugin;
HMODULE hModule;
FNAME szDLL, szFileSpec;
QUERYPROC lpQueryProc;
int retc, iCount;
FINDDATA afile;
STRING szString;

if( lpPluginList ) // If it has already been setup, get out
	return( NO );

if( !idNextNewItem )
	idNextNewItem = PS_FIRST_PLUGIN;
if( !GetPluginFileSpec( szFileSpec )) // Including the "*.eff"
	return( NO );

AstralCursor( IDC_WAIT );

// Loop through the effects directory, adding them to the menu
retc = Dos_FindFirst( szFileSpec, ATTRIB_NORMAL, &afile );
while( retc )
	{
	if( afile.dwAttrib & ATTRIB_SUBDIR )
		{ // Skip over directories
		retc = Dos_FindNext( &afile );
		continue;
		}

	// Process the matching file
	lstrcpy( szDLL, szFileSpec );
	stripfile( szDLL );
	lstrcat( szDLL, afile.szName );

	if( !( hModule = ( HMODULE )AstralLoadLibrary( szDLL ))  )
		{
		retc = Dos_FindNext( &afile );
		continue;
		}

	if( !hFirstModule )
		{
		lstrcpy( szString, Control.PouchPath );
		lstrcat( szString, "PLUGIN.DLL" );
		FileCopy( szDLL, szString );
		if( !( hFirstModule = ( HMODULE )AstralLoadLibrary( szString )))
			hFirstModule = NULL;
		}

	if( lpQueryProc = ( QUERYPROC )GetProcAddress( hModule, "PI_QueryMenu" ))
			hMem = ( *lpQueryProc )( PictPubApp.Get_hWndAstral());
	else	hMem = NULL;

	if( !hMem || !( lpP = ( LP_PIMENU )GlobalLock( hMem )))
		{
		if( hMem )
			GlobalFree( hMem );
		FreeLibrary( hModule );
		retc = Dos_FindNext( &afile );
		continue;
		}

	if( lpP->dwDataID != ID_PIMENU )
		{
		GlobalUnlock( hMem );
		GlobalFree( hMem );
		FreeLibrary( hModule );
		retc = Dos_FindNext( &afile );
		continue;
		}

	nMenuItems = lpP->nMenuItems;
	hPopupMenu = NULL;
	lpPI = lpP->piMenu;
	lpPI--;
	while( --nMenuItems >= 0 )
		{
		lpPI++;
		if( !lpPI->wType ) // Skip the PS menu placement stuff
			continue;
		if( !lpPI->wTypeInfo )
			{ // It's a popup menu name
			hPopupMenu = NULL;
			iCount = GetMenuItemCount( hMenu );
			while( --iCount >= 0 )
				{ // Search the menu for this menu string
				szString[0] = '\0';
				GetMenuString( hMenu, iCount, szString, sizeof( STRING )-1,
					MF_BYPOSITION );
				if( StringsEqual( szString, lpPI->szMenuString ))
					{
					hPopupMenu = GetSubMenu( hMenu, iCount );
					break;
					}
				}
			if( !hPopupMenu )
				{ // It's not in the menu
				if( hPopupMenu = CreatePopupMenu())
					AppendMenu( hMenu, MF_POPUP | MF_STRING, ( UINT )hPopupMenu,
						lpPI->szMenuString );
				}
			}
		else // Its' a popup menu item
		if( lpPlugin = ( LPPLUGIN )Alloc(( long )sizeof( PLUGIN )))
			{
			lpPlugin->idItem = idNextNewItem++;
			lstrcpy( lpPlugin->szDLL, szDLL );
			lstrcpy( lpPlugin->szMenuString, lpPI->szMenuString );
			lpPlugin->wOrdinal = lpPI->wTypeInfo;
			lpPlugin->lpNext = NULL;
			// Ignore the menu location record: lpPI->wType

			AppendMenu(( hPopupMenu ? hPopupMenu : hMenu ),
				MF_BYCOMMAND | MF_STRING, lpPlugin->idItem,
				lpPlugin->szMenuString );
			if( lpPluginListEnd )
				lpPluginListEnd->lpNext = lpPlugin;
			lpPluginListEnd = lpPlugin;
			if( !lpPluginList )
				lpPluginList = lpPlugin;
			}
		}

	GlobalUnlock( hMem );
	GlobalFree( hMem );
	FreeLibrary( hModule );
	retc = Dos_FindNext( &afile );
	}

AstralCursor( NULL );
return( YES );
}

/***********************************************************************/
LOCAL BOOL GetPluginFileSpec( LPSTR lpPath )
/***********************************************************************/
{
	STRING szPath;
	OFSTRUCT of;
	FINDDATA afile;

	if( !Control.UsePlugins )
		return( NO );

	// Try both plugin paths...
	lstrcpy( szPath, Control.PluginPath1 );
	FixPath( szPath );
	lstrcat( szPath, "*.eff" );

	// If the directory exists, and has a file in it, we're done
	if( Dos_FindFirst( szPath, ATTRIB_NORMAL, &afile ))
	{
		lstrcpy( lpPath, szPath );
		return( YES );
	}

	lstrcpy( szPath, Control.PluginPath2 );
	FixPath( szPath );
	lstrcat( szPath, "*.eff" );

	// If the directory exists, and has a file in it, we're done
	if( Dos_FindFirst( szPath, ATTRIB_NORMAL, &afile ))
	{
		lstrcpy( lpPath, szPath );
		return( YES );
	}

	// If either path has been specified,  then don't do search.
	if(( lstrlen( Control.PluginPath1 ) > 0 ) ||
	   ( lstrlen( Control.PluginPath2 ) > 0 ))
	{
		lpPath[0] = 0;
		return( NO );
	}

	// If there was no ini entry, or it's wrong, search the disk for the effects...
	// Search the path...
	if( OpenFile( "GALLERY.EXE", &of, OF_READ|OF_EXIST ) > 0 )
		OemToAnsi( of.szPathName,szPath );
	else // Search the C: drive ( up to 2 levels deep )
		if( !FindFile( "C:\\", "GALLERY.EXE", 2 /*MaxDepth*/, szPath ) > 0 )
			return( NO );

	stripfile( szPath );
	FixPath( szPath );
	lstrcat( szPath, "GE_POUCH\\PLUG_IN\\*.eff" );

	if( Dos_FindFirst( szPath, ATTRIB_NORMAL, &afile ))
	{
		FNAME szAdobe;

		lstrcpy( lpPath, szPath );
		stripfile( szPath );
		FixPath( Lowercase( szPath ));

		if( !GetAdobePluginFileSpec( szAdobe ))
		{
			lstrcpy( Control.PluginPath1, szPath );
		}
		else
		{
			// Get rid of the extension.
			stripfile( szAdobe );

			// Make sure all case matches
			FixPath( Lowercase( szAdobe ));
			FixPath( Lowercase( Control.PluginPath1 ));
			FixPath( Lowercase( Control.PluginPath2 ));

			if( !StringsEqual( szAdobe, Control.PluginPath1 ))
			{
				lstrcpy( Control.PluginPath2, szPath );
			}
			else
			{
				lstrcpy( Control.PluginPath1, szPath );
			}
		}

		SavePreferences();
		return( YES );
	}

	lpPath[0] = 0;
	return( NO );
}

/***********************************************************************/
long HandlePluginCommand( LPIMAGE lpImage, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
    LPPLUGIN lpPlugin;
    HMODULE hModule;
    CMDPROC lpCommandProc;
    BOOL bMask, bRet;
    RGBS rgb1, rgb2;
    LPMASK lpMask;
    LPOBJECT lpObject;
    RECT rEdits;
    int iDocType;

    if( wParam < PS_FIRST_PLUGIN || wParam > PS_LAST_PLUGIN )
	    return( NO );
    if( !lpImage )
	    return( NO );

    if( ImgCountSelObjects( lpImage, NULL ) != 1 )
    {
	    Message( IDS_OBJECTSONLYONE );
	    return( NO );
    }

    // The source is the active image
    if( !( lpObject = ImgGetSelObject( lpImage, NULL )))
	    return( NO );
    if( !( lpSrcFrame = ObjGetEditFrame( lpObject )))
	    return( NO );
    if( !ImgEditInit( lpImage, ET_OBJECT, UT_NEWDATA, lpObject ))
	    return( FALSE );

    // Find the plugin that matches the command ID from the plugin list
    lpPlugin = lpPluginList;
    while( lpPlugin )
    {
	    if( lpPlugin->idItem == wParam )
		    break;
	    lpPlugin = lpPlugin->lpNext;
    }

    // Get out if you can't find it
    if( !lpPlugin )
	    return( NO );

    // Load the effect DLL
    if( !( hModule = ( HMODULE )AstralLoadLibrary( lpPlugin->szDLL )))
	    return( NO );

    // Get the address of the effect procedure
    if( !( lpCommandProc = ( CMDPROC )GetProcAddress( hModule,
        MAKEINTRESOURCE( lpPlugin->wOrdinal ))))
    {
	    FreeLibrary( hModule );
	    return( NO );
    }

    // Create an empty destination frame
    if( !( lpDstFrame = FrameOpen( FrameType( lpSrcFrame ),
	    FrameXSize( lpSrcFrame ), FrameYSize( lpSrcFrame ),
	    FrameResolution( lpSrcFrame ))))
    {
	    FreeLibrary( hModule );
	    lpSrcFrame = NULL;
	    lpDstFrame = NULL;
	    return( NO );
    }

    // Move the mask to the destination frame
    bMask = ImgGetMaskRect( lpImage, &rMask );
    AstralIntersectRect( &rMask, &rMask, &lpObject->rObject );
    rEdits = rMask;
    OffsetRect( &rMask, -lpObject->rObject.left, -lpObject->rObject.top );

    // Execute the effect procedure
    GetActiveRGB( &rgb1 );
    GetAlternateRGB( &rgb2 );

    switch( FrameType( lpSrcFrame ))
    {
	    case FDT_LINEART :
	    case FDT_GRAYSCALE :
		    iDocType = DOC_IMG_GRAY8;
	    break;

	    case FDT_RGBCOLOR :
		    iDocType = DOC_IMG_RGB24;
	    break;

	    // Since Gallery effects do not support CMYK,
	    // we need buffers for conversion
	    case FDT_CMYKCOLOR :
		    Message( IDS_NO_CMYK_IN_GALLERY );
		    bRet = FALSE;
		    goto ErrorExit;
	    break;
    }

    typedef BOOL ( CALLBACK * CMDPROC )( HWND, int, HANDLE, HANDLE, DWORD, DWORD );
    bRet = ( *lpCommandProc )( PictPubApp.Get_hWndAstral(),
	    iDocType,
	    ( HANDLE )HDST /*hDstBuf*/,
	    ( HANDLE )HSRC /*hSrcBuf*/,
	    RGB( rgb1.red, rgb1.green, rgb1.blue ) /*dwForeColor*/,
	    RGB( rgb2.red, rgb2.green, rgb2.blue ) /*dwBackColor*/ );

ErrorExit:

    // Unload the DLL
    FreeLibrary( hModule );

    if( !bRet )     // If the effect was UNsuccessful
    { 
	    FrameClose( lpDstFrame );
	    lpSrcFrame = NULL;
	    lpDstFrame = NULL;
	    return( FALSE );
    }

    // The effect was successful
    if( bMask && ( lpMask = ImgGetMask( lpImage )))
    { 
        // Copy the pixels outside the masked area back in
	    RefreshMaskedBits( lpSrcFrame, lpDstFrame, lpMask,
						    lpObject->rObject.left,
						    lpObject->rObject.top );
    }

    // Activate the new frame
    ImgEditedObjectFrame( lpImage, lpObject, IDS_UNDOSPECIALFILTER,
					    &rEdits, lpDstFrame, NULL );
    UpdateImage( lpImage, &rEdits, YES );
    lpSrcFrame = NULL;
    lpDstFrame = NULL;
    return( bRet );
}


/***********************************************************************/
long HandlePluginMessage( LPIMAGE lpImage, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
HANDLE hMem, hImage;
LPBUFFER lpBuffer;
LPFRAME lpFrame;
static int nLineSrc, nLineDst, nMaxLines;
static BOOL bEscapable;

if( !lpPluginList )
	return( NO );
if( !lpImage )
	return( NO );

switch ( wParam )
    {
    case PS_SERVICE_ABORT_BEGIN:
    case PS_SERVICE_ABORT_BEGIN_NOESC:
//	lRet is void
//	lParam is a MAKELONG( hMem, nMaxLines )
//		where hMem is a handle to a STRING
//		where nMaxLines is a loop maximum count
	nMaxLines = HIWORD( lParam );
	bEscapable = ( wParam == PS_SERVICE_ABORT_BEGIN );
	AstralCursor( IDC_WAIT );
	return( NULL ); // void
	break;

    case PS_SERVICE_ABORT_CHECK:
//	lRet is a BOOL; FAILED if FALSE
//	lParam is a MAKELONG( 0, nLine )
	return( AstralClockCursor( HIWORD( lParam ), nMaxLines, bEscapable ));
	break;

    case PS_SERVICE_ABORT_END:
//	lRet is void
//	lParam is 0L
	AstralCursor( NULL );
	return( NULL ); // void
	break;

    case PS_SERVICE_GET_IMAGE_INFO:
//	lRet is a BOOL; FAILED if FALSE
//	lParam is a MAKELONG( hImage, hMem )
//		where hImage comes at startup or from PS_SERVICE_BUF_ALLOC
//		where hMem is a handle to a BUFFER to stuff
	hImage = (HANDLE)LOWORD( lParam );
	if( hImage == HSRC )
		lpFrame = lpSrcFrame;
	else
	if( hImage == HDST )
		lpFrame = lpDstFrame;
	else
		return( NULL );

	if( !lpFrame )
		return( FALSE );
	if( !( hMem = (HANDLE)HIWORD( lParam )))
		return( FALSE );
	if( !( lpBuffer = ( LPBUFFER )GlobalLock( hMem )))
		return( FALSE );
	clr(( LPTR )lpBuffer, sizeof( BUFFER ));

	switch( FrameType( lpFrame ))
	{
		case FDT_LINEART :
		case FDT_GRAYSCALE :
			lpBuffer->nBitsPerPixel = 8;
			lpBuffer->nBytesPerLine = RectWidth( &rMask );
		break;

		case FDT_RGBCOLOR :
			lpBuffer->nBitsPerPixel = 24;
			lpBuffer->nBytesPerLine = RectWidth( &rMask ) * 3;
		break;
	}

	lpBuffer->nPixelsPerLine = RectWidth( &rMask );
	lpBuffer->nLines = RectHeight( &rMask );
	lpBuffer->nPlanes = 1; /* Must be 1 */
	lpBuffer->BufType = BUF_FILE;
	lstrcpy( lpBuffer->BufFileName, lpImage->CurFile );
	lpBuffer->nxRes = FrameResolution( lpFrame );
	lpBuffer->nyRes = FrameResolution( lpFrame );
	lpBuffer->nImgClass = 0;
	lpBuffer->hDocPalette = ( HPALETTE )NULL;
	GlobalUnlock( hMem );
	return( TRUE );
	break;

    case PS_SERVICE_PRIVATE_INI:
//	lRet is void
//	lParam is a LPTR to stuff in the INI path name
	lstrcpy(( LPSTR )lParam, Control.ProgHome );
	lstrcat(( LPSTR )lParam, "FX.INI" );
	return( NULL ); // void
	break;

    case PS_SERVICE_BUF_LOCK1:
//	lRet is a HANDLE ( WORD ) to a locked image buffer; FAILED if <= 0
//	lParam is a MAKELONG( hImage, nStartLine )
//		where hImage comes at startup or from PS_SERVICE_BUF_ALLOC
	hImage = (HANDLE)LOWORD( lParam );
	if( hImage == HSRC )
		{
    	nLineSrc = rMask.top + HIWORD( lParam );
		return( (long)hImage );
		}
	if( hImage == HDST )
		{
   	    nLineDst = rMask.top + HIWORD( lParam );
			return( (long)hImage );
		}
		return( NULL );
	break;

    case PS_SERVICE_BUF_UNLOCK1:
//	lRet is a BOOL; FAILED if FALSE
//	lParam is a MAKELONG( hLockedImage, 0 )
//		where hLockedImage comes from PS_SERVICE_BUF_LOCK1
	hImage = (HANDLE)LOWORD( lParam );
	return( TRUE );
	break;

    case PS_SERVICE_BUF_NEXTLINE:
//	lRet is an LPTR to the image data; FAILED if NULL
//	lParam is a MAKELONG( hLockedImage, 0 )
//		where hLockedImage comes from PS_SERVICE_BUF_LOCK1
	hImage = (HANDLE)LOWORD( lParam );
	if( hImage == HSRC )
		{
		return(( long )FramePointer( lpSrcFrame, rMask.left, nLineSrc++, NO ));
		}
	else
	if( hImage == HDST )
		{
		return(( long )FramePointer( lpDstFrame, rMask.left, nLineDst++, YES ));
		}
	return( NULL );
	break;

    case PS_SERVICE_BUF_LINE_TABLE:
//	lRet is an LPPTR to the line table?; FAILED if NULL
//	lParam is a MAKELONG( hImage, hLockedImage )
//		where hImage comes at startup or from PS_SERVICE_BUF_ALLOC
//		where hLockedImage comes from PS_SERVICE_BUF_LOCK1
	return( NULL );
	break;

	// Shouldn't get called
    case PS_SERVICE_BUF_ALLOC:
//	lRet is a HANDLE ( WORD ) to an allocated buffer; FAILED if <= 0
//	lParam is a HANDLE ( WORD ) to a passed-in ( stuffed ) PS_BUF structure,
//		typedef struct
//		      {
//		      BOOL     bDelAfterUsed;
//		      int      nBitsPerPixel;
//		      int      nPixelsPerLine;
//		      int      nLines;
//		      int      nReserved[6];
//		      } PS_BUF;
//		typedef PS_BUF FAR *LPPS_BUF;
	return( NULL );
	break;

	// Shouldn't get called
    case PS_SERVICE_BUF_DELETE:
//	lRet is void
//	lParam is a MAKELONG( hImage, 0 )
//		where hImage comes at startup or from PS_SERVICE_BUF_ALLOC
	return( NULL ); // void
	break;

	// Shouldn't get called - UNKNOWN??? Message
    case PS_SERVICE_BUF_LINEPTR:
	return( NULL );
	break;

    default:
	return( FALSE );
    }
return( TRUE );
}

/*=========================================================================*\

	Code for Adobe Photoshop plugins...

\*=========================================================================*/

typedef struct ADOBEPLUGIN_tag
{
	STRING szCategory;
	STRING szFilter;
	STRING szFileName;
	int    iResourceNum;
	WORD   idItem;
	PLUGIN_TYPE piType;
	long	lData;
	struct ADOBEPLUGIN_tag FAR *lpNext;
}
ADOBEPLUGIN, FAR *LPADOBEPLUGIN;

typedef void ( FAR pascal *LPADOBEENTRYPROC )( short selector, FilterRecordPtr stuff, long FAR *data, short FAR *result );
typedef void ( FAR pascal *LPEXPORTENTRYPROC )( short selector, ExportRecordPtr stuff, long FAR *data, short FAR *result );
typedef void ( FAR pascal *LPACQUIREENTRYPROC )( short selector, AcquireRecordPtr stuff, long FAR *data, short FAR *result );

static LPADOBEPLUGIN lpAdobePluginList = NULL;
static BOOL bAdobeAborted;

LOCAL BOOL InitAdobePlugins();
LOCAL long HandleAdobeAcquireCommand( LPADOBEPLUGIN lpPlugin );
LOCAL BOOL HandleAdobeFilterCommand( LPIMAGE lpImage, LPADOBEPLUGIN lpPlugin, Handle *parameters, LPRECT lpUpdateRect );
LOCAL long HandleAdobeExportCommand( LPIMAGE lpImage, LPADOBEPLUGIN lpPlugin );

/*=========================================================================*/

LOCAL BOOL GetAdobePluginFileSpec( LPSTR lpPath )
{
	STRING szPath;
	FINDDATA afile;

	if( !Control.UsePlugins )
		return( NO );

	// Try both plugin paths...
	lstrcpy( szPath, Control.PluginPath1 );
	FixPath( szPath );
	if( Control.Crippled )
		lstrcat( szPath, "*.8bf" );
	else
		lstrcat( szPath, "*.8b?" );

	// If the directory exists, and has a file in it, we're done
	if( Dos_FindFirst( szPath, ATTRIB_NORMAL, &afile ))
	{
		lstrcpy( lpPath, szPath );
		return( YES );
	}

	lstrcpy( szPath, Control.PluginPath2 );
	FixPath( szPath );
	if( Control.Crippled )
		lstrcat( szPath, "*.8bf" );
	else
		lstrcat( szPath, "*.8b?" );

	// If the directory exists, and has a file in it, we're done
	if( Dos_FindFirst( szPath, ATTRIB_NORMAL, &afile ))
	{
		lstrcpy( lpPath, szPath );
		return( YES );
	}

	lpPath[0] = '\0';
	return( NO );
}

/*=========================================================================*/

LOCAL BOOL AddAdobeFilter( LPSTR lpCategory, LPSTR lpFilter, LPSTR lpFileName,
	                       int  iResourceNum, PLUGIN_TYPE piType, int idMenu )
{
	LPADOBEPLUGIN lpPluginWalker;
	LPADOBEPLUGIN lpNewPlugin;

	// Allocate the memory for the new plugin
	lpNewPlugin = ( LPADOBEPLUGIN ) Alloc( sizeof( ADOBEPLUGIN ));

	if( !lpNewPlugin )
		return( FALSE );

	// Stuff the data members...
	lstrcpy( lpNewPlugin->szCategory, lpCategory );
	lstrcpy( lpNewPlugin->szFilter,   lpFilter   );
	lstrcpy( lpNewPlugin->szFileName, lpFileName );
	lpNewPlugin->iResourceNum = iResourceNum;
	lpNewPlugin->idItem       = idMenu;
	lpNewPlugin->piType		  = piType;
	lpNewPlugin->lData		  = 0;
	lpNewPlugin->lpNext       = NULL;

	// Special case the first plugin
	if( lpAdobePluginList == NULL )
	{
		lpAdobePluginList = lpNewPlugin;
		return( TRUE );
	}

	// Add the plugin to the end of the list
	lpPluginWalker = lpAdobePluginList;

	while( lpPluginWalker->lpNext != NULL )
		lpPluginWalker = lpPluginWalker->lpNext;

	lpPluginWalker->lpNext = lpNewPlugin;

	return( TRUE );
}

/*=========================================================================*/

BOOL SetupAdobeImageMenu( HMENU hImageMenu )
{
	HMENU  hPopupMenu;
	STRING szString;
	LPADOBEPLUGIN lpWalker;
	int iCount;

	// Put up the wait cursor
	AstralCursor( IDC_WAIT );

	if( !Control.UsePlugins || !hImageMenu || !InitAdobePlugins())
    {
        AstralCursor( NULL );
        return( FALSE );
    }

	// Now add the image menu items...
	lpWalker = lpAdobePluginList;

	while( lpWalker != NULL )
	{
 		if( lpWalker->piType != PIT_FILTER )
		{
			lpWalker = lpWalker->lpNext;
			continue;
		}
		hPopupMenu = NULL;
		iCount = GetMenuItemCount( hImageMenu );

		// Search the menu for this category
		while( --iCount >= 0 )
		{
			szString[0] = '\0';
			GetMenuString( hImageMenu, iCount, szString, sizeof( STRING )-1,
				MF_BYPOSITION );
			if( StringsEqual( szString, lpWalker->szCategory ))
			{
				hPopupMenu = GetSubMenu( hImageMenu, iCount );
				break;
			}
		}

		// It's a new category
		if( !hPopupMenu )
		{
			if( hPopupMenu = CreatePopupMenu())
				AppendMenu( hImageMenu, MF_POPUP | MF_STRING, ( UINT )hPopupMenu,
					lpWalker->szCategory );
		}

		AppendMenu(( hPopupMenu ? hPopupMenu : hImageMenu ),
			MF_BYCOMMAND | MF_STRING, lpWalker->idItem,
			lpWalker->szFilter );

		lpWalker = lpWalker->lpNext;
	}
	AstralCursor( NULL );
	return( TRUE );
}

/*=========================================================================*/

BOOL SetupAdobeFileExportMenu( HMENU hFileMenu, int nPosition )
{
	HMENU  hPopupMenu;
	LPADOBEPLUGIN lpWalker;
	int nEmptyItems, nRelPosition, nExports = 0;


	// Put up the wait cursor
	AstralCursor( IDC_WAIT );

	if( !Control.UsePlugins || !hFileMenu || !InitAdobePlugins())
        goto DeleteItem;

	lpWalker = lpAdobePluginList;

	if( !( hPopupMenu = GetSubMenu( hFileMenu, nPosition )))
        goto DeleteItem;

    // Popup was defined with 1 empty item in PPMENU.RC
    nEmptyItems = GetMenuItemCount( hPopupMenu );
    if( nEmptyItems )
        nRelPosition = 0;

	while( lpWalker != NULL )
	{
 		if( lpWalker->piType != PIT_EXPORT )
		{
			lpWalker = lpWalker->lpNext;
			continue;
		}

		// Replace empty items with plugins, while empty items exist
		if( nEmptyItems )
        {
	        ModifyMenu( hPopupMenu, nRelPosition,
					    MF_BYPOSITION | MF_STRING, lpWalker->idItem,
					    lpWalker->szFilter );
            nEmptyItems--;
            nRelPosition++;
        }
		else
			AppendMenu( hPopupMenu, MF_STRING, lpWalker->idItem,
				lpWalker->szFilter );

   		++nExports;
		lpWalker = lpWalker->lpNext;
	}
	//Control.ExportItems = nExports;

    if( !nExports )
        goto DeleteItem;

    DrawMenuBar( AfxGetMainWnd()->GetSafeHwnd());
	AstralCursor( NULL );
	return( TRUE );

DeleteItem:
    // If no items were put into the menu, delete the File/Export menu item
	DeleteMenu( hFileMenu, nPosition, MF_BYPOSITION );
    DrawMenuBar( AfxGetMainWnd()->GetSafeHwnd());
	AstralCursor( NULL );
	return( FALSE );
}
/*=========================================================================*/

BOOL SetupAdobeFileImportMenu( HMENU hFileMenu, int nPosition )
{
	HMENU  hPopupMenu;
	LPADOBEPLUGIN lpWalker;
	int nEmptyItems, nRelPosition, nAcquires = 0;


	// Put up the wait cursor
	AstralCursor( IDC_WAIT );

	if( !Control.UsePlugins || !hFileMenu || !InitAdobePlugins())
        goto DeleteItem;

	lpWalker = lpAdobePluginList;

	if( !( hPopupMenu = GetSubMenu( hFileMenu, nPosition )))
        goto DeleteItem;

    // Popup was defined with 1 empty item in PPMENU.RC
    nEmptyItems = GetMenuItemCount( hPopupMenu );
    if( nEmptyItems )
        nRelPosition = 0;

	while( lpWalker != NULL )
	{
 		if( lpWalker->piType != PIT_ACQUIRE )
		{
			lpWalker = lpWalker->lpNext;
			continue;
		}

		// Replace empty items with plugins, while empty items exist
		if( nEmptyItems )
        {
	        ModifyMenu( hPopupMenu, nRelPosition,
					    MF_BYPOSITION | MF_STRING, lpWalker->idItem,
					    lpWalker->szFilter );
            nEmptyItems--;
            nRelPosition++;
        }
		else
			AppendMenu( hPopupMenu,	MF_STRING, lpWalker->idItem,
					    lpWalker->szFilter );

   		++nAcquires;
		lpWalker = lpWalker->lpNext;
	}
	//Control.AcquireItems = nAcquires;

    if( !nAcquires )
        goto DeleteItem;

    DrawMenuBar( AfxGetMainWnd()->GetSafeHwnd());
	AstralCursor( NULL );
	return( TRUE );

DeleteItem:
    // If no items were put into the menu, delete the File/Acquire menu item
	DeleteMenu( hFileMenu, nPosition, MF_BYPOSITION );
    DrawMenuBar( AfxGetMainWnd()->GetSafeHwnd());
	AstralCursor( NULL );
	return( FALSE );
}

/*=========================================================================*/
void FreeUpAdobePlugin()
/*=========================================================================*/
{
	LPADOBEPLUGIN lpWalker;
	// This is a cleanup call; Freeup the plugin list and return
	lpWalker = lpAdobePluginList;

	while( lpWalker )
	{
		LPADOBEPLUGIN lpDeadMeat = lpWalker;

		lpWalker = lpWalker->lpNext;

		FreeUp(( LPTR )lpDeadMeat );
	}
	lpAdobePluginList = NULL;
}

/*=========================================================================*/
LOCAL BOOL InitAdobePlugins()
/*=========================================================================*/
{
	HMODULE hModule;
	FNAME  szDLL, szFileSpec;
	FINDDATA afile;
	char LastChar;
	int retc, idMenu;

	if( lpAdobePluginList ) // If it has already been setup, get out
		return( TRUE );

	idMenu = PS_FIRST_ADOBEPLUGIN;
		
	if( !GetAdobePluginFileSpec( szFileSpec )) // Including the "*.8b?"
		return( FALSE );

	// Loop through the effects directory, adding them to the menu
	retc = Dos_FindFirst( szFileSpec, ATTRIB_NORMAL, &afile );

	// Walk the directory...
	while( retc )
	{
		if( afile.dwAttrib & ATTRIB_SUBDIR )
		{ 
            // Skip over directories
			retc = Dos_FindNext( &afile );
			continue;
		}

		LastChar = afile.szName[lstrlen( afile.szName )-1];
		if( isupper( LastChar ))
			LastChar = tolower( LastChar );

		// is it an acquire, export or filter plugin?
		if( LastChar != 'f' && LastChar != 'e' && LastChar != 'a' )
		{	
            // Skip over unsupported plugins
			retc = Dos_FindNext( &afile );
			continue;
		}

		// Process the matching file
		lstrcpy( szDLL, szFileSpec );
		stripfile( szDLL );
		lstrcat( szDLL, afile.szName );

		if( hModule = ( HMODULE )AstralLoadLibrary( szDLL ))
		{
			// Do module specifics here...
			HRSRC  hCategory, hResource;
			STRING szFilterNum, szResType;
			int    iFilterCount = 0, iExportCount = 0, iAcquireCount = 0, iCount;
			PLUGIN_TYPE piType;

			do
			{
				if( LastChar == 'f' )
				{
					iCount = iFilterCount;
					lstrcpy( szResType, "_8BFM" );
					piType = PIT_FILTER;

				}
                else
				if( LastChar == 'e' )
				{
					iCount = iExportCount;
					lstrcpy( szResType, "_8BEM" );
					piType = PIT_EXPORT;
				}
                else    // LastChar == 'a'
				{
					iCount = iAcquireCount;
					lstrcpy( szResType, "_8BAM" );
					piType = PIT_ACQUIRE;
				}

				wsprintf( szFilterNum, "#%d", iCount+1 );

				hCategory = FindResource( 
					hModule,
					( LPCSTR )szFilterNum,
					( LPCSTR )"PiMI" );

				hResource = FindResource( 
					hModule,
					( LPCSTR )szFilterNum,
					( LPCSTR )szResType );

				if( hResource && hCategory )
				{
					// Do resource specifics here...
					HGLOBAL hCatData,  hResData;
					LPTR   lpCatData, lpResData;

					hCatData  = LoadResource( hModule, hCategory );
					lpCatData = ( LPTR )LockResource( hCatData );

					hResData  = LoadResource( hModule, hResource );
					lpResData = ( LPTR )LockResource( hResData );

					// We have the category/filter data loaded and locked...
					if( lstrlen(( LPSTR )&lpCatData[2] ) > 0 )
					{
						AddAdobeFilter( 
							( LPSTR )&lpCatData[2],
							( LPSTR )&lpResData[2], 
							( LPSTR )szDLL,
							iCount+1,
							piType, idMenu );
					}
					else
					{
						AddAdobeFilter( 
							( LPSTR )"Other...",
							( LPSTR )&lpResData[2], 
							( LPSTR )szDLL,
							iCount+1,
							piType, idMenu );
					}
					UnlockResource( hCatData );
					FreeResource( hCatData );

					UnlockResource( hResData );
					FreeResource( hResData );

					++idMenu;
					if( LastChar == 'f' )
						++iFilterCount;
					else
					if( LastChar == 'e' )
						++iExportCount;
					else
					if( LastChar == 'a' )
						++iAcquireCount;
				}
			}
			while( hResource != NULL );

			FreeLibrary( hModule );
		}
		retc = Dos_FindNext( &afile );
	}
return( TRUE );
}

/*=========================================================================*/

BOOL EXPORT AdobeTestAbort()
{
	if( CANCEL && !ALT && !CONTROL )
	{
		bAdobeAborted = TRUE;
		return( TRUE );
	}

	return( FALSE );
}

/*=========================================================================*/

void EXPORT AdobeProgressProc( long done, long total )
{
	if( AstralClockCursor(( int )done, ( int )total, YES ))
		bAdobeAborted = TRUE;
}

/*=========================================================================*/

long EXPORT AdobeHostProc()
{
	return( 0L );
}


/*=========================================================================*/

static LPTR lpDispBuffer;
static long lDispBufSize;

short EXPORT AdobeDisplayPixelsProc( 	const PSPixelMap FAR *source,
										const VRect FAR *srcRect,
									  	int32 dstRow,
									  	int32 dstCol,
									  	unsigned32 platformContext )
{
RECT rDest;
int depth, y;
long lSize;
HDC hDC;
HPTR hpSrc;
BLTSESSION BltSession;
FRMTYPEINFO TypeInfo;

hDC = ( HDC )platformContext;
switch ( source->imageMode )
	{
	case plugInModeGrayScale:
		depth = 1;
		FrameSetTypeInfo( &TypeInfo, FDT_GRAYSCALE, NULL );
		break;
	case plugInModeRGBColor:
		depth = 3;
		FrameSetTypeInfo( &TypeInfo, FDT_RGBCOLOR, NULL );
		break;
	case plugInModeCMYKColor:
		depth = 4;
		FrameSetTypeInfo( &TypeInfo, FDT_CMYKCOLOR, NULL );
		break;
	default:
		return( 1 );
	}
// only support chunky data
if( source->planeBytes != 1 || source->colBytes != depth )
	return( 1 );

// allocate buffer used for display
lSize = ( srcRect->right - srcRect->left ) * ( long )depth;
if( lSize > lDispBufSize )
	{
	if( lpDispBuffer )
		FreeUp( lpDispBuffer );
	lDispBufSize = 0;
	lpDispBuffer = Alloc( lSize );
	if( !lpDispBuffer )
		return( memFullErr );
	lDispBufSize = lSize;
	}
// go to start of data in pixel map
hpSrc = ( HPTR )source->baseAddr;
hpSrc += ( srcRect->top * source->rowBytes ) + srcRect->left;

// setup SuperBlt
rDest.top = dstRow;
rDest.left = dstCol;
rDest.bottom = rDest.top + ( srcRect->bottom - srcRect->top ) - 1;
rDest.right = rDest.left + ( srcRect->right - srcRect->left ) - 1;

StartSuperBlt( &BltSession, hDC, NULL, lpBltScreen, &rDest, TypeInfo,
	10, 0, 0, YES, NULL, NULL );
for ( y = rDest.top; y <= rDest.bottom; ++y )
	{
	#ifdef WIN32
 	copy( hpSrc, lpDispBuffer, lSize);
	if (depth == 4)
		negate(lpDispBuffer, lSize);
	#else
 	CopyFromHuge( hpSrc, lpDispBuffer, lSize, depth == 4, 1 );
	#endif
	SuperBlt( &BltSession, lpDispBuffer );
	hpSrc += source->rowBytes;
	    }
SuperBlt( &BltSession, NULL );
return( 0 );
}

/*=========================================================================*/

long HandleAdobePluginCommand( LPIMAGE lpImage, int idCmd )
{
	LPADOBEPLUGIN lpWalker;

	if( idCmd < PS_FIRST_ADOBEPLUGIN || idCmd > PS_LAST_ADOBEPLUGIN )
		return( NO );

	// Find the plugin that matches the command ID from the plugin list
	lpWalker = lpAdobePluginList;

	while( lpWalker != NULL )
	{
		if( lpWalker->idItem == idCmd )
			break;
		lpWalker = lpWalker->lpNext;
	}

	// Get out if you can't find it
	if( !lpWalker )
		return( NO );

    // check for valid image unless using acquire module
    if(( lpWalker->piType != PIT_ACQUIRE ) && !lpImage )    
		return( NO );

	if( lpWalker->piType == PIT_FILTER )
	{
		Handle parameters = NULL;
		RECT rUpdate;

		if( HandleAdobeFilterCommand( lpImage, lpWalker, &parameters, &rUpdate ))
		{
			PSFILTER_PARMS parms;

			UpdateImage( lpImage, &rUpdate, YES );
			lstrcpy( parms.szFilter, lpWalker->szFilter );
			parms.lpParameters = NULL;
			parms.fHandle = NO;
			if( parameters )
			{
				if( IsBadReadPtr( parameters, 1 ))
				{
					if( HIWORD( parameters ) == 0 ) // a handle?
					{
						HGLOBAL hMem = ( HGLOBAL )parameters;
						if( GlobalSize( hMem ))
						{
							long lTemp = LOWORD(( long )hMem );
							parms.lpParameters = ( LPTR )lTemp;
							parms.fHandle = 1; // windows handle
						}
					}
				}
				else
				{
					LPTR lp;

					lp = *((LPPTR)parameters);
					if (!IsBadReadPtr(lp, 1))
						parms.fHandle = 2; // photoshop handle
					parms.lpParameters = ( LPTR )parameters;
				}
			}
			PostCommand( lpImage->lpCmdList, IDS_CMD_PSFILTER, &parms );
			return( YES );
		}
	}
	else
	if( lpWalker->piType == PIT_EXPORT )
		return( HandleAdobeExportCommand( lpImage, lpWalker ));
	else
	if( lpWalker->piType == PIT_ACQUIRE )
		return( HandleAdobeAcquireCommand( lpWalker ));

	return( NO );
}

/*=========================================================================*/

BOOL PSFilterImage( LPIMAGE lpImage, LPPSFILTER_PARMS lpParms )
{
	LPADOBEPLUGIN lpWalker;
	BOOL fRet;
	RECT rUpdate;
	Handle parameters;

	// Find the plugin that matches the filter name from the plugin list
	lpWalker = lpAdobePluginList;
	while( lpWalker != NULL )
	{
		if( StringsEqual( lpWalker->szFilter, lpParms->szFilter ))
			break;
		lpWalker = lpWalker->lpNext;
	}

	// Get out if you can't find it
	if( !lpWalker )
		return( FALSE );
	parameters = ( Handle )lpParms->lpParameters;
	fRet = HandleAdobeFilterCommand( lpImage, lpWalker, &parameters, &rUpdate );
	if( fRet )
	{
		lpParms->Common.UpdateType = UT_AREA;
		lpParms->Common.rUpdateArea = rUpdate;
	}
	return( fRet );
}

/*=========================================================================*/

LOCAL long HandleAdobeAcquireCommand( LPADOBEPLUGIN lpPlugin )
{
	HWND        hFocus;
    LPIMAGE     lpDstImage;
	FRMDATATYPE Type;
	HMODULE     hModule;
	BOOL        bInvert, bDone;
	LPLONG      lpData;
	STRING      szEntryPoint;
	short       retcode;
    long        lRowBytes;
    int         iDepth;

	AcquireRecord       AdobeRecord;
	PlatformData        AdobePlatform;
	TestAbortProc       lpAbortProc;
	ProgressProc        lpProgressProc;
	DisplayPixelsProc   lpDisplayPixelsProc;
	LPACQUIREENTRYPROC  lpEntryPoint;

    lpDstImage          = NULL;
    lpDstFrame          = NULL;
	lpAbortProc         = NULL;
	lpProgressProc      = NULL;
    lpDisplayPixelsProc = NULL;

    bInvert = bDone = FALSE;
	bAdobeAborted = FALSE;
    retcode       = 1;

	// Load the acquisition DLL
	if( !( hModule = LoadLibrary( lpPlugin->szFileName )))
		return( NO );

	hFocus = GetFocus();

	lpAbortProc         = ( TestAbortProc )MakeProcInstance(( FARPROC )AdobeTestAbort,    PictPubApp.Get_hInstAstral());
	lpProgressProc      = ( ProgressProc )MakeProcInstance(( FARPROC )AdobeProgressProc, PictPubApp.Get_hInstAstral());
	lpDisplayPixelsProc = ( DisplayPixelsProc )MakeProcInstance(( FARPROC )AdobeDisplayPixelsProc, PictPubApp.Get_hInstAstral());

	if( !lpAbortProc || !lpProgressProc )
		goto BadStart;

	// Get the entry point function label
	wsprintf( szEntryPoint, "ENTRYPOINT%d", lpPlugin->iResourceNum );

	// Get the entry point into the DLL
	if( !( lpEntryPoint = ( LPACQUIREENTRYPROC )GetProcAddress( hModule, szEntryPoint )))
		goto BadStart;

	// Set the FilterRecord Info
	clr(( LPTR )&AdobeRecord, sizeof( AcquireRecord ));

	AdobeRecord.abortProc       = lpAbortProc;
	AdobeRecord.progressProc    = lpProgressProc;
	AdobeRecord.maxData         = AvailableMemory() - ( 1024L * Control.MainMemMin );
    AdobeRecord.planeMap[0]     = 0;
    AdobeRecord.planeMap[1]     = 1;
    AdobeRecord.planeMap[2]     = 2;
    AdobeRecord.planeMap[3]     = 3;
	AdobePlatform.hwnd          = ( unsigned short )PictPubApp.Get_hWndAstral();
	AdobeRecord.platformData    = ( void * )&AdobePlatform;
	AdobeRecord.canTranspose    = FALSE;
	AdobeRecord.needTranspose   = FALSE;
	AdobeRecord.diskSpace       = -1;
	AdobeRecord.spaceProc       = NULL;
	AdobeRecord.bufferProcs     = NULL;
	AdobeRecord.resourceProcs   = NULL;
	AdobeRecord.processEvent    = NULL;
	AdobeRecord.canReadBack     = FALSE;
	AdobeRecord.displayPixels   = lpDisplayPixelsProc;
	AdobeRecord.handleProcs     = NULL;
	AdobeRecord.imageHRes       = 72;   // Adobe default
	AdobeRecord.imageVRes       = 72;   // Adobe default

	AdobeRecord.hostSig         =
  		( long )'P' << 24 | ( long )'P' << 16 | ( long )'U' << 8 | ( long )'B';

	AdobeRecord.monitor.gamma   = 65536L;
	AdobeRecord.monitor.redX    = 40960L;
	AdobeRecord.monitor.redY    = 22282L;
	AdobeRecord.monitor.greenX  = 18350L;
	AdobeRecord.monitor.greenY  = 38993L;
	AdobeRecord.monitor.blueX   = 10158L;
	AdobeRecord.monitor.blueY   = 4587L;
	AdobeRecord.monitor.whiteX  = 20493L;
	AdobeRecord.monitor.whiteY  = 21561L;
	AdobeRecord.monitor.ambient = 32768L;

	lpData = &lpPlugin->lData;

	// The acquireSelectorPrepare function sets up memory used for the 
    // acquire operation. Host must have already informed the plugin about 
    // available memory (See maxdata record entry). 
	( *lpEntryPoint )( 
		( short )acquireSelectorPrepare,
		( AcquireRecord FAR * )&AdobeRecord,
		( long FAR * )lpData,
		( short FAR * )&retcode );

	if( retcode != 0 )
	{
		bAdobeAborted = TRUE;
		goto BadStart;
	}

	// The acquireSelectorStart function informs the host of the image type, 
    // mode, and resolution. Plugin dialogs are displayed during this call. 
	( *lpEntryPoint )( 
		( short )acquireSelectorStart,
		( AcquireRecord FAR * )&AdobeRecord,
		( long FAR * )lpData,
		( short FAR * )&retcode );

	if( retcode != 0 )
	{
		bAdobeAborted = TRUE;
		goto BadStart;
	}

	if( AdobeRecord.depth != 1 && AdobeRecord.depth != 8 )
    {
		bAdobeAborted = TRUE;
		goto Done;
    }

    switch( AdobeRecord.imageMode )
    {
        case plugInModeBitmap:	   
            Type = FDT_LINEART;
            iDepth = 1;
            break;

        case plugInModeGrayScale:    
    	    Type = FDT_GRAYSCALE;
            iDepth = 1;
            break;

        case plugInModeRGBColor:	   
            Type = FDT_RGBCOLOR;    
            iDepth = 3;
            break;

        case plugInModeCMYKColor:    
            Type = FDT_CMYKCOLOR;    
            iDepth = 4;
            bInvert = TRUE;
            break;

        // unsupported image types
        case plugInModeIndexedColor: 
        case plugInModeHSLColor:	   
        case plugInModeHSBColor:	   
        case plugInModeMultichannel: 
        case plugInModeDuotone:	   
        case plugInModeLabColor:	   
	        Message( IDS_UNSUPPORTEDPLUGINTYPE );  
    		bAdobeAborted = TRUE;
	        goto Done;
    }

    if( !( lpDstFrame = FrameOpen( Type, AdobeRecord.imageSize.h, AdobeRecord.imageSize.v,
	                               ROUND( AdobeRecord.imageHRes ))))
    {
    	FrameError( IDS_EIMAGEOPEN );
   		bAdobeAborted = TRUE;
        goto Done;
    }

	// start the processing of the data
	ProgressBegin( 1, 0 );

	while( !bDone && !bAdobeAborted )
	{
	    // The acquireSelectorContinue function returns an area of the image 
        // to the host.
	    ( *lpEntryPoint )( 
		    ( short )acquireSelectorContinue,
		    ( AcquireRecord FAR * )&AdobeRecord,
		    ( long FAR * )lpData,
		    ( short FAR * )&retcode );

	    if( retcode != 0 )
	    {
    	    bAdobeAborted = TRUE;
		    break;
	    }

        if( AdobeRecord.data == NULL )  // no more data
	    {
    	    bDone = TRUE;
		    break;
	    }

        if( AdobeRecord.rowBytes < 1 )
            lRowBytes = ( AdobeRecord.theRect.right - AdobeRecord.theRect.left ) * iDepth; 
        else
            lRowBytes = AdobeRecord.rowBytes;

        WriteBuffer( lpDstFrame, ( LPTR )AdobeRecord.data, AdobeRecord.theRect, lRowBytes,
    				 AdobeRecord.loPlane, AdobeRecord.hiPlane, bInvert, iDepth );
	}
Done:
	// The acquireSelectorFinish function must be called when exiting at any 
    // point after acquireSelectorStart is called without error.
	( *lpEntryPoint )( 
		( short )acquireSelectorFinish,
		( AcquireRecord FAR * )&AdobeRecord,
		( long FAR * )lpData,
		( short FAR * )&retcode );

	if( retcode != 0 )
    	bAdobeAborted = TRUE;

    if( !bAdobeAborted )
    {
        lpDstImage = CreateImage( NULL, lpDstFrame, NULL, 
                                  NULL, Control.DefaultFileType, GetFrameFileDataType( lpDstFrame ), 
                                  IMG_DOCUMENT, NULL );
        if( lpDstImage )
        {
            lpDstImage->fChanged = TRUE;
            lpDstImage->fUntitled = TRUE;

            if( !( PictPubApp.OpenDocumentFile(( LPSTR )lpDstImage->CurFile, lpDstImage )))         
                DestroyImage( lpDstImage );
        }
    }
	ProgressEnd();

BadStart:

	// Unload the DLL
	FreeLibrary( hModule );

	// Clean up our proc's
	if( lpAbortProc )
		FreeProcInstance(( FARPROC )lpAbortProc );

	if( lpProgressProc )
		FreeProcInstance(( FARPROC )lpProgressProc );

	if( lpDisplayPixelsProc )
		FreeProcInstance(( FARPROC )lpDisplayPixelsProc );

	if( hFocus )
		SetFocus( hFocus );

	// If the effect was UNsuccessful
	if( retcode != 0 || bAdobeAborted )
	{
        if( lpDstFrame )
	        FrameClose( lpDstFrame );

		return( FALSE );
	}
	return( bDone );
}

/*=========================================================================*/

LOCAL BOOL HandleAdobeFilterCommand( LPIMAGE lpImage, LPADOBEPLUGIN lpPlugin, Handle *parameters, LPRECT lpUpdateRect )
{
	LPOBJECT lpObject;
	LPFRAME  lpSrcFrame, lpDstFrame, lpMskFrame;
	HMODULE   hModule;
	LPMASK   lpMask;
	STRING   szEntryPoint;
	short    retcode;
	BOOL	 fStarted = FALSE;
	BOOL     bInvert = FALSE;
	FilterRecord rec;
	BOOL bMask;
	LPTR lpInBuffer, lpOutBuffer, lpMaskBuffer;
	RECT rEdits, rOut;
	Rect outRect, maskRect, inRect, maskImageRect;
	int  iDepth;
	COLORINFO ActiveColor, AlternateColor;
	long lInSize, lOutSize, lMaskSize, lSize;
	int inHeight, outHeight, maskHeight;
	short outLoPlane, outHiPlane, inLoPlane, inHiPlane;
	long outRowBytes;
	int top, left;
	LPLONG lpData;

	PlatformData AdobePlatform;
	LPADOBEENTRYPROC lpEntryPoint;
	TestAbortProc    	lpAbortProc;
	ProgressProc    	lpProgressProc;
	DisplayPixelsProc   lpDisplayPixelsProc;

	AstralSetRectEmpty( lpUpdateRect );

	// make sure we have an image
	if( !lpImage )
		return( NO );

	// make sure we have only one object selected
	if( ImgCountSelObjects( lpImage, NULL ) != 1 )
	{
		Message( IDS_OBJECTSONLYONE );
		return( NO );
	}

	// The source is the currently selected object
	if( !( lpObject = ImgGetSelObject( lpImage, NULL )))
		return( NO );

	// Get the source frame for the currently selected object
	if( !( lpSrcFrame = ObjGetEditFrame( lpObject )))
		return( NO );

	// Move the mask to the destination frame
	if( bMask = ImgGetMaskRect( lpImage, &rMask ))
	{   // get intersection of mask and object rect
		if( !AstralIntersectRect( &rMask, &rMask, &lpObject->rObject ))
		{   // no intersection, tell user
			Message( IDS_EMASKINTERSECT );
			return( NO );
		}
		lpMask = ImgGetMask( lpImage );
		lpMskFrame = lpMask->Pixmap.EditFrame;
	}
	else
		rMask = lpObject->rObject;
	AstralSetRectEmpty(&rEdits);
	OffsetRect( &rMask, -lpObject->rObject.left, -lpObject->rObject.top );

	// make sure we can initalize the undo for this object
	if( !ImgEditInit( lpImage, ET_OBJECT, UT_NEWDATA, lpObject ))
		return( NO );

	// initialize buffer pointers
	hModule			= NULL;
	lpDstFrame 		= NULL;
	lpInBuffer     	= NULL;
	lpOutBuffer    	= NULL;
	lpMaskBuffer 	= NULL;
	lpDispBuffer 	= NULL;
	lDispBufSize	= 0;

	// get instances of callback procs
	lpAbortProc    = ( TestAbortProc )MakeProcInstance(( FARPROC )AdobeTestAbort,    PictPubApp.Get_hInstAstral());
	lpProgressProc = ( ProgressProc )MakeProcInstance(( FARPROC )AdobeProgressProc, PictPubApp.Get_hInstAstral());
	lpDisplayPixelsProc     = ( DisplayPixelsProc )MakeProcInstance(( FARPROC )AdobeDisplayPixelsProc, PictPubApp.Get_hInstAstral());

	if( !lpAbortProc || !lpProgressProc )
		goto BadStart;

	// Load the effect DLL
	if( !( hModule = LoadLibrary( lpPlugin->szFileName )))
		goto BadStart;

	// Get the entry point function label
	wsprintf( szEntryPoint, "ENTRYPOINT%d", lpPlugin->iResourceNum );

	// Get the entry point into the DLL
	if( !( lpEntryPoint = ( LPADOBEENTRYPROC )GetProcAddress( hModule, szEntryPoint )))
    {
		goto BadStart;
    }

	// Create an empty destination frame
	if( !( lpDstFrame = FrameCopy( lpSrcFrame, NULL )))
    {
		goto BadStart;
    }

    // indicate we have completed the initalization phase
	fStarted = TRUE;

	// If we have to swap to Adobe CMYK...
	if( FrameType( lpSrcFrame ) == FDT_CMYKCOLOR )
		bInvert = TRUE;

	// Get the active and alternate colors for the effect to use
	GetActiveColor( &ActiveColor );
	GetAlternateColor( &AlternateColor );

	// Set the FilterRecord Info
	clr(( LPTR )&rec, sizeof( FilterRecord ));

	rec.displayPixels = lpDisplayPixelsProc;

	rec.serialNumber = 0;
	rec.abortProc    = lpAbortProc;
	rec.progressProc = lpProgressProc;
	rec.parameters = *parameters;

	rec.imageSize.v = FrameYSize( lpSrcFrame );
	rec.imageSize.h = FrameXSize( lpSrcFrame );

	// bound box of the selection
	rec.filterRect.left   = rMask.left;
	rec.filterRect.right  = rMask.right+1;  // adjust for non-inclusive rects
	rec.filterRect.top    = rMask.top;
	rec.filterRect.bottom = rMask.bottom+1; // adjust for non-inclusive rects

	rec.background.red   = AlternateColor.rgb.red   << 8;
	rec.background.green = AlternateColor.rgb.green << 8;
	rec.background.blue  = AlternateColor.rgb.blue  << 8;

	rec.foreground.red   = ActiveColor.rgb.red   << 8;
	rec.foreground.green = ActiveColor.rgb.green << 8;
	rec.foreground.blue  = ActiveColor.rgb.blue  << 8;

	rec.maxSpace    = AvailableMemory() - ( 1024L * 256L ); // leave 256K
	rec.bufferSpace = 0L;

	rec.inData      = NULL;
	rec.outData     = NULL;

	rec.isFloating   = FALSE;
	rec.haveMask     = bMask;
	rec.autoMask     = TRUE;
	rec.maskData     = NULL;
	rec.maskRowBytes = 0;

	rec.inRect.top = rec.inRect.bottom = rec.inRect.left = rec.inRect.right = 0;
	rec.outRect.top = rec.outRect.bottom = rec.outRect.left = rec.outRect.right = 0;
	rec.maskRect.top = rec.maskRect.bottom = rec.maskRect.left = rec.maskRect.right = 0;

#ifdef UNUSED
	rec.floatCoord.h = FrameXSize( lpSrcFrame );
	rec.floatCoord.v = FrameYSize( lpSrcFrame );

	rec.wholeSize.h = FrameXSize( lpSrcFrame );
	rec.wholeSize.v = FrameYSize( lpSrcFrame );
#endif // UNUSED

	rec.hostSig =
		( long )'P' << 24 | ( long )'P' << 16 | ( long )'U' << 8 | ( long )'B';

	rec.hostProc     = NULL;  

	switch( FrameType( lpSrcFrame ))
	{
		case FDT_LINEART :
		case FDT_GRAYSCALE :
			rec.imageMode    = plugInModeGrayScale;
			rec.backColor[0] = AlternateColor.gray;
			rec.foreColor[0] = ActiveColor.gray;
			rec.planes       = 1;
		break;

		case FDT_RGBCOLOR :
			rec.imageMode = plugInModeRGBColor;
			rec.backColor[0] = AlternateColor.rgb.red;
			rec.backColor[1] = AlternateColor.rgb.green;
			rec.backColor[2] = AlternateColor.rgb.blue;

			rec.foreColor[0] = ActiveColor.rgb.red;
			rec.foreColor[1] = ActiveColor.rgb.green;
			rec.foreColor[2] = ActiveColor.rgb.blue;
			rec.planes       = 3;
		break;

		case FDT_CMYKCOLOR :
			rec.imageMode = plugInModeCMYKColor;
			rec.backColor[0] = AlternateColor.cmyk.c^0xFF;
			rec.backColor[1] = AlternateColor.cmyk.m^0xFF;
			rec.backColor[2] = AlternateColor.cmyk.y^0xFF;
			rec.backColor[3] = AlternateColor.cmyk.k^0xFF;

			rec.foreColor[0] = ActiveColor.cmyk.c^0xFF;
			rec.foreColor[1] = ActiveColor.cmyk.m^0xFF;
			rec.foreColor[2] = ActiveColor.cmyk.y^0xFF;
			rec.foreColor[3] = ActiveColor.cmyk.k^0xFF;
			rec.planes       = 4;
		break;
	}

 	rec.inLoPlane   = 0;
	rec.inHiPlane   = rec.planes-1;
	rec.outLoPlane  = 0;
	rec.outHiPlane  = rec.planes-1;

	rec.imageHRes  = ( long )FrameResolution( lpSrcFrame ) << 16;
	rec.imageVRes  = ( long )FrameResolution( lpSrcFrame ) << 16;

	AdobePlatform.hwnd = ( unsigned short )PictPubApp.Get_hWndAstral();

	rec.platformData = ( void * )&AdobePlatform;

	rec.monitor.gamma   = 65536L;
	rec.monitor.redX    = 40960L;
	rec.monitor.redY    = 22282L;
	rec.monitor.greenX  = 18350L;
	rec.monitor.greenY  = 38993L;
	rec.monitor.blueX   = 10158L;
	rec.monitor.blueY   = 4587L;
	rec.monitor.whiteX  = 20493L;
	rec.monitor.whiteY  = 21561L;
	rec.monitor.ambient = 32768L;

	// Make sure the aborted flag is set
	bAdobeAborted = FALSE;

	lpData = &lpPlugin->lData;

	if( !rec.parameters )
	{
		// Call the filterSelectorParameters function
		( *lpEntryPoint )( 
			( short )filterSelectorParameters,
			( FilterRecord FAR * ) &rec,
			( long FAR * )lpData,
			( short FAR * )&retcode );

		if( retcode != 0 )
		{
			bAdobeAborted = TRUE;
			goto BadStart;
		}
	}

	// Call the filterSelectorPrepare function
	( *lpEntryPoint )( 
		( short )filterSelectorPrepare,
		( FilterRecord FAR * ) &rec,
		( long FAR * )lpData,
		( short FAR * )&retcode );

	if( retcode != 0 )
	{
		bAdobeAborted = TRUE;
		goto BadStart;
	}

	// Call the filterSelectorStart function
	( *lpEntryPoint )( 
		( short )filterSelectorStart,
		( FilterRecord FAR * ) &rec,
		( long FAR * )lpData,
		( short FAR * )&retcode );

	if( retcode != 0 )
	{
		bAdobeAborted = TRUE;
		goto BadStart;
	}
	iDepth = FrameDepth( lpSrcFrame );

	if( iDepth == 0 ) 
        iDepth = 1;

	ProgressBegin( 1, 0 );

	// init buffer sizes to zero
	lInSize = lOutSize = lMaskSize = 0;

	// save top and left
	top  = lpObject->rObject.top;
	left = lpObject->rObject.left;

	// get initial heights of buffers
	inHeight = rec.inRect.bottom - rec.inRect.top;
	outHeight = rec.outRect.bottom - rec.outRect.top;
	maskHeight = rec.maskRect.bottom - rec.maskRect.top;

	// zap current rectangle states
	inRect.top = inRect.bottom = inRect.left = inRect.right = 0;
	outRect.top = outRect.bottom = outRect.left = outRect.right = 0;
	maskRect.top = maskRect.bottom = maskRect.left = maskRect.right = 0;

	// initialize to impossible values
	inLoPlane = inHiPlane = SHRT_MAX;
	outLoPlane = outHiPlane = SHRT_MAX;

	// keep going until all three rects are empty
	while( 	!bAdobeAborted && ( inHeight || outHeight ))
	{
		// plugin wants input data that's different from last data?
		if( inHeight &&
			( NOTEQUAL( rec.inRect, inRect ) ||
			  inLoPlane != rec.inLoPlane ||
			  inHiPlane != rec.inHiPlane ) )
		{
			// check to make sure buffer has not gotten bigger
			// I don't know whether this is necessary but
			// better safe than sorry
			rec.inRowBytes = rec.inRect.right-rec.inRect.left;
			if( rec.inHiPlane != rec.inLoPlane )
				rec.inRowBytes *= rec.planes;
			lSize = rec.inRowBytes * inHeight;
			if( lSize > lInSize )
			{
				if( lpInBuffer )
					FreeUp( lpInBuffer );
				lpInBuffer = Alloc( lSize );
				if( !lpInBuffer )
				{
					bAdobeAborted = TRUE;
					goto Done;
				}
				lInSize = lSize;
			}
			// actually read the data in
			inRect = rec.inRect;
			inLoPlane = rec.inLoPlane;
			inHiPlane = rec.inHiPlane;

			ReadBuffer( lpSrcFrame, lpInBuffer, rec.inRect, rec.inRowBytes,
						rec.inLoPlane, rec.inHiPlane, bInvert, iDepth );
	 		rec.inData  = ( void * )lpInBuffer;
		}

		// plugin wants output data that's different from last data?
		if( outHeight &&
			( NOTEQUAL( rec.outRect, outRect ) ||
			  outLoPlane != rec.outLoPlane ||
			  outHiPlane != rec.outHiPlane) )
		{
			// check to make sure buffer has not gotten bigger
			rec.outRowBytes = rec.outRect.right-rec.outRect.left;
			if( rec.outHiPlane != rec.outLoPlane )
				rec.outRowBytes  *= rec.planes;
			lSize = rec.outRowBytes * outHeight;
			if( lSize > lOutSize )
			{
				if( lpOutBuffer )
					FreeUp( lpOutBuffer );
				lpOutBuffer = Alloc( lSize );
				if( !lpOutBuffer )
				{
					bAdobeAborted = TRUE;
					goto Done;
				}
				lOutSize = lSize;
			}
			// now save values that might change in read in the data
			outRect = rec.outRect;
			outLoPlane = rec.outLoPlane;
			outHiPlane = rec.outHiPlane;
			outRowBytes = rec.outRowBytes;
			ReadBuffer( lpDstFrame, lpOutBuffer, rec.outRect, rec.outRowBytes,
						rec.outLoPlane, rec.outHiPlane, bInvert, iDepth );
			rec.outData = ( void * )lpOutBuffer;
		}

		// we have a mask and the
		// plugin wants mask data that's different from last data?
		if( bMask && maskHeight && NOTEQUAL( rec.maskRect, maskRect ))
		{
			// check to make sure buffer has not gotten bigger
			rec.maskRowBytes = rec.maskRect.right-rec.maskRect.left;
			lSize = rec.maskRowBytes * maskHeight;
			if( lSize > lMaskSize )
			{
				if( lpMaskBuffer )
					FreeUp( lpMaskBuffer );
				lpMaskBuffer = Alloc( lSize );
				if( !lpMaskBuffer )
				{
					bAdobeAborted = TRUE;
					goto Done;
				}
				lMaskSize = lSize;
			}
			// now save values that might change in read in the data
			maskRect = maskImageRect = rec.maskRect;
			maskImageRect.top += top;
			maskImageRect.bottom += top;
			maskImageRect.left += left;
			maskImageRect.right += left;
			ReadBuffer( lpMskFrame, lpMaskBuffer, maskImageRect, rec.maskRowBytes,
						0, 0, NO, 1 );
			rec.maskData = ( void * )lpMaskBuffer;
		}

		// Call the filterSelectorContinue function
		( *lpEntryPoint )( 
			( short )filterSelectorContinue,
			( FilterRecord FAR * ) &rec,
		    ( long FAR * )lpData,
			( short FAR * )&retcode );

		if( retcode != 0 )
		{
			bAdobeAborted = TRUE;
			goto Done;
		}

		// write out data that the plugin processed
		if( outHeight )
		{
            WriteBuffer( lpDstFrame, lpOutBuffer, outRect, outRowBytes,
  						 outLoPlane, outHiPlane, bInvert, iDepth );
			rOut.left = outRect.left;
			rOut.right = outRect.right-1;
			rOut.top = outRect.top;
			rOut.bottom = outRect.bottom-1;
			AstralUnionRect(&rEdits, &rEdits, &rOut);
		}

		// get heights of buffers that plugin has requested
		inHeight = rec.inRect.bottom - rec.inRect.top;
		outHeight = rec.outRect.bottom - rec.outRect.top;
		if( bMask )
			maskHeight = rec.maskRect.bottom - rec.maskRect.top;
	}
Done:
	
    ProgressEnd();

	// Call the filterSelectorFinish function
	( *lpEntryPoint )( 
		( short )filterSelectorFinish,
		( FilterRecord FAR * ) &rec,
		( long FAR * )lpData,
		( short FAR * )&retcode );

BadStart:
	// Unload the DLL
	if( hModule )
		FreeLibrary( hModule );

	// Clean up our proc's
	if( lpAbortProc )
		FreeProcInstance(( FARPROC )lpAbortProc );
	if( lpProgressProc )
		FreeProcInstance(( FARPROC )lpProgressProc );

	// Clean up our buffers
	if( lpInBuffer )
		FreeUp( lpInBuffer );
	if( lpOutBuffer )
		FreeUp( lpOutBuffer );
	if( lpMaskBuffer )
		FreeUp( lpMaskBuffer );
	if( lpDispBuffer )
		FreeUp( lpDispBuffer );

	// If the effect was UNsuccessful or it didn't do anything
	if( !fStarted || retcode != 0 || bAdobeAborted || AstralIsRectEmpty(&rEdits))
	{
		if( lpDstFrame )
			FrameClose( lpDstFrame );
		return( FALSE );
	}

	// The effect was successful so Copy the pixels outside 
	// the masked area back in
	if( bMask && rec.autoMask )
	{
		RefreshMaskedBits( lpSrcFrame, lpDstFrame, lpMask,
						lpObject->rObject.left,
						lpObject->rObject.top );
	}

	// Activate the new frame
	ImgEditedObjectFrame( 
		lpImage,
		lpObject,
		IDS_UNDOSPECIALFILTER,
		&rEdits,
		lpDstFrame,
		NULL );

	*lpUpdateRect = rEdits;
	*parameters = rec.parameters;

	return( TRUE );
}

#ifdef UNUSED
	if( rec.parameters )
	{
		HGLOBAL hMem;
		LPTR lpParams = NULL;
		
		fHandle = NO;
		if( HIWORD( rec.parameters ) == 0 ) // a handle?
		{
			hMem = ( HGLOBAL )rec.parameters;
			dwSize = GlobalSize( hMem );
			if( dwSize )
			{
				lpParams = ( LPTR )GlobalLock( hMem );
				fHandle = YES;
			}
		}
		else
		if( !IsBadReadPtr( rec.parameters, 1 ))
		{
			hMem = GlobalPtrHandle( rec.parameters );
			if( hMem )
			{
				dwSize = GlobalSize( hMem );
				lpParams = ( LPTR )rec.parameters;
			}
		}
		if( lpParams )
		{
			FNAME szFile;
			FILE_HANDLE fh;
			
			lstrcpy( szFile, Control.ProgHome );
			lstrcat( szFile, "PLUGIN.PRM" );
			fh = FileOpen( szFile,FO_CREATE|FO_WRITE );
			if( fh != FILE_HANDLE_INVALID )
			{
				FileWrite( fh, lpParams, dwSize );
				FileClose( fh );
			}
			if( fHandle )
				GlobalUnlockPtr( lpParams );
		}
	}
#endif

/*=========================================================================*/
LOCAL void ReadBuffer( LPFRAME lpFrame, LPTR lpBuffer, Rect Rect, long lRowBytes,
					    short LoPlane, short HiPlane, BOOL bInvert, int iDepth )
{
	HPTR hp;
	LPTR lp;
	int y;
	long lCount;

	hp = lpBuffer;
	if( LoPlane != HiPlane )
	{
		for( y=Rect.top; y < Rect.bottom; y++ )
		{
			lp  = FramePointer( lpFrame, Rect.left, y, NO );
			lCount = lRowBytes;
#ifdef WIN32
			if( bInvert )
			{
				while( lCount-- )
					*hp++ = ( *lp++ )^0xFF;
			}
			else
			{
				while( lCount-- )
					*hp++ = *lp++;
			}
#else
			CopyToHuge( lp, hp, lCount, bInvert, 1 );
		   	hp += lCount;
#endif
		}
	}
	else
	{
		for( y=Rect.top; y < Rect.bottom; y++ )
		{
			lp = FramePointer( lpFrame, Rect.left, y, NO );
			lCount = lRowBytes;

			// Get the proper plane offset
			lp += LoPlane;
#ifdef WIN32
			if( bInvert )
			{
				while( lCount-- )
				{
					*hp++ = ( *lp )^0xFF;
					lp+= iDepth;
				}
			}
			else
			{
				while( lCount-- )
				{
					*hp++ = *lp;
					lp+= iDepth;
				}
			}
#else
			CopyToHuge( lp, hp, lCount, bInvert, iDepth );
		   	hp += lCount;
#endif
		}
	}
}

/*=========================================================================*/
LOCAL void WriteBuffer( LPFRAME lpFrame, LPTR lpBuffer, Rect Rect, long lRowBytes,
					short LoPlane, short HiPlane, BOOL bInvert, int iDepth )
{
	HPTR hp, hpBuffer;
	LPTR lp;
	int y;
	long lCount;

	hpBuffer = lpBuffer;

	if( LoPlane != HiPlane )
	{
		for( y=Rect.top; y < Rect.bottom; y++ )
		{
			hp = hpBuffer;
			hpBuffer += lRowBytes;
			lp = FramePointer( lpFrame, Rect.left, y, YES );
			lCount = Rect.right - Rect.left;
			lCount *= iDepth;
#ifdef WIN32
			if( bInvert )
			{
				while( lCount-- )
					*lp++ = ( *hp++ )^0xFF;
			}
			 else
			{
				while( lCount-- )
					*lp++ = *hp++;
			}
#else
			CopyFromHuge( hp, lp, lCount, bInvert, 1 );
			hp += lCount;
#endif
		}
	}
	else
	{
		for( y=Rect.top; y < Rect.bottom; y++ )
		{
			hp = hpBuffer;
			hpBuffer += lRowBytes;
			lp = FramePointer( lpFrame, Rect.left, y, YES );
			lCount = Rect.right - Rect.left;

			lp += LoPlane;
#ifdef WIN32
			if( bInvert )
			{
				while( lCount-- )
				{
					*lp = ( *hp++ )^0xFF;
					lp += iDepth;
				}
			}
			else
			{
				while( lCount-- )
				{
					*lp = *hp++;
					lp += iDepth;
				}
			}
#else
			CopyFromHuge( hp, lp, lCount, bInvert, iDepth );
			hp += lCount;
#endif
		}
	}
}

#ifndef WIN32
/*=========================================================================*/

LOCAL void CopyToHuge( 
	LPTR	lpSrc,
	HPTR	hpDst,
	long	lCount,
	BOOL 	bInvert,
	int 	iDepth )
{
	long lLeft;

	// get current position in segment
	while( lCount )
	{
		// get how many bytes left in segment
		lLeft = 65536L - (( DWORD )hpDst & 0xFFFFL );
		if( lLeft > 32767 )
			lLeft = 32767; // never try to copy more than maxint bytes
		if( lLeft > lCount )
			lLeft = lCount;

 		if( iDepth == 1 )
 		{
 			if( bInvert )
 				not( lpSrc, ( LPTR )hpDst, ( int )lLeft );
 			else
 				copy( lpSrc, ( LPTR )hpDst, ( int )lLeft );
	 		lpSrc += lLeft;
 		}
 		else
 		{
 			if( bInvert )
 				notfromplane( lpSrc, ( LPTR )hpDst, ( int )lLeft, iDepth );
 			else
 				copyfromplane( lpSrc, ( LPTR )hpDst, ( int )lLeft, iDepth );

	 		lpSrc += ( lLeft * iDepth );
 		}
 		lCount -= lLeft;
	    hpDst += lLeft;
	}
}

/*=========================================================================*/

LOCAL void CopyFromHuge( 
	HPTR	hpSrc,
	LPTR	lpDst,
	long	lCount,
	BOOL 	bInvert,
	int 	iDepth )
{
	long lLeft;

	while( lCount )
	{
		// get how many bytes left in segment
		lLeft = 65536L - (( DWORD )hpSrc & 0xFFFFL );
		// see if we will cross a segment boundary
		if( lLeft > 32767 )
			lLeft = 32767; // never try to copy more than maxint bytes
		if( lLeft > lCount )
			lLeft = lCount;

		if( iDepth == 1 )
		{
			if( bInvert )
				not(( LPTR )hpSrc, lpDst, ( int )lLeft );
			else
				copy(( LPTR )hpSrc, lpDst, ( int )lLeft );
			lpDst += lLeft;
		}
		else
		{
			if( bInvert )
				nottoplane(( LPTR )hpSrc, lpDst, ( int )lLeft, iDepth );
			else
				copytoplane(( LPTR )hpSrc, lpDst, ( int )lLeft, iDepth );
			lpDst += ( lLeft * iDepth );
		}
		lCount -= lLeft;
   	    hpSrc += lLeft;
	}
}

/*=========================================================================*/

LOCAL void copyfromplane( LPTR lpSrc, LPTR lpDst, int iCount, int iDepth )
{
#ifdef C_CODE
while( --iCount >= 0 )
	{
	*lpDst++ = *lpSrc;
	lpSrc += iDepth;
	}
#else
__asm   {
	push	ds
	les     di, lpDst		; setup destination
	lds		si, lpSrc		; setup source
	mov     cx, iCount		; get count
	or		cx,cx			; any bytes
	je		Done			; nope?
	mov		bx, iDepth		; get plane depth
	cld
CP_LOOP:
	mov		al,ds:[si]		; get source
	add		si,bx
	stosb					; store dest
	loop	CP_LOOP
Done:
	pop		ds
	}
#endif
}

/*=========================================================================*/

LOCAL void notfromplane( LPTR lpSrc, LPTR lpDst, int iCount, int iDepth )
{
#ifdef C_CODE
while( --iCount >= 0 )
	{
	*lpDst++ = ~( *lpSrc ); // beware the squiggle in XyWrite
	lpSrc += iDepth;
	}
#else
__asm   {
	push	ds
	les     di, lpDst		; setup destination
	lds		si, lpSrc		; setup source
	mov     cx, iCount		; get count
	or		cx,cx			; any bytes to move?
	je		Done			; no bytes to move
	mov		bx, iDepth		; get plane offset
	cld
NP_LOOP:
	mov		al,ds:[si]		; get source
	add		si,bx
	not		al				; invert byte
	stosb					; store dest
	loop	NP_LOOP
Done:
	pop		ds
	}
#endif
}

/*=========================================================================*/

LOCAL void copytoplane( LPTR lpSrc, LPTR lpDst, int iCount, int iDepth )
{
#ifdef C_CODE
while( --iCount >= 0 )
	{
	*lpDst = *lpSrc++;
	lpDst += iDepth;
	}
#else
__asm   {
	push	ds
	les     di, lpDst		; setup destination
	lds		si, lpSrc		; setup source
	mov     cx, iCount		; get count
	or		cx,cx			; any bytes
	je		Done			; nope?
	mov		bx, iDepth		; get plane depth
	cld
CP_LOOP:
	lodsb
	mov		es:[di],al		; set destination
	add		di,bx
	loop	CP_LOOP
Done:
	pop		ds
	}
#endif
}

/*=========================================================================*/

LOCAL void nottoplane( LPTR lpSrc, LPTR lpDst, int iCount, int iDepth )
{
#ifdef C_CODE
while( --iCount >= 0 )
	{
	*lpDst = ~( *lpSrc++ ); // beware the squiggle in XyWrite
	lpDst += iDepth;
	}
#else
__asm   {
	push	ds
	les     di, lpDst		; setup destination
	lds		si, lpSrc		; setup source
	mov     cx, iCount		; get count
	or		cx,cx			; any bytes to move?
	je		Done			; no bytes to move
	mov		bx, iDepth		; get plane offset
	cld
NP_LOOP:
	lodsb
	not		al				; invert byte
	mov		es:[di],al		; set dest
	add		di,bx
	loop	NP_LOOP
Done:
	pop		ds
	}
#endif
}
#endif

/*=========================================================================*/

LOCAL long HandleAdobeExportCommand( LPIMAGE lpImage, LPADOBEPLUGIN lpPlugin )
{
	LPFRAME lpSrcFrame;
	HMODULE hModule;
	STRING  szEntryPoint;
	short   retcode = 1;
	BOOL    bInvert = FALSE;

	FRMDATATYPE     type;
	ExportRecord    AdobeRecord;
	PlatformData    AdobePlatform;
	TestAbortProc   lpAbortProc;
	ProgressProc    lpProgressProc;
	HostProc        lpHostProc;

	LPEXPORTENTRYPROC lpEntryPoint;

	BOOL bDone;
	LPTR lpSrc, lpBuffer, lpLine;
	HPTR hpSrc;
	int  y, iCount, iDepth, rowBytes, dx;
	long lData;
	RECT rMask;
	HWND hFocus;

	lpBuffer        = NULL;
	lpLine		    = NULL;
	lpAbortProc     = NULL;
	lpProgressProc  = NULL;
	lpHostProc      = NULL;
	lData = 0;
	hFocus = NULL;

	if( !lpImage )
		return( NO );

	// Load the effect DLL
	if( !( hModule = LoadLibrary( lpPlugin->szFileName )))
		return( NO );

	hFocus = GetFocus();

	lpAbortProc    = ( TestAbortProc )MakeProcInstance(( FARPROC )AdobeTestAbort,    PictPubApp.Get_hInstAstral());
	lpProgressProc = ( ProgressProc )MakeProcInstance(( FARPROC )AdobeProgressProc, PictPubApp.Get_hInstAstral());
//	lpHostProc     = ( HostProc )MakeProcInstance(( FARPROC )AdobeHostProc, PictPubApp.Get_hInstAstral());

	if( !lpAbortProc || !lpProgressProc )
		goto BadStart;

	// Get the entry point function label
	wsprintf( szEntryPoint, "ENTRYPOINT%d", lpPlugin->iResourceNum );

	// Get the entry point into the DLL
	if( !( lpEntryPoint = ( LPEXPORTENTRYPROC )GetProcAddress( hModule, szEntryPoint )))
		goto BadStart;

	lpSrcFrame = ImgGetBaseEditFrame( lpImage );

	iDepth = FrameDepth( lpSrcFrame );
	if( iDepth == 0 ) 
        iDepth = 1;

	if( !( lpLine = Alloc( FrameXSize( lpSrcFrame )*iDepth )))
	{
		Message( IDS_EMEMALLOC );
		goto BadStart;
	}

	// If we have to swap to Adobe CMYK...
	type = FrameType( lpSrcFrame );
	if( type == FDT_CMYKCOLOR )
		bInvert = TRUE;

	// Set the FilterRecord Info
	clr(( LPTR )&AdobeRecord, sizeof( ExportRecord ));

	AdobeRecord.serialNumber = 0;
	AdobeRecord.abortProc    = lpAbortProc;
	AdobeRecord.progressProc = lpProgressProc;
	AdobeRecord.maxData = AvailableMemory() - ( 1024L * Control.MainMemMin );

	if( type == FDT_LINEART )
	{
		AdobeRecord.imageMode = plugInModeBitmap;
		AdobeRecord.depth = 1;
		AdobeRecord.planes = 1;
	}
	else
	if( type == FDT_GRAYSCALE )
	{
		AdobeRecord.imageMode = plugInModeGrayScale;
		AdobeRecord.depth = 8;
		AdobeRecord.planes = 1;
	}
	else
	if( type == FDT_RGBCOLOR )
	{
		AdobeRecord.imageMode = plugInModeRGBColor;
		AdobeRecord.depth = 8;
		AdobeRecord.planes = 3;
	}
	else
	if( type == FDT_CMYKCOLOR )
	{
		AdobeRecord.imageMode = plugInModeCMYKColor;
		AdobeRecord.depth = 8;
		AdobeRecord.planes = 4;
	}

	AdobeRecord.imageSize.v = FrameYSize( lpSrcFrame );
	AdobeRecord.imageSize.h = FrameXSize( lpSrcFrame );
	AdobeRecord.imageHRes = TOFIXED(FrameResolution( lpSrcFrame ));
	AdobeRecord.imageVRes = TOFIXED(FrameResolution( lpSrcFrame ));

//	AdobeRecord.redLUT = NULL;
//	AdobeRecord.greenLUT = NULL;
//	AdobeRecord.blueLUT = NULL;
    
	AdobeRecord.theRect.top = 0;
	AdobeRecord.theRect.left = 0;
	AdobeRecord.theRect.bottom = 0;
	AdobeRecord.theRect.right = 0;

	AdobeRecord.loPlane = 0;
	AdobeRecord.hiPlane   = AdobeRecord.planes - 1;

	AdobeRecord.data = NULL;

	AdobeRecord.rowBytes = FrameByteWidth( lpSrcFrame );
	lstrcpy(( LPSTR )&AdobeRecord.filename[1], lpImage->CurFile );
	AdobeRecord.filename[0] = lstrlen( lpImage->CurFile );

	AdobeRecord.vRefNum = 0;
	AdobeRecord.dirty = TRUE;

	if( ImgGetMaskRect( lpImage, &rMask ))
	{
		AdobeRecord.selectBBox.top = rMask.top;
	 	AdobeRecord.selectBBox.left = rMask.left;
	 	AdobeRecord.selectBBox.bottom = rMask.bottom+1;
	 	AdobeRecord.selectBBox.right = rMask.right+1;
	}
	else
	{
		AdobeRecord.selectBBox.top = 0;
	 	AdobeRecord.selectBBox.left = 0;
	 	AdobeRecord.selectBBox.bottom = 0;
	 	AdobeRecord.selectBBox.right = 0;
	}

	AdobeRecord.hostSig =
		( long )'P' << 24 | ( long )'P' << 16 | ( long )'U' << 8 | ( long )'B';

	AdobeRecord.hostProc    = NULL;

	AdobeRecord.duotoneInfo = NULL;
	AdobeRecord.thePlane    = 0;

	AdobeRecord.monitor.gamma   = 65536L;
	AdobeRecord.monitor.redX    = 40960L;
	AdobeRecord.monitor.redY    = 22282L;
	AdobeRecord.monitor.greenX  = 18350L;
	AdobeRecord.monitor.greenY  = 38993L;
	AdobeRecord.monitor.blueX   = 10158L;
	AdobeRecord.monitor.blueY   = 4587L;
	AdobeRecord.monitor.whiteX  = 20493L;
	AdobeRecord.monitor.whiteY  = 21561L;
	AdobeRecord.monitor.ambient = 32768L;

	AdobePlatform.hwnd = ( unsigned short )PictPubApp.Get_hWndAstral();
	AdobeRecord.platformData = ( void * )&AdobePlatform;

	// Make sure the aborted flag is set
	bAdobeAborted = FALSE;

	// Call the exportSelectorPrepare function
	( *lpEntryPoint )( 
		( short )exportSelectorPrepare,
		( ExportRecord FAR * ) &AdobeRecord,
		( long FAR * )&lData,
		( short FAR * )&retcode );

	if( retcode != 0 )
	{
		bAdobeAborted = TRUE;
		goto BadStart;
	}

	// Call the exportSelectorStart function
	( *lpEntryPoint )( 
		( short )exportSelectorStart,
		( ExportRecord FAR * ) &AdobeRecord,
		( long FAR * )&lData,
		( short FAR * )&retcode );

	if( retcode != 0 )
	{
		bAdobeAborted = TRUE;
		goto BadStart;
	}

	if( AdobeRecord.hiPlane == AdobeRecord.loPlane )
	{
		AdobeRecord.rowBytes = FrameXSize( lpSrcFrame );
	}
	else
	{
		AdobeRecord.rowBytes  = FrameByteWidth( lpSrcFrame );
	}

	// inRect and outRect should now be valid...
	// so allocate the memory block required for the operation.
	lpBuffer  = Alloc(( long )AdobeRecord.rowBytes  *
                	  ( AdobeRecord.theRect.bottom  - AdobeRecord.theRect.top  + 1L ));

	if( !lpBuffer )
	{
		bAdobeAborted = TRUE;
		goto Done;
	}

	// start the processing of the data
	bDone = FALSE;

	ProgressBegin( 1, 0 );

	while( !bDone && !bAdobeAborted )
	{
		hpSrc = lpBuffer;

		rowBytes  = AdobeRecord.rowBytes;
		dx = AdobeRecord.theRect.right - AdobeRecord.theRect.left;
											  
		if( AdobeRecord.loPlane != AdobeRecord.hiPlane )
		{
			for( y=AdobeRecord.theRect.top; y < AdobeRecord.theRect.bottom; y++ )
			{
				ImgGetLine( lpImage, NULL, AdobeRecord.theRect.left, y, dx, lpLine );
				lpSrc = lpLine;
				iCount = rowBytes;

				if( bInvert )
				{
					while( iCount-- )
						*hpSrc++ = ( *lpSrc++ )^0xFF;
				}
				else
				{
					while( iCount-- )
						*hpSrc++ = *lpSrc++;
				}
			}
		}
		else
		{
			for( y = AdobeRecord.theRect.top; y < AdobeRecord.theRect.bottom; y++ )
			{
				ImgGetLine( lpImage, NULL, AdobeRecord.theRect.left, y, dx, lpLine );
				lpSrc = lpLine;
				iCount = rowBytes;

				// Get the proper plane offset
				lpSrc += AdobeRecord.loPlane;

				if( bInvert )
				{
					while( iCount-- )
					{
						*hpSrc++ = ( *lpSrc )^0xFF;
						lpSrc+= iDepth;
					}
				}
				else
				{
					while( iCount-- )
					{
						*hpSrc++ = *lpSrc;
						lpSrc+= iDepth;
					}
				}
			}
		}

		AdobeRecord.data  = ( void * )lpBuffer;

		// Call the exportSelectorContinue function
		( *lpEntryPoint )( 
			( short )exportSelectorContinue,
			( ExportRecord FAR * ) &AdobeRecord,
			( long FAR * )&lData,
			( short FAR * )&retcode );

		if( AdobeRecord.theRect.top  == AdobeRecord.theRect.bottom )
		{
			bDone = TRUE;
		}
	}
	ProgressEnd();

Done:
	// Call the filterSelectorFinish function
	( *lpEntryPoint )( 
		( short )exportSelectorFinish,
		( ExportRecord FAR * ) &AdobeRecord,
		( long FAR * )&lData,
		( short FAR * )&retcode );

BadStart:
	// Unload the DLL
	FreeLibrary( hModule );

	// Clean up our proc's
	if( lpAbortProc )
		FreeProcInstance(( FARPROC )lpAbortProc );

	if( lpProgressProc )
		FreeProcInstance(( FARPROC )lpProgressProc );

	if( lpHostProc )
		FreeProcInstance(( FARPROC )lpHostProc );

	// Clean up our buffers
	if( lpBuffer )
		FreeUp( lpBuffer );
	if( lpLine )
		FreeUp( lpLine );

	if( hFocus )
		SetFocus( hFocus );

	// If the effect was UNsuccessful
	if( retcode != 0 || bAdobeAborted )
	{
		return( FALSE );
	}
	return( bDone );
}

/*=========================================================================*/

LOCAL void RefreshMaskedBits( 
	LPFRAME lpSrcFrame,
	LPFRAME lpDstFrame,
	LPMASK  lpMask,
	int xOff,
	int yOff )
{
	LPTR lpMaskBuf, lpSrc, lpDst;
	int SrcDepth, DstDepth;
	int y, dx, dy;
	FRMTYPEINFO TypeInfo;
	LPPROCESSPROC lpProcessProc;

	if( !( lpMaskBuf = Alloc(( long )FrameXSize( lpDstFrame ))))
		return;

	MessageStatus( IDS_PLEASEWAIT );

	// get info about source and dest frames
	SrcDepth = FrameDepth( lpSrcFrame );
	DstDepth = FrameDepth( lpDstFrame );
	dx = FrameXSize( lpDstFrame );
	dy = FrameYSize( lpDstFrame );
	FrameGetTypeInfo(lpSrcFrame, &TypeInfo);

	lpProcessProc = GetProcessProc( MM_NORMAL, TypeInfo.DataType );

	// copy source data to destination data using inverted mask
	// because the destination is really the source when using
	// the mask stuff.  Since the destination data is the newly
	// created data, it should be merged in with the source, however
	// we want to return a new destination frame so the process
	// must work the other way.

	for ( y = 0; y < dy; y++ )
	{
		if( !( lpDst = FramePointer( lpDstFrame, 0, y, YES )))
			continue;
		if( !( lpSrc = FramePointer( lpSrcFrame, 0, y, NO )))
			continue;

		clr( lpMaskBuf, dx );

		// setup mask based on src
		mload( 0+xOff, y+yOff, dx, 1, lpMaskBuf, lpSrc, TypeInfo, lpMask );

		// invert mask
		negate( lpMaskBuf,dx );

		// write the src onto the dst with inverted mask
		( *lpProcessProc )( lpDst, lpSrc, lpMaskBuf, dx );

	}
	FreeUp( lpMaskBuf );

	MessageStatus( NULL );
}


/*=========================================================================*/

#define signatureSize	4
static char cSig[signatureSize] = {'O', 'T', 'O', 'F'};

static Boolean memError;


Handle NewHandle (long size)
{
	Handle mHand;
	char *p;

	memError = noErr;

	mHand =	(Handle) GlobalAllocPtr (GHND, (sizeof (Handle) + signatureSize));

	if (mHand)
		*mHand = (Ptr) GlobalAllocPtr (GHND, size);

	if (!mHand || !(*mHand))
		{
		memError = memFullErr;
		return NULL;
		}

	// put the signature after the pointer
	p = (char *) mHand;
	p += sizeof (Handle);
	memcpy (p,cSig, signatureSize);

	return mHand;
		
}

long GetHandleSize(Handle handle)
{
	Ptr p;
	HANDLE h;

	memError = noErr;

	if (handle)
		{
		p = *handle;

		h = GlobalPtrHandle (p);

		if (h)
			{
			long lSize = GlobalSize(h);
			// special KPT hack because they don't use these
			// routines to allocate handles for all filters
			// i.e. - Gradient Designer
			// they do a single alloc and stick the a pointer offset
			// 4 from the beginning of the handle
			// subtract 4(ptr size) to get real size of data
			if (HIWORD(handle) == HIWORD(p)) // see if same segment
				lSize -= 4;
			return(lSize);
			}
		}

	memError = nilHandleErr;

	return 0L;

}

void DisposHandle (Handle handle)
{

	memError = noErr;

	if (handle)
		{
		Ptr p;

		p = *handle;

		// special KPT hack - see NewHandle() for more details
		if (HIWORD(handle) != HIWORD(p))
		{
			if (p)
				GlobalFreePtr (p);
		}

		GlobalFreePtr ((Ptr)handle);
		}

	else

		memError = nilHandleErr;
}

LPTR PS_NewHandle(DWORD dwSize)
{
	return((LPTR)NewHandle((long)dwSize));
}

void PS_DisposHandle(LPTR lp)
{
	DisposHandle((Handle)lp);
}

DWORD PS_GetHandleSize(LPTR lp)
{
	return((DWORD)GetHandleSize((Handle)lp));
}



