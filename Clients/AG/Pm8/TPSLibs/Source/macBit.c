/* ===========================================================================
   Module   :	macBit.c
   Project  :	TPS Tool Library
   Abstract :	Macintosh off-screen bitmap subsystem primitives.
   Status   :	Under Development
  
   Copyright © 1994, Turning Point Software.  All Rights Reserved.
  
   Modification History :
  	0.0.0	25-July-94		R. Hood		Initial revision.
  
   Note:
	Module MUST be in a segment which can NOT be swapped out!
	Module requires System 7 or greater!
	Module requires Color QuickDraw!
	Only currently works for 256 colors or less (uses indexed color tables).
	Uses one GDevice, created at startup, throughout.
	The foreground and background RGB colors of the destination bitmap had best be defined 
		or the results of the blit calls will be undefined.

   To Do:
	Implement direct color tables for greater than 256 colors.

   =========================================================================== */
   

#include      <Palettes.h>
#include      <Memory.h>
#include      <Errors.h>

//#include      "tpstypes.h"

#include	"tpsMem.h"
#include	"tpsRect.h"
#include	"tpsBit.h"		//	includes all of the above

#include	"tpsSys.h"

#include      "tErrDefs.h"

#ifdef	_WINDOWS
	#error	This is a Mac-only file...
#endif	//	 _WINDOWS

ASSERTNAME

/***  defines  ***/
#define  kMagicBitmapKey             0x3E96
#define  kMaxRowBytes                0x3FFE 			// maximum number of bytes in a row of pixels 
#define  kDefaultResolution      0x00480000 			// default resolution is 72 DPI; Fixed type 
#define  kITabResolution                  4 			// inverse-table resolution 
#define  kAllFlagsMASK          (TBIT_ZeroInit | TBIT_Virtual | TBIT_Purgeable | TBIT_LongAlignRows | TBIT_DefaultColorTable | TBIT_16ByteAlignRows)

/***  type defs  ***/
typedef	struct  
{
   sWORD               TBit;							// internal color code
   sWORD			    QD;								// QuickDraw color code
   RGBColor			    RGB;							// 32Bit QuickDraw RGB color 
} TBitColor;
typedef	struct  
{
   sWORD               Font;							// font number
   sWORD			    Face;							// typeface style
   sWORD			    Mode;							// bit transfer mode
   sWORD			    Size;							// typeface size
} TextState;
typedef	struct  
{
   CGrafPtr             pPort;							// color graph port
   PixMapHandle			hOldPixMap;						// original fake port's pixmap
   HTBIT			    hInUseBy;						// port in use by flag/pointer
} TBitPort;
typedef struct
{
   uWORD               Flags;    						// state flags
   uWORD               Depth;							// bits per pixel
   sWORD               InPort;							// internal port in use by bitmap or -1
   sWORD               BytesPerRow;					// bytes per row 
   Rect                 Bounds;         				// bounding box of bitmap (must have {0,0} origin)
   PixMapHandle         hPixMap;      					// handle to pixel map
   Handle               hDataBits;      				// handle to pixel image
   GDHandle             hGDevice; 						// handle to GDevice
   RGBColor             ForeColor;						// foreground color
   RGBColor             BackColor;						// background color
   PenState             Pen;							// pen location, size, mode, and pattern
   TextState            Text;							// text font, face, mode, and size
   uWORD               LockCount;    					// lock count of bitmap
   uWORD				MagicKey;						// bitmap magic
} TBitRec;
typedef  TBitRec       *TBitPtr;

/***  macros  ***/
#define  TBitDeRef(h)   ((TBitPtr)tmemDeRef((h)))
#ifdef DEBUG
#define  IFF(c)         if(c)
#else
#define  IFF(c)
#define  _DebugAssertTBit(x)
#endif 

/**  local prototypes  **/
static   OSErr          _SetUpPixMap (short, Rect *, CTabHandle, short, PixMapHandle);
static   OSErr          _CreateGDevice (PixMapHandle, GDHandle);
static   sWORD         _FindFreePort (void);
static   void           _ApplyTransparentColor (RGBColorPtr);
#ifdef TPSDEBUG
static   void           _DebugAssertTBit (HTBIT hTBit);
#endif // DEBUG 

/**  debug local globals  **/
#ifdef TPSDEBUG
static   sWORD	    	_NrAllocBitmap = 0;				// number of bitmaps allocated by subsystem
#endif // DEBUG 

/**  local globals  **/
static  GrafPtr         _pScreenPort = nil;				// GrafPort of screen
static  GrafPtr         _pStartupPort = nil;			// GrafPort at startup used for save/restore
static  GDHandle        _hStartupDevice = nil;			// GDevice at startup used for save/restore
static  GDHandle        _hTBitDevice = nil;				// GDevice used by off-screen bitmap subsystem
static  CTabHandle      _hColorTable = nil; 			// handle to default off-screen color table 
static  sWORD          _TransparentColor = 0;			// color used for transparent copy bits
static  sWORD	    	_NrTPorts = 0;					// number of entries in internal port list
static  TBitPort	    *_TPorts = nil;					// internal port list
static	Boolean			fHasColorQuickdraw = FALSE;

/* ----------------------------------------------------------------------------
   tbitStartup 
   Initialize TPS off-screen bitmap subsystem at startup. 
   Returns error code indicating success.                
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( BOOLEAN )  tbitStartup (uWORD NrPorts, uWORD depth, HNATIVE hColor)
{
sWORD				i;
Rect				Bounds;
OSErr				osError = noErr; 
SignedByte			saveState; 
PixMapHandle 		hPixMap = nil;						// handle to new off-screen PixMap 
short				bytesPerRow; 						// bytes per row in PixMap 

	fHasColorQuickdraw = tsysMacHasCQD(true);
/**  validations/initializations  **/
	TpsAssert((_TPorts == nil), "Bitmap subsystem was already been started up!");
	TpsAssert((hColor != nil), "Bitmap subsystem color table must be provided!");
	TpsAssert(tsysMacHasSys7(), "System 7 or greater required!");
	TpsAssert(fHasColorQuickdraw, "32Bit Color QuickDraw required!");
	TpsAssert(tsysMacHasGWorlds(), "QuickDraw Color GWorlds required!");
	GetPort(&_pStartupPort);
	_hStartupDevice = GetGDevice();

/**  clone the clut (to be used throughout)  **/
    saveState = HGetState((Handle)hColor);				// save color table’s current state
    HNoPurge((Handle)hColor);							// make sure it isn’t purgeable 
    _hColorTable = (CTabHandle)hColor;
    osError = HandToHand((Handle *)&_hColorTable);
    HSetState((Handle)hColor, saveState);				// restore color table’s state
    if (osError != noErr)
	{
//		terrSetErrorCode(TERR_TMEMSECTION, TMEM_ERRALLOC);
		return FALSE;
	}

