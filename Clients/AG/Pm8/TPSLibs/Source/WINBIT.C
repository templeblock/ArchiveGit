/*
   Module   :	winBit.c
   Project  :	TPS Tool Library
   Abstract :	Macintosh off-screen bitmap subsystem primitives.
   Status   :	Under Development
  
   Copyright © 1995-96, Turning Point Software.  All Rights Reserved.
  
   Modification History :
  	0.0.0	2	February 1995		Michael Houle		Initial revision.
	1.0.0		May 1996				Bruce D. Rosenblum - major rev to Win 32

   To Do:

*/

#include "tpsBit.h"

#define	BITDEBUG	1

#ifdef	MAC
	#error	This is a _WINDOWS only file
#endif

ASSERTNAME

/***  defines  ***/
#define	PALETTE_VERSION			0x0300
#define	NUM_PALETTE_ENTRIES		256

#define  kMagicBitmapKey			0x3E96
#define  kMaxRowBytes				0x3FFE 	// maximum number of bytes in a row of pixels 
#define  kAllFlagsMASK          (TBIT_ZeroInit | TBIT_Virtual | TBIT_Purgeable | TBIT_LongAlignRows | TBIT_DefaultColorTable | TBIT_16ByteAlignRows)

//KDM 10/21/97 Flag for creating Identity palette - Leapfrog doesn't want it
static BOOLEAN	bCreateIdentity = TRUE;
TPSAPI_DECLP( VOID ) SetIdentityFlag (BOOLEAN CreateIdentity)
{
	bCreateIdentity = CreateIdentity;
}

typedef	struct	tagBITMAPINFO256			//	A Bitmap Info Header with a full 256 color table in the structure
{
	BITMAPINFOHEADER	bmiHeader;
	RGBQUAD				bmiColors[NUM_PALETTE_ENTRIES];
} BITMAPINFO256;

typedef	struct	tagPALETTE256
{
	sWORD 			swVersion;
	sWORD				swNumOfEntries;
	PALETTEENTRY	palEntries[NUM_PALETTE_ENTRIES];
} PALETTE256;


/***  type defs  ***/
typedef	struct	tagTBITPORT
{
	HDC				hdcDIB;						//	The DIB Drawing Device
	HBITMAP			hbmOld;						//	The orginal 1x1 b/w bitmap in the DC
   HTBIT				htbInUse;					// port in use by flag/pointer
} TBITPORT;											// tbp
typedef	TBITPORT FAR*	LPTBITPORT;			//	lptbp


typedef struct tagTBITREC
{
	uWORD				uwFlags;   						// state flags
	uWORD				uwDepth;							// bits per pixel
	sWORD				swInPort;  						// internal port in use by bitmap or -1
	sWORD				uwBytesPerRow;					// bytes per row 
	sWORD				swLockCount;					//	number of times bitmap is locked -vs- unlocked
	TRECT				trcBounds;       				// bounding box of bitmap
	LPVOID			pBits;							//	data for the DIB Section Bitmap
	HBITMAP			hbm;								//	The DIB Section Bitmap
	BITMAPINFO256	bmi;	      					// Bitmap Info including 256 color table
	uWORD				MagicKey;						// bitmap magic
} TBITREC;												//	tbr;
typedef  TBITREC  FAR*	LPTBITREC;				//	lptbr


/***  macros  ***/
#define  TBitDeRef(h)   ((LPTBITREC)tmemDeRef(h))

/**  local prototypes  **/
static	sWORD		_FindFreePort (void);
static	HPALETTE	_CreateIdentityPalette(HNATIVE hnColorData, int nColors);
static	void		_ClearSystemPalette(void);
static	uLONG		_TBitBltModeToRasterOp( uWORD uwMode );
static	HTMEM		_PaletteToColorTable( HPALETTE hPal );

#ifdef TPSDEBUG
static   void	_DebugAssertTBit (HTBIT hTBit);
#else	//	TPSDEBUG
#define			_DebugAssertTBit(x)
#endif // TPSDEBUG 

#define	USE_COMPONENT
#ifdef	USE_COMPONENT

extern PVOID	GetGlobalBitmapPool( void );
extern PVOID	SetGlobalBitmapPool( PVOID pVoid );
extern BOOLEAN	AreApplicationGlobalsAvailable( void );

typedef	struct	tagTBITGLOBALS
{
	BOOLEAN			fTBitStarted;				//	has the bitmap toolbox been started
	uBYTE				ubTransparentColor;		// color used for transparent copy bits
	sWORD				swNumPorts;					// number of entries in internal port list
	LPTBITPORT		lptbpPorts;					// internal port list
	HTMEM				htColorTable;				//	standard color table
	HWND				hWndCurDevice;				//	NULL is also HWND_DESKTOP
	HPALETTE			hpForNormDCs;				//	palette to be used for normal DC's
	HPALETTE			hpForPrinterDCs;			//	palette to be used for normal DC's
	Debug( sWORD	swAllocBitmap; )			// number of bitmaps allocated by subsystem
} TBITGLOBALS;										// tbg
typedef	TBITGLOBALS FAR*	LPTBITGLOBALS;	//	lptbg

#define	_fTBitStarted			(((LPTBITGLOBALS)GetGlobalBitmapPool())->fTBitStarted)
#define	_ubTransparentColor	(((LPTBITGLOBALS)GetGlobalBitmapPool())->ubTransparentColor)
#define	_swNumPorts				(((LPTBITGLOBALS)GetGlobalBitmapPool())->swNumPorts)
#define	_lptbpPorts				(((LPTBITGLOBALS)GetGlobalBitmapPool())->lptbpPorts)
#define	_htColorTable			(((LPTBITGLOBALS)GetGlobalBitmapPool())->htColorTable)
#define	_hWndCurDevice			(((LPTBITGLOBALS)GetGlobalBitmapPool())->hWndCurDevice)
#define	_hpForNormDCs			(((LPTBITGLOBALS)GetGlobalBitmapPool())->hpForNormDCs)
#define	_hpForPrinterDCs		(((LPTBITGLOBALS)GetGlobalBitmapPool())->hpForPrinterDCs)
#define	_swAllocBitmap			(((LPTBITGLOBALS)GetGlobalBitmapPool())->swAllocBitmap)

#else

/**  static globals  **/
Debug( static   sWORD	    	_swAllocBitmap = 0; )	// number of bitmaps allocated by subsystem

/**  static globals  **/
static	BOOLEAN			_fTBitStarted			= FALSE;		//	has the bitmap toolbox been started
static	uBYTE				_ubTransparentColor	= 0;			// color used for transparent copy bits
static	sWORD				_swNumPorts				= 0;			// number of entries in internal port list
static	LPTBITPORT		_lptbpPorts				= NULL;		// internal port list

static	HTMEM				_htColorTable			= NULL;
static	HWND				_hWndCurDevice			= NULL;		//	NULL is also HWND_DESKTOP
static	HPALETTE			_hpForNormDCs			= NULL;		//	palette to be used for normal DC's
static	HPALETTE			_hpForPrinterDCs		= NULL;		//	palette to be used for printer DC's

#endif	/* USE_COMPONENT	*/

/* 
   tbitStartup 

		Startup the bitmap library routines.  Preallocates some a given number
		of ports for use in the application

	OnEntry:
		NrPorts		- the number of internal ports to allocate for use
		uwDepth		- the depth of the bitmaps to be allocated
		hnColorTab	- the Native Color table which will be used for the application

	OnExit:
		Nothing

   Returns:
   	error code indicating success or failure...

*/

TPSAPI_DECLP( BOOLEAN )  tbitStartup (uWORD NrPorts, uWORD uwDepth, HNATIVE hnColorTab)
{
	uWORD			i;
	HDC			hdc;

#ifdef	USE_COMPONENT
	PVOID			pv;

	//	setup the global block of bitmap module static variables
	pv = tmemAllocNativePtr(sizeof(TBITGLOBALS));
	if(NULL == pv)
		return FALSE;
	tmemZeroBlock(pv, sizeof(TBITGLOBALS));
	SetGlobalBitmapPool(pv);

#endif	/*	USE_COMPONENT */

	//		validations/initializations
	TpsAssert( !_fTBitStarted, "Bitmap subsystem was already started");
	TpsAssert((hnColorTab != NULL), "Bitmap subsystem color table must be provided!");
	TpsAssert( !_hpForNormDCs, "Bitmap subsystem has a Screen Palette");
	TpsAssert( !_hpForPrinterDCs, "Bitmap subsystem has a Printer Palette");

	_lptbpPorts	= tmemAllocNativePtr((sizeof(TBITPORT) * (NrPorts + 1)));
	if (!_lptbpPorts)
	{
		return FALSE;
	}
	tmemZeroBlock( _lptbpPorts, (sizeof(TBITPORT)*(NrPorts + 1)) );

	//	Setup the System, Printer and DIB Palettes
	_hpForNormDCs	= _CreateIdentityPalette( hnColorTab, NUM_PALETTE_ENTRIES );
	_hpForPrinterDCs	= _CreateIdentityPalette( hnColorTab, NUM_PALETTE_ENTRIES );
	_htColorTable	= _PaletteToColorTable( _hpForNormDCs );

	if (!_hpForNormDCs || !_hpForPrinterDCs || !_htColorTable)
		goto Failure;

	for (i = 0; i < NrPorts; ++i)
	{
		//	Hardcoded string as per CreateDC docs in Win32 SDK
	//	hdc = CreateDC( "DISPLAY", "DISPLAY", NULL, NULL );
		hdc = CreateCompatibleDC(NULL);
		if (!hdc)
			goto Failure;

		_lptbpPorts[i].hdcDIB	= hdc;
		_lptbpPorts[i].hbmOld	= NULL;
		_lptbpPorts[i].htbInUse	= NULL;
	}

	_swNumPorts 	= NrPorts;
	_fTBitStarted	= TRUE;

	//	Clear to be able to force our palette into the device
	_ClearSystemPalette();

	return TRUE;

Failure:
	if (_lptbpPorts)
	{
		for (i = 0; i < NrPorts; i++)
		{
			if (_lptbpPorts[i].hdcDIB)
				DeleteDC(_lptbpPorts[i].hdcDIB);
		}
		tmemFreeNativePtr(_lptbpPorts);
		_lptbpPorts = NULL;
	}

	if (_hpForNormDCs)	DeleteObject(_hpForNormDCs);
	_hpForNormDCs	= NULL;
	if (_hpForPrinterDCs)	DeleteObject(_hpForPrinterDCs);
	_hpForPrinterDCs	= NULL;
	if (_htColorTable)	tmemFree(_htColorTable);
	_htColorTable	= NULL;

#ifdef	USE_COMPONENT
	tmemFreeNativePtr(GetGlobalBitmapPool());	
#endif	/*	USE_COMPONENT */

	return FALSE;
}


