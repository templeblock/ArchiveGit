/*
// TRUETYPE.H
//
//		Copyright (C) 1993 MicroLogic Software, Inc.
//
//	Definitions for client to the TrueType font scaling engine.
//
// $Header: /PM8/App/TRUETYPE.H 1     3/03/99 6:12p Gbeddow $
//
// $Log: /PM8/App/TRUETYPE.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:20   Fred
// Initial revision.
// 
//    Rev 1.6   15 May 1997 16:55:18   Fred
// New text effects
// 
//    Rev 1.5   22 Apr 1997 13:18:16   Fred
// Start of project and art browsers
// 
//    Rev 1.4   17 Apr 1997 15:36:06   Fred
// Rotated fill patterns
// 
//    Rev 1.3   03 Apr 1997 09:32:42   Fred
// More text cleanup
// 
//    Rev 1.2   26 Mar 1997 15:55:12   Fred
// Removed old geronimo stuff
// 
//    Rev 1.1   06 Jan 1997 10:17:44   Fred
// Start of new text object
// 
//    Rev 1.0   14 Mar 1996 13:46:24   Jay
// Initial revision.
// 
//    Rev 1.1   08 Mar 1996 13:55:30   VSI
// fixed stuff for MAC build
// 
//    Rev 1.0   16 Feb 1996 12:08:32   FRED
// Initial revision.
// 
//    Rev 2.5   20 Nov 1995 17:49:52   FRED
// Fixed 16 bit bug: voidFunc is now uint16
// 
//    Rev 2.4   18 Oct 1995 15:58:10   FRED
// Fixes for 16 bit version after 32 bit changes
// 
//    Rev 2.3   04 Oct 1995 10:56:30   FRED
// 32 bit TrueType engine
// 
//    Rev 2.2   20 Sep 1995 10:01:14   FRED
// PmwThunk support for PMWTT.DLL
// 
//    Rev 2.1   28 Apr 1995 13:49:14   FRED
//  
// 
//    Rev 2.0   07 Feb 1995 15:49:04   JAY
// Initial revision.
// 
//    Rev 1.3   22 Jul 1994 16:28:44   JAY
// Changed TRUETYPE_TYPEFACE_MLSSYM and TRUETYPE_TYPEFACE_ZBATS because the
// former had a value of -1 which horribly confused the code.
// 
//    Rev 1.2   16 Mar 1994 15:56:42   MACDUFF
// Added "horizontal header" table.
// 
//    Rev 1.1   13 Jan 1994 17:01:24   JAY
//  
// 
//    Rev 1.0   16 Dec 1993 16:26:16   JAY
//  
// 
//    Rev 1.5   22 Feb 1993 11:01:54   MACDUFF
// "uint16" is unsigned, not unsigned short.
// 
//    Rev 1.4   19 Feb 1993 10:21:20   MACDUFF
// NewTrans.traceFunc is word, not long
// 
//    Rev 1.3   18 Feb 1993 12:17:20   MACDUFF
// Defined TRUETYPE_TYPEFACE_MLSSYM.
// All structures are now  byte-packed, because they must exactly match
// the structures in SMALLTT.
// 
//    Rev 1.2   17 Feb 1993 11:13:36   MACDUFF
// Assorted additions and corrections.
// 
//    Rev 1.1   11 Feb 1993 17:04:54   MACDUFF
// Defined some pointer types and the TTResult constants.
// 
//    Rev 1.0   11 Feb 1993 15:06:06   MACDUFF
// Initial revision.
//
*/

#ifndef __TRUETYPE_H__
#define __TRUETYPE_H__

// IMPORTANT: These definitions must match the definitions used by the TrueType engine.

//#ifndef WIN32
//#pragma pack(1)
//#endif

// This is the code for MLSSYM.TT
#define TRUETYPE_TYPEFACE_MLSSYM	0xFFFE
#define TRUETYPE_TYPEFACE_ZBATS	0xFFFD

typedef short FUnit;
typedef unsigned short uFUnit;

typedef long Fixed;
typedef long Fract;

typedef long F26Dot6;

#ifndef CONST
#define CONST const
#endif

