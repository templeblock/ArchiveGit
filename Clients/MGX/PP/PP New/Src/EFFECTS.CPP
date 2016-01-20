//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "fxparms.h"

typedef BOOL (FAR *BEGINEFFECTPROC)(LPFXPARMS, LPTSTR);
typedef BOOL (FAR *EFFECTPROC)(LPFXPARMS);
typedef BOOL (FAR *BEGINPARMSPROC)(LPFXUIPARMS, LPTSTR);
typedef LPTSTR (FAR *GETPARMSPROC)(LPFXUIPARMS);
typedef BOOL (FAR *ENDPARMSPROC)(LPFXUIPARMS);
typedef BOOL (FAR *QUERYPROC)(FX_QUERY, LPVOID, LPVOID);

typedef struct _effectdll_parms
{
	HMODULE hModule;  					// setup in LoadEffect
	BEGINEFFECTPROC lpBeginEffect;
	EFFECTPROC lpApplyEffect;
	EFFECTPROC lpEndEffect;						// Setup in InitEffect
	BEGINPARMSPROC lpBeginParms; 				// Setup in InitParms
	GETPARMSPROC lpGetParms;				// Setup in InitParms
	ENDPARMSPROC lpEndParms; 					// Setup in InitParms
	QUERYPROC lpQueryProc;					// Setup in InitEffect & InitParms 
	//
	LPMASK lpMask;
	LPFRAME lpFrame;	
	RECT rFrame; // the frame's rect in global coords
	RECT rApply; // the area to apply in global coords
	BOOL bCannotChangeDepth;	// should we allow a depth change?
	//								 
	LPFXPARMS lpFXParms;
	LPFXUIPARMS lpUIParms;
	// for threading only
	LPTSTR lpEffectString;
	BOOL fUsingMask;
} EFFECTDLL_PARMS, FAR *LPEFFECTDLL_PARMS;


// Static prototypes
LOCAL BOOL EffectLoad(LPEFFECTDLL_PARMS lpFX, LPTSTR lpFXFile);
LOCAL BOOL EffectUnload(LPEFFECTDLL_PARMS lpFX);

LOCAL BOOL UIInit(LPEFFECTDLL_PARMS lpFX);
LOCAL BOOL UISetup(LPEFFECTDLL_PARMS lpFX, HWND hWnd, LPTSTR lpString);
LOCAL LPTSTR UIGetParms(LPEFFECTDLL_PARMS lpFX);

LOCAL BOOL EffectInit(LPEFFECTDLL_PARMS lpFX);
LOCAL LPFRAME Apply(LPEFFECTDLL_PARMS lpFX, LPTSTR lpString);
LOCAL BOOL EffectSetup(LPEFFECTDLL_PARMS lpFX, LPTSTR lpString, BOOL bCallEffect);
LOCAL LPFRAME EffectApply(LPEFFECTDLL_PARMS lpFX);

LOCAL BOOL EffectMask( LPEFFECTDLL_PARMS lpFX, LPRECT lpEditRect );
LOCAL BOOL ComputeFXArea(LPEFFECTDLL_PARMS lpFX, LPRECT lprOut, LPRECT lprEdit);

LOCAL LPVOID FXThreadGetParms(LPVOID lpParms, LPRECT lpArea);
LOCAL BOOL FXThreadProc (LPVOID lpParms, LPRECT lpArea);
LOCAL void FXThreadFreeParms(LPVOID lpParms);

LOCAL LPFRAME EffectDlgFreeFrame(HWND hDlg, LPFRAME lpFrame);

LOCAL int QueryVersion(LPEFFECTDLL_PARMS lpFX);
LOCAL BOOL QueryThreading(LPEFFECTDLL_PARMS lpFX, LPFXTHREADDATA1 lpData);

int DLL_Message( int idStr, ... );
int DLL_AstralClockCursor( int part, int whole );
LPFRAME DLL_frame_open( WORD Depth, WORD x_size, WORD y_size, WORD resolution );
void DLL_frame_close( LPFRAME lpFrame );
LPTR DLL_frame_ptr( LPFRAME lpFrame, int Depth, int x, int y, int bModify );
int DLL_frame_write( LPFRAME lpFrame, int x, int y, int dx, LPTR pixels, int count );

LPTR DLL_frame_set( LPTR lpFrame );

extern STRING szAppName;

//***********************************************************************
BOOL DoEffect( LPIMAGE lpImage, LPEFFECT_PARMS lpParms )
//***********************************************************************
{
	FNAME szFileName;
	LPTSTR lpFXString;
	LPFRAME lpNewFrame;
	LPOBJECT lpObject;
	EFFECTDLL_PARMS Effect;
	FRMDATATYPE SrcType, DstType;
	RECT rUpdate;

	// make sure all is null!
	clr((LPTR)&Effect, sizeof(Effect));

	if (!lpImage || !lpParms)
		return(FALSE);
	// make sure we are no palette color
	ImgGetInfo(lpImage, NULL, NULL, NULL, &SrcType);
	if (SrcType == FDT_PALETTECOLOR)
	{
		Message(IDS_ENOTPALETTEEFFECT);
		return(FALSE);
	}
	
	if (SrcType == FDT_LINEART)
	{
		Message(IDS_ENOTLINEARTEFFECT);
		return(FALSE);
	}
	
	if (ImgCountSelObjects(lpImage, NULL) != 1)
	{
		Message(IDS_OBJECTSONLYONE);
		return(FALSE);
	}
	
	lpObject = ImgGetSelObject(lpImage, NULL);
	if (!lpObject)
		return(FALSE);

	// Don't allow depth changes on images with objects, or if editing mask
	if (ImgInMaskEditMode(lpImage))
		Effect.bCannotChangeDepth = 2;
	else
		Effect.bCannotChangeDepth = (ImgCountObjects(lpImage) != 1);
	
	ProgressBegin(1, PROGRESS_ID(lpParms->Common.idDirty));
	
	if ( !ImgEditInit(lpImage, ET_OBJECT, UT_NEWDATA, lpObject) )
		goto ExitError;
 	 
	 
	// Get parameters string
	if (lpParms->lpString)
		lpFXString = lpParms->lpString;
	else
		lpFXString = NULL;

	// get the effect name
	if (!LookupExtFileN( lpParms->szEffect, szFileName, IDN_SPECIAL, NO ))
		goto ExitError;

	if (!EffectLoad(&Effect, szFileName))
		goto ExitError;
	if (!EffectInit(&Effect))
		goto ExitError;

	Effect.lpFrame = ObjGetEditFrame(lpObject);
	Effect.lpMask  = ImgGetMask(lpImage);
	Effect.rFrame = lpObject->rObject;
	Effect.rApply = Effect.rFrame;
	if (lpParms->bInOut && Effect.lpMask)
		Effect.lpMask->bInvert = !Effect.lpMask->bInvert;
	// Setup and apply the effect
	lpNewFrame = Apply(&Effect, lpFXString);
	if (lpParms->bInOut && Effect.lpMask)
		Effect.lpMask->bInvert = !Effect.lpMask->bInvert;

	// end effect and close dll
	EffectUnload(&Effect);
	if (!lpNewFrame)
		goto ExitError;

	ImgEditedObjectFrame(lpImage, lpObject, IDS_UNDOSPECIALFILTER, &Effect.rFrame /*&rEffect*/,					
					lpNewFrame, NULL);
					
	DstType = FrameType(lpNewFrame);
	lpParms->Common.UpdateType = 0;
	if (SrcType != DstType)
	{
		lpParms->Common.UpdateType = UT_DATATYPE;
		switch (DstType)
		{
			case FDT_LINEART:
				lpImage->DataType = IDC_SAVELA;
				break;
			case FDT_GRAYSCALE:
				lpImage->DataType = IDC_SAVECT;
				break;
			case FDT_PALETTECOLOR:
				lpImage->DataType = IDC_SAVE8BITCOLOR;
				break;
			case FDT_RGBCOLOR:
				lpImage->DataType = IDC_SAVE24BITCOLOR;
				break;
			case FDT_CMYKCOLOR:
				lpImage->DataType = IDC_SAVE32BITCOLOR;
				break;
		}
	}	
					
	rUpdate = Effect.rFrame;
	if (!AstralIsRectEmpty(&rUpdate))
	{
		lpParms->Common.UpdateType |= UT_AREA;
		lpParms->Common.rUpdateArea = rUpdate;
	}

	ProgressEnd();
	return(TRUE);

ExitError:
	// end effect and close dll
	EffectUnload(&Effect);
	ProgressEnd();
	return(FALSE);
}

/************************************************************************/
LOCAL BOOL EffectLoad(LPEFFECTDLL_PARMS lpFX, LPTSTR lpFXFile)
/************************************************************************/
{
	if ( !(lpFX->hModule = (HMODULE)AstralLoadLibrary(lpFXFile)) )
	{
		MessageBeep(0);
		return(FALSE);
	}
	return( TRUE );
}