/*
   tbitShutdown 

   Terminate TPS off-screen bitmap subsystem at shutdown. 

	OnEntry:
	OnExit:
   Returns:
   	Nothing
*/
TPSAPI_DECLP( void )  tbitShutdown (void)
{
	sWORD				i;

	TpsAssert( _fTBitStarted, "Bitmap Subsystem never initialized");
	TpsAssert( _swAllocBitmap == 0, "tbitShutdown called with some bitmaps still allocated");

	if (_lptbpPorts)
	{
		for (i = 0; i < _swNumPorts; ++i)
		{
			if (_lptbpPorts[i].hdcDIB)
				DeleteDC(_lptbpPorts[i].hdcDIB);
		}
		tmemFreeNativePtr(_lptbpPorts);
		_lptbpPorts = NULL;
	}

	if (_hpForNormDCs)	DeleteObject(_hpForNormDCs);
	_hpForNormDCs	= NULL;
	if (_hpForPrinterDCs)	DeleteObject(_hpForPrinterDCs);
	_hpForPrinterDCs	= NULL;
	if (_htColorTable)	tmemFree(_htColorTable);
	_htColorTable	= NULL;
#ifdef	USE_COMPONENT
	tmemFreeNativePtr(GetGlobalBitmapPool());	
#endif	/*	USE_COMPONENT */

}

/* 
   tbitStarted 

		Return TRUE if the tps bit libs have been started.

	OnEntry:

	OnExit:
		Nothing

   Returns:
		Return TRUE if the tps bit libs have been started.

*/

TPSAPI_DECLP( BOOLEAN )  tbitStarted ( )
{
	/*	Return TRUE if the application globals are available
		and the tbit libraries have been started. */
	return AreApplicationGlobalsAvailable( ) && GetGlobalBitmapPool( ) && _fTBitStarted;
}

/*
   tbitSetMainScreen

		Set the main drawing device (what HWND is the system going to
		draw to on the drawing calls)

	OnEntry:
		screen	- a pointer to the hwnd to draw into...

	OnExit:
	Returns:
		Nothing
*/
TPSAPI_DECLP( void )  tbitSetMainScreen (LPVOID screen)
{
	HDC	hdc;

	TpsAssert( _fTBitStarted, "Bitmap Subsystem never initialized");
	TpsAssert( _hpForNormDCs, "There is no Palette for Normal DCs");
	TpsAssert( screen != NULL, "screen pointer is null" );

	_hWndCurDevice	= *(HWND*)screen;
	hdc				= GetDC(_hWndCurDevice);
	if (hdc)
	{
		SelectPalette(hdc, _hpForNormDCs, FALSE);
		RealizePalette(hdc);
		ReleaseDC(_hWndCurDevice, hdc);
	}
	
//	Debug( if (!_hWndCurDevice) tdb_DebugMsg("Drawing device is now the desktop"); )
}


/* ----------------------------------------------------------------------------
   tbitSetTransparentColor
    Set transparent color use by subsystem.
   Returns nothing.                
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( void )  tbitSetTransparentColor (sWORD color)
{
	TpsAssert((color >= 0) && (color <= NUM_PALETTE_ENTRIES), "Transparent color out of palette entry range");
	_ubTransparentColor = (uBYTE)color;
}


/* ----------------------------------------------------------------------------
   tbitAlloc 
   Open a bitmap. 
   Returns trasparent handle to an internal bitmap or NULL.

   By Default, this will create a TopDown DIB which is not the standard
   type of DIB.  There are a few bugs with certain video drivers involving
   blitting not at 0,0.  If you need to use BottomUp dibs,
   #define BOTTOMUP_BITMAP and make sure your src & dst rects are correct
   when you blt to other bitmaps and the screen
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( HTBIT )  tbitAlloc (uWORD uwDepth, LPTRECT lptrcBounds, uWORD flags)
{
	HTBIT				hTBit;
	sWORD				swBPR; 				// bytes per row in Bitmap
	LPTBITREC		lptbr;				//	pointer to new bitmap information
	sLONG				slMod;
	sLONG				slRemainder;
	HBITMAP			hbm;

	//		validations
	TpsAssert( _fTBitStarted, "Bitmap Subsystem never initialized");
	TpsAssert(((flags & ~kAllFlagsMASK) == 0), "Bad flags!");
	TpsAssert(((flags & TBIT_Virtual) == 0), "Virtual bitmaps not supported!");
	TpsAssert(((flags & TBIT_Purgeable) == 0), "Purgable bitmaps not supported!");
	TpsAssert(((flags & TBIT_DefaultColorTable) != 0), "Default color table is the only option!");
	TpsAssert((uwDepth == 8 || uwDepth == 16 || uwDepth == 24 || uwDepth == 32), "Bad depth!");
	TpsAssert((lptrcBounds != NULL), "Bounding box not existant!");
	TpsAssert(((lptrcBounds->top == 0) && (lptrcBounds->left == 0)), "Bounding box not zero based!");
	TpsAssert(((lptrcBounds->bottom > 0) && (lptrcBounds->right > 0)), "Bounding box empty!");
	TpsAssert(((lptrcBounds->bottom > lptrcBounds->top) &&
									(lptrcBounds->right > lptrcBounds->left)), "Bounding box invalid!");

	swBPR = (((uwDepth * trcWidth(lptrcBounds)) + 31) >> 5) << 2; 
	if ( (flags & (TBIT_LongAlignRows | TBIT_16ByteAlignRows)) != 0 )
	{
		slMod = 4;															// long align rowbytes
		if ((flags & TBIT_16ByteAlignRows) != 0)				// or, double long align rowbytes
			slMod = 16;
		slRemainder = (sLONG)swBPR % slMod;
		if (slRemainder) 						
			swBPR += (sWORD)(slMod - slRemainder);
	}
	TpsAssert((swBPR < kMaxRowBytes), "Bytes per row > 16,382!");	// maximum number of bytes per row is 16,382 


	//	allocate the TBITREC	structure
	hTBit	= tmemAlloc( sizeof(TBITREC), (TMEM_AFZEROINIT) );
	if (hTBit)
	{
		lptbr = (LPTBITREC)tmemLock( (HTMEM)hTBit );

		lptbr->uwFlags			= flags;
		lptbr->uwDepth			= uwDepth;
		lptbr->swInPort		= TBIT_InvalidPortID;
		lptbr->uwBytesPerRow	= swBPR;
		lptbr->swLockCount	= 0;
		lptbr->trcBounds		= *lptrcBounds;
		lptbr->MagicKey		= kMagicBitmapKey;

		//	Fill in the Windows specific BitmapInfoHeader information
		lptbr->bmi.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
		lptbr->bmi.bmiHeader.biWidth			= trcWidth(lptrcBounds);
		lptbr->bmi.bmiHeader.biHeight			= -trcHeight(lptrcBounds);
#ifdef	BOTTOMUP_BITMAP
		lptbr->bmi.bmiHeader.biHeight			= -lptbr->bmi.bmiHeader.biHeight;	//	negative implies Top-Down DIB... (like Mac)
#endif
		lptbr->bmi.bmiHeader.biPlanes			= 1;			//see BITMAPINFOHEADER documentation
		lptbr->bmi.bmiHeader.biBitCount		= uwDepth;
		lptbr->bmi.bmiHeader.biCompression	= BI_RGB;	//see BITMAPINFOHEADER documentation
		lptbr->bmi.bmiHeader.biSizeImage		= 0;			//see BITMAPINFOHEADER documentation
		lptbr->bmi.bmiHeader.biXPelsPerMeter= 0;
		lptbr->bmi.bmiHeader.biYPelsPerMeter= 0;
		lptbr->bmi.bmiHeader.biClrUsed		= (uwDepth == 1) ? 2 : 0;	//see BITMAPINFOHEADER documentation
		lptbr->bmi.bmiHeader.biClrImportant	= 0;			//see BITMAPINFOHEADER documentation

		//	copy the application color table entries into the palette
		if(uwDepth == 1)
		{
			*(uLONG *)(&lptbr->bmi.bmiColors[0]) = 0L;
			*(uLONG *)(&lptbr->bmi.bmiColors[1]) = 0xFFFFFFFF;
			lptbr->bmi.bmiColors[1].rgbReserved = 0;
		}	
		else
		{
			tmemCopyBlock(tmemDeRef(_htColorTable), &(lptbr->bmi.bmiColors[0]),
											(uLONG)(sizeof(RGBQUAD) * NUM_PALETTE_ENTRIES));
		}

		//	Need an HDC, so just use the first one pre-allocated.
		hbm = CreateDIBSection( _lptbpPorts[0].hdcDIB,
										(BITMAPINFO FAR*)&(lptbr->bmi),
										((uwDepth > 8) ? DIB_RGB_COLORS : DIB_PAL_COLORS),
										&(lptbr->pBits),
										NULL,
										0L);
		if (!hbm)
		{
			tmemUnlock( (HTMEM)hTBit );
			tmemFree( (HTMEM)hTBit );
			return NULL;
		}

		if(TBIT_ZeroInit & flags)
			tmemZeroBlock(lptbr->pBits, (uLONG)trcHeight(lptrcBounds) * (uLONG)swBPR);

		lptbr->hbm = hbm;
		tmemUnlock( (HTMEM)hTBit );

		_DebugAssertTBit(hTBit);
		Debug( ++_swAllocBitmap; )
	}

	return hTBit;
}


/*
   tbitFree 

		Deallocate all associated memory with the bitmap...

	OnEntry:
		hTBit		- the bitmap to deallocate

	OnExit:
   Returns:
   	Nothing.
*/
TPSAPI_DECLP( void )  tbitFree (HTBIT hTBit)
{
	LPTBITREC		lptbr;

	_DebugAssertTBit(hTBit);

	lptbr	= (LPTBITREC) tmemDeRef( (HTMEM)hTBit );

	TpsAssert( (lptbr->swLockCount == 0), "Free operation on a locked bitmap");
	TpsAssert( (lptbr->swInPort == TBIT_InvalidPortID), "Free operation on bitmap in a port");

	DeleteObject( (HBITMAP)lptbr->hbm );		//	free the DIB bitmap (and data)
	tmemFree(hTBit);

	Debug( --_swAllocBitmap; )
}