typedef int32 FS_MEMORY_SIZE;

const int PlatformId_Macintosh = 1;
const int PlatformId_Microsoft = 3;
const int MAX_ELEMENTS = 2;

const uint16 FX_FLAG_no_pattern_alignment = 0x0001;
const uint32 Sqrt2 = 92682;

const int16 MEMORYFRAGMENTS = 8;


// Definitions for "Phantom Points" which are placed at the end of the curve data.
// ***** These must match the definitions in FSGLUE.H in the PMWTT project *****
const int LEFTSIDEBEARING = 0;
const int RIGHTSIDEBEARING = 1;
const int ORIGINPOINT = 2;
const int LEFTEDGEPOINT = 3;
const int PHANTOMCOUNT = 4;

typedef enum
{
	TTName_Copyright = 0,
	TTName_Family 		= 1,
	TTName_SubFamily	= 2,
	TTName_UniqueName = 3,
	TTName_FullName	= 4,
	TTName_Version		= 5,
	TTName_Postscript	= 6,
	TTName_Trademark	= 7
} tTTNameIndex;

inline uint16 SWAPW(uint16 w)
{
	return (w >> 8) | (w << 8);
}

inline uint32 SWAPL(uint32 l)
{
	return ((l & 0xff) << 24) |
			 ((l & 0xff00) << 8) |
			 ((l & 0xff0000) >> 8) |
			 (l >> 24);
}

/* Convert four characters to a long tag number */
#define NTAG(a, b, c, d) (((uint32)(d)<<24)+((uint32)(c)<<16)+((uint32)(b)<<8)+a)

const uint32 TAG_glyf	=	NTAG('g','l','y','f');
const uint32 TAG_OS_2	=	NTAG('O','S','/','2');
const uint32 TAG_cmap	=	NTAG('c','m','a','p');
const uint32 TAG_head	=	NTAG('h','e','a','d');
const uint32 TAG_hhea	=	NTAG('h','h','e','a');
const uint32 TAG_hmtx	=	NTAG('h','m','t','x');
const uint32 TAG_loca	=	NTAG('l','o','c','a');
const uint32 TAG_maxp	=	NTAG('m','a','x','p');
const uint32 TAG_name	=	NTAG('n','a','m','e');
const uint32 TAG_post	=	NTAG('p','o','s','t');
const uint32 TAG_cvt		=	NTAG('c','v','t',' ');
const uint32 TAG_prep	=	NTAG('p','r','e','p');
const uint32 TAG_fpgm	=	NTAG('f','p','g','m');

typedef struct {
	int32 xMin;
	int32 yMin;
	int32 xMax;
	int32 yMax;
} BBOX;

typedef struct Rect {
	int32 top;
	int32 left;
	int32 bottom;
	int32 right;
} Rect;

typedef struct BitMap {
	uint32* baseAddr;
   int32 rowBytes;
   Rect bounds;
} BitMap;

typedef struct {
    Fixed x, y;
} vectorType;

typedef struct {
    Fixed transform[3][3];
} transMatrix, *pTransMatrix;

typedef struct {
    F26Dot6* x; /* The Points the Interpreter modifies */
    F26Dot6* y; /* The Points the Interpreter modifies */
    F26Dot6* ox; /* Old Points */
    F26Dot6* oy; /* Old Points */
    F26Dot6* oox; /* Old Unscaled Points, really ints */
    F26Dot6* ooy; /* Old Unscaled Points, really ints */
    uint8* onCurve; /* indicates if a point is on or off the curve */
    int16* sp;  /* Start points */
    int16* ep;  /* End points */
    uint8* f;  /* Internal flags, one byte for every point */
    int16 nc;  /* Number of contours */
} fnt_ElementType;

typedef int8* OFFSET_INFO_TYPE;

typedef struct {
    OFFSET_INFO_TYPE x;
    OFFSET_INFO_TYPE y;
    OFFSET_INFO_TYPE ox;
    OFFSET_INFO_TYPE oy;
    OFFSET_INFO_TYPE oox;
    OFFSET_INFO_TYPE ooy;
    OFFSET_INFO_TYPE onCurve;
    OFFSET_INFO_TYPE sp;
    OFFSET_INFO_TYPE ep;
    OFFSET_INFO_TYPE f;
} fsg_OffsetInfo;

