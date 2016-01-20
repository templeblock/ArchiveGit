//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include <mmsystem.h>
#include "id.h"
#include "data.h"
#include "animate.h"
#include "routines.h"

static LPAOBJECT BatchSession; // The session handle
static int hAObject; // The object handle
static BOOL bAnimate;
static void FAR PASCAL PlayAnimationSound(int hAObject, LPSTR lpSound,
  BOOL bLoopSound, int iChannel);

// Resource ID's for animation Toy files
#define STR_LOOP_SOUND      1000
#define STR_NUM_BITMAPS     1001
#define STR_HAS_CHILDREN    1002
#define STR_PLAY_SOUND      1003
#define STR_ANIMATE_SOUND	1004
#define STR_DESCRIPTION     1010

#define TIFF_RES_FORMAT     "TIF_%i%c"      // TIF_1a, TIF_1b, TIF_1z ...
#define WAVE_RES_FORMAT     "WAV_%i%c"
#define OFFS_RES_FORMAT     "OFF_%i"

#define TIFF_RES_TYPE       "TIFF"
#define OFFS_RES_TYPE       "OFFSETS"
#define SCPT_RES_TYPE       "SCRIPT"


/************************************************************************/
BOOL AnimateIsOn( void )
/************************************************************************/
{
return( bAnimate );
}


/************************************************************************/
void LoadToyImageOffsets(LPAOBJECT Asession, int hAObject, int nToy, HINSTANCE hToy)
/************************************************************************/
{
// Load the frame offsets
    char        szRes[10];
    HRSRC       hGlobal;
    LPTR        lpGlobal;
    LPPOINTS    lpPoints;	// SHORT version of point "Win16 version"
    short       nPoints;

    if (!hAObject) return;
    wsprintf(szRes, OFFS_RES_FORMAT, nToy);
	
#ifdef _MAC
	hGlobal = (HRSRC)LoadMacResource(hToy, szRes, OFFS_RES_TYPE);
	if (!hGlobal) return;
	lpGlobal = *((LPTR *)hGlobal);	// dereference the Mac loacked handle
	if (!lpGlobal)
	{
		ReleaseMacResource((HANDLE)hGlobal);
		return;
	}	

    nPoints = *(short far *)lpGlobal;
    if (nPoints)
    {
		int i;
		short iVal;
        lpPoints = (LPPOINTS)(lpGlobal+2);
		
		// Swap all the x and y values.
		// The Macintosh POINT stucture is opposite of Windows!
		for (i=0; i<nPoints; i++)
		{
			iVal = lpPoints[i].x;
			lpPoints[i].x = lpPoints[i].y;
			lpPoints[i].y = iVal;
		}
        ASetAllFrameOffsets(Asession, hAObject, lpPoints, nPoints);
    }

	ReleaseMacResource(hGlobal);
#else // _MAC

	// Windows version	
    if ( !(hGlobal = FindResource(hToy, szRes, OFFS_RES_TYPE)) )
		return;
    if ( !(hGlobal = (HRSRC)LoadResource(hToy, hGlobal)) )
		return;
	if ( !(lpGlobal = (LPTR)LockResource( hGlobal )) )
	{
		FreeResource( hGlobal );
		return;
	}

    nPoints = *(short far *)lpGlobal;
    if (nPoints)
    {
        lpPoints = (LPPOINTS)(lpGlobal+2);
        ASetAllFrameOffsets(Asession, hAObject, lpPoints, nPoints);
    }

	UnlockResource( hGlobal );
    FreeResource( hGlobal );
	
#endif // _MAC
}




int CountChildren(int nToy, HINSTANCE hToy)
{
// Cound the number of children for this object.
// A child is designated by the TIFF resource -> TFI_1a, TIF_11a, TIF_111a ...
    int iChildren = 0;
    int iChild = nToy*10+1;
    char         szRes[10];

#ifdef _MAC
	HANDLE hGlobal;
	do
	{
		wsprintf(szRes, TIFF_RES_FORMAT, iChild, 'A');
		hGlobal = LoadMacResource(hToy, szRes, TIFF_RES_TYPE);
		if (hGlobal)
		{
			ReleaseMacResource(hGlobal);
        	iChild++;
        	iChildren++;
		}
		
	} while (hGlobal);	
#else	
    while (wsprintf(szRes, TIFF_RES_FORMAT, iChild, 'A') &&
            FindResource(hToy, szRes, TIFF_RES_TYPE))
    {
        iChild++;
        iChildren++;
    }
#endif

    return iChildren;
}