/**  create device (to be used throughout)  **/
	//SetRect(&Bounds, 0, 0, 1, 1);
	Bounds.left = 0;
	Bounds.top = 0;
	Bounds.right = 1;
	Bounds.bottom = 1;

	bytesPerRow = 16;									// 16 byte align rowbytes
    if ((hPixMap = (PixMapHandle)NewHandleClear(sizeof(PixMap))) == nil)
		goto Failure;
	if (_SetUpPixMap(depth, &Bounds, _hColorTable, bytesPerRow, hPixMap) != noErr)
		goto Failure;
    if ((_hTBitDevice = (GDHandle)NewHandleClear(sizeof(GDevice))) == nil)
		goto Failure;
	if (_CreateGDevice(hPixMap, _hTBitDevice) != noErr)
		goto Failure;

/**  create internal port list  **/
	if ((_TPorts = (TBitPort *)NewPtrClear(sizeof(TBitPort) * (NrPorts + 1))) == nil)
		goto Failure;
	
/**  create pool of off-screen ports  **/
	//SetRect(&Bounds, 0, 0, 1, 1);
	Bounds.left = 0;
	Bounds.top = 0;
	Bounds.right = 1;
	Bounds.bottom = 1;

	for (i = 0; i < NrPorts; i++)
	{
		if ((_TPorts[i].pPort = (CGrafPtr)NewPtrClear(sizeof(CGrafPort))) == nil)
			goto Failure;
	    OpenCPort(_TPorts[i].pPort); 
		_TPorts[i].hOldPixMap = (_TPorts[i].pPort)->portPixMap;
		SetPortPix(nil);
	}

/**  clean up and out  **/
	SetPort(_pStartupPort);
	SetGDevice(_hStartupDevice);
	_NrTPorts = NrPorts; 
	return TRUE;

Failure:
    if (hPixMap != nil)  
    {
    	if ((**hPixMap).pmTable != nil)  
            DisposCTable((**hPixMap).pmTable);
    	if ((**hPixMap).baseAddr != nil)  
            DisposHandle((Handle)(**hPixMap).baseAddr);
        DisposHandle((Handle)hPixMap);
    }
    if (_hTBitDevice != nil)
    {
    	if ((**_hTBitDevice).gdITable != nil)
			DisposHandle((Handle)(**_hTBitDevice).gdITable);
        DisposHandle((Handle)_hTBitDevice);
		_hTBitDevice = nil;
    }
	if (_TPorts != nil)
	{
		for (i = 0; i < NrPorts; i++)
		{
			if (_TPorts[i].pPort != nil)
			{
				SetPort((GrafPtr)_TPorts[i].pPort);
				SetPortPix(_TPorts[i].hOldPixMap);
				CloseCPort(_TPorts[i].pPort);
			}
		}
		tmemFreeNativePtr(_TPorts);
		_TPorts = nil;
	}
    if (_hColorTable != nil)
		DisposCTable(_hColorTable);
	SetGDevice(_hStartupDevice);
	SetPort(_pStartupPort);
//	terrSetErrorCode(TERR_TMEMSECTION, TMEM_ERRALLOC);
	return FALSE;
}


/* ----------------------------------------------------------------------------
   tbitShutdown 
   Terminate TPS off-screen bitmap subsystem at shutdown. 
   Returns nothing.                
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( void )  tbitShutdown (void)
{
sWORD				i;
PixMapHandle 	hPixMap; 

	for (i = 0; i < _NrTPorts; i++)
	{
		if (_TPorts[i].pPort != nil)
		{
			SetPort((GrafPtr)_TPorts[i].pPort);
			SetPortPix(_TPorts[i].hOldPixMap);
			CloseCPort(_TPorts[i].pPort);
		}
	}
	tmemFreeNativePtr(_TPorts);
	_TPorts = nil;
	_NrTPorts = 0;
    if (_hTBitDevice != nil)
    {
    	hPixMap = (**_hTBitDevice).gdPMap;
    	if (hPixMap != nil)  
    	{
    		if ((**hPixMap).pmTable != nil)  
            	DisposCTable((**hPixMap).pmTable);
    		if ((**hPixMap).baseAddr != nil)  
            	DisposHandle((Handle)(**hPixMap).baseAddr);
        	DisposHandle((Handle)hPixMap);
    	}
    	if ((**_hTBitDevice).gdITable != nil)
        	DisposHandle((Handle)(**_hTBitDevice).gdITable);
        DisposHandle((Handle)_hTBitDevice);
		_hTBitDevice = nil;
    }
    if (_hColorTable != nil)
		DisposCTable(_hColorTable);
    _hColorTable = nil;
	TpsAssertW((_NrAllocBitmap == 0), "Internal bipmaps not freed by application!", &_NrAllocBitmap, 1);
	SetGDevice(_hStartupDevice);
	SetPort(_pStartupPort);
}


/* ----------------------------------------------------------------------------
   tbitSetMainScreen
   Set screen use by subsystem.
   Returns nothing.                
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( void )  tbitSetMainScreen (LPVOID screen)
{
	_pScreenPort = (GrafPtr)screen;
}


/* ----------------------------------------------------------------------------
   tbitSetTransparentColor
    Set transparent color use by subsystem.
   Returns nothing.                
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( void )  tbitSetTransparentColor (sWORD color)
{
	TpsAssert((color == TBIT_White || color == TBIT_Black || color == TBIT_Red || color == TBIT_Green || color == TBIT_Blue), "Illegal transparent color!");
	_TransparentColor = color;
}


/* ----------------------------------------------------------------------------
   _ApplyTransparentColor
   Set destination port background color to transparent color.
   Returns nothing.                
   ---------------------------------------------------------------------------- */
static  void  _ApplyTransparentColor (RGBColorPtr pBackColor)
{
RGBColor            rgbColor;
short               qdColor;

	GetBackColor(pBackColor);
	if (fHasColorQuickdraw) //fColorQuickdrawAvailable(true))
	{
		switch (_TransparentColor)
		{
			case TBIT_White:
				rgbColor.red = 0xFFFF;
				rgbColor.green = 0xFFFF;
				rgbColor.blue = 0xFFFF;
				break;
			case TBIT_Black:
				rgbColor.red = 0x0000;
				rgbColor.green = 0x0000;
				rgbColor.blue = 0x0000;
				break;
			case TBIT_Red:
				rgbColor.red = 0xDD6B;
				rgbColor.green = 0x08C2;
				rgbColor.blue = 0x06A2;
				break;
			case TBIT_Green:
				rgbColor.red = 0x0000;
				rgbColor.green = 0x8000;
				rgbColor.blue = 0x11B0;
				break;
			case TBIT_Blue:
				rgbColor.red = 0x0000;
				rgbColor.green = 0x0000;
				rgbColor.blue = 0xD400;
				break;
			default:
				rgbColor.red = 0x0000;
				rgbColor.green = 0x0000;
				rgbColor.blue = 0x0000;
				break;
		}
		RGBBackColor(&rgbColor);
	}
	else
	{
		switch (_TransparentColor)
		{
			case TBIT_White:
				qdColor = whiteColor;
				break;
			case TBIT_Black:
				qdColor = blackColor;
				break;
			case TBIT_Red:
				qdColor = redColor;
				break;
			case TBIT_Green:
				qdColor = greenColor;
				break;
			case TBIT_Blue:
				qdColor = blueColor;
				break;
			default:
				qdColor = blackColor;
				break;
		}
		BackColor(qdColor);
	}
}