/*** Element Information ***/
typedef struct {
    int32               missingCharInstructionOffset;
    int32               stackBaseOffset;
    fsg_OffsetInfo      offsets[MAX_ELEMENTS];
    fnt_ElementType     interpreterElements[MAX_ELEMENTS];
} fsg_ElementInfo;

typedef struct {
	uint32 *bitMap;
	uint32 *xLines, *yLines, **xBase, **yBase;
	BBOX bounds;
	uint32 nXchanges, nYchanges;
	uint32 high, wide;
} sc_BitMapData;

typedef struct {
    Fixed       version;                /* for this table, set to 1.0 */
    uint16      numGlyphs;
    uint16      maxPoints;              /* in an individual glyph */
    uint16      maxContours;            /* in an individual glyph */
    uint16      maxCompositePoints;     /* in an composite glyph */
    uint16      maxCompositeContours;   /* in an composite glyph */
    uint16      maxElements;            /* set to 2, or 1 if no twilightzone points */
    uint16      maxTwilightPoints;      /* max points in element zero */
    uint16      maxStorage;             /* max number of storage locations */
    uint16      maxFunctionDefs;        /* max number of FDEFs in any preprogram */
    uint16      maxInstructionDefs;     /* max number of IDEFs in any preprogram */
    uint16      maxStackElements;       /* max number of stack elements for any individual glyph */
    uint16      maxSizeOfInstructions;  /* max size in bytes for any individual glyph */
    uint16      maxComponentElements;   /* number of glyphs referenced at top level */
    uint16      maxComponentDepth;      /* levels of recursion, 1 for simple components */
} sfnt_maxProfileTable;

typedef struct {
    F26Dot6 x;
    F26Dot6 y;
} point;

#ifdef WIN32
#include <setjmp.h>
#else
#define _JBLEN  9  /* bp, di, si, sp, ret addr, ds */
typedef  int  jmp_buf[_JBLEN];
#endif

typedef struct {
    uint32 bc;
    uint32 ad;
} BigDate;

typedef enum {
    sfnt_fontHeader,
    sfnt_horiHeader,
    sfnt_indexToLoc,
    sfnt_maxProfile,
    sfnt_controlValue,
    sfnt_preProgram,
    sfnt_glyphData,
    sfnt_horizontalMetrics,
    sfnt_charToIndexMap,
    sfnt_fontProgram,
    sfnt_Postscript,
    sfnt_HoriDeviceMetrics,
    sfnt_LinearThreeShold,
    sfnt_Names,
    sfnt_OS_2,
	 sfnt_Kerning,		/* 12/17/92 MH */
    sfnt_NUMTABLEINDEX
} sfnt_tableIndex;

typedef struct {
  uint32    Offset;
  uint32  Length;
} fsg_OffsetLength;

typedef fsg_OffsetLength *pOffsetTableMap;

#ifdef WIN32
typedef void* (__stdcall *GetSFNTFunc)(long, long, long);
typedef void (__stdcall *ReleaseSFNTFunc)(void*);
#else
typedef void* (pascal *GetSFNTFunc)(long, long, long);
typedef void (pascal *ReleaseSFNTFunc)(void*);
#endif


