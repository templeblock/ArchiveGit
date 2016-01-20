/*
 * ATMInterface.h
 *
 * Version 3.8
 *
 * Adobe Type Manager is a trademark of Adobe Systems Incorporated.
 * Copyright 1983-1994 Adobe Systems Incorporated.
 * All Rights Reserved.
 */
 
/***************************************************************************/
/*	Introduction														   */
/***************************************************************************/
/*
*** Notes on the Use of the Macintosh API for Adobe Type Manager v3.8 ***

Major Changes:
- added Mixed Mode Manger support.
- removed definitions for compiling with THINK 4.0 and earlier.
- changed default calling convention from C-stack-based to 
Pascal-stack-based.
-  moved Error codes to end of file.
	
The two files on this disk, ATMInterface.c and ATMInterface.h, should be
included in your own source code. ATMInterface.h defines all the data
structures required to work with ATM, and provides a definition of the array
of procedure pointers which is used to access the ATM API functions.

At a mininum, you must use the ATMInterface.h file. You may yourself
write the function wrappers for the calls to the procedure pointers,
and implement your own function for calling OpenDriver for ATM and then
PBStatus to obtain a copy of the procedure pointer structure. However, in
ATMInterface.c, Adobe does provide simple function calls to get a copy of
the procedure pointer array from ATM, and to call all of the API functions.

Adobe strongly recommends including the file ATMInterface.c in your
source code, and using the functions defined in this file to access all
of the ATM API function calls.  This is particularly important in the
case of PowerPC native programs.  ATMInterface.c and ATMInterface.h will
internally take care of all the issues involved in setting up and allocating
routine descriptors and Universal Procedure Pointers to make the API calls
work across Mixed Mode Manager switches; your code can then directly call the
functions defined in ATMInterface.c without having to be concerned with any
such issues.


You should  be aware that the ATMInterface.h and ATMInterface.c files were
designed to use either C or Pascal calling convention for the functions available  
in versions of ATM before 3.0. To be compatible with pre-3.0 ATM, you must use the 
C-stack-based calling convention. However, the C-stack-based calling convention is
not compatible with MPW C, MetroWerks, or PowerPC code; for these latter environments,
you must use the pascal-stack-based calling conventions.

These files are set up to use the pascal calling convention by default. To use the
C-stack based calling convention:
 -use the definition from the code block which follows this introduction:
		#define ATM_LANG	ATM_C


Also in the function
 		 'short initNameAndVersionATM(short version, StringPtr name)', 
in ATMInterface.c, comment out the line
		c.csCode = ATMPascalProcsStatusCode;
nad un-comment the line:
		c.csCode = ATMProcsStatusCode;



If you change either the definition of the ATM_LANG definition in ATMInterface.h or
the value of the c.csCode variable in ATMInterface.c, you must change the other 
to match.

If you are compiling for PowerPC Native code, or with MPW C, or with Think C++ or
MPW C++ compilers, you MUST  use the Pascal calling conventions. 
Both of these compilers will otherwise push a long word on the 
stack when the argument is a short word, leading to a multitude of errors when
calling any code compiled with Think C, as is the ATM API. Although PowerPC code itself pays
no attention to the 'pascal' qualifier, you still need to use it: The Mixed Mode Manager
translates 68K architecture stack arguments to PowerPC code, and vise-versa, per the 
MPW C compiler's calling conventions, and cause the same problems that MPW C does.In general,
you should use the Pascal calling convention with the ATM API for wider compatibility between
code compiled with different compilers.

The functions affected by these switches are:

ATMFontAvail
ATMShowTextErr
ATMxyShowTextErr
ATMGetOutline
ATMStartFill
ATMFillMoveTo
ATMFillLineTo
ATMFillCurveTo
ATMFillClosePath
ATMEndFill


Please refer to Adobe Technical Note #5072 for information on the use and arguments
for the different function calls.
*/



/***************************************************************************/
/*	Includes															   */
/***************************************************************************/


#ifndef _H_ATMInterface
#define _H_ATMInterface

#ifndef __DIALOGS__
#include <Dialogs.h>					/* needed for MMFPHook declaration */
#endif

#ifndef __FONTS__
#include <Fonts.h>					/* needed for private3-6 declaration */
#endif



/***************************************************************************/
/*	Compiler switches													   */
/***************************************************************************/

/*
 * ATM_LANG defines which language calling conventions you wish to use for the
 * "pre-3.0" calls with ATM 3.0. For example, do you want the field "fontAvailable"
 * a C or Pascal routine? If you wish this to be a PASCAL routine always, use
 *		#define ATM_LANG	pascal
 * otherwise use
 *		#define ATM_LANG	ATM_C
 * If you want to be compatible with versions of ATM earlier than 3.0, you must use the
 * ATM_C definition.However, this is only possible if you are compiling with Think C and two
 * byte ints: for all other development environments, you must use the pascal
 * calling convention. Remember to request either ATMPascalProcsStatusCode or
 * ATMProcsStatusCode, depending on your calling convention, in the function
 * initNameAndVersionATM.
 */
 
#ifndef ATM_C
#define ATM_C  /* this is supposed to be blank.*/
#endif

#ifndef ATM_LANG
// #define ATM_LANG	ATM_C	/* use this if you need to use C-stack based functions.*/
#define ATM_LANG	pascal /* Use this for all pascal functions */
#endif



/*
 * Does your compiler support function prototypes in structure fields? If so, we
 * will supply them for you. Older versions of Think C (pre-5.0) don't and will give
 * error messages for prototyping function pointers in structs, so you will want
 * to turn this off.
 */
#ifndef ATM_USE_PROTOTYPES /* Define as 'true' for all compilers except Think C earlier than 5.0 */
#	if !defined(THINK_C) || THINK_C >= 5 
#		define ATM_USE_PROTOTYPES	1
#	else
#		define ATM_USE_PROTOTYPES	0
#	endif
#endif



/***************************************************************************/
/*	Pre-Processor defs and typdefs 										   */
/***************************************************************************/
#if defined(powerc) && powerc
#pragma options align=mac68k
#endif


#define	MaxLegalBlendName	31			/* Maximum size of a legal font name (w/o len).	*/
#define	MaxBlendEntries		16			/* Max entries in a blend vector.				*/
#define	MaxBlendAxes		4			/* Can specify 4 dimensions.					*/


/* For the "method" parameter of CopyFit. */
enum {
	ATMCopyFitDefault,					/* Let us choose the best method for H&J.		*/
	ATMCopyFitConstStems,				/* Keep the stem widths constant.				*/
	ATMCopyFitVarStems					/* Allow stem widths to vary.					*/
};

typedef long ATMFixed;

typedef short ATMErr;

/*
 * For MPW users who don't want to use the ATMInterface.a glue routines:
 * you must use ATMPascalProcsStatusCode.  This in turn requires that you
 *		#define ATM_LANG pascal
 * (as described above) before #include'ing ATMInterface.h.
 */
#define ATMProcsStatusCode 			0
#define ATMPascalProcsStatusCode 	16
#define ATMPickerControlCode		10

/* Return codes from GetBlendedFontType													*/
#define	ATMNotBlendFont				0
#define	ATMBlendFontInstance		1
#define	ATMBlendFontBaseDesign		2
#define ATMTempBlendFont			3

typedef struct
	{
	ATMFixed a, b, c, d, tx, ty;
	} ATMFixedMatrix;

typedef struct 
	{
  	ATMFixed x, y;
	} ATMFixedPoint, *ATMPFixedPoint;

typedef struct {
  short l;
  short g;
  } ATMDevInterval;

typedef struct {
  ATMDevInterval x;
  ATMDevInterval y;
  } ATMDevBounds;

typedef struct {
  ATMDevBounds bounds;
  short datalen;
  short *data;
  short *indx;	/* unused */
  } ATMDevRun, *ATMPDevRun;

typedef struct {
  ATMDevRun	run;		   	/* Beginning of run array */
  long		xOffset;	   	/* X offset from base to char origin */
  long		yOffset;	   	/* Y offset from base to char origin */
  unsigned long	dataSize;	/* Amount of memory needed for character */
} ATMRunRec, *ATMPRunRec;


 
 
#define FontSpecsVersion	2
#define SpecReserve			22 /* Must be adjusted so that ( number of bitfields + SpecReserve = bits in long word) */


/*
 *  The following two macros used to assist in getting (isFontDataAvailable()) and
 *  setting (isFontDataAvailable()) various bit flags that are in FontSpecs,
 * 	without depending on to the bitfield datatype.. We would prefer that new code use these
 * macros, so that we can get rit of the bitfield data types in the future.
 */
#define isFontDataAvailable(a, b)		((a)->b)
#define setFontDataAvailable(a, b, c)	((a)->b = c)


/*
 *  The following bit fields were changed from Boolean to unsigned int,
 *  since IBM's RS 6000 C compiler does not like the size/shape of 'Boolean' for
 * bitfields.
 */