int CountBitmaps(int nToy, HINSTANCE hToy)
{
// Cound the number of bitmaps for this object.
// A bitmap is designated by the TIFF resource -> TFI_1a, TIF_1b, TIF_1c ...
    int  iBitmaps = 0;
    char szRes[10];

#ifdef _MAC
	HANDLE hGlobal;
	do
	{
		wsprintf(szRes, TIFF_RES_FORMAT, nToy, iBitmaps+'A');
		hGlobal = LoadMacResource(hToy, szRes, TIFF_RES_TYPE);
		if (hGlobal)
		{
			ReleaseMacResource(hGlobal);
        	iBitmaps++;
		}
		
	} while (hGlobal);	
#else	
    while (wsprintf(szRes, TIFF_RES_FORMAT, nToy, iBitmaps+'A') &&
            FindResource(hToy, szRes, TIFF_RES_TYPE))
    {
        iBitmaps++;
    }
#endif
    return iBitmaps;
}



/************************************************************************/
int LoadToy(LPAOBJECT Asession, int Parent, int nToy, int x, int y, HINSTANCE hToy)
/************************************************************************/
{
/*
This 'Mother of all toy loaders' loads a single toy and all its children.
It is called recursively to load children and grandchildren if any.
Each animation object will get a script and motion offsets set into it.
Simply call ReExecuteScript() with the return value of this function.
Parameters:
    Parent:     - The animation handle returned from ARegisterAobject() or 0.
    nToy:       - 1..9, 11..19, 111..119  - The toy number to load.
    x, y:       - Offset on screen to display
    hToy:       - .toy DLL
*/

    STRING      sz;
    HBITMAP     hImage;
    int         hAObject = 0;
    POINT       pt;
    int         i;
    int         nChildren;
    int         nBitmaps;
	int			iChannel;
    BOOL        bHasChildren;
    char        szScript[1024];
    char        szRes[10];
    BOOL        bLoopSound;
    BOOL        bAnimateSound;

// Prepare for loading
    if (Parent)
    {
        pt.x = 0;
        pt.y = 0;
        nBitmaps  = CountBitmaps(nToy, hToy);
        nChildren = CountChildren(nToy, hToy);
        bHasChildren = nChildren;
    }
    else
    {
        pt.x = x;
        pt.y = y;
        if ( !LoadOSString( hToy, STR_NUM_BITMAPS/*num bitmaps*/, sz, sizeof(sz)-1 ) )
             nBitmaps = 1;
        else nBitmaps = Ascii2Int( sz );

        if ( !LoadOSString( hToy, STR_HAS_CHILDREN/*child objects*/, sz, sizeof(sz)-1 ) )
             bHasChildren = 1;
        else bHasChildren = Ascii2Int( sz );
        nChildren = CountChildren(nToy, hToy);
    }

// Load all bitmap frames for this toy
    for (i=0; i<nBitmaps; i++)
    {
        wsprintf(szRes, TIFF_RES_FORMAT, nToy, i+'A');
    	hImage = TiffResource2Bitmap( hToy, szRes/*id*/ );
    	if (!i)
        {
            if (Parent)
                 hAObject = ARegisterAChild (Asession, Parent, hImage, 0, NULL);
            else hAObject = ARegisterAobject(Asession, hImage, 0, &pt);
        }
    	else
            AAddAobject(Asession, hAObject, hImage, 0);
    	if (hImage) DeleteObject( hImage );
    	if (!hAObject)
    		break;
    }

// Offset each image if they are different sizes
    LoadToyImageOffsets(Asession, hAObject, nToy, hToy);

// Load the script and set it into the object.  It will be executed later.
    if ( !LoadOSString( hToy, nToy/*id for script*/, szScript, sizeof(szScript)-1 ) )
    	szScript[0] = '\0';
    ASetCurrentScript(Asession, hAObject, szScript);


// Load any children if any.
    if (hAObject && bHasChildren)
    {
        for (i=0; i<nChildren; i++)
            LoadToy(Asession, hAObject, nToy*10+1, x, y, hToy);
    }

// Load sounds
    if (!Parent)
    {
        if (!LoadOSString(hToy, STR_LOOP_SOUND, sz, sizeof(sz)-1))
             bLoopSound = FALSE;
        else bLoopSound = Ascii2Int(sz);
        if (!LoadOSString(hToy, STR_ANIMATE_SOUND, sz, sizeof(sz)-1))
             bAnimateSound = FALSE;
        else bAnimateSound = Ascii2Int(sz);
		
        if (LoadOSString(hToy, STR_PLAY_SOUND, sz, sizeof(sz)-1))
        {
#ifdef USEWAVEMIX
			if (Control.bUseWaveMix)
            	iChannel = mixSoundStartResource (sz, bLoopSound, NULL /*hInst*/, -1);
			else
#endif // USEWAVEMIX
          		SoundStartResource(sz, bLoopSound, NULL /*hInst*/);
				
			if (!bAnimateSound || GetDeferedSound())
			{
				ASetPlaySoundProc(Asession, hAObject, &PlayAnimationSound);
				ASetPlaySound(Asession, hAObject, sz, bLoopSound, iChannel);
			}
        }
    }

    return hAObject;
}


