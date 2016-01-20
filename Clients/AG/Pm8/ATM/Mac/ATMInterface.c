/*
 * ATMInterface.c
 *
 * Version 3.0
 *
 * Adobe Type Manager is a trademark of Adobe Systems Incorporated.
 * Copyright 1983-1991 Adobe Systems Incorporated.
 * All Rights Reserved.
 */

#if !defined(THINK_C) || THINK_C >= 5 /* For all compilers except Think C earlier than 5.0 */
#include <Types.h>
#include <Dialogs.h>
#include <Files.h>
#include <Devices.h>
#endif


#include "ATMInterface.h"

#ifndef FALSE
#define FALSE 0
#endif

short			ATMOpen = 0;
short			ATMOpenVersion;
LATEST_PROCS	ATMProcs;

#define DRIVER_NAME		"\p.ATM"

//
//	PROTOTYPES
//
#ifndef powerc
short getOutlineATM(short c, ATMFixedMatrix *matrix, Ptr clientHook, short (*MoveTo)(char *, ATMFixedPoint *),
	short (*LineTo)(char *, ATMFixedPoint *), short (*CurveTo)(char *, ATMFixedPoint *, ATMFixedPoint *, ATMFixedPoint *), 
	short (*ClosePath)(char *));

short getOutlineATM2( char *cp, ATMFixedMatrix *matrix, Ptr clientHook, short (*MoveTo)(char *, ATMFixedPoint *),
	short (*LineTo)(char *, ATMFixedPoint *), short (*CurveTo)(char *, ATMFixedPoint *, ATMFixedPoint *, ATMFixedPoint *), 
	short (*ClosePath)(char *));
#endif	// powerc

short initVersionATM(short version)
	{
	return initNameAndVersionATM(version, (StringPtr)DRIVER_NAME, NULL);
	}

short initNameAndVersionATM(short version, StringPtr name, short *errorCode)
	{
	CntrlParam c;
	OSErr	err, *errp;
	StringPtr	driverName;
	
	ATMOpen = 0;
	
	errp = errorCode? errorCode : &err;
	*errp = ATM_NOERR;

	driverName = name;
	if (!driverName || !*driverName)
		driverName = (StringPtr)DRIVER_NAME;
	if ((*errp = OpenDriver(driverName, &c.ioCRefNum)) != 0)
		return 0;

	/* Use ATMPascalProcsStatusCode for all routines to use all pascal interfaces;
	 	ATMProcsStatusCode to use the set of C-stck based calls. */
//	c.csCode = ATMProcsStatusCode;
	c.csCode = ATMPascalProcsStatusCode;
	ATMProcs.version = version;
	*(ATMProcs5 **)c.csParam = &ATMProcs;

	if ((*errp = PBStatus((ParmBlkPtr) &c, 0)) != 0)
		return 0;

	ATMOpenVersion = version;
	return ATMOpen = 1;
	}

short fontAvailableATM(short family, short style)
	{
	return ATMOpen ? ATMXCall2i(fontAvailable, family, style) : 0;
	}

short showTextATM(Byte *text, short length, ATMFixedMatrix *matrix)
	{
	short errorCode;

	/* Note: this really is "showText", version 3 style. */
	return (ATMOpen && ATMOpenVersion == ATMProcs3Version) ?
			ATMXCall4i(showTextErr, text, length, matrix, &errorCode) : length;
	}

short showTextATMErr(Byte *text, short length, ATMFixedMatrix *matrix, short *errorCode)
	{
	return (ATMOpen && ATMOpenVersion >= ATMProcs4Version) ?
			ATMXCall4i(showTextErr, text, length, matrix, errorCode) : length;
	}

short xyshowTextATM(Byte *text, short length, ATMFixedMatrix *matrix, ATMFixed *displacements)
	{
	short errorCode;

	/* Note: this really is "xyshowText", version 3 style. */
	return (ATMOpen && ATMOpenVersion == ATMProcs3Version) ?
			ATMXCall5i(xyshowTextErr, text, length, matrix, displacements, &errorCode) :
			length;
	}

short xyshowTextATMErr(Byte *text, short length, ATMFixedMatrix *matrix,
					ATMFixed *displacements, short *errorCode)
	{
	return (ATMOpen && ATMOpenVersion >= ATMProcs4Version) ?
			ATMXCall5i(xyshowTextErr, text, length, matrix, displacements,
									  errorCode) : length;
	}
 
#ifndef powerc
short getOutlineATM(
	short c,
	ATMFixedMatrix *matrix, 
	Ptr clientHook,
	short (*MoveTo)(char *, ATMFixedPoint *),
	short (*LineTo)(char *, ATMFixedPoint *),
	short (*CurveTo)(char *, ATMFixedPoint *, ATMFixedPoint *, ATMFixedPoint *), 
	short (*ClosePath)(char *))