/*
 * This structure is filled via getFontSpecsATM() and gives additional information
 * about the current font than we can get from just the FOND. Make sure that the
 * appropriate bits are set when the fields are filled in, and the "SpecsReserved"
 * is initialized to zero!
 *
 * As of the 3.0 release, only the vertical stem width and xheight are used,
 * though the cap height might be used if the xheight is not available. This structure
 * is designed to be expandable as needed in the future.
 *
 * The version number MUST be initialized to zero or one...
 */

typedef struct FontSpecs {
	short	version;
	/* The total number of bitfields should always add up to 32. As specific */
	/* bitfields are assinged by Adobe, the SpecReserve will decrease accordingly. */
	unsigned int	vertStemWidthAvail : 1;		/* Signals that data is available.		*/
	unsigned int	horizStemWidthAvail : 1;
	unsigned int	xHeightAvail : 1;
	unsigned int	capHeightAvail : 1;
	unsigned int	serifWidthAvail : 1;
	unsigned int	serifHeightAvail : 1;
			/* These are in version 1 and above. */
	unsigned int	italicAngleAvail : 1;
	unsigned int	flagsAvail : 1;
	unsigned int	lowerCaseScaleAvail : 1;
			/* These are in version 2 and above. */
	unsigned int	capHeightScaleAvail : 1;

#if defined(MPW) || (defined(powerc) && powerc) // MPW and IBM AIX compiler insists that bitfields must be unsigned int.
												// Use this as default for all PowerPC compilers as well; these will all
												// 4-byte int's.
	unsigned int		SpecsReserved : SpecReserve;	/* Must be initialized to zero (0)!		*/
#else
	unsigned long	SpecsReserved : SpecReserve;	/* Must be initialized to zero (0)!		*/
#endif
	
	Fixed	SpecsVertStemWidth;			/* Thickness of the vertical stems.		*/
	Fixed	SpecsHorizStemWidth;		/* Thickness of the horizontal stems.	*/
	Fixed	SpecsxHeight;				/* The height of the letter 'x'.		*/
	Fixed	SpecsCapHeight;				/* The height of a capital letter.		*/
	Fixed	SpecsSerifWidth;			/* The width of a serif.				*/
	Fixed	SpecsSerifHeight;			/* The height of a serif, ie. how tall	*/
										/* are the tips off the base line.		*/					
			/* These are in version 1 and above. */
	Fixed	SpecsItalicAngle;			/* How much the font "leans".			*/
	long	SpecsFlags;					/* See below for flag values.			*/
	Fixed	SpecsLowerCaseScale;		/* Amount to scale lowercase for
										 * x-height matching */

	Fixed	SpecsResSpace[SpecReserve];	/* Reserved fields for later.			*/
				/* These are in version 2 and above. */
	Fixed	SpecsCapHeightScale;			/* Amount to scale all chars for
										 * x-height matching with x-height axis. */

} FontSpecs;

#define	SPECS_FORCE_BOLD		0x1		/* Font should be artificially emboldened. */
#define	SPECS_ALL_CAPS			0x2		/* Font contains uppercase characters in lowercase positions. */
#define	SPECS_SMALL_CAPS		0x4		/* Font contains small capitals in lowercase positions. */


/* 
 * Defines and structs for using ATMGetInfo
 */
/* 
 * List of recognized tags.
 */

/***
	'NAME',	// name of ATM user. Returns pointer to Pascal string.
	'ORG ',		// name of organization. Returns pointer to Pascal string.
	'SUBS',		// Boolean: true = ATM and Font substitution are on and functional
	'LINE',		// Boolean: true = line height preserved, False = char spacing preserved.
	'OFF ',		// ATM is turned off either by control panel or by internal error.
	'DBYT'		// Boolean: double byte functionality available.
***/

typedef struct {
	unsigned long	tag;
	long	length;
	long	tagValue; /* may need to be interpreted either as as a long or long*. */
} ATMInfoTag;
	

typedef struct {
	long	tagCount;
	ATMInfoTag*	tags;	/* This must be a pointer to an array of ATMInfoTags */
} ATMInfo;



/***************************************************************************/
/*	Function call wrappers												   */
/***************************************************************************/


/**********************  Power PC Definitions  **************************/
#if defined(powerc) && powerc


#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif

/*
 *  Define/declare function call sequences
 */
#define ATMxProto0(name,ret)							UniversalProcPtr name
#define ATMxProto1(name,ret,v1)							UniversalProcPtr name
#define ATMxProto2(name,ret,v1,v2)						UniversalProcPtr name
#define ATMxProto3(name,ret,v1,v2,v3)					UniversalProcPtr name
#define ATMxProto4(name,ret,v1,v2,v3,v4)				UniversalProcPtr name
#define ATMxProto5(name,ret,v1,v2,v3,v4,v5)				UniversalProcPtr name
#define ATMxProto6(name,ret,v1,v2,v3,v4,v5,v6)			UniversalProcPtr name
#define ATMxProto7(name,ret,v1,v2,v3,v4,v5,v6,v7)		UniversalProcPtr name
#define ATMxProto8(name,ret,v1,v2,v3,v4,v5,v6,v7,v8)	UniversalProcPtr name
#define ATMxProto9(name,ret,v1,v2,v3,v4,v5,v6,v7,v8,v9)	UniversalProcPtr name
#define ATMxProto10(name,ret,v1,v2,v3,v4,v5,v6,v7,v8,v9,v10)	UniversalProcPtr name
/*
 *  Make function calls
 */
 // For ATM public call-backs.
#define ATMXCall0i(name)							CallUniversalProc(ATMProcs.name,name ## ProcInfo)
#define ATMXCall1i(name,v1)							CallUniversalProc(ATMProcs.name,name ## ProcInfo,v1)
#define ATMXCall2i(name,v1,v2)						CallUniversalProc(ATMProcs.name,name ## ProcInfo,v1,v2)
#define ATMXCall3i(name,v1,v2,v3)					CallUniversalProc(ATMProcs.name,name ## ProcInfo,v1,v2,v3)
#define ATMXCall4i(name,v1,v2,v3,v4)				CallUniversalProc(ATMProcs.name,name ## ProcInfo,v1,v2,v3,v4)
#define ATMXCall5i(name,v1,v2,v3,v4,v5)				CallUniversalProc(ATMProcs.name,name ## ProcInfo,v1,v2,v3,v4,v5)
#define ATMXCall6i(name,v1,v2,v3,v4,v5,v6)			CallUniversalProc(ATMProcs.name,name ## ProcInfo,v1,v2,v3,v4,v5,v6)
#define ATMXCall7i(name,v1,v2,v3,v4,v5,v6,v7)		CallUniversalProc(ATMProcs.name,name ## ProcInfo,v1,v2,v3,v4,v5,v6,v7)
#define ATMXCall8i(name,v1,v2,v3,v4,v5,v6,v7,v8)	CallUniversalProc(ATMProcs.name,name ## ProcInfo,v1,v2,v3,v4,v5,v6,v7,v8)
#define ATMXCall9i(name,v1,v2,v3,v4,v5,v6,v7,v8,v9)	CallUniversalProc(ATMProcs.name,name ## ProcInfo,v1,v2,v3,v4,v5,v6,v7,v8,v9)
#define ATMXCall10i(name,v1,v2,v3,v4,v5,v6,v7,v8,v9,v10)	CallUniversalProc(ATMProcs.name,name ## ProcInfo,v1,v2,v3,v4,v5,v6,v7,v8,v9,v10)

// For Database Backdoor call-backs
#define ATMDB_XCall0i(name)								CallUniversalProc(p_procs.name,name ## ProcInfo)
#define ATMDB_XCall1i(name,v1)							CallUniversalProc(p_procs.name,name ## ProcInfo,v1)
#define ATMDB_XCall2i(name,v1,v2)						CallUniversalProc(p_procs.name,name ## ProcInfo,v1,v2)
#define ATMDB_XCall3i(name,v1,v2,v3)					CallUniversalProc(p_procs.name,name ## ProcInfo,v1,v2,v3)
#define ATMDB_XCall4i(name,v1,v2,v3,v4)					CallUniversalProc(p_procs.name,name ## ProcInfo,v1,v2,v3,v4)
#define ATMDB_XCall5i(name,v1,v2,v3,v4,v5)				CallUniversalProc(p_procs.name,name ## ProcInfo,v1,v2,v3,v4,v5)
#define ATMDB_XCall6i(name,v1,v2,v3,v4,v5,v6)			CallUniversalProc(p_procs.name,name ## ProcInfo,v1,v2,v3,v4,v5,v6)
#define ATMDB_XCall7i(name,v1,v2,v3,v4,v5,v6,v7)		CallUniversalProc(p_procs.name,name ## ProcInfo,v1,v2,v3,v4,v5,v6,v7)
#define ATMDB_XCall8i(name,v1,v2,v3,v4,v5,v6,v7,v8)		CallUniversalProc(p_procs.name,name ## ProcInfo,v1,v2,v3,v4,v5,v6,v7,v8)
#define ATMDB_XCall9i(name,v1,v2,v3,v4,v5,v6,v7,v8,v9)	CallUniversalProc(p_procs.name,name ## ProcInfo,v1,v2,v3,v4,v5,v6,v7,v8,v9)
#define ATMDB_XCall10i(name,v1,v2,v3,v4,v5,v6,v7,v8,v9,v10)	CallUniversalProc(p_procs.name,name ## ProcInfo,v1,v2,v3,v4,v5,v6,v7,v8,v9,v10)