void FAR PASCAL PlayAnimationSound(int hAObject, LPSTR lpSound, BOOL bLoopSound, int iChannel)
{
#ifdef USEWAVEMIX
	if (Control.bUseWaveMix)
		mixSoundStartResource (lpSound, bLoopSound, NULL /*hInst*/, iChannel);
	else
#endif // USEWAVEMIX
		SoundStartResource(lpSound, bLoopSound, NULL /*hInst*/);
}

/************************************************************************/
void AnimateToy( HWND hWnd, int x, int y, LPSTR lpToyFileName, BOOL bPressed, HINSTANCE hToy, int hAObject, LPAOBJECT Asession )
/************************************************************************/
{
// Send 0 into hToy and it will load the toy for you.
    BOOL bLoopSound;
    STRING sz;
    int ButtonState;
	MSG msg;

    if (!Asession && !(Asession = ACreateNewAnimator()) )
    	return;

    // Perform a LoadLibrary to get the instance handle
    if ( !hToy && ((hToy = LoadLibrary( lpToyFileName )) < (HINSTANCE)HINSTANCE_ERROR) )
    {
    	ADeleteAnimator( Asession );
    	return;
    }

    if ( !LoadOSString( hToy, STR_LOOP_SOUND/*loop sound bool*/, sz, sizeof(sz)-1 ) )
         bLoopSound = NO;
    else bLoopSound = Ascii2Int( sz );

    // Load this toy and all its children, scripts, etc.
    if (!hAObject)
    	hAObject = LoadToy(Asession, NULL, 1, x, y, hToy);
    AReExecuteScript(Asession, hAObject);

    // Hide the static animation placeholder objects while playing
    AstralUpdateWindow(lpImage->hWnd);

    ACopyBackgroundScreen( Asession, hWnd, NULL/*lpClipRect*/ );
//	SoundStartResource( "WAV_1a", bLoopSound, hToy );
    if (bPressed)
        bPressed = LBUTTON;

    ButtonState = bPressed != 0;

	AstralCursor(IDC_ARROW);
    while ( AWhere( Asession, hAObject, NULL ) && LBUTTON==ButtonState )
	{
    	AMoveAllObjects( Asession );
#ifdef USEWAVEMIX
		if (Control.bUseWaveMix)
		{
    		if (PeekMessage(&msg, NULL, MM_WOM_DONE, MM_WOM_DONE, PM_REMOVE))
    		{	
        		TranslateMessage(&msg);
        		DispatchMessage (&msg);
			}
		}
#endif // USEWAVEMIX
	}

    ADeleteAllObjects( Asession );
    ADeleteAnimator( Asession );
    SoundStop();
#ifdef USEWAVEMIX
	if (Control.bUseWaveMix)
    	mixSoundStopAll();
#endif // USEWAVEMIX
    FreeLibrary( hToy );
}


/************************************************************************/
void CALLBACK EXPORT AnimateDibID (HWND hWnd, LPRECT lpClipRect,
	HINSTANCE hInstance, ITEMID idDib, LPTR lpSession, LPSTR lpScript)
/************************************************************************/
{
STRING sz;
int hAObject; // The object handle
HBITMAP hBitmap;
BOOL bDeleteSession;
LPAOBJECT Asession;

if ( !IsWindowVisible( hWnd ) )
	return;

Asession = (LPAOBJECT)lpSession;
if ( bDeleteSession = !Asession )
	if ( !(Asession = ACreateNewAnimator()) )
		return;

if ( !(hBitmap = DibResource2Bitmap( hInstance, idDib )) )
	{
	if ( bDeleteSession )
		ADeleteAnimator( Asession );
	return;
	}

hAObject = ARegisterAobject( Asession, hBitmap, NULL/*hBitmapMask*/,
	NULL/*lpOffset*/ );
DeleteObject( hBitmap );
if ( !hAObject )
	{
	if ( bDeleteSession )
		ADeleteAnimator( Asession );
	return;
	}

ACopyBackgroundScreen( Asession, hWnd, lpClipRect );
//APaintAllObjects( Asession ); // redraw all current objects
AExecuteScript( Asession, hAObject, lpScript, sz, sizeof(sz)-1 );
while ( AWhere( Asession, hAObject, NULL ) && !LBUTTON  )
	AMoveAllObjects( Asession );
if ( bDeleteSession )
	{
	ADeleteAllObjects( Asession );
	ADeleteAnimator( Asession );
	}
}