/*** The Internal Key ***/
typedef struct fsg_SplineKey {
	int32						clientID;
	GetSFNTFunc				GetSfntFragmentPtr;	/* User function to eat sfnt */
	ReleaseSFNTFunc		ReleaseSfntFrag;		/* User function to relase sfnt */
	uint16 (*mappingF)(uint8*, uint16);			/* mapping function */
	unsigned					mappOffset;				/* Offset to platform mapping data */
	int16						glyphIndex;
	uint16					elementNumber;			/* Character Element */
    
	char**					memoryBases;			/* array of memory Areas */

	fsg_ElementInfo		elementInfoRec;		/* element info structure */
	sc_BitMapData			bitMapInfo;				/* bitmap info structure */

	uint16					emResolution;			/* used to be int32 <4> */

	Fixed						fixedPointSize;		/* user point size */
	Fixed						interpScalarX;			/* scalar for instructable things */
	Fixed						interpScalarY;			/* scalar for instructable things */
	Fixed						interpLocalScalarX;	/* Local scalar for instructable things */
	Fixed						interpLocalScalarY;	/* Local scalar for instructable things */
	Fixed						metricScalarX;			/* scalar for metric things */
	Fixed						metricScalarY;			/* scalar for metric things */

	transMatrix				currentTMatrix;		/* Current Transform Matrix */
	transMatrix				localTMatrix;			/* Local Transform Matrix */
	int8						localTIsIdentity;
	int8						phaseShift;				/* 45 degrees flag <4> */
	int16						identityTransformation;
	int16						indexToLocFormat;
    
	uint16					fontFlags;				/* copy of header.flags */

	Fixed						pixelDiameter;
	uint16					nonScaledAW;
	int16						nonScaledLSB;

	unsigned					state;					/* for error checking purposes */
	int32						scanControl;			/* flags for dropout control etc.  */
    
	/* for key->memoryBases[PRIVATE_FONT_SPACE_BASE] */
	OFFSET_INFO_TYPE		offset_storage;
	OFFSET_INFO_TYPE		offset_functions;
	OFFSET_INFO_TYPE		offset_instrDefs;		/* <4> */
	OFFSET_INFO_TYPE		offset_controlValues;
	OFFSET_INFO_TYPE		offset_globalGS;
	OFFSET_INFO_TYPE		offset_FontProgram;
	OFFSET_INFO_TYPE		offset_PreProgram;

	/* for outline caching */
	unsigned					glyphLength;
    
    
	/* copy of profile */
	sfnt_maxProfileTable	maxProfile;

	fsg_OffsetLength		offsetTableMap[sfnt_NUMTABLEINDEX];
	uint16					numberOf_LongHorMetrics;
    
	uint16					totalContours;			/* for components */
	uint16					totalComponents;		/* for components */
	uint16					weGotComponents;		/* for components */
	uint16					compFlags;
	int16						arg1, arg2;
    
	int32						instructControl;		/* set to inhibit execution of instructions */  
	int32						imageState;				/* is glyph rotated, stretched, etc. */
    
	int						numberOfRealPointsInComponent;
	uint16					lastGlyph;
	uint8						executePrePgm;
	uint8						executeFontPgm;		/* <4> */

	point						devLSB, devRSB;		/* 3 jan 92 DNW added for useMyMetric */
	int16						useMyMetrics;			/* support taken from the apple code  */

	jmp_buf					env;

	/* global bounding box information in emResolution. These are read from the 'head' table. */

	BBOX						GlobalBBox;

	/*
	// These are metric values used by the scaling shell.
	*/

	uint32					SpaceWidth_EAE;
	int32						UnderlineDistance;
	BBOX						GlobalBBox_EAE;
	BBOX						CharacterBBox_EAE;
	uint32					DeltaX_EAE;

	transMatrix				RotationMatrix;
	transMatrix				ScalingMatrix;

	/* Save glyph location here */
	uint32					GlyphOffset;
} fsg_SplineKey;

typedef struct {
	vectorType				advanceWidth, leftSideBearing;
	vectorType				leftSideBearingLine, devLeftSideBearingLine;	/* along AW line */
	vectorType				devAdvanceWidth, devLeftSideBearing;
} metricsType;

typedef struct {
	FS_MEMORY_SIZE			memorySizes[MEMORYFRAGMENTS];

	uint16					glyphIndex;

	metricsType				metricInfo;
	BitMap					bitMapInfo;

	/* Spline Data */
	uint16					outlinesExist;
	uint16					numberOfContours;
	F26Dot6*					xPtr;
	F26Dot6*					yPtr;
	int16*					startPtr;
	int16*					endPtr;
	uint8*					onCurve;
	/* End of spline data */

	/* Only of interest to editors */
	F26Dot6*					scaledCVT;

	/* This is a far pointer to the internal Key. */
	fsg_SplineKey*			Key;

} fs_GlyphInfoType, *pfs_GlyphInfoType;