/* ----------------------------------------------------------------------------
   tbitAlloc 
   Open a bitmap. 
   Returns trasparent handle to an internal bitmap or nil.                
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( HTBIT )  tbitAlloc (uWORD depth, TRECT *Bounds, uWORD flags)
{
HTBIT				hTBit;
short				bytesPerRow; 						// bytes per row in PixMap 
TBitPtr				pTBit;
Rect				box;
PixMapHandle 		hPixMap = nil;						// handle to new off-screen PixMap 
Handle     			hDataBits;  

/**  validations  **/
	TpsAssert(((flags & ~kAllFlagsMASK) == 0), "Bad flags!");
	TpsAssert(((flags & TBIT_Virtual) == 0), "Virtual bitmaps not supported!");
	TpsAssert(((flags & TBIT_Purgeable) == 0), "Purgable bitmaps not supported!");
	TpsAssert(((flags & TBIT_DefaultColorTable) != 0), "Default color table is the only option!");
	TpsAssert((depth == 1 || depth == 2 || depth == 4 || depth == 8 || depth == 16 || depth == 32), "Bad depth!");
	TpsAssert((Bounds != nil), "Bounding box not existant!");
	TpsAssert(((Bounds->top == 0) && (Bounds->left == 0)), "Bounding box not zero based!");
	TpsAssert(((Bounds->bottom > 0) && (Bounds->right > 0)), "Bounding box empty!");

/**  compute row bytes  **/
	bytesPerRow = tbitCalcRowBytes((Bounds->right - Bounds->left), depth, flags);
	TpsAssert((bytesPerRow < kMaxRowBytes), "Bytes per row > 16,382!");	// maximum number of bytes per row is 16,382 

/**  allocate a new PixMap  **/
	SetRect(&box, Bounds->left, Bounds->top, Bounds->right, Bounds->bottom);
    if ((hPixMap = (PixMapHandle)NewHandleClear(sizeof(PixMap))) == nil)
        return nil;
	if (_SetUpPixMap(depth, &box, _hColorTable, bytesPerRow, hPixMap) != noErr)
	{
        if (hPixMap != nil)  
            DisposHandle((Handle)hPixMap);
        return nil;
	}
	hDataBits = (Handle)(**hPixMap).baseAddr;			// handle to pixel image

/**  create internal bitmap record  **/
//	if ((hTBit = tmemAlloc(sizeof(TBitRec), (TMEM_AFNONPURGEABLE | TMEM_AFNONVIRTUAL | TMEM_AFNONSUBALLOC | TMEM_AFZEROINIT))) == nil)
	if ((hTBit = tmemAlloc(sizeof(TBitRec), TMEM_AFZEROINIT)) == nil)
	{
        if (hPixMap != nil)  
        {
            DisposCTable((**hPixMap).pmTable);
            DisposHandle((Handle)hDataBits);
            DisposHandle((Handle)hPixMap);
        }
		return nil;
	}	

/**  load bitmap data  **/
	pTBit = TBitDeRef(hTBit);
	pTBit->Flags = 0;    								// no state flags yet
	pTBit->Depth = depth;								// bits per pixel
	pTBit->InPort = -1;									// no internal port in use yet
	pTBit->BytesPerRow = bytesPerRow; 					// bytes per row
	pTBit->Bounds = box;         						// bounding box 
	pTBit->hPixMap = hPixMap;      						// pixel image
	pTBit->hDataBits = hDataBits;						// reference to image 
	pTBit->hGDevice = _hTBitDevice;						// reference to subsystem device 
	pTBit->ForeColor.red = 0x000;						// initailize foreground color to black
	pTBit->ForeColor.green = 0x000;
	pTBit->ForeColor.blue = 0x000;
	pTBit->BackColor.red = 0xFFFF;						// initailize background color to white
	pTBit->BackColor.green = 0xFFFF;
	pTBit->BackColor.blue = 0xFFFF;
	SetPt(&pTBit->Pen.pnLoc, 0, 0);						// pen normal
	SetPt(&pTBit->Pen.pnSize, 1, 1);
	pTBit->Pen.pnMode = patCopy;
	pTBit->Pen.pnPat = qd.black;
	pTBit->Text.Font = 0;								// default system font 
	pTBit->Text.Face = 0;								// plain
	pTBit->Text.Mode = srcCopy;							// overwrite
	pTBit->Text.Size = 0;								// default font size
	pTBit->LockCount = 0;    							// unlocked  
	pTBit->MagicKey = kMagicBitmapKey;					// bitmap magic
	Debug( _NrAllocBitmap++; )
	_DebugAssertTBit(hTBit);

	return hTBit;
}


/* ----------------------------------------------------------------------------
   tbitFree 
   Close bitmap. 
   Returns nothing.                
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( void )  tbitFree (HTBIT hTBit)
{
TBitPtr				pTBit;

	_DebugAssertTBit(hTBit);
	TpsAssertL(((TBitDeRef(hTBit))->LockCount == 0), "Free operation on locked bitmap!", &hTBit, 1);
	pTBit = (TBitPtr)tmemLock(hTBit);
    if (pTBit->hPixMap != nil)  
    {
        DisposCTable((**(pTBit->hPixMap)).pmTable);
        DisposHandle((Handle)pTBit->hPixMap);
    }
    if (pTBit->hDataBits != nil && *(pTBit->hDataBits) != nil)  
        DisposHandle(pTBit->hDataBits);
	tmemUnlock(hTBit);
	IFF (hTBit != nil)
		tmemZeroBlock(tmemDeRef(hTBit), sizeof(TBitRec));				// paranoia
	tmemFree(hTBit);
	Debug( _NrAllocBitmap--; )
}


/* ----------------------------------------------------------------------------
   tbitLock 
   Assign bitmap a port, lock data bits and SetPort. 
   Returns subsystem error code.                
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( BOOLEAN )  tbitLock (HTBIT hTBit)
{
TBitPtr				pTBit;
sWORD				imp;
CGrafPtr            port;

/**  already locked  **/
	_DebugAssertTBit(hTBit);
	pTBit = TBitDeRef(hTBit);
	if (pTBit->LockCount > 0)
	{
		pTBit->LockCount++;
		port = _TPorts[pTBit->InPort].pPort;
		SetPort((GrafPtr)port);
		return TRUE;
	}