/*
   tbitLock 

		Assign a bitmap to a device.

	OnEntry:
		hTBit		- the bitmap to lock into a device

	OnExit:
		Nothing

	Returns:
		Error code
*/
TPSAPI_DECLP( BOOLEAN )  tbitLock (HTBIT hTBit)
{
	HBITMAP		hbm;
	sWORD			swFreePort;
	LPTBITPORT	lpPort;
	LPTBITREC	lptbr;

	_DebugAssertTBit(hTBit);

	//	already in a port...
	lptbr			= TBitDeRef(hTBit);
	if (0 != lptbr->swLockCount)
	{
		++lptbr->swLockCount;
		return TRUE;
	}

	if ((swFreePort = _FindFreePort()) == TBIT_InvalidPortID)
		return FALSE;

	//	Remember the port we just found...
	++lptbr->swLockCount;
	lptbr->swInPort	= swFreePort;
	hbm					= lptbr->hbm;

	lpPort	= &(_lptbpPorts[swFreePort]);

	TpsAssert( lpPort->htbInUse == NULL, "Port is in use by another Bitmap");
	TpsAssert( lpPort->hbmOld == NULL, "Port has old bitmap information");

	lpPort->hbmOld		= SelectObject( lpPort->hdcDIB, hbm );
	lpPort->htbInUse	= hTBit;

	//	If select Object was successful, hbmOld now contains the 1x1 b/w bitmap
	TpsAssert( lpPort->hbmOld, "Failed to select the Bitmap into the port" );

	return TRUE;
}


/*
   tbitUnlock 
   
		Removes a bitmap from a device, and lets the device be re-used

	OnEntry:
		hTBit		- the bitmap to remove from the device;

	OnExit:
		Nothing

	Returns:
		Nothing.                 
*/
TPSAPI_DECLP( void )  tbitUnlock (HTBIT hTBit)
{
	LPTBITPORT	lpPort;
	LPTBITREC	lptbr;
	HBITMAP		hbm;

	_DebugAssertTBit(hTBit);

	//	already in a port more than once
	lptbr			= TBitDeRef(hTBit);
	if (--lptbr->swLockCount == 0)
	{
		lpPort	= &(_lptbpPorts[lptbr->swInPort]);
		lptbr->swInPort = TBIT_InvalidPortID;

		TpsAssert( lpPort->htbInUse == hTBit, "Port does not think it is using this bitmap");
		TpsAssert( lpPort->hbmOld != NULL, "Port has no old bitmap information");

		hbm	= SelectObject( lpPort->hdcDIB, lpPort->hbmOld );

		TpsAssert( hbm, "No Bitmap was selected out of the DC");
		TpsAssert( (hbm == (TBitDeRef(hTBit))->hbm), "Bitmap selected out of DC was not the htBits bitmap");

		lpPort->htbInUse	= NULL;
		lpPort->hbmOld		= NULL;
	}
}


/*
   tbitGetBytesPerRow 
   
   Returns number of bytes per row in bitmap.
*/
TPSAPI_DECLP( uWORD )  tbitGetBytesPerRow (HTBIT hTBit)
{
	_DebugAssertTBit(hTBit);
	return (TBitDeRef(hTBit))->uwBytesPerRow;
}


/*
   tbitGetRect 
   
   Returns nothing.
*/
TPSAPI_DECLP( void )  tbitGetRect (HTBIT hTBit, TRECT *rect)
{
	_DebugAssertTBit(hTBit);
	*rect	= (TBitDeRef(hTBit))->trcBounds;
}


/*
   tbitGetDepth 
   Get image depth of off-screen bitmap.
   Returns bits per pixel depth of bitmap.
*/
TPSAPI_DECLP( uWORD )  tbitGetDepth (HTBIT hTBit)
{
	_DebugAssertTBit(hTBit);
	return (TBitDeRef(hTBit))->uwDepth;
}


/*
	tbitGetDevice

		Return the device associated with the bitmap
		for use with normal drawing commands

	OnEntry:
		hTBit	- the bitmap to retreive the device from

	OnExit:
		pDevice	- the platform specific Device associated with the bitmap.

	Returns:
		TRUE if the bitmap is associated with a device. FALSE otherwise

*/

TPSAPI_DECLP( VOID ) tbitGetDevice (HTBIT hTBit, LPVOID lpDevice)
{
	sWORD		swInPort;

	_DebugAssertTBit(hTBit);

	swInPort	= (TBitDeRef(hTBit))->swInPort;

	TpsAssert( swInPort != TBIT_InvalidPortID, 
					"The Bitmap is not associated with any ports");
	TpsAssert( _lptbpPorts[ swInPort ].htbInUse == hTBit, 
					"The Port does not really use this bitmap");
	TpsAssert( _lptbpPorts[ swInPort ].hbmOld, 
					"The Port does not have an old bitmap");

	*((HDC FAR*)lpDevice) = _lptbpPorts[ swInPort ].hdcDIB;
}



/*
   tbitGetBaseAddr 
   Get image base address of off-screen bitmap.
   Returns pointer to start of bitmap image.
*/
TPSAPI_DECLP( LPVOID )  tbitGetBaseAddr (HTBIT hTBit)
{
	_DebugAssertTBit(hTBit);
	return (TBitDeRef(hTBit))->pBits;
}


/*
   tbitGetImageHandle 
   Get image pixmap of off-screen bitmap.
   Returns native handle to pixmap image.
*/
TPSAPI_DECLP( HNATIVE )  tbitGetImageHandle (HTBIT hTBit)
{
	_DebugAssertTBit(hTBit);
	UnimplementedCode();
	return NULL;
}