//************************************************************************/
//	If this fails call effect unload to free proc instances and memory
//************************************************************************/
LOCAL BOOL EffectInit(LPEFFECTDLL_PARMS lpFX)
/************************************************************************/
{
	lpFX->lpFXParms = (LPFXPARMS)Alloc(sizeof(FXPARMS));
	if (!lpFX->lpFXParms)
		return(FALSE);
	
	// NOTE: this assures that the proc instances are NULL! 
	clr((LPTR)lpFX->lpFXParms, sizeof(FXPARMS));
	lpFX->lpBeginEffect = NULL;
	lpFX->lpApplyEffect = NULL;
	lpFX->lpEndEffect = NULL;
	lpFX->lpQueryProc = NULL;
	
	GetActiveColor(&lpFX->lpFXParms->ActiveColor);
	GetAlternateColor(&lpFX->lpFXParms->AlternateColor);
	
	if ( !(lpFX->lpBeginEffect	= (BEGINEFFECTPROC)GetProcAddress(lpFX->hModule, _T("FXBeginEffect"))) )
		goto ErrorExit;
	if ( !(lpFX->lpApplyEffect	= (EFFECTPROC)GetProcAddress(lpFX->hModule, _T("FXApplyEffect"))) )
		goto ErrorExit;
	if ( !(lpFX->lpEndEffect 	= (EFFECTPROC)GetProcAddress(lpFX->hModule, _T("FXEndEffect"))) )
		goto ErrorExit;

	lpFX->lpQueryProc = (QUERYPROC)GetProcAddress(lpFX->hModule, _T("FXQuery"));
		
	if ( !(lpFX->lpFXParms->AstralClockCursor	= (LPCLOCKPROC)
	     MakeProcInstance( (FARPROC)DLL_AstralClockCursor, PictPubApp.Get_hInstAstral() )) )
		goto ErrorExit;
	if ( !(lpFX->lpFXParms->frame_ptr		= (LPTRPROC)
	     MakeProcInstance( (FARPROC)DLL_frame_ptr, PictPubApp.Get_hInstAstral() )) )
		goto ErrorExit;
	if ( !(lpFX->lpFXParms->frame_write	= (LPROC)
	     MakeProcInstance( (FARPROC)DLL_frame_write, PictPubApp.Get_hInstAstral())) )
		goto ErrorExit;
	if ( !(lpFX->lpFXParms->frame_open	   = (LPTRPROC)
	     MakeProcInstance( (FARPROC)DLL_frame_open, PictPubApp.Get_hInstAstral())) )
		goto ErrorExit;
	if ( !(lpFX->lpFXParms->frame_close	= (LPTRPROC)
	     MakeProcInstance( (FARPROC)DLL_frame_close, PictPubApp.Get_hInstAstral())) )
		goto ErrorExit;
	
	if ( lpFX->lpUIParms)
		lpFX->lpFXParms->Message = lpFX->lpUIParms->Message;
	else
		lpFX->lpFXParms->Message = (LPMESSAGEPROC)MakeProcInstance(
								(FARPROC)DLL_Message, PictPubApp.Get_hInstAstral());
	if (!lpFX->lpFXParms->Message)
		goto ErrorExit;
	
	if (!(QueryVersion(lpFX) & FXVERSION))
		goto ErrorExit;
	
	lpFX->lpFXParms->hInstance = PictPubApp.Get_hInstAstral();
	return(TRUE);

ErrorExit:
	return(FALSE);
}

//************************************************************************
// Init effect UI.
//************************************************************************
LOCAL BOOL UIInit(LPEFFECTDLL_PARMS lpFX)
//************************************************************************
{
	lpFX->lpUIParms = (LPFXUIPARMS)Alloc(sizeof(FXUIPARMS));
	if (!lpFX->lpUIParms)
		return(FALSE);
	clr((LPTR)lpFX->lpUIParms, sizeof(FXUIPARMS));
	
	lpFX->lpUIParms->Message = NULL;
	
	GetActiveColor(&lpFX->lpUIParms->ActiveColor);
	GetAlternateColor(&lpFX->lpUIParms->AlternateColor);
	
	if ( !(lpFX->lpBeginParms	= (BEGINPARMSPROC)GetProcAddress(lpFX->hModule, _T("FXBeginParms"))) )
		goto ErrorExit;
	if ( !(lpFX->lpGetParms	= (GETPARMSPROC)GetProcAddress(lpFX->hModule, _T("FXGetParms"))) )
		goto ErrorExit;
	if ( !(lpFX->lpEndParms 	= (ENDPARMSPROC)GetProcAddress(lpFX->hModule, _T("FXEndParms"))) )
		goto ErrorExit;
	lpFX->lpQueryProc = (QUERYPROC)GetProcAddress(lpFX->hModule, _T("FXQuery"));

	if ( lpFX->lpFXParms)
		lpFX->lpUIParms->Message = lpFX->lpFXParms->Message;
	else
		lpFX->lpUIParms->Message = (LPMESSAGEPROC)MakeProcInstance(
								(FARPROC)DLL_Message, PictPubApp.Get_hInstAstral());
	if (!lpFX->lpUIParms->Message)
		goto ErrorExit;
	
	if (!(QueryVersion(lpFX) & FXVERSION))
		goto ErrorExit;
		 
	lpFX->lpUIParms->hInstance = PictPubApp.Get_hInstAstral();
	return(TRUE);

ErrorExit:
	if (!lpFX->lpFXParms)
		if (lpFX->lpUIParms->Message)
			FreeProcInstance((FARPROC)lpFX->lpUIParms->Message);
	FreeUp(lpFX->lpUIParms);
	lpFX->lpUIParms = NULL;
	return(FALSE);
}

//************************************************************************
// Brings the effect UI up in hWnd.
// Setup lpFX->lpFrame before calling or Fill frame data in lpFX->lpUIParms
// lpString can be NULL.
//************************************************************************
//************************************************************************
LOCAL BOOL UISetup(LPEFFECTDLL_PARMS lpFX, HWND hWnd, LPTSTR lpString)
//************************************************************************
{
	BOOL bRet;
	LPFRAME lpFrame;
	if (!lpFX->lpUIParms)
		return(FALSE);
	lpFX->lpUIParms->hWnd = hWnd;
	
	GetActiveColor(&lpFX->lpUIParms->ActiveColor);
	GetAlternateColor(&lpFX->lpUIParms->AlternateColor);
	
	lpFrame = lpFX->lpFrame;
	if (lpFrame)
	{
		lpFX->lpUIParms->src_npix = FrameXSize(lpFrame);
		lpFX->lpUIParms->src_nlin = FrameYSize(lpFrame);
		lpFX->lpUIParms->src_resolution = FrameResolution(lpFrame);
		if (FrameType(lpFrame) == FDT_LINEART)
			lpFX->lpUIParms->Depth    = 0;
		else
			lpFX->lpUIParms->Depth    = FrameDepth(lpFrame);
	}
	
	bRet = (*lpFX->lpBeginParms)(lpFX->lpUIParms, lpString);
	return(bRet);
}

//************************************************************************
LOCAL LPTSTR UIGetParms(LPEFFECTDLL_PARMS lpFX)
//************************************************************************
{
	LPTSTR lpString;
	if (!lpFX->lpUIParms)
		return(NULL);
	lpString = (*lpFX->lpGetParms)(lpFX->lpUIParms);
	return (lpString);
}

//************************************************************************/
//	Setup lpFX->lpFrame before calling
//	If !bCallEffect then lpFX is filled but the effect is not setup.
//************************************************************************/
LOCAL BOOL EffectSetup(LPEFFECTDLL_PARMS lpFX, LPTSTR lpString, BOOL bCallEffect)
//************************************************************************/
{
	LPFRAME lpFrame;
	
	lpFrame = lpFX->lpFrame;
	lpFX->lpFXParms->lpSrcFrame =  lpFrame;
	lpFX->lpFXParms->lpDstFrame = NULL;
	lpFX->lpFXParms->dst_npix = lpFX->lpFXParms->src_npix = FrameXSize(lpFrame);
	lpFX->lpFXParms->dst_nlin = lpFX->lpFXParms->src_nlin = FrameYSize(lpFrame);
	lpFX->lpFXParms->dst_resolution = lpFX->lpFXParms->src_resolution =
			FrameResolution(lpFrame);
	if (FrameType(lpFrame) == FDT_LINEART)
	{
		lpFX->lpFXParms->Depth    = 1;
		lpFX->lpFXParms->DstDepth = FrameDepth(lpFrame);
	}
	else
	{
		lpFX->lpFXParms->Depth    = FrameDepth(lpFrame);
		lpFX->lpFXParms->DstDepth = FrameDepth(lpFrame);
	}

	lpFX->lpFXParms->rOut.left = lpFX->lpFXParms->rOut.top = lpFX->lpFXParms->rOut.right = lpFX->lpFXParms->rOut.bottom = 0;
	if (bCallEffect)
		if (!(*lpFX->lpBeginEffect)((LPFXPARMS)lpFX->lpFXParms, lpString))
			return(FALSE);
	return(TRUE);
}	