#else
	short getOutlineATM(
	short c,
	ATMFixedMatrix *matrix, 
	Ptr clientHook,
	short (*MoveTo)(),
	short (*LineTo)(),
	short (*CurveTo)(), 
	short (*ClosePath)())
#endif	// powerc
{

#ifdef PPC
	UniversalProcPtr	MoveToFPtr = NewAdd_MoveProc(MoveTo);
	UniversalProcPtr	LineToFPtr = NewAdd_LineProc(LineTo);
	UniversalProcPtr	CurveToFPtr = NewAdd_CurveProc(CurveTo);
	UniversalProcPtr	ClosePathFPtr = NewAdd_CloseProc(ClosePath);
#endif

	if (!ATMOpen)
		return ATM_NOT_ON;

#ifdef PPC
	return ATMOpenVersion >= ATMProcs4Version ? 
			ATMXCall7i(getOutline, c, matrix, clientHook, MoveToFPtr, LineToFPtr, CurveToFPtr,
								   ClosePathFPtr) : ATM_WRONG_VERSION;
#endif

#ifndef PPC
	return ATMOpenVersion >= ATMProcs4Version ? 
			ATMXCall7i(getOutline, c, matrix, clientHook, MoveTo, LineTo, CurveTo,
								   ClosePath) : ATM_WRONG_VERSION;
#endif

	}				

short startFillATM(void)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs4Version ? 
			ATMXCall0i(startFill) : ATM_WRONG_VERSION;
	}				

short fillMoveToATM(ATMPFixedPoint pc)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs4Version ? 
		ATMXCall1i(fillMoveTo, pc) : ATM_WRONG_VERSION;
	}				

short fillLineToATM(ATMPFixedPoint pc)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs4Version ? 
			ATMXCall1i(fillLineTo, pc) : ATM_WRONG_VERSION;
	}				

short fillCurveToATM(ATMPFixedPoint pc1, ATMPFixedPoint pc2, ATMPFixedPoint pc3)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs4Version ? 
			ATMXCall3i(fillCurveTo, pc1, pc2, pc3) : ATM_WRONG_VERSION;
	}				

short fillClosePathATM(void)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs4Version ? 
			ATMXCall0i(fillClosePath) : ATM_WRONG_VERSION;
	}				

short endFillATM(void)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs4Version ? 
			ATMXCall0i(endFill) : ATM_WRONG_VERSION;
	}				

#ifndef powerc
short getOutlineATM2(
	char *cp,
	ATMFixedMatrix *matrix, 
	Ptr clientHook,
	short (*MoveTo)(char *, ATMFixedPoint *),
	short (*LineTo)(char *, ATMFixedPoint *),
	short (*CurveTo)(char *, ATMFixedPoint *, ATMFixedPoint *, ATMFixedPoint *), 
	short (*ClosePath)(char *))
#else
short getOutlineATM2(
	char *cp,
	ATMFixedMatrix *matrix, 
	Ptr clientHook,
	short (*MoveTo)(),
	short (*LineTo)(),
	short (*CurveTo)(), 
	short (*ClosePath)())
#endif	// powerc
	{
#ifdef PPC
	UniversalProcPtr	MoveToFPtr = NewAdd_MoveProc(MoveTo);
	UniversalProcPtr	LineToFPtr = NewAdd_LineProc(LineTo);
	UniversalProcPtr	CurveToFPtr = NewAdd_CurveProc(CurveTo);
	UniversalProcPtr	ClosePathFPtr = NewAdd_CloseProc(ClosePath);
#endif
	if (!ATMOpen)
		return ATM_NOT_ON;
	 return (ATMOpenVersion >= ATMProcs6Version) ?
          	ATMXCall7i(getOutline2, cp, matrix, clientHook, MoveTo, LineTo, CurveTo, ClosePath) : ATM_WRONG_VERSION;
	}				

void disableATM(void)
	{
	if (!ATMOpen)
		return;
	if (ATMOpenVersion >= ATMProcs5Version) 
		ATMXCall0i(disable);
	}				

void reenableATM(void)
	{
	if (!ATMOpen)
		return;
	if (ATMOpenVersion >= ATMProcs5Version) 
		ATMXCall0i(reenable);
	}				

short getBlendedFontTypeATM(StringPtr fontName, short fondID)
	{
	if (!ATMOpen)
		return ATMNotBlendFont;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall2i(getBlendedFontType, fontName, fondID) : ATMNotBlendFont;
	}