/************************************************************************/
void AnimateBatchStart( HWND hWnd )
/************************************************************************/
{
	HBITMAP hImageBitmap;
	
    if ( !BatchSession )
        BatchSession = ACreateNewAnimator();

    if (BatchSession)
    {
        // Hide the static animation placeholder objects while playing
        //    (the hidden flag should be set for each animation object)
        AStartAll( BatchSession );
		
		// Build a bitmap of the image minus the animated objects.
		// Pass this bitmap to the animation session to be used as the 
		// virgin background.  Animations will be played on this bitmap.
		hImageBitmap = Image2Bitmap(hWnd);
		AGiveBackgroundBitmap(BatchSession, hWnd, hImageBitmap, FALSE);
					   
		// Start all the sounds playing now that they are all loaded.
		APlayAllSounds(BatchSession);

        bAnimate = TRUE;
    }
}


/************************************************************************/
void AnimateBatchEnd(HWND hWnd)
/************************************************************************/
{
	HBITMAP hImageBitmap;

	if (!bAnimate || !BatchSession) return;
    bAnimate = FALSE;

// The animations will be removed with this new background bitmap.
// This itmap has all the animated sticker objects visible.
// The image will look just like it did before the animation began.
	hImageBitmap = Image2Bitmap(hWnd);
	AGiveBackgroundBitmap(BatchSession, hWnd, hImageBitmap, FALSE);

    if (BatchSession)
	{
        ADeleteAllObjects(BatchSession);
		ARepaintBackground(BatchSession);
		ADeleteAnimator(BatchSession);
	}
    BatchSession = NULL;
}


/************************************************************************/
BOOL AnimateBatchAdd( int x, int y, LPSTR lpToyFileName )
/************************************************************************/
{
    BOOL bResult = FALSE;
    HINSTANCE hToy;

    if ( !BatchSession )
        BatchSession = ACreateNewAnimator();

    // Perform a LoadLibrary to get the instance handle
    if ( BatchSession && (hToy = LoadLibrary( lpToyFileName )) >= (HINSTANCE)HINSTANCE_ERROR )
    {
        // Load this toy and all its children, scripts, etc.
        int hobject = LoadToy (BatchSession, NULL, 1, x, y, hToy);
        if (hobject)
        {
			// Tell the object to automatically repeat the script when it finishes.
			// This must be done before ReExecuteScript().
			ASetAutoRepeat(BatchSession, hobject, TRUE);
            AReExecuteScript(BatchSession, hobject);
            bResult = TRUE;
        }
        FreeLibrary( hToy );
    }
    return bResult;
}


/************************************************************************/
void AnimateBatchPlay( void )
/************************************************************************/
{
    AMoveAllObjects (BatchSession);
}


/************************************************************************/
LPFRAME CALLBACK EXPORT TiffResource2Frame( HINSTANCE hInst, LPSTR lpRes )
/************************************************************************/
{
int 	DataType;
LPTR 	lpData;
HRSRC 	hData;
LPFRAME lpFrame;
#ifdef WIN32
DWORD	dwSize = 0;
HGLOBAL hGlobal;
LPTR	lpGlobal;
#endif

#ifdef _MAC
	hData = (HRSRC)LoadMacResource(hInst, lpRes, TIFF_RES_TYPE);
	if (!hData) return NULL;
	lpData = *((LPTR *)hData);	// dereference the Mac loacked handle
	if (!lpData)
	{
		ReleaseMacResource(hData);
		return NULL;
	}	
	dwSize = GetMacResourceSize(hData);
#else // Windows code below
	
if ( !(hData = FindResource( hInst, lpRes, TIFF_RES_TYPE )) )
	return( NULL );

#ifdef WIN32
dwSize = SizeofResource(hInst, hData);
#endif

if ( !(hData = (HRSRC)LoadResource( hInst, hData )) )
	return( NULL );
if ( !(lpData = (LPTR)LockResource( hData )) )
	{
	FreeResource( hData );
	return( NULL );
	}
	
#endif // _MAC


#ifdef WIN32
// The memory must be duplicated in to a Global Memory block for Win32.
// This is because later a call to GlobalHandle() is used for lpGlobal.
// GlobalHandle() fails if the memory block is not a proper Global handle.
	lpFrame = NULL;
	hGlobal = GlobalAlloc(GHND, dwSize);
	if (hGlobal)
	{
		lpGlobal = (LPTR)GlobalLock(hGlobal);
		if (lpGlobal)
		{
			CopyMemory(lpGlobal, lpData, dwSize);

			lpFrame = tifrdr( (LPSTR)lpGlobal, -1/*3outDepth*/, &DataType, NO/*bReadOnly*/ );
			GlobalUnlock(hGlobal);
		}
	}

 #ifdef _MAC
	ReleaseMacResource(hData);
 #else
	UnlockResource( hData );
	FreeResource( hData );
 #endif // _MAC
 	
	return lpFrame;
#endif // WIN32

lpFrame = tifrdr( (LPSTR)lpData, -1/*3outDepth*/, &DataType, NO/*bReadOnly*/ );
UnlockResource( hData );
FreeResource( hData );
return( lpFrame );
}