/************************************************************************/
LOCAL BOOL EffectUnload(LPEFFECTDLL_PARMS lpFX)
/************************************************************************/
{
	if (lpFX->lpFXParms)
	{
		if (lpFX->lpEndEffect)
			(*lpFX->lpEndEffect)((LPFXPARMS)lpFX->lpFXParms);
		if (lpFX->lpFXParms->lpOut )
			FreeUp(lpFX->lpFXParms->lpOut);
		
		// free proc instances
		if (lpFX->lpFXParms->AstralClockCursor)
			FreeProcInstance ((FARPROC)lpFX->lpFXParms->AstralClockCursor);
		if (lpFX->lpFXParms->frame_ptr) 
			FreeProcInstance  ((FARPROC)lpFX->lpFXParms->frame_ptr);
		if (lpFX->lpFXParms->frame_write) 
			FreeProcInstance  ((FARPROC)lpFX->lpFXParms->frame_write);
		if (lpFX->lpFXParms->frame_open)
			FreeProcInstance  ((FARPROC)lpFX->lpFXParms->frame_open);
		if (lpFX->lpFXParms->frame_close)
			FreeProcInstance  ((FARPROC)lpFX->lpFXParms->frame_close);
		// if there is an lpUIParms then it will free the message proc
		if (!lpFX->lpUIParms)
			FreeProcInstance((FARPROC)lpFX->lpFXParms->Message);
		FreeUp(lpFX->lpFXParms);
		lpFX->lpFXParms = NULL;
	}

	if (lpFX->lpUIParms)
	{
		if (lpFX->lpEndParms)
			(*lpFX->lpEndParms)((LPFXUIPARMS)lpFX->lpUIParms);
		if (lpFX->lpUIParms->Message)
			FreeProcInstance((FARPROC)lpFX->lpUIParms->Message);
		FreeUp(lpFX->lpUIParms);
		lpFX->lpUIParms = NULL;
	}

	if ( lpFX->hModule )
	FreeLibrary(lpFX->hModule);
	lpFX->hModule = NULL;
	return(TRUE);	
}

								
//************************************************************************/
// computes rOut (for the effect) and rEdit(The total apply area in global 
//	coords.
//************************************************************************/
LOCAL BOOL ComputeFXArea(LPEFFECTDLL_PARMS lpFX, LPRECT lprOut, LPRECT lprEdit)
//************************************************************************/
{
	LPFRAME lpFrame;
	RECT rMask, rOut, rEdit;
	
	if (!lpFX->lpFXParms)
		return(FALSE);
	lpFrame = lpFX->lpFXParms->lpSrcFrame;
	if (!lpFrame )
		return(FALSE);
	
	// see where this frame rectangle intersects original effect image
	if (!AstralIntersectRect(&rEdit, &lpFX->rApply, &lpFX->rFrame))
		return(FALSE);
	rOut = rEdit;
	
	// get mask rect - if no mask, set to the frame rectangle
	if (MaskRect(lpFX->lpMask, &rMask))
	{
		// intersect the mask rectangle with effect rectangle
		if (!AstralIntersectRect(&rOut, &rMask, &rOut))
		{
			Message( IDS_FXNOINTERSECT );
			return(FALSE);
		}
	}
	// offset rOut to be in frames coordinates
	OffsetRect(&rOut, -lpFX->rFrame.left, -lpFX->rFrame.top);
	
	if (lprOut)
		*lprOut = rOut;
	if (lprEdit)
		*lprEdit = rEdit;
	return(TRUE);
}	
	

//************************************************************************/
//	Setup lpFX->lpMask, lpFX->rApply, lpFX->rFrame before calling
// Uses lpFX->lpFXParms->lpSrcFrame.
//	If lpOutFrame is null then one is created.
//************************************************************************/
LOCAL LPFRAME EffectApply(LPEFFECTDLL_PARMS lpFX)
/************************************************************************/
{
	BOOL fSuccess, fUsingMask, fSameDepth;
	RECT rOut, rEdit;
	int DstDepth;
	LPTR lpOut = NULL;
	LPFRAME lpDstFrame = NULL;
	
	// guard against depth change
	if (lpFX->bCannotChangeDepth && lpFX->lpFXParms->DstDepth != lpFX->lpFXParms->Depth)
	{
		Message(lpFX->bCannotChangeDepth == 1? IDS_EEFFECTDEPTHCHANGE :
														IDS_EEFFECTDEPTHCHANGE2);
		return(NULL);
	}
	
	if (!ComputeFXArea(lpFX, &rOut, &rEdit))
		return(NULL);
	lpFX->lpFXParms->rOut = rOut;

// Setup the Dest Frame
	fSameDepth = lpFX->lpFXParms->DstDepth == lpFX->lpFXParms->Depth;
	if (fSameDepth)
	{
		lpDstFrame = FrameOpen(FrameTypeInfo(lpFX->lpFXParms->lpSrcFrame),
				lpFX->lpFXParms->dst_npix,
				lpFX->lpFXParms->dst_nlin,
				lpFX->lpFXParms->dst_resolution);
	}
	else
	{
		lpDstFrame = FrameOpen((FRMDATATYPE)lpFX->lpFXParms->DstDepth,
				lpFX->lpFXParms->dst_npix,
				lpFX->lpFXParms->dst_nlin,
				lpFX->lpFXParms->dst_resolution);
	}
	if ( !lpDstFrame)
	{
		FrameError( IDS_EPROCESSOPEN );
		goto ExitError;
	}
	// take care of areas the effect will not edit in the destination by
	// setting up a background color.  EffectMask() will always take care of
	// the problem.
	fUsingMask = lpFX->lpMask || (ColorMask.Mask && ColorMask.On);
	if ((RectWidth(&rOut) < lpFX->lpFXParms->dst_npix ||
		RectHeight(&rOut) < lpFX->lpFXParms->dst_nlin) && !fUsingMask)
	{
		if (FrameType(lpDstFrame) == FDT_CMYKCOLOR)
			FrameSetBackground(lpDstFrame, 0);
		else
			FrameSetBackground(lpDstFrame, 255);
	}
	lpFX->lpFXParms->lpDstFrame = lpDstFrame;

// Setup lpOut
	DstDepth = (lpFX->lpFXParms->DstDepth ? lpFX->lpFXParms->DstDepth:1);
	lpOut = Alloc( (long)FrameXSize(lpDstFrame) * DstDepth );
	if (!lpOut)
		goto ExitError;
	lpFX->lpFXParms->lpOut = lpOut;
	
// Execute the effect
	fSuccess = (*lpFX->lpApplyEffect)((LPFXPARMS)lpFX->lpFXParms);
	if (!fSuccess)
		goto ExitError;

// Apply Masking	
	if (fUsingMask)
		EffectMask(lpFX, &rEdit);
	return(lpDstFrame);

ExitError:
	if (lpDstFrame)
		FrameClose(lpDstFrame);
	return(NULL);
}

//************************************************************************/
//	Setup lpFX->lpMask, lpFX->rFrame before calling
//************************************************************************/
/************************************************************************/
LOCAL BOOL EffectMask( LPEFFECTDLL_PARMS lpFX, LPRECT lpEditRect )
/************************************************************************/
{
	BOOL fConvert;
	LPTR lpMaskBuf, lpSrc, lpDst, lpMaskPtr;
	LPMASK lpMask;
	int SrcDepth, DstDepth;
	FRMTYPEINFO SrcType, DstType;
	int y, dx, dy, xOff, yOff, mdx, ys, ye, xs;
	LPPROCESSPROC lpProcessProc;
	LPFRAME lpSrcFrame, lpDstFrame;
	CFrameTypeConvert TypeConvert;
	
	lpSrcFrame = lpFX->lpFXParms->lpSrcFrame;
	lpDstFrame = lpFX->lpFXParms->lpDstFrame;
	lpMask = lpFX->lpMask;
		
	// get info about source and dest frames
	SrcDepth = FrameDepth(lpSrcFrame);
	if (!SrcDepth)
		SrcDepth = 1;
	DstDepth = FrameDepth(lpDstFrame);
	if (!DstDepth)
		DstDepth = 1;
	FrameGetTypeInfo(lpSrcFrame, &SrcType);
	if (SrcType.DataType == FDT_LINEART)
		FrameSetTypeInfo(&SrcType, FDT_GRAYSCALE);
	FrameGetTypeInfo(lpDstFrame, &DstType);
	if (DstType.DataType == FDT_LINEART)
		FrameSetTypeInfo(&DstType, FDT_GRAYSCALE);
	fConvert = (SrcType.DataType != DstType.DataType) ||
		!FrameColorMapsEqual(SrcType.ColorMap, DstType.ColorMap);
	dx = FrameXSize(lpDstFrame);
	dy = FrameYSize(lpDstFrame);
	
	if (!TypeConvert.Init(SrcType, DstType, dx))
		return(FALSE);

	if (!(lpMaskBuf = Alloc( (long)FrameXSize(lpDstFrame))))
		return(FALSE);
	
	MessageStatus( IDS_PLEASEWAIT );
	
	xOff = lpFX->rFrame.left;
	yOff = lpFX->rFrame.top;
	
	xs = lpEditRect->left - xOff;
	ys = lpEditRect->top - yOff;
	ye = ys + RectHeight(lpEditRect) - 1;
	
	mdx = RectWidth(lpEditRect);
	lpMaskPtr = lpMaskBuf + xs;
	xOff += xs;
	
	lpProcessProc = GetProcessProc(MM_NORMAL, DstType.DataType);
	
	// copy source data to destination data using inverted mask
	// because the destination is really the source when using
	// the mask stuff.  Since the destination data is the newly
	// created data, it should be merged in with the source, however
	// we want to return a new destination frame so the process
	// must work the other way.
	for (y = 0; y < dy; y++)
	{
		if (!(lpDst = FramePointer(lpDstFrame, 0, y, YES)))
			continue;
		if (!(lpSrc = FramePointer(lpSrcFrame, 0, y, NO)))
			continue;
		if (y >= ys && y <= ye)
		{
			clr(lpMaskBuf, dx);
			// setup mask based on src
			mload( 0+xOff, y+yOff, mdx, 1, lpMaskPtr, lpSrc,
						SrcType, lpMask);
			// invert mask
			negate(lpMaskBuf,dx);
		}
		else
			set(lpMaskBuf, dx, 255);
	
		// convert src data to dst frame format
		if (fConvert)
		{
			TypeConvert.ConvertData(lpSrc, lpFX->lpFXParms->lpOut, y, dx);
			lpSrc = lpFX->lpFXParms->lpOut;
		}
		// write the src onto the dst with inverted mask
		(*lpProcessProc)( lpDst, lpSrc, lpMaskBuf, dx );
	}	// for y
	FreeUp(lpMaskBuf);
	MessageStatus( 0 );
	return(TRUE);
}