/*
   tbitBlitToBitmap 

   	Copy image from off-screen bitmap to off-screen bitmap.

	OnEntry:
		htbSrc			- the Source bitmap
		htbDst			- the Destination bitmap
		lptrcSrc			- the Source rectangle
		lptrcDst			- the Destination rectangle
		mode				- the Blit mode (defined in tpsbit.h)
		fTransparent	- Draw the bitmap transparently

	OnExit:
		htbDst			- the destination with the source copied into it

   Returns
		Any subsystem errors

*/
TPSAPI_DECLP( BOOLEAN )  tbitBlitToBitmap (HTBIT htbSrc, HTBIT htbDst, LPTRECT lptrcSrc, LPTRECT lptrcDst, uWORD mode, BOOLEAN fTransparent)
{
	TRECT			trcSrc;
	TRECT			trcDst;
	HDC			hdcSrc;
	HDC			hdcDst;

	TpsAssert( mode != 0xFFFFFFFF, "Using Copy mode that is not yet supported !!!");
	TpsAssert( !fTransparent, "Transparent Blitting is not yet supported!!!");

	_DebugAssertTBit(htbSrc);
	_DebugAssertTBit(htbDst);

/**  get blit area in Mac OS rect  **/
	if (lptrcSrc == NULL)
		tbitGetRect(htbSrc, &trcSrc);
	else
		trcSrc	= *lptrcSrc;

	if (trcIsEmpty( &trcSrc ))
		return TRUE;
		
	if (lptrcDst == NULL)
		tbitGetRect(htbDst, &trcDst);
	else
		trcDst = *lptrcDst;

	if (trcIsEmpty( &trcDst ))
		return TRUE;
		
	if (! tbitLock( htbSrc ))
		return FALSE;

	if (! tbitLock( htbDst ))
	{
		tbitUnlock( htbSrc );
		return FALSE;
	}

	//	Get the bitmap DC's
	TpsAssert( (TBitDeRef(htbSrc))->swInPort != TBIT_InvalidPortID, "Bad Port ID in bitmap");
	TpsAssert( (TBitDeRef(htbDst))->swInPort != TBIT_InvalidPortID, "Bad Port ID in bitmap");

	TpsAssert	( _lptbpPorts[ (TBitDeRef(htbSrc))->swInPort ].hbmOld, "The Source port has no old bitmap.");
	TpsAssert	( _lptbpPorts[ (TBitDeRef(htbSrc))->swInPort ].htbInUse == htbSrc, "The Source port is not using the source bitmap.");
	TpsAssert	( _lptbpPorts[ (TBitDeRef(htbDst))->swInPort ].hbmOld, "The Dest port has no old bitmap.");
	TpsAssert	( _lptbpPorts[ (TBitDeRef(htbDst))->swInPort ].htbInUse == htbDst, "The Dst port is not using the dest bitmap.");

	hdcSrc	= _lptbpPorts[ (TBitDeRef(htbSrc))->swInPort ].hdcDIB;
	hdcDst	= _lptbpPorts[ (TBitDeRef(htbDst))->swInPort ].hdcDIB;

#ifdef   TPSDEBUG
{
   BOOL  bDraw = FALSE;
   if (bDraw)
   {
      TRECT trc;
      HDC   hdc = GetDC( HWND_DESKTOP );
      SelectPalette(hdc, _hpForNormDCs, FALSE);
      RealizePalette(hdc);

      tbitGetRect( htbSrc, &trc);
		BitBlt( hdc,  0, 0, trcWidth(&trc), trcHeight(&trc),
						hdcSrc, 0, 0, SRCCOPY );
      ReleaseDC( HWND_DESKTOP, hdc );
   }

   if (bDraw)
   {
      TRECT trc;
      HDC   hdc = GetDC( HWND_DESKTOP );
      SelectPalette(hdc, _hpForNormDCs, FALSE);
      RealizePalette(hdc);

      tbitGetRect( htbDst, &trc);
		BitBlt( hdc,  0, 0, trcWidth(&trc), trcHeight(&trc),
						hdcDst, 0, 0, SRCCOPY );
      ReleaseDC( HWND_DESKTOP, hdc );
   }
}
#endif	/* TPSDEBUG */
   
	//	REVIEW BDR - check for use of BitBlt and performance check StretchDIBits
   StretchBlt( hdcDst,  trcDst.left, trcDst.top, trcWidth(&trcDst), trcHeight(&trcDst),
					hdcSrc,	trcSrc.left, trcSrc.top, trcWidth(&trcSrc), trcHeight(&trcSrc),
					_TBitBltModeToRasterOp(mode));


#ifdef   TPSDEBUG
{
   BOOL  bDraw = FALSE;
   if (bDraw)
   {
      TRECT trc;
      HDC   hdc = GetDC( HWND_DESKTOP );
      SelectPalette(hdc, _hpForNormDCs, FALSE);
      RealizePalette(hdc);

      tbitGetRect( htbDst, &trc);
		BitBlt( hdc,  0, 0, trcWidth(&trc), trcHeight(&trc),
						hdcDst, 0, 0, SRCCOPY );
      ReleaseDC( HWND_DESKTOP, hdc );
   }
}
#endif	/* TPSDEBUG */
   
   tbitUnlock(htbSrc);
	tbitUnlock(htbDst);

	return TRUE;
}

/*
   tbitBlitToDevice 

   	Copy image from off-screen bitmap to HDC.

	OnEntry:
		htbSrc			- the Source bitmap
		ulDst				- the Destination HDC as opaque long
		lptrcSrc			- the Source rectangle
		lptrcDst			- the Destination rectangle
		mode				- the Blit mode (defined in tpsbit.h)
		fTransparent	- Draw the bitmap transparently

	OnExit:
		htbDst			- the destination with the source copied into it

   Returns
		Boolean on success

*/
TPSAPI_DECLP( BOOLEAN )  tbitBlitToDevice (HTBIT htbSrc, uLONG ulDst, LPTRECT lptrcSrc, LPTRECT lptrcDst, uWORD mode, BOOLEAN fTransparent)
{
	TRECT			trcSrc;
	TRECT			trcDst;
	HDC			hdcSrc;
	HDC			hdcDst;

	UntestedCode();

	TpsAssert( mode != 0xFFFFFFFF, "Using Copy mode that is not yet supported !!!");
	TpsAssert( !fTransparent, "Transparent Blitting is not supported!!!");
	TpsAssert( ulDst != 0L, "Bad destination device");
	TpsAssert( lptrcDst != NULL, "Null dest rect" );

	_DebugAssertTBit(htbSrc);

/**  get blit area in OS rect  **/
	if (lptrcSrc == NULL)
		tbitGetRect(htbSrc, &trcSrc);
	else
		trcSrc	= *lptrcSrc;

	if (trcIsEmpty( &trcSrc ))
		return TRUE;

	{
		Debug( TRECT trcDebug; )
		Debug( tbitGetRect(htbSrc, &trcDebug); )
		TpsAssert ( trcSrc.left < trcDebug.right && trcSrc.top < trcDebug.bottom,
			"Source rect outside of offscreen bitmap");
	}

	trcDst = *lptrcDst;

	if (trcIsEmpty( &trcDst ))
		return TRUE;
		
	if (! tbitLock( htbSrc ))
		return FALSE;

	//	Get the bitmap DC
	TpsAssert( (TBitDeRef(htbSrc))->swInPort != TBIT_InvalidPortID, "Bad Port ID in bitmap");

	TpsAssert	( _lptbpPorts[ (TBitDeRef(htbSrc))->swInPort ].hbmOld, "The Source port has no old bitmap.");
	TpsAssert	( _lptbpPorts[ (TBitDeRef(htbSrc))->swInPort ].htbInUse == htbSrc, "The Source port is not using the source bitmap.");

	hdcSrc	= _lptbpPorts[ (TBitDeRef(htbSrc))->swInPort ].hdcDIB;
	hdcDst	= (HDC)ulDst;
   
	//	REVIEW BDR - check for use of BitBlt and performance check StretchDIBits
   StretchBlt( hdcDst,  trcDst.left, trcDst.top, trcWidth(&trcDst), trcHeight(&trcDst),
					hdcSrc,	trcSrc.left, trcSrc.top, trcWidth(&trcSrc), trcHeight(&trcSrc),
					_TBitBltModeToRasterOp(mode));

   tbitUnlock(htbSrc);

	return TRUE;
}

/*
   tbitBlitToScreen 

   Copy image from off-screen bitmap to screen.

	OnEntry:
		htbSrc			- the Source bitmap
		lptrcSrc			- the Source rectangle
		lptrcDst			- the Destination rectangle
		mode				- the Blit mode (defined in tpsbit.h)
		fTransparent	- Draw the bitmap transparently

	OnExit:
		Nothing

   Returns
   	Any subsystem error code.                
*/
TPSAPI_DECLP( BOOLEAN ) tbitBlitToScreen (HTBIT htbSrc, LPTRECT lptrcSrc, LPTRECT lptrcDst, uWORD mode, BOOLEAN fTransparent)
{
	TRECT			trcSrc;
	TRECT			trcDst;
	HDC			hdcSrc;
	HDC			hdcDst;
	BOOLEAN		fStretch;

	TpsAssert( !fTransparent, "Transparent Blitting is not yet supported!!!");

	_DebugAssertTBit(htbSrc);
//Debug( if (!_hWndCurDevice)	tdbDebugOut("Blitting to the Desktop"); )

	/**  get blit area in Mac OS rect  **/
	if (lptrcSrc == NULL)
		tbitGetRect(htbSrc, &trcSrc);
	else
		trcSrc	= *lptrcSrc;
	if (trcIsEmpty( &trcSrc ))
		return TRUE;
	{
		Debug( TRECT trcDebug; )
		Debug( tbitGetRect(htbSrc, &trcDebug); )
		TpsAssert ( trcSrc.left < trcDebug.right && trcSrc.top < trcDebug.bottom,
			"Source rect outside of offscreen bitmap");
	}

	if (lptrcDst == NULL)
	{
		NRECT	nrc;
		GetClientRect( _hWndCurDevice, &nrc );
		nrcTotrc( nrc, trcDst );
	}
	else
		trcDst	= *lptrcDst;
	if (trcIsEmpty( &trcDst ))
		return TRUE;

	if (! tbitLock( htbSrc ))
		return FALSE;

	// Check type of blitting (strech blit or bit blit)
	if ( (trcWidth(&trcSrc)==trcWidth(&trcDst)) &&
			(trcHeight(&trcSrc)==trcHeight(&trcDst)) )
		fStretch = FALSE;
	else
		fStretch = TRUE;

	//	Get the bitmap DC's
	TpsAssert( (TBitDeRef(htbSrc))->swInPort != TBIT_InvalidPortID, "Bad Port ID in bitmap");
	TpsAssert	( _lptbpPorts[ (TBitDeRef(htbSrc))->swInPort ].hbmOld, "The Source port has no old bitmap.");
	TpsAssert	( _lptbpPorts[ (TBitDeRef(htbSrc))->swInPort ].htbInUse == htbSrc, "The Source port is not using the source bitmap.");

	hdcSrc	= _lptbpPorts[ (TBitDeRef(htbSrc))->swInPort ].hdcDIB;
	hdcDst	= GetDC( _hWndCurDevice );

#ifdef   TPSDEBUG
{
   BOOL  bDraw = FALSE;
   if (bDraw)
   {
      TRECT trc;
      HDC   hdc = GetDC( HWND_DESKTOP );
      SelectPalette(hdc, _hpForNormDCs, FALSE);
      RealizePalette(hdc);

      tbitGetRect( htbSrc, &trc); 
		BitBlt( hdc,0, 0, trcWidth(&trc), trcHeight(&trc),
							hdcSrc, 0, 0, _TBitBltModeToRasterOp(mode) );
		
		ReleaseDC( HWND_DESKTOP, hdc );
   }
}
#endif	/* TPSDEBUG */

   if (hdcDst)
	{
		HPALETTE hPalOld	= SelectPalette(hdcDst, _hpForNormDCs, FALSE);
		RealizePalette(hdcDst);

		// Do either strech or bit blitting
		if( fStretch )
			StretchBlt( hdcDst, trcDst.left, trcDst.top, trcWidth(&trcDst), trcHeight(&trcDst),
								 hdcSrc,	trcSrc.left, trcSrc.top, trcWidth(&trcSrc), trcHeight(&trcSrc),
								 _TBitBltModeToRasterOp(mode) );
		else
			BitBlt( hdcDst, trcDst.left, trcDst.top, trcWidth(&trcDst), trcHeight(&trcDst),
								 hdcSrc,	trcSrc.left, trcSrc.top,
								 _TBitBltModeToRasterOp(mode) );

		ReleaseDC( _hWndCurDevice, hdcDst );

		tbitUnlock(htbSrc);
		return TRUE;
	}
	else
	{
		tbitUnlock(htbSrc);
		return FALSE;
	}
}