/************************************************************************/
static HBITMAP TiffResource2Bitmap( HINSTANCE hInst, LPSTR lpRes )
/************************************************************************/
{
LPFRAME lpFrame;
HBITMAP hBitmap;

if ( !(lpFrame = TiffResource2Frame( hInst, lpRes )) )
	return( NULL );
hBitmap = FrameToBitmap( lpFrame, NULL /*lpRect*/ );
FrameClose( lpFrame );
return( hBitmap );
}


/************************************************************************/
static LPFRAME DibResource2Frame( HINSTANCE hInst, ITEMID idResource )
/************************************************************************/
{
LPTR 	lpData;
HRSRC 	hData;
LPFRAME lpFrame;

#ifdef _MAC
	hData = (HRSRC)LoadMacResourceID(hInst, idResource, "WBMP");
	if (!hData) return NULL;
	lpData = *((LPTR *)hData);	// dereference the Mac loacked handle
	if (!lpData)
	{
		ReleaseMacResource((HANDLE)hData);
		return NULL;
	}	

lpFrame = DIBToFrame( lpData, YES );
ReleaseMacResource(hData);
#else	

if ( !(hData = FindResource( hInst, MAKEINTRESOURCE(idResource), RT_BITMAP )) )
	return( NULL );
if ( !(hData = (HRSRC)LoadResource( hInst, hData )) )
	return( NULL );
if ( !(lpData = (LPTR)LockResource( hData )) )
	{
	FreeResource( hData );
	return( NULL );
	}

lpFrame = DIBToFrame( lpData, YES );
UnlockResource( hData );
FreeResource( hData );
#endif // _MAC

return( lpFrame );
}


/************************************************************************/
HBITMAP DibResource2Bitmap( HINSTANCE hInst, ITEMID idResource )
/************************************************************************/
{
LPFRAME lpFrame;
HBITMAP hBitmap;

if ( !(lpFrame = DibResource2Frame( hInst, idResource )) )
	return( NULL );
hBitmap = FrameToBitmap( lpFrame, NULL /*lpRect*/ );
FrameClose( lpFrame );
return( hBitmap );
}


#ifdef UNUSED
/************************************************************************/
static HBITMAP TiffResource2DIB( HINSTANCE hInst, LPSTR lpRes )
/************************************************************************/
{
LPFRAME lpFrame;
DWORD dwValue;
LPTR lpDIB;

if ( !(lpFrame = TiffResource2Frame( hInst, lpRes )) )
	return( NULL );
lpDIB = FrameToDIB( lpFrame, NULL /*lpRect*/ );
FrameClose( lpFrame );
if ( !lpDIB )
	return( NULL );
dwValue = GlobalHandle( HIWORD(lpDIB) );
if ( HIWORD(dwValue) != HIWORD(lpDIB) ) // segment portions should match
	return( NULL );
return( LOWORD( dwValue ) );
}
#endif




void ShowCoverAnimatedButtons(HWND hWnd, BOOL bShow)
{
	int 	i;
	HWND	hCtrl;
	
	for (i=IDC_COVER_ANIMATE1; i<IDC_COVER_ANIMATE10; i++)
	{
		hCtrl = GetDlgItem(hWnd, i);
		if (hCtrl)
			ShowWindow(hCtrl, bShow ? SW_SHOW : SW_HIDE);
	}
}