ATMErr encodeBlendedFontNameATM(StringPtr familyName, short numAxes,
				Fixed *coords, StringPtr blendName)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall4i(encodeBlendedFontName, familyName, numAxes, coords, blendName) :
			ATM_WRONG_VERSION;
	}

ATMErr decodeBlendedFontNameATM(StringPtr blendName, StringPtr familyName,
			short *numAxes, Fixed *coords, StringPtr displayInstanceStr)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall5i(decodeBlendedFontName, blendName, familyName, numAxes, coords,
											  displayInstanceStr) : ATM_WRONG_VERSION;
	}

ATMErr	addMacStyleToCoordsATM(Fixed *coords, short macStyle, Fixed *newCoords,
							   short *stylesLeft)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall4i(addMacStyleToCoords, coords, macStyle, newCoords, stylesLeft) :
			ATM_WRONG_VERSION;
	}

ATMErr convertCoordsToBlendATM(Fixed *coords, Fixed *weightVector)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall2i(convertCoordsToBlend, coords, weightVector) : ATM_WRONG_VERSION;
	}

ATMErr normToUserCoordsATM(Fixed *normalCoords, Fixed *coords)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall2i(normToUserCoords, normalCoords, coords) : ATM_WRONG_VERSION;
	}

ATMErr userToNormCoordsATM(Fixed *coords, Fixed *normalCoords)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall2i(userToNormCoords, coords, normalCoords) : ATM_WRONG_VERSION;
	}

ATMErr createTempBlendedFontATM(short numAxes, Fixed *coords, short *useFondID)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall3i(createTempBlendedFont, numAxes, coords, useFondID) : ATM_WRONG_VERSION;
	}

ATMErr disposeTempBlendedFontATM(short fondID)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall1i(disposeTempBlendedFont, fondID) : ATM_WRONG_VERSION;
	}

ATMErr createPermBlendedFontATM(StringPtr fontName, short fontSize, short fontFileID, short *retFondID)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall4i(createPermBlendedFont, fontName, fontSize, fontFileID,
											  retFondID) : ATM_WRONG_VERSION;
	}

ATMErr disposePermBlendedFontATM(short fondID)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall1i(disposePermBlendedFont, fondID) : ATM_WRONG_VERSION;
	}

ATMErr getTempBlendedFontFileIDATM(short *fileID)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall1i(getTempBlendedFontFileID, fileID) : ATM_WRONG_VERSION;
	}

ATMErr getNumAxesATM(short *numAxes)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	*numAxes = 0;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall1i(getNumAxes, numAxes) : ATM_WRONG_VERSION;
	}

ATMErr getNumMastersATM(short *numMasters)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	*numMasters = 0;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall1i(getNumMasters, numMasters) : ATM_WRONG_VERSION;
	}

ATMErr getMasterFONDATM(short i, short *masterFOND)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall2i(getMasterFOND, i, masterFOND) : 0;
	}

ATMErr copyFitATM(short method, Fixed TargetWidth, Fixed *beginCoords,
					Fixed *baseWidths, Fixed *resultWidth, Fixed *resultCoords)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall6i(copyFit, method, TargetWidth, beginCoords, baseWidths,
								resultWidth, resultCoords) : ATM_WRONG_VERSION;
	}

ATMErr showTextDesignATM(StringPtr fontFamily, Byte *text, short len, ATMFixedMatrix *matrix,
				Fixed *coords, Fixed *displacements, short *lenDisplayed)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall7i(showTextDesign, fontFamily, text, len, matrix, coords,
									   displacements, lenDisplayed) : ATM_WRONG_VERSION;
	}

ATMErr getAxisBlendInfoATM(short axis, short *userMin, short *userMax, StringPtr axisType,
					StringPtr axisLabel, StringPtr axisShortLabel)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall6i(getAxisBlendInfo, axis, userMin, userMax, axisType,
										 axisLabel, axisShortLabel) : ATM_WRONG_VERSION;
	}

ATMErr getFontSpecsATM(FontSpecs *specs)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall1i(getFontSpecs, specs) : ATM_WRONG_VERSION;
	}

ATMErr fontFitATM(Fixed *origCoords, short numTargets, short *varyAxes,
				Fixed *targetMetrics, Fixed **masterMetrics,
				Fixed *retCoords, Fixed *retWeightVector)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall7i(fontFit, origCoords, numTargets, varyAxes, targetMetrics,
					   masterMetrics, retCoords, retWeightVector) : ATM_WRONG_VERSION;
	}

ATMErr		getNumBlessedFontsATM(short *numBlessedFonts)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall1i(getNumBlessedFonts, numBlessedFonts) : ATM_WRONG_VERSION;
	}