//************************************************************************/
//	Setup and Apply the effect.  Use multithreading if possible.
//************************************************************************/
LOCAL LPFRAME Apply(LPEFFECTDLL_PARMS lpFX, LPTSTR lpString)
{
	LPFRAME lpNewFrame;
	FXTHREADDATA1 td;
	BOOL fSuccess, fUsingMask, fSameDepth;
	RECT rOut, rEdit;
	LPEFFECTDLL_PARMS lpNewFX;

// fill lpFX but don't call effect
	if (!EffectSetup(lpFX, lpString, NO))
		return(NULL);
// Should we do it the old way?
	if (!QueryThreading(lpFX, &td))
	{		
		if (!EffectSetup(lpFX, lpString, YES))
			return(NULL);
		lpNewFrame = EffectApply(lpFX);
		if (!lpNewFrame)
			return(NULL);
		return(lpNewFrame);
	}

// get total area
	if (!ComputeFXArea(lpFX, &rOut, NULL))
		return(NULL);
	rEdit = rOut;
// Setup the Dest Frame
	fSameDepth = lpFX->lpFXParms->DstDepth == lpFX->lpFXParms->Depth;
	if (fSameDepth)
	{
		lpNewFrame = FrameOpen(FrameTypeInfo(lpFX->lpFXParms->lpSrcFrame),
				lpFX->lpFXParms->dst_npix,
				lpFX->lpFXParms->dst_nlin,
				lpFX->lpFXParms->dst_resolution);
	}
	else
	{
		lpNewFrame = FrameOpen((FRMDATATYPE)lpFX->lpFXParms->DstDepth,
				lpFX->lpFXParms->dst_npix,
				lpFX->lpFXParms->dst_nlin,
				lpFX->lpFXParms->dst_resolution);
	}
	if ( !lpNewFrame)
	{
		FrameError( IDS_EPROCESSOPEN );
		return(NULL);
	}
	fUsingMask = lpFX->lpMask || (ColorMask.Mask && ColorMask.On);
	if ((RectWidth(&rOut) < lpFX->lpFXParms->dst_npix ||
		RectHeight(&rOut) < lpFX->lpFXParms->dst_nlin) && !fUsingMask)
	{
		if (FrameType(lpNewFrame) == FDT_CMYKCOLOR)
			FrameSetBackground(lpNewFrame, 0);
		else
			FrameSetBackground(lpNewFrame, 255);
	}
	lpFX->lpFXParms->lpDstFrame = lpNewFrame;
	lpFX->fUsingMask = fUsingMask;

	// Setup string so each thread can initialize the effect
	lpFX->lpEffectString = lpString;
	
	// Do effect!	
	
	// setup the one thread which get called directly in SplitThreads
	lpNewFX = (LPEFFECTDLL_PARMS)FXThreadGetParms((LPVOID)lpFX, &rOut);
	
	fSuccess = FALSE;
	if (lpNewFX)
	{
		fSuccess = SplitThreads(td.wNumberOfThreads,
					FXThreadProc,
					FXThreadGetParms,
					FXThreadFreeParms,
					(LPVOID)lpNewFX,
					&rOut);
		FXThreadFreeParms((LPVOID)lpNewFX);
	}
	if (!fSuccess)
	{
		FrameClose(lpNewFrame);
		return(NULL);				
	}
	
// Apply Masking	
	if (lpFX->fUsingMask)
	{
		OffsetRect(&rEdit, lpFX->rFrame.left, lpFX->rFrame.top);
		EffectMask(lpFX, &rEdit);
	}

	return(lpNewFrame);
}

							    

//************************************************************************
LOCAL LPVOID FXThreadGetParms(LPVOID lpParms, LPRECT lpArea)
//************************************************************************
{
	LPEFFECTDLL_PARMS lpFX = NULL;
	LPFXPARMS lpFXParms = NULL;
	LPTR lpOut = NULL;
	int DstDepth;
	
	// Copy The parms
	lpFX = (LPEFFECTDLL_PARMS)Alloc(sizeof(EFFECTDLL_PARMS));
	if (!lpFX)
		goto ExitError;
	*lpFX = *(LPEFFECTDLL_PARMS)lpParms;
	
	// Copy lpFX->lpFXParms
	lpFXParms = (LPFXPARMS)Alloc(sizeof(FXPARMS));
	if (!lpFXParms)
		goto ExitError;
	*lpFXParms = *(lpFX->lpFXParms);
	lpFX->lpFXParms = lpFXParms;
	
	// the user data should be for the original only
	lpFX->lpFXParms->lpData = NULL;
	
	// Setup lpOut
	DstDepth = (lpFX->lpFXParms->DstDepth ? lpFX->lpFXParms->DstDepth : 1);
	lpOut = Alloc( (long)FrameXSize(lpFX->lpFXParms->lpDstFrame) * DstDepth );
	if (!lpOut)
		goto ExitError;
	lpFX->lpFXParms->lpOut = lpOut;
	
	// setup effect
	if (!(*lpFX->lpBeginEffect)((LPFXPARMS)lpFX->lpFXParms, lpFX->lpEffectString))
		goto ExitError;
	
	// setup rOut
	lpFX->lpFXParms->rOut = *lpArea;
	
	return ((LPVOID)lpFX);
	
ExitError:
	if (lpFX)
		FreeUp((LPTR)lpFX);
	if (lpFXParms)
		FreeUp((LPTR)lpFXParms);
	if (lpOut)
		FreeUp((LPTR)lpOut);
	return(NULL);

}

//************************************************************************
LOCAL BOOL FXThreadProc (LPVOID lpParms, LPRECT lpArea)
//************************************************************************
{
	BOOL fSuccess;
	LPEFFECTDLL_PARMS lpFX = (LPEFFECTDLL_PARMS)lpParms;
	
// Since one thread will not be properly initialized with lpArea
	lpFX->lpFXParms->rOut = *lpArea;
	
// Execute the effect
	fSuccess = (*lpFX->lpApplyEffect)((LPFXPARMS)lpFX->lpFXParms);
	if (!fSuccess)
		return(FALSE);

	return(TRUE);
}

//************************************************************************
LOCAL void FXThreadFreeParms(LPVOID lpParms)
//************************************************************************
{
	LPEFFECTDLL_PARMS lpFX = (LPEFFECTDLL_PARMS)lpParms;
	
	if (!lpFX)
		return;
   	if (lpFX->lpEndEffect)
   		(*lpFX->lpEndEffect)((LPFXPARMS)lpFX->lpFXParms);
 	FreeUp((LPTR)lpFX->lpFXParms->lpOut);
 	FreeUp((LPTR)lpFX->lpFXParms);
	FreeUp((LPTR)lpFX);
}


//************************************************************************
// 	Returns	the version FLAGS for the effect.
//	If the Effect can/will not return a version then it assumes the 
//		current version.
//************************************************************************
LOCAL int QueryVersion(LPEFFECTDLL_PARMS lpFX)
//************************************************************************
{
	LONG version;
	
	if (!lpFX->lpQueryProc)
		return(FXVERSION);
	if (!(*lpFX->lpQueryProc)(FXQ_VERSION, NULL, &version))
		return(FXVERSION);
	return(version);
}

//************************************************************************
//	 Fills the LPFXTHREADDATA1 structure
//	 Note lpFX->lpFXParms MUST BE setup first.
//************************************************************************
LOCAL BOOL QueryThreading(LPEFFECTDLL_PARMS lpFX, LPFXTHREADDATA1 lpData)
//************************************************************************
{
	int nThreads;
	
	if (!lpFX->lpQueryProc)
		return(FALSE);
	nThreads = SuggestThreads(0);
	if (nThreads <= 1)
		return(FALSE);
	lpData->wNumberOfThreads = nThreads;
	if (!(*lpFX->lpQueryProc)(FXQ_MULTITHREAD1, (LPVOID)lpFX->lpFXParms, (LPVOID)lpData))
		return(FALSE);
	// Dont Handle custom breakup now
	if (lpData->lpRectList && lpData->bMustUseRectList)
		return(FALSE);
	return(lpData->bCanMultithread);
}

//************************************************************************
int EXPORT DLL_Message( int idStr, ... )
//************************************************************************
{
	// TED-7/15/94 - to do this properly, we would need to create
	// a vectored form of Message, like MessageV.  I'm leaving this
	// in cause only one effect calls this (stform.c).
	Message(IDS_EMEMALLOC);
	return(idStr);
}


/************************************************************************/
int EXPORT DLL_AstralClockCursor( int part, int whole )
/************************************************************************/
{
	return(AstralClockCursor(part, whole, YES));
}