typedef void (* voidFunc)();

// These MUST, MUST, MUST match the definitions in \TT\FSCALER.H
// Note that the word "Component" is overloaded. In the TT engine
// it usually means "path component". Here it means a glyph component.
enum
{
	COMPONENT_DontCare		=		0,		// used when sizing the character.
	COMPONENT_WhiteOutMask	=		1,
	COMPONENT_ShadowMask		=		2,
	COMPONENT_Shadow			=		3,
	COMPONENT_FillMask		=		4,
	COMPONENT_Fill				=		5,
	COMPONENT_OutlineMask	=		6,
	COMPONENT_Outline			=		7
};

struct TrueTypeFillFormat
{
	enum
	{
		// Style
		Undefined		=	0x7fff,
		None				=	-1,
		Black				=	0,
		White				=	1,
		Gray2Percent	=	2,
		Gray5Percent	=	3,
		Gray15Percent	=	4,
		Gray30Percent	=	5,
		Gray50Percent	=	6,
		Gray70Percent	=	7,
		Gray90Percent	=	8,
		Horizontal		=	9,
		Vertical			=	10,
		GraphPaper		=	11,
		WoodGrain		=	12,
		StarryNight		=	13,
		Sunset			=	14,
		Fountain			=	15,
		Rattan			=	16,
		Sunrise			=	17,
		Starburst		=	18,
		Radial			=	19,
		Swirl				=	20,
		Stripes			=	21,
		Zigzag			=	22,
		Plaid				=	23,
		Brick				=	24,
		LondonFog		=	25,
		Cloud				=	26,
		Tubular			=	27
	};

	int16 m_nPattern;
};

struct TrueTypeOutlineFormat
{
	enum
	{
		// Style
		Undefined	=		0x7fff,
		Solid			=		-1,
		None			=		0,
		Light			=		1,
		Medium		=		2,
		Heavy			=		3,
		Calligraphy	=		4,
		Contour1		=		5,
		Contour2		=		6
	};
	enum
	{
		// Width Type
		Absolute		=		0,
		Relative		=		1
	};

	int16 m_nStyle;
	int16 m_nWidthType;
	int32 m_lWidth;			// 16.16 percent or points
	int16 m_nPattern;
};

struct TrueTypeShadowFormat
{
	enum
	{
		// Style
		Undefined	=		0x7fff,
		None			=		0,
		Hairline		=		1,
		Drop			=		2,
		Extruded		=		3
	};
	enum
	{
		// Offset Type
		Absolute		=		0,
		Relative		=		1
	};

	int16 m_nStyle;
	int16 m_nOffsetType;
	int32 m_lXOffset;		// 16.16 percent or points
	int32 m_lYOffset;		// 16.16 percent or points
	int16 m_nPattern;
};

typedef struct _tNewTrans {
	Fixed							pointSize;
   int16							xResolution;
   int16							yResolution;
   Fixed							pixelDiameter;			/* compute engine char from this */
	transMatrix*				transformMatrix;
   voidFunc						traceFunc;
//	int16							pattern;
	TrueTypeFillFormat		Fill;
//	int16							outline;
	TrueTypeOutlineFormat	Outline;
//	int16							shadow;
	TrueTypeShadowFormat		Shadow;
	Fixed							xStretch;
	Fixed							yStretch;
	Fixed							effects_rotation;
} newtrans;