/*
   tbitBlitFromScreen 

   Copy image from screen to off-screen bitmap.

   Returns subsystem error code.                
*/
TPSAPI_DECLP( BOOLEAN ) tbitBlitFromScreen (HTBIT hDest, LPTRECT lptrcSrc, LPTRECT lptrcDest,
														  uWORD mode, BOOLEAN fTransparent)
{
	TRECT			trcSrc;
	HDC			hdcSrc;
	BOOLEAN		fReturn = FALSE;
		
	if (lptrcSrc == NULL)
	{
		NRECT	nrc;
		GetClientRect( _hWndCurDevice, &nrc );
		nrcTotrc( nrc, trcSrc );
	}
	else
	{
		trcSrc	= *lptrcSrc;
	}

	hdcSrc	= GetDC( _hWndCurDevice );
   if (hdcSrc)
	{
		tbitBlitFromDevice ( hDest, (uLONG) hdcSrc,  &trcSrc, lptrcDest, mode, fTransparent);
		ReleaseDC( _hWndCurDevice, hdcSrc );
		fReturn = TRUE;
	}
	return fReturn;
}

/*
   tbitBlitFromDevice 

   Copy image from HDC to off-screen bitmap.

   Returns Boolean TRUE if successful.                
*/
TPSAPI_DECLP( BOOLEAN ) tbitBlitFromDevice (HTBIT hDest, uLONG ulSrc, LPTRECT lptrcSrc, LPTRECT lptrcDest,
														  uWORD mode, BOOLEAN fTransparent)
{
	TRECT			trcSrc;
	TRECT			trcDst;
	HDC			hdcSrc;
	HDC			hdcDst;
	HPALETTE		hPalOld;

	_DebugAssertTBit(hDest);
	TpsAssert( ulSrc != 0, "Bad source device" );
	TpsAssert( lptrcSrc != NULL, "Bad source rectangle" );
	TpsAssert( !fTransparent, "Transparent Blitting is not yet supported!!!");

	/**  get blit area in Mac OS rect  **/
	if (lptrcDest == NULL)
		tbitGetRect(hDest, lptrcDest);
	else
		trcDst	= *lptrcDest;
	if (trcIsEmpty( &trcDst ))
		return TRUE;

	trcSrc	= *lptrcSrc;
	if (trcIsEmpty( &trcSrc ))
		return TRUE;

	if (! tbitLock( hDest ))
		return FALSE;

	//	Get the bitmap DC's
	TpsAssert( (TBitDeRef(hDest))->swInPort != TBIT_InvalidPortID, "Bad Port ID in bitmap");
	TpsAssert	( _lptbpPorts[ (TBitDeRef(hDest))->swInPort ].hbmOld, "The Source port has no old bitmap.");
	TpsAssert	( _lptbpPorts[ (TBitDeRef(hDest))->swInPort ].htbInUse == hDest, "The Source port is not using the source bitmap.");

	hdcDst	= _lptbpPorts[ (TBitDeRef(hDest))->swInPort ].hdcDIB;
	hdcSrc	= (HDC)ulSrc;

	hPalOld	= SelectPalette(hdcSrc, _hpForNormDCs, FALSE);
	RealizePalette(hdcSrc);

	// Do either strech or bit blitting
		// Check type of blitting (strech blit or bit blit)
	if ( (trcWidth(&trcSrc)==trcWidth(&trcDst)) && (trcHeight(&trcSrc)==trcHeight(&trcDst)) )
		BitBlt( hdcDst, trcDst.left, trcDst.top, trcWidth(&trcDst), trcHeight(&trcDst),
							 hdcSrc,	trcSrc.left, trcSrc.top,
							 _TBitBltModeToRasterOp(mode) );
	else
		StretchBlt( hdcDst, trcDst.left, trcDst.top, trcWidth(&trcDst), trcHeight(&trcDst),
							 hdcSrc,	trcSrc.left, trcSrc.top, trcWidth(&trcSrc), trcHeight(&trcSrc),
							 _TBitBltModeToRasterOp(mode) );

	tbitUnlock(hDest);
	return TRUE;
}


/*
   tbitScrollRect 
   Scroll bitmap. 
   Returns nothing.                                        
*/
TPSAPI_DECLP( BOOLEAN ) tbitScrollRect (HTBIT hBit, LPTRECT lptrcBit, TPT ptdelta)
{
	HDC 	hdcBit;
	NRECT	nrcBit;
		
	if (! tbitLock( hBit ))
		return FALSE;

	TpsAssert( (TBitDeRef(hBit))->swInPort != TBIT_InvalidPortID, "Bad Port ID in bitmap");
	TpsAssert( _lptbpPorts[ (TBitDeRef(hBit))->swInPort ].hbmOld, "The Source port has no old bitmap.");
	TpsAssert( _lptbpPorts[ (TBitDeRef(hBit))->swInPort ].htbInUse == hBit, "The Source port is not using the source bitmap.");

	//	Get the bitmap DC's
	hdcBit = _lptbpPorts[ (TBitDeRef(hBit))->swInPort ].hdcDIB;

	trcTonrc( *lptrcBit, nrcBit );

	// Actual scrolling
  	if (hdcBit)
		ScrollDC( hdcBit, ptdelta.x, ptdelta.y, &nrcBit, &nrcBit, NULL, NULL );

	tbitUnlock( hBit );
	return ( hdcBit != NULL);		//	true if blit done, false if failed to get DC
}


/*
   tbitGetScreenPalette 
   Returns the screen palette.                                        
*/
TPSAPI_DECLP( HNATIVE ) tbitGetScreenPalette ()
{
	return (HNATIVE)_hpForNormDCs;
}


/*
   tbitGetPrinterPalette 
   Returns the printer palette.                                        
*/
TPSAPI_DECLP( HNATIVE ) tbitGetPrinterPalette ()
{
	return (HNATIVE)_hpForPrinterDCs;
}


/*
   tbitGetColorTable 
   Returns the color table which backs the application palette.                                        
*/
TPSAPI_DECLP( HTMEM ) tbitGetColorTable ( uWORD* puwNumEntries )
{
	TpsAssert( puwNumEntries, "Null input ptr" );

	*puwNumEntries = NUM_PALETTE_ENTRIES;

	return (HTMEM)_htColorTable;
}


/* ----------------------------------------------------------------------------
   _FindFreePort 
   
   Returns indec into port table or -1.                
   ---------------------------------------------------------------------------- */
static  sWORD  _FindFreePort (void)
{
	sWORD		i;

	for (i = 0; i < _swNumPorts; i++)
		if (_lptbpPorts[i].htbInUse == NULL)
			return i;
	return TBIT_InvalidPortID;
}


#ifdef TPSDEBUG
/* ----------------------------------------------------------------------------
   _DebugAssertTBit 
   Validates a TPS internal bitmap handle and asserts on error. 
   Returns nothing.                
   ---------------------------------------------------------------------------- */
static  void  _DebugAssertTBit (HTBIT hTBit)
{
	LPTBITREC	lptbr;

	if (hTBit != NULL)
	{
		lptbr = TBitDeRef(hTBit);

		TpsAssertL(lptbr->MagicKey == kMagicBitmapKey, "Bitmap handle is not a TPS bitmap!", &hTBit, 1);
		TpsAssertL(lptbr->uwBytesPerRow < kMaxRowBytes, "Bitmap handle has bad bytes per row > 16,382!", &hTBit, 1);
		TpsAssertL((lptbr->uwDepth == 1 || lptbr->uwDepth == 8 || lptbr->uwDepth == 16 ||
													lptbr->uwDepth == 24 || lptbr->uwDepth == 32),
						"Bitmap handle has bad depth!", &hTBit, 1);
		TpsAssertL(lptbr->pBits != NULL, "Bitmap data is NULL", &hTBit, 1);

		if (lptbr->swInPort < TBIT_InvalidPortID || lptbr->swInPort > _swNumPorts)
		{
			TpsAssertL(FALSE, "Bitmap handle assigned to bad port!", &hTBit, 1);
		}
		else if (trcIsEmpty(&(lptbr->trcBounds)))
		{
			TpsAssertL(FALSE, "Bitmap handle has empty bounding box!", &hTBit, 1);
		}
	}
}
#endif