/************************************************************************/
LPTR EXPORT DLL_frame_ptr(LPFRAME lpFrame, int Depth, int x, int y, BOOL bModify )
/************************************************************************/
{
	if (Depth == -1)
		return(FramePointerRaw(lpFrame, x, y, bModify));
	else
		return(FramePointer(lpFrame, x, y, bModify));
}


/************************************************************************/
void EXPORT DLL_frame_close( LPFRAME lpFrame )
/************************************************************************/
{
	FrameClose(lpFrame);
}

/************************************************************************/
LPFRAME EXPORT DLL_frame_open( WORD Depth, WORD x_size, WORD y_size, WORD Resolution )
/************************************************************************/
{
	LPFRAME lpNewFrame = NULL;

	switch(Depth)
	{
	 	case 0 : lpNewFrame = FrameOpen(FDT_LINEART,   x_size, y_size, Resolution); break;
	 	case 1 : lpNewFrame = FrameOpen(FDT_GRAYSCALE, x_size, y_size, Resolution); break;
	 	case 3 : lpNewFrame = FrameOpen(FDT_RGBCOLOR,  x_size, y_size, Resolution); break;
	 	case 4 : lpNewFrame = FrameOpen(FDT_CMYKCOLOR, x_size, y_size, Resolution); break;
	}

	if (lpNewFrame == NULL)
		FrameError( IDS_EPROCESSOPEN );

	return(lpNewFrame);
}

/************************************************************************/
int EXPORT DLL_frame_write(LPFRAME lpFrame, int x, int y, int dx, LPTR pixels, int count )
/************************************************************************/
{
	int ret;
	
	ret = FrameWrite(lpFrame, x, y, dx, pixels, count);
	return( ret );
}


/************************************************************************/
LPTR EXPORT DLL_frame_set( LPTR lpFrame )
/************************************************************************/
{
	return((LPTR)frame_set((LPFRAME)lpFrame));
}


// LOCAL prototypes
LOCAL BOOL Effects_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Effects_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void Effects_OnPaletteChanged(HWND hDlg, HWND hwndPaletteChange);

LOCAL BOOL DlgEffectLoad( HWND hWnd, LPTSTR lpFXFile );
LOCAL LPFRAME DlgEffectExec( LPFRAME lpFrame, LPRECT lpFrameRect, LPMASK lpMask, LPRECT lpApplyRect, HWND hDlg, BOOL bAddToList  );
LOCAL void DlgEffectUnload(void);
LOCAL BOOL DlgEffectAdd( LPMASK lpMask );

LOCAL BOOL FxToolProc( HWND hWindow, HDC hDC, int x, int y, UINT msg );
static BOOL MoveImageRect( int x, int y );
static void SetBasisRect( LPRECT lpRect );
static void GetBasisRect( LPRECT lpRect );
static void DisplayImageControls( HWND hDlg, BOOL bPreviewing );
static BOOL AttachPreviewFrame( HWND hDlg, LPFRAME lpFrame, int id);
LOCAL void DrawImageRect( HDC hDC, HWND hWnd );
static LPFRAME CreatePreviewFrame( HWND hDlg, LPPOINT lpPoint, LPFRAME lpFrame);
LOCAL BOOL AddResultString(LPTSTR lpEffect, LPTSTR lpString, BOOL bInOut);
LOCAL VOID FreeUpResultStrings(BOOL bFreeParms);

// statics for dialog only!
static LPFRAME lpEffectsFrame, lpOrigEffectsFrame;
static LPROC lpStartProc, lpProcessProc, lpEndProc;
static RECT PreviewRect;
static RECT EffectRect;
static RECT rApply;
static int iImageWidth, iImageHeight;
static RECT rControl;
static BOOL bInvertOrig,bEffectApplied, bPreviewing;
static BOOL bDisableCancel = FALSE;
static RECT rBasisRect;
static LPFRAME lpPreviewBasisFrame, lpPreviewFrame;
static int iLastSel;
static EFFECTDLL_PARMS effect;		// for use in DIALOG stuff only!!!
static LPTSTR *lppResultStrings;
static LPINT lpbInOut;
static int numStrings, numEffects;
static FNAME szEffectName;
LPMASK lpMask;

/***********************************************************************/
BOOL DoEffectsDlg(LPIMAGE lpImage, LPFRAME lpFrame, LPMASK lpMaskParam, LPRECT lpEffectRect, LPRECT lpEditRect)
/***********************************************************************/
{
	
	EFFECT_PARMS parms;
	BOOL bRet;
	int i;
	LPOBJECT lpObject;
	RECT rArea;
	FRMDATATYPE SrcType, DstType;
	
	if ( !lpFrame || !lpImage)
		return(FALSE);
	
	EffectRect = *lpEffectRect;
	lpMask = lpMaskParam;
	lppResultStrings = NULL;
	lpbInOut = NULL;
	numStrings = numEffects = 0;
	szEffectName[0] = '\0';
	SetRect(&rApply, INT_MAX, INT_MAX, INT_MIN, INT_MIN);
	
	lpEffectsFrame = lpOrigEffectsFrame = lpFrame;
	
	// Don't allow depth changes on images with objects, or if editing mask
	if (ImgInMaskEditMode(lpImage))
		effect.bCannotChangeDepth = 2;
	else
		effect.bCannotChangeDepth = (ImgCountObjects(lpImage) != 1);
	
	bRet =(BOOL)AstralDlg( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_SPECIALFXFILT,DlgEffectsProc);
	#ifndef WIN32
	if (!bRet || (lpEffectsFrame == lpOrigEffectsFrame || numEffects < 1))
		goto ExitFalse;
	// Put the new frame in 
	lpObject = ImgGetSelObject(lpImage, NULL);
	if (!lpObject)
		goto ExitFalse;
	
	if ( !ImgEditInit(lpImage, ET_OBJECT, UT_NEWDATA, lpObject) )
		goto ExitFalse;
		
	rArea = *lpEffectRect;
	
	// handle type change
	ImgGetInfo(lpImage, NULL,NULL,NULL, &SrcType);
	DstType = FrameType(lpEffectsFrame);
	if (SrcType != DstType)
	{
		switch (DstType)
		{
			case FDT_LINEART:
				lpImage->DataType = IDC_SAVELA;
				break;
			case FDT_GRAYSCALE:
				lpImage->DataType = IDC_SAVECT;
				break;
			case FDT_PALETTECOLOR:
				lpImage->DataType = IDC_SAVE8BITCOLOR;
				break;
			case FDT_RGBCOLOR:
				lpImage->DataType = IDC_SAVE24BITCOLOR;
				break;
			case FDT_CMYKCOLOR:
				lpImage->DataType = IDC_SAVE32BITCOLOR;
				break;
		}
	}	
	ImgEditedObjectFrame(lpImage, lpObject, IDS_UNDOSPECIALFILTER, &rArea /* should be lpEditRect*/,
				lpEffectsFrame, NULL);
				
	#else
	if (!bRet || numEffects < 1)
		goto ExitFalse;
	#endif
	
	// Post the effects!
	for (i=0; i<numEffects; i++)
	{	
		lstrcpy (parms.szEffect,lppResultStrings[i*2]);
		parms.lpString = lppResultStrings[i*2+1];  
		parms.bInOut = lpbInOut[i];
		#ifndef WIN32
		PostCommand(lpImage->lpCmdList, IDS_CMD_EFFECT, &parms);
		#else
		AddCommand(lpImage->lpCmdList, IDS_CMD_EFFECT, &parms);
		#endif
	}

	#ifndef WIN32	
	// Update the image
	UpdateImage(lpImage, lpEffectRect, YES );
	#else
	ExecuteCommands(lpImage->lpCmdList);
	#endif

	FreeUpResultStrings(NO);
	return(TRUE);


ExitFalse:
	if (lpEffectsFrame && lpEffectsFrame != lpOrigEffectsFrame)
	{
		FrameClose(lpEffectsFrame);
		lpEffectsFrame = NULL;
	}
	FreeUpResultStrings(YES);
	return(FALSE);
}



/***********************************************************************/
BOOL WINPROC EXPORT DlgEffectsProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Effects_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Effects_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_PALETTECHANGED, Effects_OnPaletteChanged);

	HANDLE_DLGRET(hDlg, WM_QUERYNEWPALETTE, Dialog_OnQueryNewPalette);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_SPECIALFXFILT);
	}
}