/*
 *  PowerPC function interface definitions...
 */
#define MMFPHookProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(DialogPtr)))

#define fontAvailableProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(short)))

#define showTextProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(Byte *)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(ATMFixedMatrix *)))

#define xyshowTextProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(Byte *)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(ATMFixedMatrix *)))\
		 | STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(Fixed *)))

#define showTextErrProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(Byte *)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(ATMFixedMatrix *)))\
		 | STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(short *)))

#define xyshowTextErrProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(Byte *)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(ATMFixedMatrix *)))\
		 | STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(Fixed *)))\
		 | STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof(short *)))

#define getOutlineProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(ATMFixedMatrix *)))\
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(Ptr)))\
		 | STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(UniversalProcPtr)))\
		 | STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof(UniversalProcPtr)))\
		 | STACK_ROUTINE_PARAMETER(6, SIZE_CODE(sizeof(UniversalProcPtr)))\
		 | STACK_ROUTINE_PARAMETER(7, SIZE_CODE(sizeof(UniversalProcPtr)))

#define closePathHookProcInfo \
		 kCStackBased \
		 | RESULT_SIZE(SIZE_CODE(sizeof(short))) \
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(Ptr)))

#define curveToHookProcInfo \
		 kCStackBased \
		 | RESULT_SIZE(SIZE_CODE(sizeof(short))) \
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(Ptr))) \
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(ATMPFixedPoint))) \
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(ATMPFixedPoint))) \
		 | STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(ATMPFixedPoint)))

#define lineToHookProcInfo \
		 kCStackBased \
		 | RESULT_SIZE(SIZE_CODE(sizeof(short))) \
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(Ptr))) \
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(ATMPFixedPoint)))

#define moveToHookProcInfo \
		 kCStackBased \
		 | RESULT_SIZE(SIZE_CODE(sizeof(short))) \
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(Ptr))) \
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(ATMPFixedPoint)))

#define startFillProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(short)))

#define fillMoveToProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(ATMPFixedPoint)))

#define fillLineToProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(ATMPFixedPoint)))

#define fillCurveToProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(ATMPFixedPoint)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(ATMPFixedPoint)))\
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(ATMPFixedPoint)))

#define fillClosePathProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(short)))

#define endFillProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(short)))\

#define disableProcInfo	kPascalStackBased

#define reenableProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(short)))

#define getBlendedFontTypeProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(StringPtr)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(short)))

#define encodeBlendedFontNameProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(StringPtr)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(Fixed *)))\
		 | STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(StringPtr)))

#define decodeBlendedFontNameProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(StringPtr)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(StringPtr)))\
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(short *)))\
		 | STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(Fixed *)))\
		 | STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof(StringPtr)))

#define addMacStyleToCoordsProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(Fixed *)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(Fixed *)))\
		 | STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(short *)))

#define convertCoordsToBlendProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(Fixed *)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(Fixed *)))

#define normToUserCoordsProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(Fixed *)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(Fixed *)))\

#define userToNormCoordsProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(Fixed *)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(Fixed *)))

#define createTempBlendedFontProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(Fixed *)))\
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(short *)))

#define disposeTempBlendedFontProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short)))

#define createPermBlendedFontProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(StringPtr)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(short *)))

#define disposePermBlendedFontProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short)))

#define getTempBlendedFontFileIDProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short *)))

#define getNumAxesProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short *)))

#define getNumMastersProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short *)))

#define getMasterFONDProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(short *)))

#define copyFitProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(Fixed)))\
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(Fixed *)))\
		 | STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(Fixed *)))\
		 | STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof(Fixed *)))\
		 | STACK_ROUTINE_PARAMETER(6, SIZE_CODE(sizeof(Fixed *)))

#define getFontSpecsProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(FontSpecs *)))

#define showTextDesignProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(StringPtr)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(Byte *)))\
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(ATMFixedMatrix *)))\
		 | STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof(Fixed *)))\
		 | STACK_ROUTINE_PARAMETER(6, SIZE_CODE(sizeof(Fixed *)))\
		 | STACK_ROUTINE_PARAMETER(7, SIZE_CODE(sizeof(short *)))

#define getAxisBlendInfoProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(short *)))\
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(StringPtr)))\
		 | STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof(StringPtr)))\
		 | STACK_ROUTINE_PARAMETER(6, SIZE_CODE(sizeof(StringPtr)))

#define fontFitProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(Fixed *)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(short *)))\
		 | STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(Fixed *)))\
		 | STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof(Fixed *)))\
		 | STACK_ROUTINE_PARAMETER(6, SIZE_CODE(sizeof(Fixed *)))\
		 | STACK_ROUTINE_PARAMETER(7, SIZE_CODE(sizeof(Fixed *)))

#define getNumBlessedFontsProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short *)))

#define getBlessedFontNameProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(StringPtr)))\
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(Fixed *)))

#define getRegularBlessedFontProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short *)))

#define flushCacheProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))

#define getFontFamilyFONDProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(StringPtr)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(short *)))

#define MMFontPickerProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(MMFP_Parms *)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(MMFP_Reply *)))

#define isSubstFontProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(Boolean)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(StringPtr)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(FontSpecs ***)))\
		 | STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof(Handle *)))

#define getRunsProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(ATMFixedMatrix *)))\
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(Ptr)))\
		 | STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(ATMPRunRec)))

#define getPSNumProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(StringPtr)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(short *)))\
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(Boolean)))

#define getATMInfoProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(ATMInfo)))

#define obsolete1ProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))

#define getOutline2ProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))\
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(char *)))\
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(ATMFixedMatrix *)))\
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(Ptr)))\
		 | STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(UniversalProcPtr)))\
		 | STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof(UniversalProcPtr)))\
		 | STACK_ROUTINE_PARAMETER(6, SIZE_CODE(sizeof(UniversalProcPtr)))\
		 | STACK_ROUTINE_PARAMETER(7, SIZE_CODE(sizeof(UniversalProcPtr)))


#define spare1ProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))
		 
#define spare2ProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))

#define spare3ProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))

#define spare4ProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))

#define spare5ProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))

#define spare6ProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))

#define spare7ProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))

#define spare12ProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))

#define spare13ProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))

#define spare14ProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))

#define spare15ProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))

#define spare16ProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))

#define spare17ProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))

#define spare18ProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))

#define spare19ProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))

#define spare20ProcInfo	kPascalStackBased\
		 | RESULT_SIZE(SIZE_CODE(sizeof(ATMErr)))

#define NewAdd_MoveProc(userRoutine)		\
		NewRoutineDescriptor((ProcPtr)(userRoutine), moveToHookProcInfo, (ISAType) kPowerPCISA)
#define NewAdd_LineProc(userRoutine)		\
		NewRoutineDescriptor((ProcPtr)(userRoutine), lineToHookProcInfo,(ISAType) kPowerPCISA)
#define NewAdd_CurveProc(userRoutine)		\
		NewRoutineDescriptor((ProcPtr)(userRoutine), curveToHookProcInfo,(ISAType) kPowerPCISA)
#define NewAdd_CloseProc(userRoutine)		\
		NewRoutineDescriptor((ProcPtr)(userRoutine), closePathHookProcInfo,(ISAType) kPowerPCISA)
#else

/*
 *  Define/declare function call sequences
 */
#if ATM_USE_PROTOTYPES

#define ATMxProto0(name,ret)							ret (*name)(void)
#define ATMxProto1(name,ret,v1)							ret (*name)(v1)
#define ATMxProto2(name,ret,v1,v2)						ret (*name)(v1,v2)
#define ATMxProto3(name,ret,v1,v2,v3)					ret (*name)(v1,v2,v3)
#define ATMxProto4(name,ret,v1,v2,v3,v4)				ret (*name)(v1,v2,v3,v4)
#define ATMxProto5(name,ret,v1,v2,v3,v4,v5)				ret (*name)(v1,v2,v3,v4,v5)
#define ATMxProto6(name,ret,v1,v2,v3,v4,v5,v6)			ret (*name)(v1,v2,v3,v4,v5,v6)
#define ATMxProto7(name,ret,v1,v2,v3,v4,v5,v6,v7)		ret (*name)(v1,v2,v3,v4,v5,v6,v7)
#define ATMxProto8(name,ret,v1,v2,v3,v4,v5,v6,v7,v8)	ret (*name)(v1,v2,v3,v4,v5,v6,v7,v8)
#define ATMxProto9(name,ret,v1,v2,v3,v4,v5,v6,v7,v8,v9)	ret (*name)(v1,v2,v3,v4,v5,v6,v7,v8,v9)
#define ATMxProto10(name,ret,v1,v2,v3,v4,v5,v6,v7,v8,v9,v10)	ret (*name)(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10)