/* ----------------------------------------------------------------------------
   tbitIsRectInBitmap 
   Validates a rect within an internal bitmap area. 
   Returns boolean indicating         
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( BOOLEAN ) tbitIsRectInBitmap (HTBIT hTBit, LPTRECT lptrcBit)
{
	TRECT		trc;

	TpsAssert( hTBit, "Null Bitmap Reference");
	TpsAssert( lptrcBit, "Null Rectangle Reference");

	tbitGetRect(hTBit, &trc);
	return trcIsIntersect(lptrcBit, &trc);
}

//	REVIEW BDR - move to XBIT.C
/* ----------------------------------------------------------------------------
   tbitFindEdge 
   Given a bitmap, find the appropriate non-transparent edges.
   This is useful for finding a minimum bounding box.
   Returns nothing.
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( void )  tbitFindEdge (HTBIT hTBit, TRECT *trcMask, uBYTE ubEdgeMask, uBYTE ubTransColor)
{
	TRECT			trcBounds;
	uLONG			ulTransToPack;
	uLONG			ulTrans;
	uLONG			ulRowBytes;
	TSIZE			iRow, iColumn;	
	TSIZE			iColumns;
	sWORD			swByte, swByteOld;
	sWORD			swCnt;
	LPTBITREC	lpTBit;
	HPSBYTE		hpBaseAddr;
	HPSBYTE		hpBase;
	HPSBYTE		hpAddr;
	BOOL			fFound;


	// Find the orientation
	lpTBit	= TBitDeRef( hTBit );

	//	REVIEW BDR - this should be simple flip of resulting rectangle
	TpsAssert( lpTBit->bmi.bmiHeader.biHeight < 0, "BottomUp not implemented yet" );
	TpsAssert( tbitGetDepth(hTBit) == 8, "Only 8 bit depth supported");

	// Pack transparent color bytes into long
	((uBYTE *) &ulTransToPack)[0] = ubTransColor;
	((uBYTE *) &ulTransToPack)[1] = ubTransColor;
	((uBYTE *) &ulTransToPack)[2] = ubTransColor;
	((uBYTE *) &ulTransToPack)[3] = ubTransColor;
	ulTrans	= ulTransToPack;

	// get the pointer to the row data
	hpBaseAddr = (HPSBYTE)lpTBit->pBits;

	// get the size of a row of data
	ulRowBytes = (uLONG)lpTBit->uwBytesPerRow;

	// Get bitmap rect, and initialize bounding rect with it
	trcBounds	= lpTBit->trcBounds;

	*trcMask = trcBounds;

	// Find the number of columns (LONG based). 
	// Add one more column if bitmap is not aligned to LONG
	iColumns = (trcBounds.right - trcBounds.left) / sizeof(uLONG);
	if ((trcBounds.right - trcBounds.left) % sizeof(uLONG))
		iColumns += 1; 		

	// Find the most LEFT edge
	if (ubEdgeMask & TBIT_kLeftEdge)
	{
		fFound = FALSE;

		hpBase = hpBaseAddr + (ulRowBytes * (uLONG) trcBounds.top );

 		for (iColumn = trcBounds.left/sizeof(uLONG); iColumn < iColumns; iColumn++)
		{
			hpAddr = hpBase + iColumn * sizeof(uLONG);

			swByte = swByteOld = sizeof(uLONG) * (iColumn + 1);

			for (iRow = trcBounds.top; iRow < trcBounds.bottom; iRow++)
			{
				if (*((HPSLONG)hpAddr) != (sLONG) ulTrans)
				{
					for (swCnt = 0; swCnt < 4; swCnt++)
						if( hpAddr[swCnt] != ubTransColor )
							break;

					swByte = iColumn * sizeof(uLONG) + swCnt;

					if( swByte < swByteOld )
						swByteOld = swByte;

					fFound = TRUE;
				}
					
				hpAddr += ulRowBytes;	//	advance to next row
			}
				
			if (fFound)
			{
				trcBounds.left = swByteOld-1;
				break;
			}
		}
	}
		
	// Find TOP edge
	if (ubEdgeMask & TBIT_kTopEdge)
	{
		fFound = FALSE;

		hpBase = hpBaseAddr + trcBounds.left;

		for (iRow = trcBounds.top; iRow < trcBounds.bottom; iRow++)
		{
			hpAddr = hpBase + (ulRowBytes * (uLONG) iRow );

			for (iColumn = trcBounds.left/sizeof(uLONG); iColumn < iColumns; iColumn++)
			{
				if (*((HPSLONG)hpAddr) != (sLONG) ulTrans)
				{
					trcBounds.top = iRow;

					fFound = TRUE;

					break;
				}

				hpAddr += sizeof(uLONG);

			}

			if (fFound)
				break;

		}
	}
	
	// Find the most RIGHT edge
	if (ubEdgeMask & TBIT_kRightEdge)
	{
		fFound = FALSE;

		hpBase = hpBaseAddr + (ulRowBytes * (uLONG) trcBounds.top);

		for (iColumn = iColumns; iColumn > (sWORD) (trcBounds.left/sizeof(uLONG)); iColumn--)
		{
			hpAddr = hpBase + sizeof(uLONG) * (iColumn - 1);

			swByte = swByteOld = sizeof(uLONG) * (iColumn - 1);

			for (iRow = trcBounds.top; iRow < trcBounds.bottom; iRow++)
			{
				if (*((HPSLONG)hpAddr) != (sLONG) ulTrans)
				{
					for (swCnt = 0; swCnt < 4; swCnt++)
						if( hpAddr[swCnt] != ubTransColor )
							break;

					swByte = sizeof(uLONG) * (iColumn - 1) + swCnt;

					if( swByte > swByteOld )
						swByteOld = swByte;

					fFound = TRUE;
				}
					
				hpAddr += ulRowBytes;	//	advance to next row
			}
				
			if (fFound)
			{
				trcBounds.right = swByteOld + 1;

				break;
			}
		}
	}


	// Find BOTTOM edge
	if (ubEdgeMask & TBIT_kBottomEdge)
	{
		fFound = FALSE;

		hpBase = hpBaseAddr + trcBounds.left;

		for (iRow = trcBounds.bottom - 1; iRow >= trcBounds.top; iRow--)
		{
			hpAddr = hpBase + (ulRowBytes * (uLONG) iRow );

			for (iColumn = trcBounds.left/sizeof(uLONG); iColumn < iColumns; iColumn++)
			{
				if (*((HPSLONG)hpAddr) != (sLONG) ulTrans)
				{
					trcBounds.bottom = iRow + 1;

					fFound = TRUE;

					break;
				}

				hpAddr += sizeof(uLONG);

			}

			if (fFound)
				break;

		}
	}

	*trcMask = trcBounds;
}


/* ----------------------------------------------------------------------------
   tbitFlatten 
   
   Returns internal handle to pixmap image.
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( HTMEM ) tbitFlatten (HTBIT hTBit)
{
	UnimplementedCode();
	return NULL;
//	see alternate code in XBIT.C
#if 0
HTMEM				hTMem;
short				bytesPerRow; 						// bytes per row in PixMap 
uWORD 				depth;
TBitPtr				pTBit;
Rect				bounds;
uLONG				size;
Handle     			hDataBits;  
Ptr     			ptr;  
uWORD     			flags;  

/**  validations  **/
	_DebugAssertTBit(hTBit);

/**  grab relivant bitmap data  **/
	pTBit = TBitDeRef(hTBit);
	depth = pTBit->Depth;								// bits per pixel
	bytesPerRow = pTBit->BytesPerRow; 					// bytes per row
	bounds = pTBit->Bounds;         					// bounding box 
	hDataBits = pTBit->hDataBits;						// handle to pixel image
	size = (unsigned long)(bytesPerRow * (bounds.bottom - bounds.top));

/**  create flat bitmap record  **/
	flags = TMEM_AFNONPURGEABLE | TMEM_AFNONVIRTUAL | TMEM_AFNONSUBALLOC | TMEM_AFZEROINIT;
	if ((size + sizeof(TBitRec)) >= TMEM_HUGE_SIZE)
		flags |= TMEM_AFHUGE;
	if ((hTMem = tmemAlloc((size + sizeof(TBitRec)), flags)) == NULL)
		return NULL;

/**  copy relivant bitmap data  **/
	ptr = tmemLock(hTMem);
	tmemCopyBlock(tmemDeRef(hTBit), ptr, sizeof(TBitRec));
	pTBit = (TBitPtr)ptr;
	pTBit->Flags = 0;    								// no state flags yet
	pTBit->Depth = depth;								// bits per pixel
	pTBit->InPort = -1;									// no internal port in use yet
	pTBit->BytesPerRow = bytesPerRow; 					// bytes per row
	pTBit->Bounds = bounds;         					// bounding box 
	pTBit->hPixMap = NULL;      							// pixel image
	pTBit->hDataBits = NULL;								// reference to image 
	pTBit->hGDevice = NULL;								// reference to subsystem device 
	pTBit->LockCount = 0;    							// unlocked  