/************************************************************************/
LOCAL BOOL Effects_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	FNAME szFileName;
	HWND hControl, hImageControl;
	LPTSTR lp;
	RECT rRect;
	int dx, dy;

	bEffectApplied = FALSE;
	bDisableCancel = NO;
	CenterPopup( hDlg );
	
	szEffectName[0] = '\0';
	// Initialize the In/Out button
	if ( !lpMask )
		ControlEnable( hDlg, IDC_FXINOUT, NO );
	else
	{
		bInvertOrig = (lpMask ? lpMask->bInvert: FALSE);
		CheckDlgButton( hDlg, IDC_FXINOUT, !bInvertOrig );
		if ( hControl = GetDlgItem( hDlg, IDC_FXINOUT ) )
			if ( AstralStr(( bInvertOrig ? IDS_OUTSIDE : IDS_INSIDE), &lp) )
				SetWindowText( hControl, lp );
	}
	
	// Initialize the effects list box
	SetWindowFont(GetDlgItem(hDlg, IDC_EXTNAMES),
				GetStockObject(Window.iLabelFont), FALSE);
	InitExtNameN( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Special,
		IDN_SPECIAL, 0, YES );
	iLastSel = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_EXTNAMES));
	if ( LookupExtFile( Names.Special, szFileName, IDN_SPECIAL ) )
	{
		if ( hControl = GetDlgItem(hDlg,IDC_FXBOX) )
		{
			DlgEffectLoad( hControl, szFileName );
			lstrcpy(szEffectName, Names.Special);
		}
	}
	lstrcpy( Names.Saved, Names.Special );
	
	// Initialize the preview control and get its rectangle
	if ( !(hImageControl = GetDlgItem( hDlg, IDC_FXIMAGE_PREVIEW )) )
		return(TRUE);
	Image_SetFrame(hImageControl, NULL);
	GetClientRect( hImageControl, &rRect );
	
	// Initialize the fullview image control with the preview rectangle
	if ( !(hImageControl = GetDlgItem( hDlg, IDC_FXIMAGE_FULLVIEW )) )
		return(TRUE);
	bPreviewing = NO;
	Image_SetFrame( hImageControl, lpEffectsFrame);
	SetWindowLong( hImageControl, GWL_IMAGE_TOOLPROC, (long)FxToolProc );
	iImageWidth  = FrameXSize(lpEffectsFrame);
	iImageHeight = FrameYSize(lpEffectsFrame);
	dx = RectWidth( &rRect );
	dy = RectHeight( &rRect );
	rBasisRect.left = rBasisRect.top = 0;
	if ( iImageWidth > dx )
		rBasisRect.left += (iImageWidth - dx) / 2;
	if ( iImageHeight > dy )
		rBasisRect.top +=  (iImageHeight - dy) / 2;
	rBasisRect.right = rBasisRect.left + dx - 1;
	rBasisRect.bottom = rBasisRect.top + dy - 1;
	BoundRect( &rBasisRect, 0, 0, iImageWidth-1, iImageHeight-1 );
	SetBasisRect( &rBasisRect );
	
	// Initiaize the Preview frame pointers
	lpPreviewBasisFrame = NULL;
	lpPreviewFrame = NULL;
	
	DisplayImageControls( hDlg, NO/*bPreviewing*/ );
#ifdef OLD
	ControlEnable( hDlg, IDOK, NO ); // No OK until Preview or Apply
#endif // OLD
	return(TRUE);
}

/***********************************************************************/
LOCAL void Effects_OnPaletteChanged(HWND hDlg, HWND hwndPaletteChange)
/***********************************************************************/
{
if (IsOurWindow(hwndPaletteChange))
	{
	AstralControlPaint(hDlg, IDC_FXIMAGE_PREVIEW);
	AstralControlPaint(hDlg, IDC_FXIMAGE_FULLVIEW);
	}
}

/************************************************************************/
LOCAL void Effects_OnCommand(HWND hDlg, int id, HWND hCtl, UINT codeNotify)
/************************************************************************/
{
	FNAME szFileName;
	HWND hControl;
	LPTSTR lp;
	RECT rRect;
	int iCurSel, iCount, ret;
	LPFRAME lpNewFrame;
	
	if ( id != IDCANCEL )
		bDisableCancel = NO;
	switch (id)
	{
		case IDC_FXINOUT:
			if ( !lpMask )
				break;
			lpMask->bInvert = !lpMask->bInvert;
			CheckDlgButton( hDlg, id, !lpMask->bInvert );
			if ( hControl = GetDlgItem( hDlg, id ) )
				if ( AstralStr( ( lpMask->bInvert ? IDS_OUTSIDE : IDS_INSIDE ),&lp))
					SetWindowText( hControl, lp );
		break;
	
		case IDC_FXIMAGE_FULLVIEW:
			if ( codeNotify != 1 )
				break;
			// Fall through if a double click
			
		case IDC_FXPREVIEW:
			if ( !effect.hModule )
				break;
			AttachPreviewFrame( hDlg, NULL, IDC_FXIMAGE_PREVIEW ); // Unattach any preview frame
			bPreviewing = NO;
			AttachPreviewFrame( hDlg, lpEffectsFrame, IDC_FXIMAGE_FULLVIEW );
			GetBasisRect( &rRect );
		 	// If a new basis rect, zap any previous basis frame
			if ( !EqualRect( &rRect, &rBasisRect ) && lpPreviewBasisFrame )
				lpPreviewBasisFrame =  EffectDlgFreeFrame(hDlg, lpPreviewBasisFrame);
			rBasisRect = rRect;
			if ( !lpPreviewBasisFrame )
	    		if ( !(lpPreviewBasisFrame = CreatePreviewFrame( hDlg,
			 	(LPPOINT)&rBasisRect.left, lpEffectsFrame )) )
						break;
			if ( lpPreviewFrame )
			{
				FrameClose( lpPreviewFrame );
				lpPreviewFrame = NULL;
			}
			AstralCursor( IDC_WAIT ); /* Put up the wait cursor */
			
			lpPreviewFrame = DlgEffectExec( lpPreviewBasisFrame, &PreviewRect,
										lpMask, NULL, hDlg, NO );
			
			if ( !lpPreviewFrame )
			{
				bDisableCancel = CANCEL;
				break;
			}
			
			AstralCursor( NULL ); /* Revert back to the old cursor */
			bPreviewing = YES;
			AttachPreviewFrame( hDlg, lpPreviewFrame, IDC_FXIMAGE_PREVIEW );
			AttachPreviewFrame( hDlg, lpPreviewBasisFrame, IDC_FXIMAGE_FULLVIEW );
			DisplayImageControls( hDlg, YES /*bPreviewing*/ );
#ifdef UNUSED
			ControlEnable( hDlg, IDOK, YES );
#endif // UNUSED
			bEffectApplied = FALSE;
		break;
	
		case IDC_FXIMAGE_PREVIEW:
			if ( codeNotify != 1 )
				break;
			// Fall through if a double click
	
		case IDC_FXRESET:
			if ( !effect.hModule )
				break;
			// zap any basis frame
			lpPreviewBasisFrame =  EffectDlgFreeFrame(hDlg, lpPreviewBasisFrame);
			// Zap any previous preview frame
			lpPreviewFrame = EffectDlgFreeFrame(hDlg, lpPreviewFrame);
			// Put in the full view
			bPreviewing = NO;
			AttachPreviewFrame( hDlg, lpEffectsFrame, IDC_FXIMAGE_FULLVIEW );
			DisplayImageControls( hDlg, NO /*bPreviewing*/ );
		break;
	
		case IDC_FXAPPLY:
			if ( !effect.hModule )
				break;

			// Unattach any preview frame
			AttachPreviewFrame( hDlg, NULL, IDC_FXIMAGE_PREVIEW ); 

			// Put up the wait cursor 
			AstralCursor( IDC_WAIT ); 
			
			lpNewFrame = DlgEffectExec(lpEffectsFrame, &EffectRect,lpMask, NULL, hDlg,YES);
			if (lpNewFrame)
			{
				bEffectApplied = TRUE;
				bPreviewing = NO;
				AttachPreviewFrame( hDlg, lpNewFrame, IDC_FXIMAGE_FULLVIEW );
				DisplayImageControls( hDlg, NO /*bPreviewing*/ );
				if (lpEffectsFrame != lpOrigEffectsFrame)
					FrameClose(lpEffectsFrame);
				lpEffectsFrame = lpNewFrame;
				// zap any basis frame
				lpPreviewBasisFrame =  EffectDlgFreeFrame(hDlg, lpPreviewBasisFrame);
			}
			else
			{
				bPreviewing = NO;
				AttachPreviewFrame( hDlg, lpEffectsFrame, IDC_FXIMAGE_FULLVIEW );
				bDisableCancel = CANCEL;
			}
			
			AstralCursor( NULL ); /* Revert back to the old cursor */
			break;

		case IDOK:
			if ( !effect.hModule )
				break;

			#ifndef WIN32
			// do we need to prompt?
			if (!bEffectApplied)
			{
				TCHAR     szBuffer[512];
			
				if ( !AstralStrEx( IDS_FXOKTOEXIT, szBuffer, sizeof(szBuffer) ) )
					break;
				ret = MessageBox( hDlg, szBuffer, szAppName,
							MB_ICONQUESTION | MB_YESNOCANCEL | MB_TASKMODAL );
				if (ret == IDCANCEL)
					break;
			}
			#else
				ret = IDYES;
			#endif
			
			if (!bEffectApplied && ret == IDYES)
			{
				// Apply the effect before quitting
				// Unattach any frames
				AttachPreviewFrame( hDlg, NULL, IDC_FXIMAGE_PREVIEW ); 
				AttachPreviewFrame( hDlg, NULL, IDC_FXIMAGE_FULLVIEW ); 

				#ifndef WIN32
				// Put up the wait cursor 
				AstralCursor( IDC_WAIT ); 
				// Apply the effect

				lpNewFrame = DlgEffectExec(lpEffectsFrame, &EffectRect,lpMask, NULL, hDlg,YES);
				if (lpNewFrame)
				{
					if (lpEffectsFrame != lpOrigEffectsFrame)
						FrameClose(lpEffectsFrame);
					lpEffectsFrame = lpNewFrame;
				}
				AstralCursor( NULL ); /* Revert back to the old cursor */
				#else
				if (!DlgEffectAdd(lpMask))
				{
					Message(IDS_EMEMALLOC);
					break;
				}
				#endif
			 }

			// handle IDOK
			DlgEffectUnload();
			if ( lpMask )
				lpMask->bInvert = bInvertOrig;
			if ( lpPreviewFrame ) // Zap the preview frame
			{
				FrameClose( lpPreviewFrame );
				lpPreviewFrame = NULL;
			}
			// zap any basis frame
			lpPreviewBasisFrame =  EffectDlgFreeFrame(hDlg, lpPreviewBasisFrame);
			AstralDlgEnd( hDlg, TRUE );
		break;
	
		case IDC_CANCEL:
		case IDCANCEL:
			if ( bDisableCancel && id != IDC_CANCEL && !hCtl)
			{
				
//				if (!lpPreviewFrame)
//				{
//					// Attach the preview basis frame
//					AttachPreviewFrame( hDlg, lpPreviewBasisFrame, IDC_FXIMAGE_PREVIEW );
//					if ( lpPreviewFrame )
//					{ // Zap any previous preview frame
//						FrameClose( lpPreviewFrame );
//						lpPreviewFrame = NULL;
//					}
//					DisplayImageControls( hDlg, NO /*bPreviewing*/ );
//				}
				break;
			}
			lstrcpy( Names.Special, Names.Saved );
			AttachPreviewFrame( hDlg, NULL, IDC_FXIMAGE_PREVIEW ); // Unattach any preview frame
			AttachPreviewFrame( hDlg, NULL, IDC_FXIMAGE_FULLVIEW );
			DlgEffectUnload();
			if ( lpMask )
				lpMask->bInvert = bInvertOrig;
			if ( lpPreviewFrame ) // Zap the preview frame
			{
				FrameClose( lpPreviewFrame );
				lpPreviewFrame = NULL;
			}
			// Zap the preview basis frame
			lpPreviewBasisFrame =  EffectDlgFreeFrame(hDlg, lpPreviewBasisFrame);
			if (lpEffectsFrame && lpEffectsFrame != lpOrigEffectsFrame)
			{
				FrameClose(lpEffectsFrame);
				lpEffectsFrame = NULL;
			}
			AstralDlgEnd( hDlg, FALSE );
		break;
	
		case IDC_EXTNAMES:
			if ( codeNotify == LBN_DBLCLK )
				{
				SEND_WM_COMMAND( hDlg, IDC_FXPREVIEW, NULL, 0 );
				break;
				}
			if ( codeNotify != LBN_SELCHANGE )
				break;
			GetExtNameN( hDlg, IDC_EXTNAMES, IDC_FILENAME,
				Names.Special, IDN_SPECIAL, codeNotify, 0, YES );
			iCurSel = ListBox_GetCurSel(GetDlgItem(hDlg, id));
			if (IsCategory(Names.Special))
				{
				if (!iCurSel || iCurSel > iLastSel || abs(iCurSel-iLastSel) != 1 )
					++iCurSel;
				else
				if (iCurSel < iLastSel)
					--iCurSel;
				iCount = ListBox_GetCount(GetDlgItem(hDlg, id));
				iCurSel = bound(iCurSel, 0, iCount-1);
				ListBox_SetCurSel(GetDlgItem(hDlg, id), iCurSel);
				GetExtNameN( hDlg, IDC_EXTNAMES, IDC_FILENAME,
					Names.Special, IDN_SPECIAL, codeNotify, 0, YES );
				}
			if (iLastSel == iCurSel)
				break;
			iLastSel = iCurSel;
			SEND_WM_COMMAND( hDlg, IDC_FXRESET, NULL, 0 );
			DlgEffectUnload();
			if ( !(hControl = GetDlgItem(hDlg,IDC_FXBOX)) )
				break;
			UpdateWindow( hControl );
			if ( LookupExtFile( Names.Special, szFileName, IDN_SPECIAL ) )
			{
				DlgEffectLoad( hControl, szFileName );
				lstrcpy(szEffectName, Names.Special);
			}
		break;
	
		case IDC_EXTMANAGE:
			PopupMenu( hDlg, id, IDC_FILENAME );
		break;
	
		case IDC_ADDEXT:
		case IDC_DELETEEXT:
		case IDC_RENAMEEXT:
			ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Special,
				IDN_SPECIAL, id, NO );
		break;
	
	   default:
		break;
   }
}