#else

#define ATMxProto0(name,ret)							ret (*name)()
#define ATMxProto1(name,ret,v1)							ret (*name)()
#define ATMxProto2(name,ret,v1,v2)						ret (*name)()
#define ATMxProto3(name,ret,v1,v2,v3)					ret (*name)()
#define ATMxProto4(name,ret,v1,v2,v3,v4)				ret (*name)()
#define ATMxProto5(name,ret,v1,v2,v3,v4,v5)				ret (*name)()
#define ATMxProto6(name,ret,v1,v2,v3,v4,v5,v6)			ret (*name)()
#define ATMxProto7(name,ret,v1,v2,v3,v4,v5,v6,v7)		ret (*name)()
#define ATMxProto8(name,ret,v1,v2,v3,v4,v5,v6,v7,v8)	ret (*name)()
#define ATMxProto9(name,ret,v1,v2,v3,v4,v5,v6,v7,v8,v9)	ret (*name)()
#define ATMxProto10(name,ret,v1,v2,v3,v4,v5,v6,v7,v8,v9,v10)	ret (*name)()

#endif

/*
 *  Make function calls
 */
 // For ATM public call-backs.
#define ATMXCall0i(name)							(*ATMProcs.name)()
#define ATMXCall1i(name,v1)							(*ATMProcs.name)(v1)
#define ATMXCall2i(name,v1,v2)						(*ATMProcs.name)(v1,v2)
#define ATMXCall3i(name,v1,v2,v3)					(*ATMProcs.name)(v1,v2,v3)
#define ATMXCall4i(name,v1,v2,v3,v4)				(*ATMProcs.name)(v1,v2,v3,v4)
#define ATMXCall5i(name,v1,v2,v3,v4,v5)				(*ATMProcs.name)(v1,v2,v3,v4,v5)
#define ATMXCall6i(name,v1,v2,v3,v4,v5,v6)			(*ATMProcs.name)(v1,v2,v3,v4,v5,v6)
#define ATMXCall7i(name,v1,v2,v3,v4,v5,v6,v7)		(*ATMProcs.name)(v1,v2,v3,v4,v5,v6,v7)
#define ATMXCall8i(name,v1,v2,v3,v4,v5,v6,v7,v8)	(*ATMProcs.name)(v1,v2,v3,v4,v5,v6,v7,v8)
#define ATMXCall9i(name,v1,v2,v3,v4,v5,v6,v7,v8,v9)	(*ATMProcs.name)(v1,v2,v3,v4,v5,v6,v7,v8,v9)
#define ATMXCall10i(name,v1,v2,v3,v4,v5,v6,v7,v8,v9,v10)	(*ATMProcs.name)(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10)

 // For ATM Database Backdoor call-backs.
#define ATMDB_XCall0i(name)								(*p_procs.name)()
#define ATMDB_XCall1i(name,v1)							(*p_procs.name)(v1)
#define ATMDB_XCall2i(name,v1,v2)						(*p_procs.name)(v1,v2)
#define ATMDB_XCall3i(name,v1,v2,v3)					(*p_procs.name)(v1,v2,v3)
#define ATMDB_XCall4i(name,v1,v2,v3,v4)					(*p_procs.name)(v1,v2,v3,v4)
#define ATMDB_XCall5i(name,v1,v2,v3,v4,v5)				(*p_procs.name)(v1,v2,v3,v4,v5)
#define ATMDB_XCall6i(name,v1,v2,v3,v4,v5,v6)			(*p_procs.name)(v1,v2,v3,v4,v5,v6)
#define ATMDB_XCall7i(name,v1,v2,v3,v4,v5,v6,v7)		(*p_procs.name)(v1,v2,v3,v4,v5,v6,v7)
#define ATMDB_XCall8i(name,v1,v2,v3,v4,v5,v6,v7,v8)		(*p_procs.name)(v1,v2,v3,v4,v5,v6,v7,v8)
#define ATMDB_XCall9i(name,v1,v2,v3,v4,v5,v6,v7,v8,v9)	(*p_procs.name)(v1,v2,v3,v4,v5,v6,v7,v8,v9)
#define ATMDB_XCall10i(name,v1,v2,v3,v4,v5,v6,v7,v8,v9,v10)	(*p_procs.name)(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10)

#endif



#ifndef spare1ProcEntry
#define spare1ProcEntry 	ATMxProto0(spare1, pascal ATMErr)
#endif
#ifndef spare2ProcEntry
#define spare2ProcEntry 	ATMxProto0(spare2, pascal ATMErr)
#endif
#ifndef spare3ProcEntry
#define spare3ProcEntry 	ATMxProto0(spare3, pascal ATMErr)
#endif
#ifndef spare4ProcEntry
#define spare4ProcEntry 	ATMxProto0(spare4, pascal ATMErr)
#endif
#ifndef spare5ProcEntry
#define spare5ProcEntry 	ATMxProto0(spare5, pascal ATMErr)
#endif
#ifndef spare6ProcEntry
#define spare6ProcEntry 	ATMxProto0(spare6, pascal ATMErr)
#endif
#ifndef spare7ProcEntry
#define spare7ProcEntry 	ATMxProto0(spare7, pascal ATMErr)
#endif
#ifndef spare12ProcEntry
#define spare12ProcEntry	ATMxProto0(spare12, pascal ATMErr)
#endif
#ifndef spare13ProcEntry
#define spare13ProcEntry	ATMxProto0(spare13, pascal ATMErr)
#endif
#ifndef spare14ProcEntry
#define spare14ProcEntry	ATMxProto0(spare14, pascal ATMErr)
#endif
#ifndef spare15ProcEntry
#define spare15ProcEntry	ATMxProto0(spare15, pascal ATMErr)
#endif
#ifndef spare16ProcEntry
#define spare16ProcEntry	ATMxProto0(spare16, pascal ATMErr)
#endif
#ifndef spare17ProcEntry
#define spare17ProcEntry	ATMxProto0(spare17, pascal ATMErr)
#endif
#ifndef spare18ProcEntry
#define spare18ProcEntry	ATMxProto0(spare18, pascal ATMErr)
#endif
#ifndef spare19ProcEntry
#define spare19ProcEntry	ATMxProto0(spare19, pascal ATMErr)
#endif
#ifndef spare20ProcEntry
#define spare20ProcEntry 	ATMxProto0(spare20, pascal ATMErr)
#endif



/***************************************************************************/
/*	Multiple Master Font Creator dialog structs and ProcPtrs.			   */
/***************************************************************************/

/*
 * Creator/Picker option flags
 */
#define CREATOR_DIALOG	0x0001	/* Creator dialog, else Picker dialog */
#define OK_IS_QUIT		0x0002	/* Ok button title becomes Quit */

/*
 * Multiple Master Font Picker parameters
 *
 * All strings are Pascal format.
 *
 * The MMFP_Parms apply to both the Font Picker and Font Creator dialogs.
 * Set the CREATOR_DIALOG bit in "flags" to get the Creator rather than the
 * Picker dialog.  Use "where" to specify the dialog's position, or use [0,0]
 * to get the standard positioning.  Pass in your own "prompt" string or use 
 * NULL to get the standard prompt.  Pass in your own "sample" string or use
 * NULL to get the standard sample.  The sample string is continuously redrawn
 * to illustrate the current font as the user navigates through the Multiple
 * Master design space.  A short fragment of the user's selection in the
 * current document is a reasonable alternative to the default sample text.
 * The user is also able to type in the sample box to see other characters.
 * Pass in your own sample "startSize" or use 0 to get the default initial 
 * sample size.  The user can also adjust the sample size through a popup
 * menu and type-in size box.  Pass in your own "dlgHook" dialog event hook
 * function if you wish to change the dialog behavior or add your own items.
 * If you specify a "dlgHook", it is called by the Picker immediately after
 * each call to ModalDialog().  Refer to the Standard File Package chapter of
 * Inside Mac for more details on how to write and use a dialog hook routine.
 * Despite their different appearances, both the Creator and Picker dialogs
 * share the same item numbering.
 *
 * When the Picker or Creator dialog is first displayed, the family and 
 * instance popup menus and the sliders and axis values are all set to reflect
 * the initial font, and the sample string is rendered in this font at the 
 * initial sample size.  There are a number of ways to choose this initial font.
 * If "startFondID" is a Multiple Master font then it is used as the initial
 * font.  If "startFondID" is -1 or a regular (non-Multiple Master) font, then
 * "startFamilyName" is checked.  If this is the name of a Multiple Master
 * font, then the instance at "startCoords" is used as the initial font.
 * "StartCoords" must have as many elements as there are design axes for
 * the specified family.  If "startCoords" is NULL, then the first instance
 * in "startFamilyName" is used as the initial font.  If "startFamilyName"
 * is NULL, then "startCoords" is ignored and the default initial font is
 * chosen.
 */
 
 