/**  copy bitmap image  **/
	tbitCompress(hTBit, (ptr + sizeof(TBitRec)), &size);
	tmemUnlock(hTMem);
	(void)tmemReSize(hTMem, (size + sizeof(TBitRec)), 0);

	return hTMem; 
#endif
}


/* ----------------------------------------------------------------------------
   tbitUnFlatten 
   
   Returns handle to internal bitmap image.
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( HTBIT )  tbitUnFlatten (HTMEM hm)
{
	UnimplementedCode();
	return NULL;
//	see alternate code in XBIT.C
#if 0
HTBIT				hTBit;
TBitPtr				pTBit;
short				bytesPerRow; 						// bytes per row in PixMap 
uWORD 				depth;
PixMapHandle 		hPixMap = NULL;						// handle to new off-screen PixMap 
Rect				bounds;
Size				size;
Handle     			hDataBits;  
Ptr     			ptr;  

/**  grab old bitmap data  **/
	pTBit = TBitDeRef(hm);
	depth = pTBit->Depth;								// bits per pixel
	bytesPerRow = pTBit->BytesPerRow; 					// bytes per row
	bounds = pTBit->Bounds;         					// bounding box 
	size = (unsigned long)(bytesPerRow * (bounds.bottom - bounds.top));

/**  allocate a new PixMap  **/
    if ((hPixMap = (PixMapHandle)NewHandleClear(sizeof(PixMap))) == NULL)
        return NULL;
	if (_SetUpPixMap(depth, &bounds, _hColorTable, bytesPerRow, hPixMap) != noErr)
	{
        if (hPixMap != NULL)  
            DisposHandle((Handle)hPixMap);
        return NULL;
	}
	hDataBits = (Handle)(**hPixMap).baseAddr;			// handle to pixel image

/**  create internal bitmap record  **/
	if ((hTBit = tmemAlloc(sizeof(TBitRec), (TMEM_AFNONPURGEABLE | TMEM_AFNONVIRTUAL | TMEM_AFNONSUBALLOC | TMEM_AFZEROINIT))) == NULL)
	{
        if (hPixMap != NULL)  
        {
            DisposCTable((**hPixMap).pmTable);
            DisposHandle((Handle)hDataBits);
            DisposHandle((Handle)hPixMap);
        }
		return NULL;
	}	

/**  load bitmap data  **/
	ptr = tmemLock(hm);
	tmemCopyBlock(ptr, tmemDeRef(hTBit), sizeof(TBitRec));
	pTBit = TBitDeRef(hTBit);
	pTBit->Flags = 0;    								// no state flags yet
	pTBit->Depth = depth;								// bits per pixel
	pTBit->InPort = -1;									// no internal port in use yet
	pTBit->BytesPerRow = bytesPerRow; 					// bytes per row
	pTBit->Bounds = bounds;         					// bounding box 
	pTBit->hPixMap = hPixMap;      						// pixel image
	pTBit->hDataBits = hDataBits;						// reference to image 
	pTBit->hGDevice = _hTBitDevice;						// reference to subsystem device 
	pTBit->LockCount = 0;    							// unlocked  

/**  copy bitmap image  **/
	tbitUncompress(hTBit, (ptr + sizeof(TBitRec)));
	tmemUnlock(hm);
	Debug( _NrAllocBitmap++; )
	_DebugAssertTBit(hTBit);

	return hTBit;
#endif
}


//
//		Local And Support Functions

/*
		_CreateIdentityPalette

		This code was copied from the WinG Help file which describes
		how to create an identity palette.  It was modified slightly
		to conform to coding standards.

		OnEntry:
			hnColorData		- the RGBQUAD color data for the palette to create
			nColors			- the number of colors in the above handle

		OnExit:
			Nothing

		Returns:
			A Proper identity palette or NULL

		Side Effects:
			May move memory...
*/
HPALETTE _CreateIdentityPalette(HNATIVE hnColorData, int nColors)
{
	int 			i,j;
	int 			nStaticColors;
	int 			nUsableColors;
	RGBQUAD FAR* 	pRGB;
	PALETTE256	Palette	= {PALETTE_VERSION, NUM_PALETTE_ENTRIES};

	//*** Just use the screen DC where we need it
	HDC hdc = GetDC(HWND_DESKTOP);
	if (!hdc)
		return NULL;

	//		DeReferences the native memory - we have explicit knowledge that
	//		this palette handle is a resource!!
	pRGB = (RGBQUAD*)tmemDeRefNativeResource(hnColorData);
	TpsAssert( pRGB != NULL, "Problem with color palette resource handle");

	//		For SYSPAL_NOSTATIC, just copy the color table into
	//		a PALETTEENTRY array and replace the first and last entries
	//		with black and white
	if (GetSystemPaletteUse(hdc) != SYSPAL_STATIC)
	{
		nStaticColors	= 1;

		//		Fill in the palette with the given values, marking each
		//		as PC_NOCOLLAPSE
		for(i = 0; i < nColors; ++i)
		{
			Palette.palEntries[i].peRed	= pRGB[i].rgbRed;
			Palette.palEntries[i].peGreen	= pRGB[i].rgbGreen;
			Palette.palEntries[i].peBlue	= pRGB[i].rgbBlue;
			Palette.palEntries[i].peFlags	= PC_NOCOLLAPSE;
		}

		//		Mark any unused entries PC_NOCOLLAPSE
		//			relies on i remaining set from above loop
		for (; i < NUM_PALETTE_ENTRIES; ++i)
			Palette.palEntries[i].peFlags	= PC_NOCOLLAPSE;

		//*** Make sure the last entry is white
		//*** This may replace an entry in the array!
		Palette.palEntries[NUM_PALETTE_ENTRIES-1].peRed = 255;
		Palette.palEntries[NUM_PALETTE_ENTRIES-1].peGreen = 255;
		Palette.palEntries[NUM_PALETTE_ENTRIES-1].peBlue = 255;
		Palette.palEntries[NUM_PALETTE_ENTRIES-1].peFlags = 0;

		//*** And the first is black
		//*** This may replace an entry in the array!
		Palette.palEntries[0].peRed = 0;
		Palette.palEntries[0].peGreen = 0;
		Palette.palEntries[0].peBlue = 0;
		Palette.palEntries[0].peFlags = 0;
	}
	else
	{
		//		For SYSPAL_STATIC, get the twenty static colors into
		//		the array, then fill in the empty spaces with the
		//		given color table
		//		Get the static colors from the system palette
		int iDevCapRet	= GetDeviceCaps(hdc, RASTERCAPS);
		if (!(iDevCapRet & RC_PALETTE))
			nStaticColors=20;
		else
			nStaticColors = GetDeviceCaps(hdc, NUMCOLORS);
		
		GetSystemPaletteEntries(hdc, 0, NUM_PALETTE_ENTRIES, Palette.palEntries);

		//		Set the peFlags of the lower static colors to zero
		nStaticColors = nStaticColors / 2;

		for (i=0; i<nStaticColors; i++)
			Palette.palEntries[i].peFlags = 0;

		//		Fill in the entries from the given color table
		nUsableColors = nColors - nStaticColors;
		//			relies on i remaining set from above loop
		for (; i<nUsableColors; i++)
		{
			Palette.palEntries[i].peRed	= pRGB[i].rgbRed;
			Palette.palEntries[i].peGreen	= pRGB[i].rgbGreen;
			Palette.palEntries[i].peBlue	= pRGB[i].rgbBlue;
			Palette.palEntries[i].peFlags	= PC_NOCOLLAPSE;
		}

		//		Mark any empty entries as PC_NOCOLLAPSE
		//			relies on i remaining set from above loop
		for (; i < NUM_PALETTE_ENTRIES - nStaticColors; i++)
			Palette.palEntries[i].peFlags = PC_NOCOLLAPSE;

		//		Set the peFlags of the upper static colors to zero
		//			relies on i remaining set from above loop
		for (i = NUM_PALETTE_ENTRIES - nStaticColors; i < NUM_PALETTE_ENTRIES; i++)
			Palette.palEntries[i].peFlags = 0;
	}


	if (bCreateIdentity)	
	{
		
		//
		//		Perform a stupid loop to verify that no two colors are the same...
		//
		//		If two colors are the same, this palette will not be an identity palette
		//		and performance will crawl.....  Therefor, if a match is found, change
		//		the non-system color by increasing the intensity by 1 for red, green, and blue
		for (i = nStaticColors; i < NUM_PALETTE_ENTRIES - nStaticColors; ++i)
		{
			for (j = 0; j < NUM_PALETTE_ENTRIES; ++j)
			{
				if (i == j)
					continue;
				if ( (Palette.palEntries[i].peRed == Palette.palEntries[j].peRed) &&
						(Palette.palEntries[i].peBlue == Palette.palEntries[j].peBlue) &&
						(Palette.palEntries[i].peGreen == Palette.palEntries[j].peGreen) )
				{
					char	msg[256];
					//
					//		Try to adjust the color without causing a color wrap...
					if (Palette.palEntries[i].peRed < (uBYTE)0x80)
						++Palette.palEntries[i].peRed;
					else if (Palette.palEntries[i].peRed > (uBYTE)0x80)
						--Palette.palEntries[i].peRed;

					if (Palette.palEntries[i].peBlue < (uBYTE)0x80)
						++Palette.palEntries[i].peBlue;
					else if (Palette.palEntries[i].peBlue > (uBYTE)0x80)
						--Palette.palEntries[i].peBlue;

					if (Palette.palEntries[i].peGreen < (uBYTE)0x80)
						++Palette.palEntries[i].peGreen;
					else if (Palette.palEntries[i].peGreen > (uBYTE)0x80)
						--Palette.palEntries[i].peGreen;

					if ( (Palette.palEntries[i].peRed != Palette.palEntries[j].peRed) &&
							(Palette.palEntries[i].peBlue != Palette.palEntries[j].peBlue) &&
							(Palette.palEntries[i].peGreen != Palette.palEntries[j].peGreen) )
					{
						wsprintf(msg, "Palette Entry %d matches %d, changing %d\n", i, j, i);

						//	Reset i & j to start loop over again....
						i = nStaticColors;
						j = -1;
					}
					else
						wsprintf(msg, "Could not create identity palette. Entry %d could not be modified\n", i);
	#ifdef	TPSDEBUG
					OutputDebugString(msg);
	#endif	//	TPSDEBUG
				}
			}
		}
	}	 // if bCreateIdentity
	else
	{
		//
		//		Perform a more inteligent loop to verify that no duplicates of
		//		black or white are found...
		//
		//		If either is duplicated the masking opperations fail in 8-bit
		//		mode.....  Therefor, if a match is found, change the non-system
		//		color by increasing the intensity by 1 for red, green, and blue
		for (i = nStaticColors; i < NUM_PALETTE_ENTRIES - nStaticColors; ++i)
		{
			// Check for black...
			if ( (Palette.palEntries[i].peRed == 0) &&
				 (Palette.palEntries[i].peBlue == 0) &&
				 (Palette.palEntries[i].peGreen == 0) )
			{
				++Palette.palEntries[i].peRed;
				++Palette.palEntries[i].peGreen;
				++Palette.palEntries[i].peBlue;
			}
			// Check for white...
			else if ( (Palette.palEntries[i].peRed == 255) &&
					  (Palette.palEntries[i].peBlue == 255) &&
					  (Palette.palEntries[i].peGreen == 255) )
			{
				--Palette.palEntries[i].peRed;
				--Palette.palEntries[i].peGreen;
				--Palette.palEntries[i].peBlue;
			}
		}
	}

	//		Remember to release the DC!
	ReleaseDC(HWND_DESKTOP, hdc);

	// 	Return the palette
	return CreatePalette((LOGPALETTE *)&Palette);
}