//************************************************************************
LOCAL BOOL DlgEffectLoad( HWND hWnd, LPTSTR lpFXFile )
//************************************************************************
{
	if (!EffectLoad(&effect, lpFXFile))
		return(FALSE);
	if (!EffectInit(&effect))
		goto ExitError;
	if (!UIInit(&effect))
		goto ExitError;
	effect.lpFrame = lpEffectsFrame;
	if (!UISetup(&effect, hWnd, NULL))
		goto ExitError;
	return(TRUE);

ExitError:
	EffectUnload(&effect);
	return(FALSE);
}


//************************************************************************
LOCAL void DlgEffectUnload(void)
//************************************************************************
{
   	EffectUnload(&effect);
}


//***********************************************************************
LOCAL LPFRAME DlgEffectExec( LPFRAME lpFrame, LPRECT lpFrameRect, LPMASK lpMask, LPRECT lpApplyRect, HWND hDlg, BOOL bAddToList )
//***********************************************************************
{
	LPTSTR lpString, lpEffect;
	BOOL  bInOut;
	
	LPFRAME lpResultFrame = NULL;
	
	// Get the effect's params
	lpString = UIGetParms(&effect);
	if (!lpString)
		return(NULL);
	
	// setup params 
	effect.lpMask = lpMask;
	effect.lpFrame = lpFrame;
	if (lpFrameRect)
		effect.rFrame = *lpFrameRect;
	else
		SetRect(&effect.rFrame,0,0,lpFrame->Xsize-1, lpFrame->Ysize-1);

	if (lpApplyRect)
		effect.rApply = *lpApplyRect;
	else
		effect.rApply = effect.rFrame;
	ProgressBegin(1, PROGRESS_ID(IDS_UNDOEFFECT), hDlg);
	// Do the Effect
	if (EffectSetup(&effect, lpString, YES))
 		lpResultFrame = EffectApply(&effect);
	ProgressEnd();
	// Add it to the list
	if (lpResultFrame && bAddToList)
	{
		lpEffect = (LPTSTR)Alloc(lstrlen(szEffectName)+1);
		if (!lpEffect)
		{
			FreeUp(lpString);
			FrameClose(lpResultFrame);
			return(NULL);
		}
		lstrcpy(lpEffect, szEffectName);
		if (lpMask)
			bInOut = (lpMask->bInvert != bInvertOrig);
		else
			bInOut = FALSE;
		if (!AddResultString(lpEffect, lpString, bInOut))
		{
			FreeUp(lpEffect);
			FreeUp(lpString);
			FrameClose(lpResultFrame);
			return(NULL);
		}
	}
	else	
		FreeUp(lpString);
	return(lpResultFrame);
}

//***********************************************************************
LOCAL BOOL DlgEffectAdd( LPMASK lpMask )
//***********************************************************************
{
	LPTSTR lpString, lpEffect;
	BOOL  bInOut;
	
	// Get the effect's params
	lpString = UIGetParms(&effect);
	if (!lpString)
		return(FALSE);
	
	lpEffect = (LPTSTR)Alloc(lstrlen(szEffectName)+1);
	if (!lpEffect)
	{
		FreeUp(lpString);
		return(FALSE);
	}
	lstrcpy(lpEffect, szEffectName);
	if (lpMask)
		bInOut = (lpMask->bInvert != bInvertOrig);
	else
		bInOut = FALSE;
	if (!AddResultString(lpEffect, lpString, bInOut))
	{
		FreeUp(lpEffect);
		FreeUp(lpString);
		return(FALSE);
	}
	return(TRUE);
}

/************************************************************************/
LOCAL BOOL FxToolProc( HWND hWindow, HDC hDC, int x, int y, UINT msg )
/************************************************************************/
{
static HDC shDC;
static BOOL bTrack;

if (bPreviewing)
	return(TRUE);

switch (msg)
	{
	case WM_SIZE: // Values used for Image2Control mapping
	rControl.right  = rControl.left + x - 1;
	rControl.bottom = rControl.top  + y - 1;
	SetImageControlMapping( iImageWidth, iImageHeight, &rControl );
	break;

	case WM_MOVE: // Values used for Image2Control mapping
	rControl.left = x;
	rControl.top = y;
	SetImageControlMapping( iImageWidth, iImageHeight, &rControl );
	break;

	case WM_LBUTTONDOWN:
	if (bTrack)
		break;
	bTrack = YES;
	shDC = GetDC( hWindow );
	DrawImageRect( shDC, hWindow );
	MoveImageRect( x, y );
	DrawImageRect( shDC, hWindow );
	break;

	case WM_LBUTTONUP:
	if (!bTrack)
		break;
	bTrack = NO;
	if ( shDC )
		ReleaseDC( hWindow, shDC );
	shDC = NULL;
	break;

	case WM_MOUSEMOVE:	// sent when ToolActive is on
	if (!bTrack)
		break;
	
	DrawImageRect( shDC, hWindow );
	MoveImageRect( x, y );
	DrawImageRect( shDC, hWindow );
	break;

	case WM_LBUTTONDBLCLK:
	break;

	case WM_PAINT:	// The cancel operation message
	DrawImageRect( hDC, hWindow );
	break;
	}

return(TRUE);
}