ATMErr		getBlessedFontNameATM(short i, StringPtr blessedFontName, Fixed *coords)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall3i(getBlessedFontName, i, blessedFontName, coords) :
			ATM_WRONG_VERSION;
	}

ATMErr		getRegularBlessedFontATM(short *regularID)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall1i(getRegularBlessedFont, regularID) : ATM_WRONG_VERSION;
	}

ATMErr		flushCacheATM(void)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall0i(flushCache) : ATM_WRONG_VERSION;
	}

ATMErr		getFontFamilyFONDATM(StringPtr familyName, short *retFondID)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall2i(getFontFamilyFOND, familyName, retFondID) : ATM_WRONG_VERSION;
	}

ATMErr		MMFontPickerATM(struct MMFP_Parms *parms, struct MMFP_Reply *reply)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs5Version ? 
			ATMXCall2i(MMFontPicker, parms, reply) : ATM_WRONG_VERSION;
	}

Boolean isSubstFontATM(StringPtr fontName, short fondID, short style, FontSpecs ***fontSpecs, Handle *chamName)
	{
	if (!ATMOpen)
		return FALSE;
	return ATMOpenVersion >= ATMProcs8Version ? 
			ATMXCall5i(isSubstFont, fontName, fondID, style, fontSpecs, chamName) :
			 FALSE;
	}

ATMErr getRunsATM(
	short c,
	ATMFixedMatrix *matrix, 
	Ptr clientHook,
	ATMPRunRec pRunRec)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs8Version ?
			ATMXCall4i(getRuns, c, matrix, clientHook, pRunRec) : ATM_WRONG_VERSION;
	}				

ATMErr getPSNumATM(StringPtr psName, short *retFondID, Boolean doCreate)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
	return ATMOpenVersion >= ATMProcs8Version ?
				ATMXCall3i(getPSNum, psName, retFondID, doCreate) : ATM_WRONG_VERSION;
}

ATMErr getATMInfoATM(ATMInfo* request)
	{
	if (!ATMOpen)
		return ATM_NOT_ON;
		
	if (ATMOpenVersion >= ATMProcs11Version) {
		return ATMXCall1i(getATMInfo, request);
	} else
		return ATM_WRONG_VERSION;
}

/************************************************************************************/
/*	Example path functions for use with getATMOutline, when compiling for both		*/
/*	68K code and for PowerPC.
/*	Delete these if you will do not use them.
/************************************************************************************/

#if 0 /* Keep this example code commented out by default. */

/*
 * Defs for building path
 */
/* op codes */
#define LINE_TO		'lt'
#define CURVE_TO	'ct'
#define MOVE_TO		'mt'
#define CLOSE_PATH	'cp'

/* convenient structs for specific path args */
typedef struct LineToRecord
{
	short				op_code;
	ATMFixedPoint		pt;
}LineToRecord;

typedef struct MoveToRecord
{
	short				op_code;
	ATMFixedPoint		pt;
}MoveToRecord;

typedef struct CurveToRecord
{
	short				op_code;
	ATMFixedPoint		pt1, pt2, pt3;
}CurveToRecord;

typedef struct ClosePathRecord
{
	short				op_code;
}ClosePathRecord;

/* 
 * Globals to remember firts point, and flag
 * to determine if it IS the first point.
 */
 Boolean		gFirstMoveTo;
 long			gX, gY;

	
/*
 * Misc defs
 */
#define FAILURE	0
#define SUCCESS	1


/*
 * Conditionally declare Universal ProcPtrs for PowerPC
 */
 
#if USESROUTINEDESCRIPTORS
	UniversalProcPtr	myLineToUPP, myMoveToUPP, myCurveToUPP, myClosePathUPP;
#endif

	Handle gPathHandle;




/*
 * Draw Path functions.
 */

// Close Path routines


// *************************************************************************************************

static short myLineTo( Handle path_handle, ATMFixedPoint *pt)
{
	LineToRecord	ltR;
	OSErr			err;
	int				x,y;
	
	ltR.op_code = LINE_TO;
	ltR.pt = *pt;
	err = PtrAndHand((Ptr) &ltR, path_handle, sizeof(ltR));
	x = (int) ((pt->x) / 65536);
	y = (int) ((pt->y) / 65536);
	PlotPoint(x-2,x+2,y-2,y+2);
	LineTo(x,y);
	return (err?FAILURE:SUCCESS);
}