/**  get a port  **/
	if ((imp = _FindFreePort()) < 0)
	{
//		terrSetErrorCode(TERR_TBITSECTION, TBIT_ERROUTOFPORTS);
		return FALSE;
	}

/**  lock the image  **/
	pTBit = TBitDeRef(hTBit);
    HLock(pTBit->hDataBits);    							// lock the handle to the pixel image 
    (**(pTBit->hPixMap)).baseAddr = *(pTBit->hDataBits);	// put pixel image master pointer into baseAddr so that QuickDraw can use it
	pTBit->InPort = imp;
	pTBit->LockCount++;

/**  set the port  **/
	port = _TPorts[imp].pPort;
	pTBit = TBitDeRef(hTBit);
	SetPort((GrafPtr)port);
	SetPortPix(pTBit->hPixMap);
	_TPorts[imp].hInUseBy = hTBit;

/**  adjust port data  **/
	pTBit = TBitDeRef(hTBit);
	RectRgn(port->visRgn, &(pTBit->Bounds));
	ClipRect(&(pTBit->Bounds));
	PortSize((pTBit->Bounds.right - pTBit->Bounds.left), (pTBit->Bounds.bottom - pTBit->Bounds.top));
	SetPenState(&(pTBit->Pen));
	RGBForeColor(&pTBit->ForeColor);
	RGBBackColor(&pTBit->BackColor);
#ifndef NUMBERS
	TextFont(pTBit->Text.Font);
	TextFace(pTBit->Text.Face);
	TextMode(pTBit->Text.Mode);
	TextSize(pTBit->Text.Size);
#endif

	return TRUE;
}


/* ----------------------------------------------------------------------------
   tbitUnlock 
   
   Returns nothing.                 
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( void )  tbitUnlock (HTBIT hTBit)
{
TBitPtr				pTBit;
GrafPtr				savePort;
sWORD				imp;
CGrafPtr            port;
RGBColor            foreColor;
RGBColor            backColor;
PenState            pen; 
#ifndef NUMBERS
TextState           text; 
#endif

/**  still locked  **/
	_DebugAssertTBit(hTBit);
	pTBit = TBitDeRef(hTBit);
	pTBit->LockCount--;
	imp = pTBit->InPort;
	port = _TPorts[imp].pPort;
	GetPort(&savePort);
	if (pTBit->LockCount > 0)
	{
		if (savePort == (GrafPtr)port)
			if (_pScreenPort != nil)
				SetPort(_pScreenPort);
			else 
				SetPort(_pStartupPort);
		return;
	}

/**  save off port data  **/
	SetPort((GrafPtr)port);
	GetForeColor(&foreColor);
	GetBackColor(&backColor);
	GetPenState(&pen);
#ifndef NUMBERS
	text.Font = port->txFont;
	text.Face = port->txFace;
	text.Mode = port->txMode;
	text.Size = port->txSize;
#endif
	SetPortPix(nil);

/**  reset the port  **/
	if (savePort != (GrafPtr)port)
		SetPort(savePort);
	else if (_pScreenPort != nil)
		SetPort(_pScreenPort);
	else 
		SetPort(_pStartupPort);
	_TPorts[imp].hInUseBy = nil;

/**  unlock the image  **/
	pTBit = TBitDeRef(hTBit);
	HUnlock(pTBit->hDataBits); 
    (**(pTBit->hPixMap)).baseAddr = nil; 
	pTBit->InPort = -1;
	pTBit->ForeColor = foreColor;
	pTBit->BackColor = backColor;
	pTBit->Pen = pen;
#ifndef NUMBERS
	pTBit->Text = text;
#endif
	pTBit->LockCount = 0;
}


/* ----------------------------------------------------------------------------
   tbitGetBytesPerRow 
   
   Returns number of bytes per row in bitmap.
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( uWORD )  tbitGetBytesPerRow (HTBIT hTBit)
{
	_DebugAssertTBit(hTBit);
	return (TBitDeRef(hTBit))->BytesPerRow;
}


/* ----------------------------------------------------------------------------
   tbitGetRect 
   
   Returns nothing.
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( void )  tbitGetRect (HTBIT hTBit, TRECT *rect)
{
TBitPtr				pTBit;

	_DebugAssertTBit(hTBit);
	pTBit = TBitDeRef(hTBit);
	rect->top = pTBit->Bounds.top;
	rect->left = pTBit->Bounds.left;
	rect->right = pTBit->Bounds.right;
	rect->bottom = pTBit->Bounds.bottom;
}


/* ----------------------------------------------------------------------------
   tbitGetDepth 
   Get image depth of off-screen bitmap.
   Returns bits per pixel depth of bitmap.
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( uWORD )  tbitGetDepth (HTBIT hTBit)
{
	_DebugAssertTBit(hTBit);
	return (TBitDeRef(hTBit))->Depth;
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

	swInPort	= TBitDeRef(hTBit)->InPort;

	TpsAssert( swInPort != TBIT_InvalidPortID, 
					"The Bitmap is not associated with any ports");
	TpsAssert( swInPort < _NrTPorts, "The Bitmap port is not valid");
	TpsAssert( _TPorts[ swInPort ].hInUseBy == hTBit, 
					"The Port does not really use this bitmap");

	*((CGrafPtr*)lpDevice) = _TPorts[ swInPort ].pPort;
}


/* ----------------------------------------------------------------------------
   tbitGetBaseAddr 
   Get image base address of off-screen bitmap.
   Returns pointer to start of bitmap image.
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( LPVOID )  tbitGetBaseAddr (HTBIT hTBit)
{
	_DebugAssertTBit(hTBit);
	return *(TBitDeRef(hTBit))->hDataBits; 
}


/* ----------------------------------------------------------------------------
   tbitGetImageHandle
   Get image pixmap of off-screen bitmap.
   Returns native handle to pixmap image.
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( HNATIVE )  tbitGetImageHandle (HTBIT hTBit)
{
	_DebugAssertTBit(hTBit);
	return (HNATIVE)((TBitDeRef(hTBit))->hPixMap); 
}


/* ----------------------------------------------------------------------------
   tbitBlitToBitmap 
   Copy image from off-screen bitmap to off-screen bitmap.
   Returns subsystem error code.                
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( BOOLEAN )  tbitBlitToBitmap (HTBIT hSrc, HTBIT hDest, TRECT *rSrc, TRECT *rDest, uWORD mode, BOOLEAN fTransparent)
{
BOOLEAN				fSrcWasLocked = false;
BOOLEAN				fDestWasLocked = false;
GrafPtr				savePort = nil;
CGrafPtr			srcPort = nil;
CGrafPtr			destPort = nil;
Rect				srcRect, destRect;
TRECT				TRect;
sWORD				imp;
RGBColor            oldBackColor;

/**  validations  **/
	_DebugAssertTBit(hSrc);
	_DebugAssertTBit(hDest);
	GetPort(&savePort);