typedef struct {
	Fixed               version;
	char*               memoryBases[MEMORYFRAGMENTS];

	GetSFNTFunc         GetSfntFragmentPtr; /* (clientID, offset, length) */
	ReleaseSFNTFunc     ReleaseSfntFrag;

	int32               clientID; /* client private id/stamp (eg. handle for the sfnt) */

	union {

		struct {
			uint16	platformID;
			uint16	specificID;
		} newsfnt;

		struct _tNewTrans newtrans;

		struct {
			uint16	characterCode;
			uint16	glyphIndex;
		} newglyph;

		struct {
			void		(*styleFunc) (fs_GlyphInfoType*);
			voidFunc	traceFunc;
		} gridfit;

		struct {
			int32		bottomClip;
			int32		topClip;
			int32		Component;
			int32*	BitMapBuffer;
		} scan;

		struct {
			F26Dot6*	x;
			F26Dot6*	y;
			uint8*	onCurve;
			int16*	sp;
			int16*	ep;
			int16		nc;
			int16    use_new_metrics;
			uint32	fx_cell_height;
			uint32	fx_cell_width;
			int32		fx_cell_ascent;
			int32		fx_char_x0;
			int32		fx_char_x1;
			int32		fx_char_y0;
			int32		fx_char_y1;
		} newoutline;

	} param;

} fs_GlyphInputType, *pfs_GlyphInputType;

typedef struct {
	Fixed		version;					/* for this table, set to 1.0 */
	Fixed		fontRevision;			/* For Font Manufacturer */
	uint32	checkSumAdjustment;
	uint32	magicNumber;			/* signature, should always be 0x5F0F3CF5  == MAGIC */
	uint16	flags;
	uint16	unitsPerEm;				/* Specifies how many in Font Units we have per EM */

	BigDate	created;
	BigDate	modified;

	/** This is the font wide bounding box in ideal space
	(baselines and metrics are NOT worked into these numbers) **/
	FUnit		xMin;
	FUnit		yMin;
	FUnit		xMax;
	FUnit		yMax;

	uint16	macStyle;				/* macintosh style word */
	uint16	lowestRecPPEM;			/* lowest recommended pixels per Em */

	/* 0: fully mixed directional glyphs,
	   1: only strongly L->R or T->B glyphs, 
	  -1: only strongly R->L or B->T glyphs,
	   2: like 1 but also contains neutrals,
	  -2: like -1 but also contains neutrals */
	int16		fontDirectionHint;

	int16		indexToLocFormat;
	int16		glyphDataFormat;
} sfnt_FontHeader;

typedef struct {
	Fixed		version;							/* for this table, set to 1.0 */

	FUnit		yAscender;
	FUnit		yDescender;
	FUnit		yLineGap;						/* Recommended linespacing = ascender - descender + linegap */
	uFUnit	advanceWidthMax;
	FUnit		minLeftSideBearing;
	FUnit		minRightSideBearing;
	FUnit		xMaxExtent;						/* Max of (LSBi + (XMAXi - XMINi)), i loops through all glyphs */

	int16		horizontalCaretSlopeNumerator;
	int16		horizontalCaretSlopeDenominator;

	uint16	reserved0;
	uint16	reserved1;
	uint16	reserved2;
	uint16	reserved3;
	uint16	reserved4;

	int16		metricDataFormat;				/* set to 0 for current format */
	uint16	numberOf_LongHorMetrics;	/* if format == 0 */
} sfnt_HorizontalHeader;

typedef struct {
	uint16	format;
	uint16	length;
	uint16	version;
} sfnt_mappingTable;

typedef struct {
	uint16	platformID;
	uint16	specificID;
	uint32	offset;
} sfnt_platformEntry;

typedef struct {
	uint16	version;
	uint16	numTables;
	sfnt_platformEntry platform[1];		/* platform[numTables] */
} sfnt_char2IndexDirectory;

typedef struct {
	uint16	platformID;
	uint16	specificID;
	uint16	languageID;
	uint16	nameID;
	uint16	length;
	uint16	offset;
} sfnt_NameRecord;

typedef struct {
	uint16	format;
	uint16	count;
	uint16	stringOffset;
	sfnt_NameRecord NameRecords[1];		// VARIABLE LENGTH ARRAY
} sfnt_NamingTable;

typedef struct {
	Fixed		version;							/* 1.0 */
	Fixed		italicAngle;
	FUnit		underlinePosition;
	FUnit		underlineThickness;
	int16		isFixedPitch;
	int16		pad;
	uint32	minMemType42;
	uint32	maxMemType42;
	uint32	minMemType1;
	uint32	maxMemType1;

	uint16	numberGlyphs;
	union
	{
		uint16	glyphNameIndex[1];		/* version == 2.0 */
		int8		glyphNameIndex25[1];		/* version == 2.5 */
	} postScriptNameIndices;

} sfnt_PostScriptInfo;