static short myMoveTo( Handle path_handle, ATMFixedPoint *pt)
{
	MoveToRecord	mtR;
	OSErr			err;
	int				x,y;
	
	mtR.op_code = MOVE_TO;
	mtR.pt = *pt;
	err = PtrAndHand((Ptr) &mtR, path_handle, sizeof(mtR));
	x = (int) ((pt->x) / 65536);
	y = (int) ((pt->y) / 65536);
	PlotPoint(x-2,x+2,y-2,y+2);
	MoveTo(x,y);
	if (gFirstMoveTo)
	{
		gX = x;
		gY = y;
		gFirstMoveTo = FALSE;
	}
	return (err?FAILURE:SUCCESS);
}

static short myCurveTo( Handle path_handle, ATMFixedPoint *pt1, ATMFixedPoint *pt2, ATMFixedPoint *pt3)
{
	CurveToRecord	ctR;
	OSErr			err;
	short			x,y;
	
	ctR.op_code = CURVE_TO;
	ctR.pt1 = *pt1;
	ctR.pt2 = *pt2;
	ctR.pt3 = *pt3;
	err = PtrAndHand((Ptr) &ctR, path_handle, sizeof(ctR));
	x = (short) ((pt1->x) / 65536);
	y = (short) ((pt1->y) / 65536);
	PlotPoint(x-2,x+2,y-2,y+2);
	LineTo(x,y);
	x = (short) ((pt2->x) / 65536);
	y = (short) ((pt2->y) / 65536);
	DrawPoint(x-2,x+2,y-2,y+2);
	LineTo(x,y);
	x = (short) ((pt3->x) / 65536);
	y = (short) ((pt3->y) / 65536);
	PlotPoint(x-2,x+2,y-2,y+2);
	LineTo(x,y);
	return (err?FAILURE:SUCCESS);
}

static short myClosePath( Handle path_handle)
{
	ClosePathRecord	cpR;
	OSErr			err;
	
	cpR.op_code = CLOSE_PATH;
	err = PtrAndHand((Ptr) &cpR, path_handle, sizeof(cpR));
	PlotPoint(gX-2,gX+2,gY-2,gY+2);
	LineTo(gX,gY);
	gFirstMoveTo = TRUE;
	return (err?FAILURE:SUCCESS);
}


#if USESROUTINEDESCRIPTORS
	typedef	UniversalProcPtr	closePathHookPI;
	
	#define	NewClosePathProc(userRoutine)	\
			(closePathHookPI) NewRoutineDescriptor((ProcPtr) (userRoutine), closePathHookProcInfo, GetCurrentISA())
#endif

// Line To routines

#if USESROUTINEDESCRIPTORS
	typedef	UniversalProcPtr	lineToHookPI;
	
	#define	NewLineToProc(userRoutine)	\
			(lineToHookPI) NewRoutineDescriptor((ProcPtr) (userRoutine), lineToHookProcInfo, GetCurrentISA())
#endif

// Move To routines

#if USESROUTINEDESCRIPTORS
	typedef	UniversalProcPtr	moveToHookPI;
	
	#define	NewMoveToProc(userRoutine)	\
			(moveToHookPI) NewRoutineDescriptor((ProcPtr) (userRoutine), moveToHookProcInfo, GetCurrentISA())
#endif

// Curve To routines

#if USESROUTINEDESCRIPTORS
	typedef	UniversalProcPtr	curveToHookPI;
	
	#define	NewCurveToProc(userRoutine)	\
			(curveToHookPI) NewRoutineDescriptor((ProcPtr) (userRoutine), curveToHookProcInfo, GetCurrentISA())
#endif

/*
 * Initialize Proc arguments. To be called ONCE ONLY in program.
 */  
static void BootGetOutlineTest(void)
{

	gPathHandle = NewHandle(0);
	
	#if USESROUTINEDESCRIPTORS
		myMoveToUPP = NewMoveToProc( myMoveTo );
		myLineToUPP = NewLineToProc( myLineTo );
		myCurveToUPP = NewCurveToProc( myCurveTo );
		myClosePathUPP = NewClosePathProc( myClosePath );
	#endif
		
}


/* Example call to getATMoutline using conditional compile for PowerPC. */

/*
.
.
.
	#if USESROUTINEDESCRIPTORS
		myErr = getOutlineATM((short) gGOTheChar, &myMatrix, (Ptr) path_handle, (short (*)()) myMoveToUPP, (short (*)()) myLineToUPP, (short (*)()) myCurveToUPP, (short (*)()) myClosePathUPP);
	#else
		myErr = getOutlineATM((short) gGOTheChar, &myMatrix, (Ptr) path_handle, myMoveTo, myLineTo, myCurveTo, myClosePath);
	#endif
.
.
*/

#endif /* Keep this example code commented out by default. */