/***********************************************************************/
LOCAL void DrawImageRect( HDC hDC, HWND hWnd )
/***********************************************************************/
{
RECT rRect;
LFIXED lfScale;

GetBasisRect( &rRect );

lfScale = FGET( RectWidth(&rControl), iImageWidth );
rRect.left   = FMUL( rRect.left, lfScale );
rRect.right  = FMUL( rRect.right, lfScale );

lfScale = FGET( RectHeight(&rControl), iImageHeight );
rRect.top    = FMUL( rRect.top, lfScale );
rRect.bottom = FMUL( rRect.bottom, lfScale );

OffsetRect( &rRect, rControl.left, rControl.top );
InvertSelection( hWnd, hDC, &rRect, SL_BOX );
}


/***********************************************************************/
static BOOL MoveImageRect( int x, int y )
/***********************************************************************/
{
RECT rRect;
LFIXED lfScale;
int dx, dy, cx, cy;

GetBasisRect( &rRect );

// Convert the new center point (x,y) to image coordinates
lfScale = FGET( iImageWidth, RectWidth(&rControl) );
x = FMUL( x - rControl.left, lfScale );
lfScale = FGET( iImageHeight, RectHeight(&rControl) );
y = FMUL( y - rControl.top, lfScale );

// Compute the amount of movement to be made
cx = ( rRect.left + rRect.right ) / 2;
cy = ( rRect.top + rRect.bottom ) / 2;
dx = x - cx;
dy = y - cy;

// Bound any movement so the rectangle stays on the image
if ( rRect.left + dx < 0 )
	dx = -rRect.left;
if ( rRect.right + dx > (iImageWidth - 1) )
	dx = (iImageWidth - 1) - rRect.right;
if ( rRect.top + dy < 0 )
	dy = -rRect.top;
if ( rRect.bottom + dy > (iImageHeight - 1) )
	dy = (iImageHeight - 1) - rRect.bottom;

// Get out if there is nothing to do
if ( !dx && !dy )
	return( NO );

OffsetRect( &rRect, dx, dy );
SetBasisRect( &rRect );
return( YES );
}


static RECT rControlBasisRect;
/***********************************************************************/
static void SetBasisRect( LPRECT lpRect )
/***********************************************************************/
{
rControlBasisRect = *lpRect;
}


/***********************************************************************/
static void GetBasisRect( LPRECT lpRect )
/***********************************************************************/
{
*lpRect = rControlBasisRect;
}


/***********************************************************************/
static void DisplayImageControls( HWND hDlg, BOOL bPreviewing )
/***********************************************************************/
{
//ShowDlgItem( hDlg, IDC_FXIMAGE_PREVIEW, TRUE /*bPreviewing*/);
//ShowDlgItem( hDlg, IDC_FXIMAGE_FULLVIEW, TRUE /*!bPreviewing*/ );
ControlEnable( hDlg, IDC_FXRESET, bPreviewing );
}


/************************************************************************/
static BOOL AttachPreviewFrame( HWND hDlg, LPFRAME lpFrame, int id )
/************************************************************************/
{
HWND hControl;

// Get the handle to the preview image control
if ( !(hControl = GetDlgItem( hDlg, id )) )
	return( FALSE );

// Link the passed in frame to the sample image control
Image_SetFrame( hControl, lpFrame);
return( TRUE );
}

/************************************************************************/
// returns NULL always!
/************************************************************************/
LOCAL LPFRAME EffectDlgFreeFrame(HWND hDlg, LPFRAME lpFrame)
/************************************************************************/
{
	LPFRAME lpTest;
	HWND hControl;
	
	if (!lpFrame)
		return(NULL);
	hControl = GetDlgItem( hDlg, IDC_FXIMAGE_PREVIEW);
	if (hControl)
	{
		lpTest = (LPFRAME)GetWindowLong( hControl, GWL_IMAGE);
		if (lpTest == lpFrame)
			Image_SetFrame( hControl, NULL);
	}
	hControl = GetDlgItem( hDlg, IDC_FXIMAGE_FULLVIEW);
	if (hControl)
	{
		lpTest = (LPFRAME)GetWindowLong( hControl, GWL_IMAGE);
		if (lpTest == lpFrame)
			Image_SetFrame( hControl, NULL);
	}
	FrameClose(lpFrame);
	return(NULL);
}

/************************************************************************/
static LPFRAME CreatePreviewFrame( HWND hDlg, LPPOINT lpPoint, LPFRAME lpFrame)
/************************************************************************/
{
HWND hControl;
LPFRAME lpNewFrame;
LPTR lpSrc, lpDst;
int sx, sy, cx, cy, bx, ix, iy, idx, idy, ey;
RECT rRect;

if ( !lpFrame )
	return( NULL );

// Get the handle to the preview image control
if ( !(hControl = GetDlgItem( hDlg, IDC_FXIMAGE_PREVIEW )) )
	return( FALSE );

// The preview frame will be created as big as the destination control
GetClientRect( hControl, &rRect );
cx = RectWidth( &rRect );
cy = RectHeight( &rRect );

// Create a new sample image frame to fit the control
if (FrameType(lpFrame) == FDT_LINEART)
{
	// if we have a lineart image,  make sure that the prevew is gray
	if ( !(lpNewFrame = FrameOpen( FDT_GRAYSCALE, cx, cy,
   	  FrameResolution(lpFrame))) )
		{
		FrameError( IDS_EPROCESSOPEN );
		return( NULL );
		}
}
else
{
	if ( !(lpNewFrame = FrameOpen( FrameTypeInfo(lpFrame), cx, cy,
   	  FrameResolution(lpFrame))) )
		{
		FrameError( IDS_EPROCESSOPEN );
		return( NULL );
		}
}

if (FrameType(lpNewFrame) == FDT_CMYKCOLOR)
	FrameSetBackground(lpNewFrame, 0);
else
	FrameSetBackground(lpNewFrame, 255);

// Compute the source image rectangle to be the same size as the control
if ( (ix = lpPoint->x) < 0 )
	ix = 0;
if ( (iy = lpPoint->y) < 0 )
	iy = 0;
if ( (idx = cx) > FrameXSize(lpFrame) - ix )
	idx = FrameXSize(lpFrame) - ix;
if ( (idy = cy) > FrameYSize(lpFrame) - iy )
	idy = FrameYSize(lpFrame) - iy;

// Compute a start location in case the image area is smaller the the control
sx = ( cx - idx ) / 2;
sy = ( cy - idy ) / 2;

// Save the offset between the effects frame and the preview frame
PreviewRect.left = EffectRect.left + ix - sx;
PreviewRect.top = EffectRect.top + iy - sy;
PreviewRect.right = PreviewRect.left + cx - 1;
PreviewRect.bottom = PreviewRect.top + cy - 1;

// Set the copy constant variable
bx = idx * FrameDepth(lpNewFrame);

// Copy the data from the base frame to the new frame
ey = iy + idy;
while ( iy < ey )
	{
	if ( !(lpSrc = FramePointer( lpFrame, ix, iy++, NO )) )
		break;
	if ( !(lpDst = FramePointer( lpNewFrame, sx, sy++, YES )) )
		break;
	copy( lpSrc, lpDst, bx );
	}
return( lpNewFrame );
}



/************************************************************************/
// Adds the given string to lppResultStrings.
// Returns FALSE if there was a memory Error.
/************************************************************************/
LOCAL BOOL AddResultString(LPTSTR lpEffect, LPTSTR lpString, BOOL bInOut)
/************************************************************************/
{
	int i;
	LPTSTR *lppNew;
	LPINT lpbNewInOut;

	if (numStrings <= numEffects*2)
	{
		lppNew = (LPTSTR *)Alloc((numStrings+10)*(sizeof(LPTSTR) + (sizeof(int)/2)));
		if (!lppNew)
			return(FALSE);
		lpbNewInOut =  (LPINT)(lppNew + (numStrings+10));	
			
		numStrings += 10;
		if (lppResultStrings)
		{
			// copy old strings
			for (i=0;i<numEffects*2;i++)
			{
				lppNew[i] = lppResultStrings[i];
				lpbNewInOut[i/2] = lpbInOut[i/2];
			}
			FreeUp(lppResultStrings);
		}
		lppResultStrings = lppNew;
		lpbInOut = lpbNewInOut;
	}
	lppResultStrings[numEffects*2] = lpEffect;
	lppResultStrings[numEffects*2+1] = lpString;
	lpbInOut[numEffects] = bInOut;
	numEffects++;
	return(TRUE);
}

/************************************************************************/
// 	Frees up lppResultStrings & all strings in it.
//	if (!bFreeParms) then only the lppResultStrings && the lpEffects are freed
/************************************************************************/
LOCAL VOID FreeUpResultStrings(BOOL bFreeParms)
/************************************************************************/
{
	LPTSTR *lpp;
	int i;

	if (!lppResultStrings)
		return;
	// free up strings
   	for (i=0;i<numEffects;i++)
	{
		FreeUp((LPTR)lppResultStrings[i*2]);
		if (bFreeParms)
			FreeUp((LPTR)lppResultStrings[i*2+1]);
	}	
   	FreeUp((LPTR)lppResultStrings);
   	lppResultStrings = NULL;
   	lpbInOut = NULL;
   	numStrings = numEffects = 0;
}