typedef struct {
	uint16	Version;
	int16		xAvgCharWidth;
	uint16	usWeightClass;
	uint16	usWidthClass;
	int16		fsType;
	int16		ySubscriptXSize;
	int16		ySubscriptYSize;
	int16		ySubscriptXOffset;
	int16		ySubscriptYOffset;
	int16		ySuperScriptXSize;
	int16		ySuperScriptYSize;
	int16		ySuperScriptXOffset;
	int16		ySuperScriptYOffset;
	int16		yStrikeOutSize;
	int16		yStrikeOutPosition;
	int16		sFamilyClass;
	uint8		Panose[10];
	uint32	ulCharRange[4];
	char		achVendID[4];
	uint16	usSelection;
	uint16	usFirstChar;
	uint16	usLastChar;
	int16		sTypoAscender;
	int16		sTypoDescender;
	int16		sTypoLineGap;
	uint16	usWinAscent;
	uint16	usWinDescent;
} sfnt_OS2;

typedef int16 tTTResult;

const tTTResult	TTResult_DLLNotInitialized				= -1;
const tTTResult	TTResult_FontFileNotFound				= -2;

const tTTResult	TTResult_NULL_KEY_ERR					= 0x1001;
const tTTResult	TTResult_NULL_INPUT_PTR_ERR			= 0x1002;
const tTTResult	TTResult_NULL_MEMORY_BASES_ERR		= 0x1003;
const tTTResult	TTResult_VOID_FUNC_PTR_BASE_ERR		= 0x1004;
const tTTResult	TTResult_OUT_OFF_SEQUENCE_CALL_ERR	= 0x1005;
const tTTResult	TTResult_BAD_CLIENT_ID_ERR				= 0x1006;
const tTTResult	TTResult_NULL_SFNT_DIR_ERR				= 0x1007;
const tTTResult	TTResult_NULL_SFNT_FRAG_PTR_ERR		= 0x1008;
const tTTResult	TTResult_NULL_OUTPUT_PTR_ERR			= 0x1009;
const tTTResult	TTResult_INVALID_GLYPH_INDEX			= 0x100A;

const tTTResult	TTResult_UNDEFINED_INSTRUCTION_ERR	= 0x1101;
const tTTResult	TTResult_TRASHED_MEM_ERR				= 0x1102;

const tTTResult	TTResult_POINT_MIGRATION_ERR			= 0x1201;

const tTTResult	TTResult_BAD_START_POINT_ERR			= 0x1301;
const tTTResult	TTResult_SCAN_ERR							= 0x1302;

const tTTResult	TTResult_SFNT_DATA_ERR					= 0x1400;
const tTTResult	TTResult_POINTS_DATA_ERR				= 0x1401;
const tTTResult	TTResult_INSTRUCTION_SIZE_ERR			= 0x1402;
const tTTResult	TTResult_CONTOUR_DATA_ERR				= 0x1403;
const tTTResult	TTResult_GLYPH_INDEX_ERR				= 0x1404;
const tTTResult	TTResult_BAD_MAGIC_ERR					= 0x1405;
const tTTResult	TTResult_OUT_OF_RANGE_SUBTABLE		= 0x1406;
const tTTResult	TTResult_UNKNOWN_COMPOSITE_VERSION	= 0x1407;
const tTTResult	TTResult_CLIENT_RETURNED_NULL			= 0x1408;
const tTTResult	TTResult_MISSING_SFNT_TABLE			= 0x1409;
const tTTResult	TTResult_UNKNOWN_CMAP_FORMAT			= 0x140A;

const tTTResult	TTResult_BAD_CALL_ERR					= 0x1500;

const tTTResult	TTResult_TRASHED_OUTLINE_CACHE		= 0x1600;



typedef pfs_GlyphInputType tinp;
typedef pfs_GlyphInfoType tout;

//#ifndef WIN32
//#pragma pack()
//#endif

#endif