/**  get blit area in Mac OS rect  **/
	if (rSrc == nil)
		tbitGetRect(hSrc, &TRect);
	else
		TRect = *rSrc;
	//SetRect(&srcRect, TRect.left, TRect.top, TRect.right, TRect.bottom);
	srcRect.left = TRect.left;
	srcRect.right = TRect.right;
	srcRect.top = TRect.top;
	srcRect.bottom = TRect.bottom;
	
	if (rDest == nil)
		tbitGetRect(hDest, &TRect);
	else
		TRect = *rDest;
	//SetRect(&destRect, TRect.left, TRect.top, TRect.right, TRect.bottom);
	destRect.left = TRect.left;
	destRect.right = TRect.right;
	destRect.top = TRect.top;
	destRect.bottom = TRect.bottom;
	
	if (EmptyRect(&srcRect) || EmptyRect(&destRect)) 
		return TRUE;

/**  lock bitmaps  **/
	if ((TBitDeRef(hSrc))->InPort > -1)
		fSrcWasLocked = true;
	else if (!tbitLock(hSrc))
		return FALSE;
	if ((TBitDeRef(hDest))->InPort > -1)
		fDestWasLocked = true;
	else if (!tbitLock(hDest))
	{
		tbitUnlock(hSrc);
		return FALSE;
	}

/**  get ports  **/
	imp = (TBitDeRef(hSrc))->InPort;
	srcPort = _TPorts[imp].pPort;
	imp = (TBitDeRef(hDest))->InPort;
	destPort = _TPorts[imp].pPort;

/**  do copy  **/
	SetPort((GrafPtr)destPort);							// use clip region of destination port
	if (fTransparent)									// set destination port background color to transparent color
	{
		_ApplyTransparentColor(&oldBackColor);
		mode += transparent;
	}
	CopyBits(&(((GrafPtr)srcPort)->portBits), &(((GrafPtr)destPort)->portBits), &srcRect, &destRect, mode, nil);
	if (fTransparent)
		RGBBackColor(&oldBackColor);

/**  cleanup  **/
	if (!fSrcWasLocked)
		tbitUnlock(hSrc);
	if (!fDestWasLocked)
		tbitUnlock(hDest);
	SetPort(savePort);

	return TRUE;
}


/* ----------------------------------------------------------------------------
   tbitBlitToScreen 
   Copy image from off-screen bitmap to screen.
   Returns subsystem error code.                
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( BOOLEAN )  tbitBlitToScreen (HTBIT hSrc, TRECT *rSrc, TRECT *rDest, uWORD mode, BOOLEAN fTransparent)
{
BOOLEAN				fWasLocked = false;
GrafPtr				savePort = nil;
CGrafPtr			port = nil;
sWORD				imp;
Rect				srcRect, destRect;
TRECT				TRect;
RGBColor            oldBackColor;

/**  validations  **/
	_DebugAssertTBit(hSrc);
	TpsAssert((_pScreenPort != nil), "Blit to undefined screen!");
	GetPort(&savePort);

/**  get blit area in Mac OS rect  **/
	if (rSrc == nil)
		tbitGetRect(hSrc, &TRect);
	else
		TRect = *rSrc;
	//SetRect(&srcRect, TRect.left, TRect.top, TRect.right, TRect.bottom);
	srcRect.left = TRect.left;
	srcRect.right = TRect.right;
	srcRect.top = TRect.top;
	srcRect.bottom = TRect.bottom;

	if (rDest == nil)
		destRect = _pScreenPort->portRect;  
	else
		{
		//SetRect(&destRect, rDest->left, rDest->top, rDest->right, rDest->bottom);
		destRect.left = rDest->left;
		destRect.right = rDest->right;
		destRect.top = rDest->top;
		destRect.bottom = rDest->bottom;
		}
	if (EmptyRect(&srcRect) || EmptyRect(&destRect)) 
		return TRUE;

/**  lock bitmap  **/
	if ((TBitDeRef(hSrc))->InPort > -1)
		fWasLocked = true;
	else if (! tbitLock(hSrc))
		return FALSE;

/**  get port  **/
	imp = (TBitDeRef(hSrc))->InPort;
	port = _TPorts[imp].pPort;

/**  do copy  **/
	SetPort(_pScreenPort);								// use clip region of destination port
	if (fTransparent)									// set destination port background color to transparent color
	{
		_ApplyTransparentColor(&oldBackColor);
		mode += transparent;
	}
	CopyBits(&(((GrafPtr)port)->portBits), &(_pScreenPort->portBits), &srcRect, &destRect, mode, nil);
	if (fTransparent)
		RGBBackColor(&oldBackColor);

/**  cleanup  **/
	if (!fWasLocked)
		tbitUnlock(hSrc);
	SetPort(savePort);

	return TRUE;
}


/* ----------------------------------------------------------------------------
   tbitBlitFromScreen 
   Copy image from screen to off-screen bitmap.
   Returns subsystem error code.                
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( BOOLEAN )  tbitBlitFromScreen (HTBIT hDest, TRECT *rSrc, TRECT *rDest, uWORD mode, BOOLEAN fTransparent)
{
BOOLEAN				fWasLocked = false;
GrafPtr				savePort = nil;
CGrafPtr			port = nil;
sWORD				imp;
Rect				srcRect, destRect;
TRECT				TRect;
RGBColor            oldBackColor;

/**  validations  **/
	_DebugAssertTBit(hDest);
	TpsAssert((_pScreenPort != nil), "Blit from undefined screen!");
	GetPort(&savePort);

/**  get blit area in Mac OS rect  **/
	if (rSrc == nil)
		srcRect = _pScreenPort->portRect;  
	else
		{
		//SetRect(&srcRect, rSrc->left, rSrc->top, rSrc->right, rSrc->bottom);
		srcRect.left = rSrc->left;
		srcRect.right = rSrc->right;
		srcRect.top = rSrc->top;
		srcRect.bottom = rSrc->bottom;
		}
	if (rDest == nil)
		tbitGetRect(hDest, &TRect);
	else
		TRect = *rDest;
	//SetRect(&destRect, TRect.left, TRect.top, TRect.right, TRect.bottom);
	destRect.left = TRect.left;
	destRect.right = TRect.right;
	destRect.top = TRect.top;
	destRect.bottom = TRect.bottom;

	if (EmptyRect(&srcRect) || EmptyRect(&destRect)) 
		return TRUE;

/**  lock bitmap  **/
	if ((TBitDeRef(hDest))->InPort > -1)
		fWasLocked = true;
	else if (! tbitLock(hDest))
		return FALSE;

/**  get port  **/
	imp = (TBitDeRef(hDest))->InPort;
	port = _TPorts[imp].pPort;