/*
		_ClearSystemPalette

			This function will 'Blast' a pure black palette into the 
		system palette to remove the effect of any other application
		palette in the system

		OnEntry:
		OnExit:
		Returns:
				Nothing

		Side Effects:
				The system palette will be ALL BLACK
*/
void _ClearSystemPalette(void)
{
	PALETTE256	Palette			= {PALETTE_VERSION, NUM_PALETTE_ENTRIES};
	HPALETTE		ScreenPalette	= NULL;
	HDC			ScreenDC;
	int			i;

	//*** Reset everything in the system palette to black
	for(i = 0; i < NUM_PALETTE_ENTRIES; ++i)
	{
		Palette.palEntries[i].peRed	= 0;
		Palette.palEntries[i].peGreen	= 0;
		Palette.palEntries[i].peBlue	= 0;

		//	mark the entry as NOCOLLAPSE so it will not move and force others out...
		Palette.palEntries[i].peFlags = PC_NOCOLLAPSE;
	}

	//*** Create, select, realize, deselect, and delete the palette
	ScreenDC			= GetDC(HWND_DESKTOP);
	ScreenPalette	= CreatePalette((LOGPALETTE *)&Palette);
	if (ScreenDC && ScreenPalette)
	{
		//	The palette is selected in and the old palette is returned
		//		this old palette is reselected in and the new palette is
		//		then delete.
		ScreenPalette	= SelectPalette(ScreenDC, ScreenPalette, FALSE);
		RealizePalette(ScreenDC);
		ScreenPalette	= SelectPalette(ScreenDC, ScreenPalette, FALSE);
		DeleteObject(ScreenPalette);
	}
	if (ScreenDC)
		ReleaseDC(HWND_DESKTOP, ScreenDC);
}


/*
		_PaletteToColorTable

			Take a palette and return a HTMEM that corrasponds
			to the identical palette entries

		OnEntry:
			hPal		- the Windows Palette

		OnExit:
			Nothing

		Returns:
			The HTMEM consisting of the color table entries
*/
static	HTMEM		_PaletteToColorTable( HPALETTE hPal )
{
	int 				i;
	HTMEM				htColors;
	PALETTEENTRY	pal[NUM_PALETTE_ENTRIES];
	RGBQUAD FAR*	pRGB;

	htColors	= tmemAlloc( (uLONG)(NUM_PALETTE_ENTRIES * sizeof(RGBQUAD)), TMEM_AFZEROINIT);
	if (htColors)
	{
		GetPaletteEntries( hPal, 0, NUM_PALETTE_ENTRIES, pal );
		pRGB	= (RGBQUAD FAR*)tmemDeRef(htColors);

		//	Copy all of the entries into the handle
		for (i = 0; i < NUM_PALETTE_ENTRIES; ++i)
		{
			pRGB[i].rgbRed			= pal[i].peRed;
			pRGB[i].rgbBlue 		= pal[i].peBlue;
			pRGB[i].rgbGreen		= pal[i].peGreen;
			pRGB[i].rgbReserved	= 0;
		}
	}

	return htColors;
}


/*
		_TBitBltModeToRasterOp

			Take a platform independant BltMode and convert it
			to a Windows ROP code for the blt routines

		OnEntry:
			uwMode	- the TBIT_BltMode to convert

		OnExit:
			Nothing

		Returns:
			a Windows ROP code

		SideEffects:
			If it doesn't know the code (1-800-CALLATT), it returns SRCCOPY
*/
static	uLONG		_TBitBltModeToRasterOp( uWORD uwMode )
{
	uLONG	rop;
	switch (uwMode)
	{
		case TBIT_SrcCopy:
			rop	= SRCCOPY;
			break;
		case TBIT_SrcOr:
			rop	= SRCPAINT;
			break;
		case TBIT_SrcXor:
			rop	= SRCINVERT;
			break;

		case TBIT_SrcBic:
		case TBIT_NotSrcCopy:
		case TBIT_NotSrcOr:
		case TBIT_NotSrcXor:
		case TBIT_NotSrcBic:
		default:
			TpsAssertAlways("The given TBIT_Blit Mode is not yet supported");
			rop = SRCCOPY;
			break;
	}

	return rop;
}

#ifdef	TPSDEBUG
#define	TESTCODE
#ifdef	TESTCODE

//	This routine assumes tbitStartup and tbitShutdown are called by test app shell
#define	kBitWidth	101
#define	kBitHeight	101

static void _DoBlits(HTBIT hBit, int x, int y);
static void _DoDepth(uWORD uwDepth, int x, int y);

TPSAPI_DECLP( void ) tbitTestAPI(void)
{
	TpsAssert( _fTBitStarted, "Bitmap Subsystem never initialized");
//	_DoDepth(1, 0, 0);
	_DoDepth(8, 0, kBitHeight * 2);
	_DoDepth(16, 0, kBitHeight * 4);
	_DoDepth(24, 0, kBitHeight * 6);
}

static void _DoDepth(uWORD uwDepth, int x, int y)
{
	TRECT		trc, trcSrc, trcDest;
	HTBIT		hBit = NULL;
	HTBIT		hBit2 = NULL;

	trcMaketrc(&trc, 0, 0, kBitWidth, kBitHeight);
	hBit = tbitAlloc (uwDepth, &trc,
				TBIT_DefaultColorTable | TBIT_ZeroInit | TBIT_LongAlignRows);
	TpsAssert(hBit != NULL, "tbitAlloc failed");

	trcMaketrc(&trcDest, 0, 0, kBitWidth, kBitHeight);
	trcMaketrc(&trcSrc, 300, 300, 300+kBitWidth, 300+kBitHeight);
	tbitBlitFromScreen (hBit, &trcSrc, &trcDest, TBIT_SrcCopy, FALSE);
	_DoBlits(hBit, x, y);

	tbitGetRect(hBit, &trc);
	hBit2 = tbitAlloc (tbitGetDepth(hBit), &trc,
				TBIT_DefaultColorTable | TBIT_ZeroInit | TBIT_LongAlignRows);
	TpsAssert(hBit != NULL, "tbitAlloc failed");

	tbitGetRect(hBit, &trcSrc);
	tbitGetRect(hBit2, &trcDest);
	tbitBlitToBitmap (hBit, hBit2, &trcSrc, &trcDest, TBIT_SrcCopy, FALSE);
	_DoBlits(hBit2, x, y + kBitHeight);

	tbitFree(hBit2);
	hBit2 = NULL;
	tbitFree(hBit);
	hBit = NULL;
}

static void _DoBlits(HTBIT hBit, int x, int y)
{
	TRECT		trcSrc, trcDest;

	if(tbitLock(hBit))
	{
		tbitGetRect(hBit, &trcSrc);
		trcDest = trcSrc;
		trcOffset( &trcDest, x, y );
		tbitBlitToScreen (hBit, &trcSrc, &trcDest, TBIT_SrcCopy, FALSE);
		trcOffset( &trcDest, trcSrc.right, 0 );
		tbitBlitToScreen (hBit, &trcSrc, &trcDest, TBIT_SrcOr, FALSE);
		trcOffset( &trcDest, trcSrc.right, 0 );
		tbitBlitToScreen (hBit, &trcSrc, &trcDest, TBIT_SrcXor, FALSE);
		tbitUnlock(hBit);
	}
}

#endif	/* TPSDEBUG */
#endif	/* TESTCODE */