#if  (defined(powerc) && powerc) || ATM_USE_PROTOTYPES
typedef ATMxProto2(MMFPHook, pascal short, short item, DialogPtr theDialog);
#else
typedef ProcPtr	MMFPHook;
#endif

struct MMFP_Parms
{
	short		version;			/* (in) always 1 */
	short		flags;				/* (in) option flags, 0 for default Picker dialog */
	Point		where;				/* (in) dialog's top-left corner, [0,0] for default */
	StringPtr	prompt;				/* (in) prompt string, NULL for default */
	StringPtr	sample;				/* (in) sample string, NULL for default */
	short		startFondID;		/* (in) initial font, -1 for none */
	StringPtr	startFamilyName;	/* (in) initial family, NULL for default */
	Fixed		*startCoords;		/* (in) initial axis coordinates, NULL for default */
	short		startSize;			/* (in) initial sample size, 0 for default */
	MMFPHook 	dlgHook;			/* (in) application dialog hook function, NULL for none */
};

typedef struct MMFP_Parms		MMFP_Parms, *MMFP_ParmsPtr;

/*
 * Multiple Master Font Picker Reply
 *
 * All strings are Pascal format.
 *
 * While the user manipulates the Picker or Creator dialog's controls to
 * navigate through the Multiple Master design space, the sample text is
 * continuously rerendered in the font instance with the selected design
 * coordinates.  A temporary instance is created on the fly whenever a
 * permanent instance does not already exist.  Only the permanent instances
 * remain when the dialog is dismissed.  The Creator dialog has only an
 * OK button while the Picker dialog has both OK and Cancel.  (For both
 * dialogs, OK can be retitled Quit using the OK_IS_QUIT flag bit).  The
 * Picker call returns ATM_NOERR (0) for OK, and ATM_PICKER_CANCELLED for 
 * Cancel.  Regardless of the way the user dismisses the dialog, any fields 
 * specified in MMFP_Reply are filled in to reflect the state in which the 
 * user last left the dialog.
 *
 * Any pointer argument specified as NULL is ignored.  The others are always
 * filled in.  Specify "sample" to get a copy of the user's current sample
 * text string.  Specify "familyName" to get the name of the current
 * Multiple Master family.  Specify "coords" to get an array of design
 * coordinates for the current instance.  "Coords" will contain "numAxes"
 * elements.  Use MaxBlendAxes to safely allocate an array long enough for
 * the returned coordinates.  If the font instance corresponding to these
 * design coordinates was a permanent one, its ID is returned in "fondID".
 * If the instance was a temporary one, it no longer exists, so "fondID" is 
 * set to -1.  The calling program may make appropriate ATM calls to create
 * a new temporary or permanent instance of that font using the family name
 * and design coordinates.
 */
struct MMFP_Reply
{
	StringPtr	sample;				/* (out) last sample string (Str255), NULL to ignore */
	short		fondID;				/* (out) selected font if permanent, else -1 */
	StringPtr	familyName;			/* (out) selected family (Str32), NULL to ignore */
	short		numAxes;			/* (out) number of design axes in selected family */
	Fixed		*coords;			/* (out) coords of selected instance, NULL to ignore */
	short		size;				/* (out) last sample size */
};
typedef struct MMFP_Reply		MMFP_Reply, *MMFP_ReplyPtr;



/***************************************************************************/
/*	** The ATM Proc Struct **											   */
/***************************************************************************/

 
#define ATMProcs3Version 3
typedef struct
	{
	long version;
	ATMxProto2(fontAvailable, short, short family, short style);
	ATMxProto3(showText, short, Byte *text, short length, ATMFixedMatrix *matrix);
	ATMxProto4(xyshowText, short, Byte *text, short length, ATMFixedMatrix *matrix,
						   Fixed *displacements);
	} ATMProcs3;

#define ATMProcs4Version 4
typedef struct
	{
	long version;
	ATMxProto2(fontAvailable, short, short family, short style);
	ATMxProto4(showTextErr, short, Byte *text, short length, ATMFixedMatrix *matrix,
							short *errorCode);
	ATMxProto5(xyshowTextErr, short, Byte *text, short length, ATMFixedMatrix *matrix, 
							  Fixed *displacements, short *errorCode);
	ATMxProto7(getOutline, short, short c, ATMFixedMatrix *matrix, Ptr clientHook,
						   ATMxProto2(MoveTo, short, Ptr clientHook, ATMPFixedPoint pc),
						   ATMxProto2(LineTo, short, Ptr clientHook, ATMPFixedPoint pc),
						   ATMxProto4(CurveTo, short, Ptr clientHook, ATMPFixedPoint pc1,
						   					   ATMPFixedPoint pc2, ATMPFixedPoint pc3), 
						   ATMxProto1(ClosePath, short, Ptr clientHook));
	ATMxProto0(startFill, short);
	ATMxProto1(fillMoveTo, short, ATMPFixedPoint pc);
	ATMxProto1(fillLineTo, short, ATMPFixedPoint pc);
	ATMxProto3(fillCurveTo, short, ATMFixedPoint pc1, ATMPFixedPoint pc2,
							ATMPFixedPoint pc3);
	ATMxProto0(fillClosePath, short);
	ATMxProto0(endFill, short);
	} ATMProcs4;

#define ATMProcs5Version	 5
#define ATMProcs6Version	 6
#define ATMProcs7Version	 7
#define ATMProcs8Version	 8
#define ATMProcs9Version	 9
#define ATMProcs10Version	10
#define ATMProcs11Version	11
/*
 * Note for version 5 and above.
 *
 * All the routines new for version 5 have pascal interfaces.
 * Depending on how the interface is initialized, the older routines may or may not be
 * pascal interface. 
 */