/**  do copy  **/
	SetPort((GrafPtr)port);								// use clip region of destination port
	if (fTransparent)									// set destination port background color to transparent color
	{
		_ApplyTransparentColor(&oldBackColor);
		mode += transparent;
	}
	CopyBits(&(_pScreenPort->portBits), &(((GrafPtr)port)->portBits), &srcRect, &destRect, mode, nil);
	if (fTransparent)
		RGBBackColor(&oldBackColor);

/**  cleanup  **/
	if (!fWasLocked)
		tbitUnlock(hDest);
	SetPort(savePort);

	return TRUE;
}


/* ----------------------------------------------------------------------------
   tbitScrollRect 
   Scroll bitmap. 
   Returns nothing.                                        
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( BOOLEAN )  tbitScrollRect (HTBIT hBit, TRECT *rBit, TPT delta)
{
BOOLEAN				fRetVal	= TRUE;
BOOLEAN				fWasLocked = false;
GrafPtr     		savePort;
CGrafPtr			port = nil;
sWORD				imp;
Rect				bitRect;
TRECT				TRect;
RgnHandle			hRgn;

/**  validations  **/
	_DebugAssertTBit(hBit);
	if (delta.x == 0 && delta.y == 0)
		return TRUE;
	GetPort(&savePort);

/**  get scroll area in Mac OS rect  **/
	if (rBit == nil)
		tbitGetRect(hBit, &TRect);
	else
		TRect = *rBit;
	//SetRect(&bitRect, TRect.left, TRect.top, TRect.right, TRect.bottom);
	bitRect.left = TRect.left;
	bitRect.right = TRect.right;
	bitRect.top = TRect.top;
	bitRect.bottom = TRect.bottom;

	if (EmptyRect(&bitRect)) 
		return TRUE;

/**  lock bitmap  **/
	if ((TBitDeRef(hBit))->InPort > -1)
		fWasLocked = true;
	else if (! tbitLock(hBit))
		return FALSE;

/**  get port  **/
	imp = (TBitDeRef(hBit))->InPort;
	port = _TPorts[imp].pPort;

/**  do scroll  **/
	SetPort((GrafPtr)port);
	if ((hRgn = NewRgn()) == nil)
		fRetVal = FALSE;
	else
	{
		ScrollRect(&bitRect, delta.x, delta.y, hRgn); 
		DisposeRgn(hRgn);
	}

/**  cleanup  **/
	if (!fWasLocked)
		tbitUnlock(hBit);
	SetPort(savePort);

	return fRetVal;
}


/* ----------------------------------------------------------------------------
   _CreateGDevice 
   
   Returns system error code.
   ---------------------------------------------------------------------------- */
static  OSErr  _CreateGDevice (
    PixMapHandle 	basePixMap,  						// handle to the PixMap to base GDevice on
    GDHandle     	hGDevice) 							// handle to the new GDevice 
{
ITabHandle 			embryoITab = nil; 					// Handle to the embryonic inverse table 
Rect       			deviceRect; 						// Rectangle of GDevice 
OSErr				osError = noErr;       				// error code 

/**  allocate the embryonic inverse table  **/
	if ((embryoITab = (ITabHandle)NewHandleClear(2)) == nil)
        return MemError();

/**  set rectangle of device to PixMap bounds  **/
	deviceRect = (**basePixMap).bounds;

/**  initialize the new GDevice fields  **/
	(**hGDevice).gdRefNum = 0;	    					// Only used for screens 
	(**hGDevice).gdID = 0;								// Won’t normally use 
	if ((**basePixMap).pixelSize <= 8)
		(**hGDevice).gdType = clutType;   				// Depth≤8; clut device 
	else
		(**hGDevice).gdType = directType; 				// Depth>8; direct device 
	(**hGDevice).gdITable = embryoITab;   				// 2-byte handle for now 
	(**hGDevice).gdResPref = kITabResolution;    		// Normal inv table res 
	(**hGDevice).gdSearchProc = nil;      				// No color-search proc 
	(**hGDevice).gdCompProc = nil;						// No complement proc 
	(**hGDevice).gdFlags = 0;             				// Will set these later 
    (**hGDevice).gdPMap = basePixMap;     				// Reference our PixMap 
	(**hGDevice).gdRefCon = 0;            				// Won’t normally use 
	(**hGDevice).gdNextGD = nil;          				// Not in GDevice list 
	(**hGDevice).gdRect = deviceRect;     				// Use PixMap dimensions 
	(**hGDevice).gdMode = -1;             				// For nonscreens 
	(**hGDevice).gdCCBytes = 0;           				// Only used for screens 
	(**hGDevice).gdCCDepth = 0;           				// Only used for screens 
	(**hGDevice).gdCCXData = 0;           				// Only used for screens 
	(**hGDevice).gdCCXMask = 0;           				// Only used for screens 
	(**hGDevice).gdReserved = 0;          				// Currently unused 

/**  set color-device bit if PixMap isn’t black & white  **/
	if ((**basePixMap).pixelSize > 1)
		SetDeviceAttribute(hGDevice, gdDevType, true);

/**  set bit to indicate that the GDevice has no video driver  **/
	 SetDeviceAttribute(hGDevice, noDriver, true);

/**  initialize the inverse table  **/
	if ((**basePixMap).pixelSize <= 8)
	{
		MakeITable((**basePixMap).pmTable, (**hGDevice).gdITable, (**hGDevice).gdResPref);
		osError = QDError();
	}

/**  handle any errors along the way  **/
    if (osError != noErr) 
    {
        if (embryoITab != nil)
            DisposHandle((Handle)embryoITab);
    }
    return osError;
}


/* ----------------------------------------------------------------------------
   _SetUpPixMap 
   
   Returns          
   ---------------------------------------------------------------------------- */