typedef struct
	{
	long version;
	ATMxProto2(fontAvailable, ATM_LANG short, short family, short style);
	ATMxProto4(showTextErr, ATM_LANG short, Byte *text, short length,
							ATMFixedMatrix *matrix, ATMErr *errorCode);
	ATMxProto5(xyshowTextErr, ATM_LANG short, Byte *text, short length,
							  ATMFixedMatrix *matrix, Fixed *displacements,
							  ATMErr *errorCode);
	ATMxProto7(getOutline, ATM_LANG short, short c, ATMFixedMatrix *matrix,
						   Ptr clientHook,
						   ATMxProto2(MoveTo, short, Ptr clientHook, ATMPFixedPoint pc),
						   ATMxProto2(LineTo, short, Ptr clientHook, ATMPFixedPoint pc),
						   ATMxProto4(CurveTo, short, Ptr clientHook, ATMPFixedPoint pc1,
											   ATMPFixedPoint pc2, ATMPFixedPoint pc3), 
						   ATMxProto1(ClosePath, short, Ptr clientHook));	
	ATMxProto0(startFill, ATM_LANG short);
	ATMxProto1(fillMoveTo, ATM_LANG short, ATMPFixedPoint pc);
	ATMxProto1(fillLineTo, ATM_LANG short, ATMPFixedPoint pc);
	ATMxProto3(fillCurveTo, ATM_LANG short, ATMPFixedPoint pc1, ATMPFixedPoint pc2,
							ATMPFixedPoint pc3);
	ATMxProto0(fillClosePath, ATM_LANG short);
	ATMxProto0(endFill, ATM_LANG short);
	
	/* New for version 5 -- control functions for use with control panel (&testing). */
	ATMxProto0(disable, pascal void);
	ATMxProto0(reenable, pascal void);

	/* New for version 5 (with blended fonts) */
	ATMxProto2(getBlendedFontType, pascal short, StringPtr fontName, short fondID);
	ATMxProto4(encodeBlendedFontName, pascal ATMErr, StringPtr familyName,
									  short numAxes, Fixed *coords, StringPtr blendName);
	ATMxProto5(decodeBlendedFontName, pascal ATMErr, StringPtr blendName,
									  StringPtr familyName, short *numAxes,
									  Fixed *coords, StringPtr displayInstanceStr);
	ATMxProto4(addMacStyleToCoords, pascal ATMErr, Fixed *coords, short macStyle,
									Fixed *newCoords, short *stylesLeft);
	ATMxProto2(convertCoordsToBlend, pascal ATMErr, Fixed *coords,
									 Fixed *weightVector);
	ATMxProto2(normToUserCoords, pascal ATMErr, Fixed *normalCoords, Fixed *coords);
	ATMxProto2(userToNormCoords, pascal ATMErr, Fixed *coords, Fixed *normalCoords);
	ATMxProto3(createTempBlendedFont, pascal ATMErr, short numAxes, Fixed *coords,
									  short *useFondID);
	ATMxProto1(disposeTempBlendedFont, pascal ATMErr, short fondID);
	ATMxProto4(createPermBlendedFont, pascal ATMErr,StringPtr fontName,
									  short fontSize, short fontFileID, short *retFondID);
	ATMxProto1(disposePermBlendedFont, pascal ATMErr, short fondID);
	ATMxProto1(getTempBlendedFontFileID, pascal ATMErr, short *fileID);
	ATMxProto1(getNumAxes, pascal ATMErr, short *numAxes);
	ATMxProto1(getNumMasters, pascal ATMErr, short *numMasters);
	ATMxProto2(getMasterFOND, pascal ATMErr, short i, short *masterFOND);
	ATMxProto6(copyFit, pascal ATMErr, short method, Fixed TargetWidth,
						Fixed *beginCoords, Fixed *baseWidths, Fixed *resultWidth,
						Fixed *resultCoords);
	ATMxProto1(getFontSpecs, pascal ATMErr, FontSpecs *hints);
	spare1ProcEntry;		/* expansion */
	spare2ProcEntry;		/* expansion */
	ATMxProto7(showTextDesign, pascal ATMErr, StringPtr fontFamily, Byte *text,
							   short len, ATMFixedMatrix *matrix, Fixed *coords,
							   Fixed *displacements, short *lenDisplayed);
	ATMxProto6(getAxisBlendInfo, pascal ATMErr, short axis, short *userMin,
								 short *userMax, StringPtr type, StringPtr label,
								 StringPtr shortLabel);
	ATMxProto7(fontFit, pascal ATMErr, Fixed *origCoords, short numTargets,
						short *varyAxes, Fixed *targetMetrics, Fixed **masterMetrics,
						Fixed *retCoords, Fixed *retWeightVector);
	ATMxProto1(getNumBlessedFonts, pascal ATMErr, short *numBlessedFonts);
	ATMxProto3(getBlessedFontName, pascal ATMErr, short i, StringPtr blessedFontName,
								   Fixed *coords);
	ATMxProto1(getRegularBlessedFont, pascal ATMErr, short *regularIndex);
	ATMxProto0(flushCache, pascal ATMErr);
	ATMxProto2(getFontFamilyFOND, pascal ATMErr, StringPtr familyName,
								  short *retFondID);
	ATMxProto2(MMFontPicker, pascal ATMErr, MMFP_Parms *parms, MMFP_Reply *reply);

	/* New for version 8 (with faux fonts && "getRuns()" */
	ATMxProto5(isSubstFont, pascal Boolean, StringPtr fontName, short fondID,
							short style, FontSpecs ***fontSpecs, Handle *chamName);
	ATMxProto4(getRuns, pascal ATMErr, short c, ATMFixedMatrix *matrix,
						Ptr clientHook, ATMPRunRec pRunRec);	
	ATMxProto3(getPSNum, pascal ATMErr, StringPtr psName, short *retFondID,
						 Boolean doCreate);
	ATMxProto0(obsolete1, pascal ATMErr);

	/* New for version 10 */
	ATMxProto7(getOutline2, pascal short,char *cp, ATMFixedMatrix *matrix, Ptr clientHook,
					short (*MoveTo)(Ptr clientHook, ATMPFixedPoint pc), 
					short (*LineTo)(Ptr clientHook, ATMPFixedPoint pc), 
					short (*CurveTo)(Ptr clientHook, ATMPFixedPoint pc1, ATMPFixedPoint pc2, ATMPFixedPoint pc3), 
					short (*ClosePath)(Ptr clientHook));	/* double-byte only */
	spare3ProcEntry;		/* expansion */ 
	spare4ProcEntry;		/* expansion */
	spare5ProcEntry;		/* expansion */

	spare6ProcEntry; 
	/* New for version 11. Replaces spare 10 and 11 */
	ATMxProto1(getATMInfo, pascal ATMErr, ATMInfo* request);
	spare7ProcEntry;
	
	spare12ProcEntry;		/* expansion */
	spare13ProcEntry;		/* expansion */
	spare14ProcEntry;		/* expansion */
	spare15ProcEntry;		/* expansion */
	spare16ProcEntry;		/* expansion */
	spare17ProcEntry;		/* expansion */
	spare18ProcEntry;		/* expansion */
	spare19ProcEntry;		/* expansion */
	spare20ProcEntry;		/* expansion */

	} ATMProcs5, ATMProcs6, ATMProcs7, ATMProcs8, ATMProcs9, ATMProcs10, ATMProcs11;

#define LATEST_VERSION ATMProcs11Version
#define LATEST_PROCS ATMProcs11



/***************************************************************************/
/*	ATMInterface.c function declarations: single function calls for each   */
/*	ProcPtr in the ATMProcs struct.										   */
/***************************************************************************/

/* **************** The following routines are available under ATMProcs3Version: **************** */

#ifdef __cplusplus
extern "C" {
#endif

/* Initializes ATMInterface for a given version, returns 1 if and only if that version
	of the ATM interface is available */
short initVersionATM(short);
short initNameAndVersionATM(short version, StringPtr name, short *errorCode);

/* Returns 1 if and only if ATM can image the specified family and style */
short fontAvailableATM(short family, short style);

/* Show length characters starting at text transformed by the specified matrix */
/* Returns the number of characters not shown */
/* Matrix maps one point character space to device space, relative to current pen position */
/* Matrix's tx and ty components are updated */
short showTextATM(Byte *text, short length, ATMFixedMatrix *matrix);

/* Show length characters starting at text transformed by the specified matrix */
/* Matrix maps one point character space to device space, relative to current pen position */
/* Matrix's tx and ty components are updated */
/* Character x and y widths are specified by displacements */
/* Returns the number of characters not shown */
short xyshowTextATM(Byte *text, short length, ATMFixedMatrix *matrix, ATMFixed *displacements);

#ifdef __cplusplus
}
#endif

/* ****************************** end of ATMProcs3Version routines ****************************** */

/* **************** The following routines are available under ATMProcs4Version: **************** */

#ifdef __cplusplus
extern "C" {
#endif

/* Initializes ATMInterface for a given version, returns 1 if and only if that version
	of the ATM interface is available */
short initVersionATM(short);

/* Returns 1 if and only if ATM can image the specified family and style */
short fontAvailableATM(short family, short style);


/* Show length characters starting at text transformed by the specified matrix */
/* Returns the number of characters not shown */
/* Matrix maps one point character space to device space, relative to current pen position */
/* Matrix's tx and ty components are updated */
/* *errorCode is set to ATM_NOERR if all went well, otherwise to one of the above error codes */
short showTextATMErr(Byte *text, short length, ATMFixedMatrix *matrix, short *errorCode);

/* Show length characters starting at text transformed by the specified matrix */
/* Matrix maps one point character space to device space, relative to current pen position */
/* Matrix's tx and ty components are updated */
/* Character x and y widths are specified by displacements */
/* Returns the number of characters not shown */
/* *errorCode is set to ATM_NOERR if all went well, otherwise to one of the above error codes */
short xyshowTextATMErr(Byte *text, short length, ATMFixedMatrix *matrix, ATMFixed *displacements, short *errorCode);

/* Get, via call-back procs, the PostScript definition of a character outline */
/* The call-backs should return 1 if they wish to continue being called for the remainder */
/* of the current character; they should return 0 if they wish to terminate getOutlineATM */
/* Returns ATM_NOERR if successful, otherwise one of the ATM error codes */
/* Matrix maps one point character space to device space */
short getOutlineATM(short c, ATMFixedMatrix *matrix, Ptr clientHook,
					short (*MoveTo)(/* Ptr clientHook, ATMPFixedPoint pc */), 
					short (*LineTo)(/* Ptr clientHook, ATMPFixedPoint pc */), 
					short (*CurveTo)(/* Ptr clientHook, ATMPFixedPoint pc1, ATMPFixedPoint pc2, ATMPFixedPoint pc3 */), 
					short (*ClosePath)(/* Ptr clientHook */));

/* Render the specified path according to the graphic state implicit in the current GrafPort */
/* All these routines return ATM_NOERR if successful, otherwise one of the ATM error codes */

short startFillATM(void);
short fillMoveToATM(ATMPFixedPoint pc);
short fillLineToATM(ATMPFixedPoint pc);
short fillCurveToATM(ATMPFixedPoint pc1, ATMPFixedPoint pc2, ATMPFixedPoint pc3);
short fillClosePathATM(void);
short endFillATM(void);

short getOutlineATM2(char *cp, ATMFixedMatrix *matrix, Ptr clientHook,
					short (*MoveTo)(/* Ptr clientHook, ATMPFixedPoint pc */), 
					short (*LineTo)(/* Ptr clientHook, ATMPFixedPoint pc */), 
					short (*CurveTo)(/* Ptr clientHook, ATMPFixedPoint pc1, ATMPFixedPoint pc2, ATMPFixedPoint pc3 */), 
					short (*ClosePath)(/* Ptr clientHook */));

#ifdef __cplusplus
}
#endif