static  OSErr  _SetUpPixMap (
    short        	depth,       						// desired number of bits/pixel in off-screen 
    Rect         	*bounds,     						// bounding rectangle of off-screen 
    CTabHandle   	colors,      						// color table to assign to off-screen 
    short        	bytesPerRow, 						// number of bytes per row in the PixMap 
    PixMapHandle 	aPixMap)     						// handle to the PixMap being initialized 
{
CTabHandle 			newColors = nil;   					// color table used for the off-screen PixMap 
Handle        		offBaseAddr = nil; 					// handle to the off-screen pixel image 
OSErr				osError = noErr;       				// returns error code 

/**  clone the clut if indexed color; allocate a dummy clut if direct color  **/
    if (depth <= 8)
    {
        newColors = colors;
        if ((osError = HandToHand((Handle *)&newColors)) != noErr)
			return osError;
    }
    else
    {
    	if ((newColors = (CTabHandle)NewHandle(sizeof(ColorTable) - sizeof(CSpecArray))) == nil)
			return MemError();
    }

/**  allocate pixel image; long integer multiplication avoids overflow  **/
	if ((offBaseAddr = NewHandle((unsigned long)bytesPerRow * (bounds->bottom - bounds->top))) == nil)
	{
		DisposHandle((Handle)newColors);
        return MemError();
	}

/**  initialize fields common to indexed and direct PixMaps  **/
	(**aPixMap).baseAddr = (Ptr)offBaseAddr;  			// reference to image 
	(**aPixMap).rowBytes = bytesPerRow | 0x8000;		// MSB set for PixMap 
	(**aPixMap).bounds = *bounds;        				// Use given bounds 
	(**aPixMap).pmVersion = 0;           				// No special stuff 
	(**aPixMap).packType = 0;            				// Default PICT pack 
	(**aPixMap).packSize = 0;            				// Always zero in mem 
	(**aPixMap).hRes = kDefaultResolution;      		// 72 DPI default res 
	(**aPixMap).vRes = kDefaultResolution;      		// 72 DPI default res 
	(**aPixMap).pixelSize = depth;       				// Set # bits/pixel 
	(**aPixMap).planeBytes = 0;          				// Not used 
	(**aPixMap).pmReserved = 0;         				// Not used 

/**  Initialize fields specific to indexed and direct PixMaps  **/
	if (depth <= 8)                						// PixMap is indexed 
	{
		(**aPixMap).pixelType = 0;       				// Indicates indexed 
		(**aPixMap).cmpCount = 1;        				// Have 1 component 
		(**aPixMap).cmpSize = depth;     				// Component size=depth 
		(**aPixMap).pmTable = newColors; 				// Handle to CLUT 
	}
	 else         										// PixMap is direct 
	{
		(**aPixMap).pixelType = RGBDirect; 				// Indicates direct 
		(**aPixMap).cmpCount = 3;          				// Have 3 components 
		if (depth == 16)
			(**aPixMap).cmpSize = 5;       				// 5 bits/component 
		else
			(**aPixMap).cmpSize = 8;       				// 8 bits/component 
		(**newColors).ctSeed = 3 * (**aPixMap).cmpSize;
		(**newColors).ctFlags = 0;
		(**newColors).ctSize = 0;
		(**aPixMap).pmTable = newColors;
	}

/**  cleanup if errors occurred  **/
    if (osError != noErr)    			 
    {
        if (newColors != nil)
            DisposCTable(newColors);
    }
    return osError;    									// return error code 
}


/* ----------------------------------------------------------------------------
   _FindFreePort 
   
   Returns indec into port table or -1.                
   ---------------------------------------------------------------------------- */
static  sWORD  _FindFreePort (void)
{
sWORD				i;

	for (i = 0; i < _NrTPorts; i++)
		if (_TPorts[i].hInUseBy == nil)
			return i;
	return -1;
}


#ifdef DEBUG
/* ----------------------------------------------------------------------------
   _DebugAssertTBit 
   Validates a TPS internal bitmap handle and asserts on error. 
   Returns nothing.                
   ---------------------------------------------------------------------------- */
static  void  _DebugAssertTBit (HTBIT hTBit)
{
TBitPtr				pTBit;

	tmemDebugAssertHandle(hTBit);
	if (hTBit != nil)
	{
		pTBit = TBitDeRef(hTBit);
		if ((*(Handle)hTBit) == nil)
		{
			TpsAssertL(false, "Bitmap handle has been purged!", &hTBit, 1);
		}
		else if (pTBit->MagicKey != kMagicBitmapKey)
		{
			TpsAssertL(false, "Bitmap handle is not a TPS bitmap!", &hTBit, 1);
		}
		else if (pTBit->InPort < -1 || pTBit->InPort > _NrTPorts)
		{
			TpsAssertL(false, "Bitmap handle assigned to bad port!", &hTBit, 1);
		}
		else if (pTBit->BytesPerRow > kMaxRowBytes)
		{
			TpsAssertL(false, "Bitmap handle has bad bytes per row > 16,382!", &hTBit, 1);
		}
		else if (pTBit->Depth != 1 && pTBit->Depth != 2 && pTBit->Depth != 4 && pTBit->Depth != 8)
		{
			TpsAssertL(false, "Bitmap handle has bad depth!", &hTBit, 1);
		}
		else if (pTBit->hGDevice != _hTBitDevice || *_hTBitDevice == nil)
		{
			TpsAssertL(false, "Bitmap handle assigned to bad device!", &hTBit, 1);
		}
		else if (*(pTBit->hPixMap) == nil)
		{
			TpsAssertL(false, "Bitmap handle PixMap has been purged!", &hTBit, 1);
		}
		else if (*(pTBit->hDataBits) == nil)
		{
			TpsAssertL(false, "Bitmap handle pixel image has been purged!", &hTBit, 1);
		}
		else if (EmptyRect(&pTBit->Bounds))
		{
			TpsAssertL(false, "Bitmap handle has empty bounding box!", &hTBit, 1);
		}
//		else if (pTBit->LockCount > 0 && (HGetState(pTBit->hDataBits) & 0x80) == 0)
//		{
//			TpsAssertL(false, "Bitmap handle unlocked when count says locked!", &hTBit, 1);
//		}
//		else if (pTBit->LockCount == 0 && (HGetState(pTBit->hDataBits) & 0x80) != 0)
//		{
//			TpsAssertL(false, "Bitmap handle locked when count says unlocked!", &hTBit, 1);
//		}
	}
}
#endif