/* ****************************** end of ATMProcs4Version routines ****************************** */


/* **************** The following routines are available under ATMProcs5Version: **************** */

#ifdef __cplusplus
extern "C" {
#endif

/* Returns TRUE if the specified font is a multi-master or "blended" font. */
/* Uses the "fontName" if not NULL, otherwise looks at the fondID. */
short getBlendedFontTypeATM(StringPtr fontName, short fondID);

/*
 * Given a "familyName" (eg. "Minion") and coordinates in the design space, create
 * a blended font name. This really isn't implemented well as it doesn't add labels (yet).
 */
ATMErr encodeBlendedFontNameATM(StringPtr familyName, short numAxes,
				Fixed *coords, StringPtr blendName);

/*
 * Given a "Blended" fontName (eg. "Minion_67 BLD 2 CND"), return coordinates
 * in the design space and the font's family name. The "family name" is really
 * just the stuff before the '_' (eg. "Minion"), and the display instance string
 * is the stuff afterwards (eg. "67 BLD 2 CND"). The "family name" could be
 * "AdobeSans", "Adobe Sans" or "AdobeSan", as the only criteria is that the
 * PostScript filename ("AdobeSan") can be derived from it according to the
 * 533 rule (use first 5 letters from the first word, and first 3 letters from
 * every word afterwards.
 *
 * You may pass NULL for any of the return parameters.
 *
 */
ATMErr decodeBlendedFontNameATM(StringPtr blendName, StringPtr familyName,
				short *numAxes, Fixed *coords, StringPtr displayInstanceStr);

/*
 * When the BOLD bit is set on a multiple master font, we may add a calculated amount
 * to the WEIGHT axis (if any). This routines returns the new coordinates for any given
 * Mac style for this font. Note that a font designer may not choose to modify the
 * coordinates for the Bold, Condensed or Expanded bits, so these will return unchanged.
 *
 * Whatever styles a blended font design can't handle is returned in "stylesLeft", though
 * these styles might be handled by QuickDraw (like Outline, Shadow, etc).
 */
ATMErr	addMacStyleToCoordsATM(Fixed *coords, short macStyle, Fixed *newCoords, short *stylesLeft);

/*
 * Convert the design coordinates for the current font to a blend vector. This is necessary
 * for those who need to output PostScript directlyÑthis is suitable for passing to 
 * the PostScript operator "makeBlendedFont".
 */
ATMErr convertCoordsToBlendATM(Fixed *coords, Fixed *weightVector);

/*
 * Convert the design coordinates to a range usermin..usermax instead of 0.0..1.0.
 */
ATMErr normToUserCoordsATM(Fixed *normalCoords, Fixed *coords);

/*
 * Convert the design coordinates to a range 0.0..1.0 instead of usermin..usermax.
 */
ATMErr userToNormCoordsATM(Fixed *coords, Fixed *normalCoords);

/*
 * Create a temporary blended font for user interface purposes (so users can choose a blend). 
 * The first time, initialize "*useFondID" to zero and a new fondID will be returned. 
 * While displaying various blends, use the returned value in "*useFondID" until the user 
 * is satisfied. Then dispose of this font with disposeTempBlendedFontATM(). 
 * Don't expect the name of the font to be what you gave it, we may change it for internal reasons.
 */ 
ATMErr createTempBlendedFontATM(short numAxes, Fixed *coords, short *useFondID);
ATMErr disposeTempBlendedFontATM(short fondID);

/*
 * Create a permanent blended font, of a given size in the given font file. 
 * Note that the zero-sized case is the only one supported, which will create a "dummy" 
 * 10 point font.
 */
ATMErr createPermBlendedFontATM(StringPtr fontName, short fontSize, short fontFileID, short *retFondID);
ATMErr disposePermBlendedFontATM(short fondID);

/*
 * We store newly created fonts in a temporary file. If you wish to use it (ie, for the 
 * above routines), feel free.
 */
ATMErr getTempBlendedFontFileIDATM(short *fileID);

/*
 * Given the family name (eg. "Minion") and axis ID, return the number of axes.
 */
ATMErr getNumAxesATM(short *numAxes);

/*
 * Get the number of base designs (useful for the "CopyFit" routine, below).
 */
ATMErr getNumMastersATM(short *numMasters);

/*
 * Get the i'th base design for the given multiple master font (useful for the "CopyFit"
 * routine, below).
 */
ATMErr getMasterFONDATM(short i, short *masterFOND);

/*
 * Used to choose a multiple-master instance that will match the given widths.
 * First, fill in the baseWidths array with the width of the string for each one of
 * the base designs. Then choose a method for us to follow (default, constantStems, varStems).
 * Give us the targetWidth you want your string and the "beginDesign", which says what your
 * original coords were. We will return the best fit we could find in resultDesign, and
 * the actual width in resultWidth.
 */
ATMErr copyFitATM(short method, Fixed TargetWidth, Fixed *beginDesign,
					Fixed *baseWidths, Fixed *resultWidth, Fixed *resultDesign);

/*
 * This extends previous "ShowText" calls so that we can display strings without ever
 * creating an explicit FOND (eventually). For the moment, "fontFamily" is ignored, and
 * should be passed NULL (this is for future expansion).  Pass NULL for "coords" unless
 * this is a multiple master font. If "displacements" is NULL then we use the widths from
 * the FOND/NFNT; otherwise "displacements" gives us the x and y deltas between each
 * character.
 */
ATMErr showTextDesignATM(StringPtr fontFamily, Byte *text, short len, ATMFixedMatrix *matrix,
				Fixed *coords, Fixed *displacements, short *lenDisplayed);

/*
 * Given the family name (eg. "Minion") and axis ID, return information about the axis.
 * Axis numbers start with 1.
 * 		userMin, userMax -	Lowest and highest values allowed on this axis.
 *		axisType -			Type of axis. Examples are "Weight" for the weight axis and
 *							"Width" for the axis modifying width.
 *		axisLabel -			What to label the axis (eg. "Weight").
 *		numSubranges -		How many labels for subranges there are (ie. Light, Medium, Bold).
 * NOTE: passing NULL for any of the parameters ignores that parameter. Thus you don't always
 * have to get ALL the information if you only want a little.
 */
ATMErr getAxisBlendInfoATM(short axis, short *userMin, short *userMax, StringPtr axisType,
					StringPtr axisLabel, StringPtr axisShortLabel);

/*
 * Get the specifications about the current font; this information can be used with
 * fontFitATM(), below.
 */
ATMErr getFontSpecsATM(FontSpecs *specs);

/*
 * This routine does some amazing calculations, and is the center of the code for dealing
 * with copyfitting and other of the fancier multiple master operations. The current font
 * and the "origCoords" is given to, for example, adjust a line typed in a multiple master
 * typeface so that it exactly matches a given set of dimensions; here we will use string length
 * and stem width.
 *
 * The numTargets is 2 (one for string length and one for stem width). The first entry in
 * "targetMetrics" is the length you want the text to be (ie. the width of a column), and the
 * second what you want the stem width to be (get this information from getFontSpecsATM(), above).
 * The first entry in masterMetrics is a pointer to the widths of the strings at each master
 * design (loop through the masters using getMasterFONDATM()). The second entry in masterMetrics
 * is the stemwidths for each master design. fontFitATM tries to find the closest match
 * possible by adjusting the axes given in varyAxes, and returns the results in retCoords
 * and retWeightVector--if either argument is NULL, it is ignored. To get the axis id's, use
 * getAxisBlendInfoATM and look for the "axisType". A list of type strings will be given (soon?).
 */
ATMErr fontFitATM(Fixed *origCoords, short numTargets, short *varyAxes,
				Fixed *targetMetrics, Fixed **masterMetrics,
				Fixed *retCoords, Fixed *retWeightVector);

/*
 * There are a number of font instances for each multiple-master family that have been
 * "blessed" by our Type group to go well together. The font names for this group can be
 * gotten through the following calls: getNumBlessedFonts, getBlessedFontName, and
 * getRegularBlessedFont. Most applications will not need these calls....
 *
 * getNumBlessedFontsATM() finds how many "blessed" fonts exist.
 */
ATMErr getNumBlessedFontsATM(short *numBlessedFonts);

/*
 * Get the nth "blessed" font name (see above discussion). "i" is indexed from 1.
 */
ATMErr getBlessedFontNameATM(short i, StringPtr blessedFontName, Fixed *coords);

/*
 * When Adobe Illustrator and Adobe Type Reunion display a list of instances for a
 * multiple master family, a "reasonable" choice for the default is chosen. The choice
 * is given by this call. Essentially, this finds the equivalent of "Helvetica Regular"
 * for multiple master fonts.
 *
 * The returned value "regularID" is to be used as input to getBlessedFontName.
 */
ATMErr getRegularBlessedFontATM(short *regularID);

/*
 * This flushes ATM's font cache. This is basically the counterpart for System 7's
 * FlushFonts function.
 */
ATMErr flushCacheATM(void);

/*
 * When using many of the above calls for working with multiple master fonts, ATM
 * expects the current font to be set to a font in the font family. What happens if
 * you only have a font family name?  This routine will return a fondID that is usable
 * to handle information for that font family.
 */
ATMErr getFontFamilyFONDATM(StringPtr familyName, short *retFondID);

/*
 * Multiple Master Font Picker/Creator interface.
 *
 * This call displays a dialog allowing the user to navigate through the
 * design space of any installed Multiple Master font and to pick and/or
 * create an instance within that design space.  The caller receives
 * information about the user's selection, if any.
 *
 * See MMFP_Parms and MMFP_Reply definitions, above, for a full description.
 */
ATMErr MMFontPickerATM( MMFP_Parms *parms, MMFP_Reply *reply );

/* Used internally by ATM, don't use.  */
void disableATM(void);
void reenableATM(void);

/* ****************************** end of ATMProcs5Version routines ****************************** */

/* **************** The following routines are available under ATMProcs8Version: **************** */
/*
 * Returns TRUE if this is a substitute font.
 * If TRUE and fontSpecs != NULL, fontSpecs will return a handle to the
 * fontSpecs record in the file. It is up to the caller to dispose of this
 * handle when done.
 * if TRUE and chamName != NULL, chamName will return a handle containing the
 * PostScript name of the chameleon font used for substitution. It is up to
 * the caller to dispose of this handle when done.
 * If fontName is NULL, we use the "fondID" parameter; otherwise, the fondID
 * parameter is ignored.
 */
Boolean isSubstFontATM(StringPtr fontName, short fondID, short style, FontSpecs ***fontSpecs, Handle *chamName);

/* 
 * Get the run-encoded compressed form of a character bitmap.
 * The client should set devRun->run.data to point to storage for the runs, and should set 
 * devRun->dataSize to the size of this storage in bytes.
 * Returns ATM_NOERR if successful, otherwise one of the ATM error codes.
 * If ATM_NOERR is returned, devRun->run.datalen will be set to the number of 16-bit run values;
 * if ATM_RUN_MEMORY is returned, devRun->dataSize will be set to the required number of bytes.
 * Matrix maps one point character space to device space.
 */
short getRunsATM(short c, ATMFixedMatrix *matrix, Ptr clientHook, ATMPRunRec pRunRec);

/*
 * If the PostScript name is in our database then create its FOND if the parameter
 * "doCreate" is TRUE. It's expected that doCreate will generally be TRUE, but we've
 * added the flexibility just in case an application wants to ask permission
 * before creating the file.
 *
 * The application MUST check to see if a matching FOND already exists as this
 * function will create a FOND every time.
 *
 * Note that this function does NOT provide a mapping between PostScript names and
 * their associated FOND id and style--this must be implemented by the application.
 */
ATMErr getPSNumATM(StringPtr psName, short *retFondID, Boolean doCreate);

#ifdef __cplusplus
}
#endif