/* ----------------------------------------------------------------------------
   tbitCalcRowBytes 
    
   Returns long          
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( uLONG )  tbitCalcRowBytes (uLONG uWidth, uLONG uBitDepth, uWORD uFlags)
{
short				bytesPerRow; 										// bytes per row in PixMap 
long				mod;
long				remainder;

    bytesPerRow = ((uBitDepth * uWidth + 31) >> 5) << 2; 
	if (((uFlags & TBIT_LongAlignRows)) != 0 || ((uFlags & TBIT_16ByteAlignRows) != 0))  
	{
		mod = 4;														// long align rowbytes
		if ((uFlags & TBIT_16ByteAlignRows) != 0)  						// or, double long align rowbytes
			mod = 16;							
		remainder = bytesPerRow % mod;
		if (remainder) 						
			bytesPerRow += mod - remainder;
	}
	TpsAssert((bytesPerRow < kMaxRowBytes), "Bytes per row > 16,382!");	// maximum number of bytes per row is 16,382 
	return bytesPerRow;
}

/* ----------------------------------------------------------------------------
   tbitIsRectInBitmap 
   Validates a rect within an internal bitmap area. 
   Returns boolean indicating         
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( BOOLEAN )  tbitIsRectInBitmap (HTBIT hTBit, TRECT *rBit)
{
TRECT				TRect;

	if (hTBit == nil || rBit == nil)
		return false;
	tbitGetRect(hTBit, &TRect);
	if ((rBit->left < TRect.left) || (rBit->right > TRect.right) || 
		(rBit->top < TRect.top) || (rBit->bottom > TRect.bottom))
		return false;
	return true;
}


/* ----------------------------------------------------------------------------
   tbitFindEdge 
   Given a bitmap, find the appropriate non-transparent edges.
   This is useful for finding a minimum bounding box.
   Returns nothing.
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( void )  tbitFindEdge (HTBIT hBit, TRECT *rMask, uBYTE edge_mask, uBYTE back_color)
{
	if (tbitLock(hBit))
	{
		TRECT	rBounds;
		uWORD	row_bytes;
		int		rows;
		int		columns;
		
		int		row, column;	//	loop counters
		
//		int		row_slop;
		
		uBYTE	*addr, *bit_base_addr = (uBYTE *)tbitGetBaseAddr(hBit);
		
		tbitGetRect(hBit, &rBounds);
		*rMask = rBounds;
		
		row_bytes = tbitGetBytesPerRow(hBit) & 0x7FFF;
		rows = (rBounds.bottom - rBounds.top);
		columns = (rBounds.right - rBounds.left);
		
		if (edge_mask & TBIT_kLeftEdge)
		{
			uBYTE	*base = bit_base_addr + (rBounds.top * row_bytes);
			
			char	found = FALSE;
			
			for (column = rBounds.left; column < rBounds.right; column++)
			{
				addr = base + column;

				for (row = rBounds.top; row < rBounds.bottom; row++)
				{
					if (*addr != back_color)
					{
						rBounds.left = column;
						found = TRUE;
						break;
					}
					
					addr += row_bytes;	//	advance to next row
				}
				
				if (found)
					break;
			}
		}
		
		if (edge_mask & TBIT_kTopEdge)
		{
			char	found = FALSE;
			int		row_slop = (row_bytes - rBounds.right) + rBounds.left - 1;
			uBYTE	*base = bit_base_addr + rBounds.left;
			
			for (row = rBounds.top; row < rBounds.bottom; row++)
			{
				addr = base + (row * row_bytes) ;
				for (column = rBounds.left; column < rBounds.right; column++,addr++)
				{
					if (*addr != back_color)
					{
						rBounds.top = row;
						found = TRUE;
						break;
					}
				}

				if (found)
					break;

				addr += row_slop;	//	advance to next row				
			}
		}
		
		if (edge_mask & TBIT_kRightEdge)
		{
			uBYTE	*base = bit_base_addr + (rBounds.top * row_bytes);
			
			char	found = FALSE;
			
			for (column = rBounds.right; column > rBounds.left; column--)
			{
				addr = base + column - 1;

				for (row = rBounds.top; row < rBounds.bottom; row++)
				{
					if (*addr != back_color)
					{
						rBounds.right = column;
						found = TRUE;
						break;
					}
					
					addr += row_bytes;	//	advance to next row
				}
				
				if (found)
					break;
			}
		}
		
		if (edge_mask & TBIT_kBottomEdge)
		{
			char	found = FALSE;
			int		row_slop = rBounds.right - row_bytes;
			uBYTE	*base = bit_base_addr + rBounds.left;

			for (row = rBounds.bottom - 1; row >= rBounds.top; row--)
			{
				addr = base + (row * row_bytes);
				for (column = rBounds.left; column < rBounds.right; column++,addr++)
				{
					if ( *addr != back_color)
					{
						rBounds.bottom = row + 1;
						found = TRUE;
						break;	
					}
				}

				if (found)
					break;

				addr -= row_slop;	//	go to previous row				
			}
		}

		*rMask = rBounds;
		tbitUnlock(hBit);
	}
}


/* ----------------------------------------------------------------------------
   tbitFlatten 
   
   Returns internal handle to pixmap image.
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( HTMEM )  tbitFlatten (HTBIT hTBit)
{
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
//	flags = TMEM_AFNONPURGEABLE | TMEM_AFNONVIRTUAL | TMEM_AFNONSUBALLOC | TMEM_AFZEROINIT;
	flags = TMEM_AFZEROINIT;
//	if ((size + sizeof(TBitRec)) >= TMEM_HUGE_SIZE)
//		flags |= TMEM_AFHUGE;
	if ((hTMem = tmemAlloc((size + sizeof(TBitRec)), flags)) == nil)
		return nil;

/**  copy relivant bitmap data  **/
	ptr = (Ptr)tmemLock(hTMem);
	tmemCopyBlock(tmemDeRef(hTBit), ptr, sizeof(TBitRec));
	pTBit = (TBitPtr)ptr;
	pTBit->Flags = 0;    								// no state flags yet
	pTBit->Depth = depth;								// bits per pixel
	pTBit->InPort = -1;									// no internal port in use yet
	pTBit->BytesPerRow = bytesPerRow; 					// bytes per row
	pTBit->Bounds = bounds;         					// bounding box 
	pTBit->hPixMap = nil;      							// pixel image
	pTBit->hDataBits = nil;								// reference to image 
	pTBit->hGDevice = nil;								// reference to subsystem device 
	pTBit->LockCount = 0;    							// unlocked  

/**  copy bitmap image  **/
	tbitCompress(hTBit, (ptr + sizeof(TBitRec)), &size);
	tmemUnlock(hTMem);
	(void)tmemReSize(hTMem, (size + sizeof(TBitRec)));

	return hTMem; 
}


/* ----------------------------------------------------------------------------
   tbitUnFlatten 
   
   Returns handle to internal bitmap image.
   ---------------------------------------------------------------------------- */
TPSAPI_DECLP( HTBIT )  tbitUnFlatten (HTMEM hm)
{
HTBIT				hTBit;
TBitPtr				pTBit;
short				bytesPerRow; 						// bytes per row in PixMap 
uWORD 				depth;
PixMapHandle 		hPixMap = nil;						// handle to new off-screen PixMap 
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
    if ((hPixMap = (PixMapHandle)NewHandleClear(sizeof(PixMap))) == nil)
        return nil;
	if (_SetUpPixMap(depth, &bounds, _hColorTable, bytesPerRow, hPixMap) != noErr)
	{
        if (hPixMap != nil)  
            DisposHandle((Handle)hPixMap);
        return nil;
	}
	hDataBits = (Handle)(**hPixMap).baseAddr;			// handle to pixel image

/**  create internal bitmap record  **/
//	if ((hTBit = tmemAlloc(sizeof(TBitRec), (TMEM_AFNONPURGEABLE | TMEM_AFNONVIRTUAL | TMEM_AFNONSUBALLOC | TMEM_AFZEROINIT))) == nil)
	if ((hTBit = tmemAlloc(sizeof(TBitRec), TMEM_AFZEROINIT)) == nil)
	{
        if (hPixMap != nil)  
        {
            DisposCTable((**hPixMap).pmTable);
            DisposHandle((Handle)hDataBits);
            DisposHandle((Handle)hPixMap);
        }
		return nil;
	}	

/**  load bitmap data  **/
	ptr = (Ptr)tmemLock(hm);
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
}