/* ****************************** end of ATMProcs8Version routines ****************************** */

/* **************** The following routines are available under ATMProcs11Version: **************** */

/* getATMInfoATM
 *
 * Purpose: Return info about ATM to a caller: is font substitution enabled, etc.
 * Caller fills in 'tag' field of 'request' argument. Function fills in
 * the byte length and value. If the byte length is more than 4 bytes, 
 * the value is a pointer to the data. If less than 5 bytes, the value is passed
 * the 'tagValue' field.
 * Note that pointers are to data areas inside ATM - do not change them!.
 * All strings are Pascal strings.
 */



#ifdef __cplusplus
extern "C" {
#endif

	ATMErr getATMInfoATM(ATMInfo* request);
	
#ifdef __cplusplus
}
#endif
	
/* ****************************** end of ATMProcs11Version routines ****************************** */


#if defined(powerc) && powerc
#pragma options align=reset
#endif



/***************************************************************************/
/*	Error codes												   			   */
/***************************************************************************/


/* These error codes are returned by the routines below: */
#define ATM_NOERR			(0)		/* Normal return */
#define ATM_NO_VALID_FONT	(-1)	/* can't find an outline font, or found a bad font - note that
									 * ATMGetOutline requires an outline font
									 * with exactly the current GrafPort's style(s) */
#define ATM_CANTHAPPEN		(-2)	/* Internal ATM error */
#define ATM_BAD_MATRIX		(-3)	/* Matrix inversion undefined or matrix too big */
#define ATM_MEMORY			(-4)	/* Ran out of memory */
#define ATM_WRONG_VERSION	(-5)	/* currently installed ATM driver doesn't support this interface */
#define ATM_NOT_ON			(-6)	/* the ATM driver is missing or has been turned off */
#define ATM_FILL_ORDER		(-7)	/* inconsistent fill calls, e.g. ATMFillMoveTo() without ATMStartFill() */
#define ATM_CANCELLED		(-8)	/* the client halted an operation, e.g. a callback from ATMGetOutline returned 0 */
#define ATM_NO_CHAR			(-9)	/* the font does not have an outline for this character code */
#define ATM_BAD_LENGTH		(-10)	/* ATMShowText() or ATMxyShowText() was called with length argument <= 0 or > 255 */
#define ATM_PIC_SAVE 		(-11)
#define	ATM_NOT_BLENDED_FONT (-12)	/* This font is not a blended font. */
#define	ATM_BASEDESIGN		(-13)	/* This operation is not allowed on a base design (eg. deleting FOND). */
#define	ATM_TEMPFONT_PROB	(-14)	/* We had a problem working with a temporary font. */
#define ATM_ILL_OPER		(-15)	/* Can't perform this operation on this font. */
#define ATM_FONTFIT_FAIL	(-16)	/* FontFit() failed (also from CopyFit()!). */
#define ATM_MISS_BASEDESIGN	(-17)	/* Missing base design FOND. */
#define ATM_NO_BLENDED_FONTS (-18)	/* no Multiple Master fonts installed */
#define ATM_PICKER_CANCELLED (-19)	/* user hit Picker/Creator "Cancel" button */
#define ATM_CREATE_FONT_FAIL (-20)	/* general font creation failure */
#define ATM_DISK_FULL		(-21)	/* out of disk space */
#define ATM_WRITE_PROTECTED	(-22)	/* volume or file is locked */
#define ATM_IO_ERROR		(-23)	/* I/O error */
#define ATM_COPY_PROT		(-24)	/* font is copy-protected */
#define ATM_PROT_OUTLINE	(-25)	/* this outline is copy-protected */
#define ATM_RUN_MEMORY		(-26)	/* ATMGetRuns needs a bigger client buffer */
#define ATM_BAD_MMFOND_STRUCT		(-27)	/* CopyOldToNewFond failed, with copy of data past end of new.*/
#define ATM_LOADABLE		(-28)	/* ATM is busy. */

/* new error code range so ATM error codes don't overrun any more OS error codes */
#define ATM_SUBST_PROT		(-2627)	/* font substitution is copy-protected */
#define ATM_SUBST_DISABLED	(-2628)	/* font substitution is turned off */
#define	ATM_BAD_FAUX_OUTLINE (-2629) /* Failed updating faux font file because
										it really was a file with the same name that wasn't
										a faux font file. */


/* new error codes for ATM's font substitution database. */
#ifndef DB_FILE_EXPIRED
#define DB_FILE_EXPIRED     -2500   /* Database has expired (beta only) */
#define DB_FILE_DAMAGED     -2501   /* Database has been damaged        */
#define DB_FILE_MISSING     -2502   /* Font database missing            */
#define DB_FILE_BUSY        -2503   /* Font database already in use     */
#define DB_OUT_OF_MEMORY    -2504   /* Not enough memory for task       */
#define DB_FONT_NOT_FOUND   -2505   /* Font not found in database       */
#define DB_BAD_REF_NUM      -2506   /* Illegal ref_num sent to database */
#define DB_BAD_VERSION      -2507   /* Requested version not available  */
#define DB_NOT_A_MM_FONT    -2508   /* Font is not Multiple Master Font */
#endif

/* new error codes returend by getATMInfo backdoor call */
#define ATM_INFO_UNKNOWN_TAG	-2400	/* did not recognize tag */
#define ATM_INFO_BAD_TAG_COUNT	-2401	/* tagCount is less than 1. */
#define ATM_INFO_BAD_VALUE		-2402	/* Requested value is bad/ given pointer is NULL. */


#endif	/* _H_ATMInterface */
