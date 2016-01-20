/*
$Header: /PM8/TT/FSCALER.H 1     3/03/99 6:35p Gbeddow $
$Log: /PM8/TT/FSCALER.H $
// 
// 1     3/03/99 6:35p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:14:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:45:16   Fred
// Initial revision.
// 
//    Rev 1.3   01 Jul 1997 09:08:24   Fred
// More changes for PMG 4.0
// 
//    Rev 1.2   02 May 1997 10:15:06   Fred
//  
// 
//    Rev 1.1   18 Apr 1997 12:49:58   Fred
// Changes for PMG 4.0
// 
//    Rev 1.0   14 Mar 1996 13:21:22   Jay
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:15:00   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:41:46   FRED
// Initial revision.
// 
//    Rev 1.1   04 Oct 1995 10:59:24   FRED
// 32 bit TrueType engine
// 
//    Rev 1.0   29 Sep 1995 13:37:06   FRED
// Initial revision.
 * 
 *    Rev 1.13   28 Apr 1995 13:38:56   FRED
 * Added coordinatePrecision field to output structure
 * in which the number of fraction bits is passed back
 * to the caller after a grid fit call.
 * 
 * 
 *    Rev 1.12   20 Oct 1992 12:04:40   MACDUFF
 * xStretch, yStretch defined iff TRUEFX.
 * 
 *    Rev 1.11   20 Oct 1992 11:50:26   MACDUFF
 * Added xStretch and yStretch to GlyphInputType for NewTrans.
 * 
 *    Rev 1.10   31 Aug 1992 23:11:40   FRED
 * Added "L" to constant to turn off some warnings.
 * 
 * 
 *    Rev 1.9   23 Aug 1992 12:50:38   ADAM
 * Support for the Breathe function.
 * 
 *    Rev 1.8   12 Aug 1992 18:09:46   FRED
 * Added effectsFlags field to input structure.
 * 
 * 
 *    Rev 1.7   10 Aug 1992 17:43:26   ADAM
 * Removed ReadTable. Now that the client has access to the offset & length
 * table in the key, it seems unecessary.
 * 
 *    Rev 1.6   06 Aug 1992 13:53:56   ADAM
 * Allows the client to access the key through the output structure.
 * 
 *    Rev 1.5   04 Aug 1992 16:01:48   ADAM
 * Made the fs_ReadTable input structure ID value a word from a long.
 * 
 *    Rev 1.4   04 Aug 1992 12:53:54   ADAM
 * New font scaler function: fs_ReadTable.
 * 
 *    Rev 1.3   31 Jul 1992 12:41:56   ADAM
 * Made bitmap far.
 * 
 *    Rev 1.2   29 Jul 1992 17:18:30   FRED
 * Added pattern, outline, shadow, and background
 * fields to the NewTrans input structure.
 * 
 * 
 *    Rev 1.1   07 Jul 1992 19:41:46   ADAM
 * 
 *    Rev 1.0   07 Jul 1992 12:19:38   ADAM
 * Initial revision.
*/

/*
    File:       fscaler.h

    Contains:   xxx put contents here (or delete the whole line) xxx

    Written by: xxx put name of writer here (or delete the whole line) xxx

    Copyright:  c 1988-1990 by Apple Computer, Inc., all rights reserved.

    Change History (most recent first):

        <9+>     7/17/90    MR      Conditionalize names in FSInput
         <9>     7/14/90    MR      rename SQRT2 to FIXEDSQRT2, removed specificID and lowestRecPPEM
                                    from FSInfo
         <8>     7/13/90    MR      FSInput now has a union to save space, points to matrix instead
                                    of storing it
         <6>     6/21/90    MR      Change fillFunc to ReleaseSfntFrag
         <5>      6/5/90    MR      remove readmvt and mapcharcodes
         <4>      5/3/90    RB      Added memory area for new scan converter. MIKE REED - Removed
                                    .error from fsinfo structure. Added MapCharCodes and ReadMVT
                                    calls.
         <3>     3/20/90    CL      New comment style for BBS. 
         <2>     2/27/90    CL      New CharToIndexMap Table format.
       <3.5>    11/15/89    CEL     Placed an ifdef around inline MPW calls to the trap. This makes
                                    it easier to compile for skia and the likes who do not use the
                                    MPW compiler.
       <3.4>    11/14/89    CEL     Left Side Bearing should work right for any transformation. The
                                    phantom points are in, even for components in a composite glyph.
                                    They should also work for transformations. Device metric are
                                    passed out in the output data structure. This should also work
                                    with transformations. Another leftsidebearing along the advance
                                    width vector is also passed out. whatever the metrics are for
                                    the component at it's level. Instructions are legal in
                                    components. Instructions are legal in components. Five
                                    unnecessary element in the output data structure have been
                                    deleted. (All the information is passed out in the bitmap data
                                    structure) fs_FindBMSize now also returns the bounding box.
       <3.3>     9/27/89    CEL     Took out devAdvanceWidth & devLeftSideBearing.
       <3.2>     9/25/89    CEL     Took out Mac specific functions.
       <3.1>     9/15/89    CEL     Re-working dispatcher.
       <3.0>     8/28/89    sjk     Cleanup and one transformation bugfix
       <2.2>     8/14/89    sjk     1 point contours now OK
       <2.1>      8/8/89    sjk     Improved encryption handling
       <2.0>      8/2/89    sjk     Just fixed EASE comment
       <1.5>      8/1/89    sjk     Added composites and encryption. Plus some enhancements.
       <1.4>     6/13/89    SJK     Comment
       <1.3>      6/2/89    CEL     16.16 scaling of metrics, minimum recommended ppem, point size 0
                                    bug, correct transformed integralized ppem behavior, pretty much
                                    so
       <1.2>     5/26/89    CEL     EASE messed up on "c" comments
      <,1.1>  5/26/89    CEL     Integrated the new Font Scaler 1.0 into Spline Fonts
       <1.0>     5/25/89    CEL     Integrated 1.0 Font scaler into Bass code for the first time.

    To Do:
*/

#ifndef __FSCALER_H__
#define __FSCALER_H__

/* QuickDraw Types */
#ifdef PC_OS
typedef int8* Ptr;

struct BitMap {
    uint32* baseAddr;
    int32 rowBytes;
    BBOX bounds;
};

#ifndef __cplusplus
typedef struct BitMap BitMap;
#endif

#else   /* not PC_OS */

#ifndef _MacTypes_
#ifndef __TYPES__
    typedef struct Rect {
        int16 top;
        int16 left;
        int16 bottom;
        int16 right;
    } Rect;
#endif
#endif

#ifndef _Quickdraw_
#ifndef __QUICKDRAW__   
    typedef struct BitMap {
        char* baseAddr;
        int16 rowBytes;
        Rect bounds;
    } BitMap;
#endif
#endif

#endif /* PC_OS */

#define MEMORYFRAGMENTS 8

#define NONVALID 0xffff

/* For the flags field in the flags field */

/* set on 68020, do not set on 68000 */
#define READ_NONALIGNED_SHORT_IS_OK 0x0001          /* set when calling fs_OpenFonts() */
/* set on 68020, do not set on 68000 */
#define READ_NONALIGNED_LONG_IS_OK  0x0002          /* set when calling fs_OpenFonts() */

typedef struct {
    vectorType			advanceWidth, leftSideBearing;
    vectorType			leftSideBearingLine, devLeftSideBearingLine;/* along AW line */
    vectorType			devAdvanceWidth, devLeftSideBearing;
} metricsType;

#define FS_MEMORY_SIZE int32

/*
 * Output data structure to the Font Scaler.
 */
typedef struct {
    FS_MEMORY_SIZE	memorySizes[MEMORYFRAGMENTS];

    uint16				glyphIndex;
#ifndef PC_OS
    uint16				numberOfBytesTaken; /* from the character code */
#endif  

    metricsType		metricInfo;
    BitMap				bitMapInfo;

    /* Spline Data */
    uint16				outlinesExist;
    uint16				numberOfContours;
    F26Dot6*			xPtr;
	 F26Dot6*			yPtr;
    int16*				startPtr;
    int16*				endPtr;
    uint8*				onCurve;
    /* End of spline data */

    /* Only of interest to editors */
    F26Dot6*			scaledCVT;


	 /* This is a far pointer to the internal Key. */

	 fsg_SplineKey*	Key;

} fs_GlyphInfoType;

// These MUST, MUST, MUST match the definitions in \PMW\TRUETYPE.H
// Note that the word "Component" is overloaded. In the TT engine
// it usually means "path component". Here it means a glyph component.
// This is a concept introduced for PMG where a character is composed
// several layers of "components" which are drawn separately -- possibly
// in different colors.
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

struct FillFormat
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

struct OutlineFormat
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

struct ShadowFormat
{
	enum
	{
		// Style
		Undefined	=		0x7fff,
		None			=		0,
		Drop			=		1,
		Extruded		=		2
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

/*
 * Input data structure to the Font Scaler.
 *
 * if styleFunc is set to non-zero it will be called just before the transformation
 * will be applied, but after the grid-fitting with a pointer to fs_GlyphInfoType.
 * so this is what styleFunc should be voidFunc StyleFunc (fs_GlyphInfoType *data);
 * For normal operation set this function pointer to zero.
 *
 */

typedef struct {
    Fixed					version;
    char*					memoryBases[MEMORYFRAGMENTS];
    GetSFNTFunc         GetSfntFragmentPtr; /* (clientID, offset, length) */
    ReleaseSFNTFunc		ReleaseSfntFrag;

    int32					clientID; /* client private id/stamp (eg. handle for the sfnt) */

    union {

        struct {
            uint16			platformID;
            uint16			specificID;
        } newsfnt;

        struct {
            Fixed				pointSize;
            int16				xResolution;
            int16				yResolution;
            Fixed				pixelDiameter;      /* compute engine char from this */
            transMatrix*	transformMatrix;
            voidFunc			traceFunc;
//				int16				pattern;
				FillFormat		Fill;
//				int16				outline;
				OutlineFormat	Outline;
//				int16				shadow;
				ShadowFormat	Shadow;
				Fixed				xStretch;
				Fixed				yStretch;
				Fixed				effects_rotation;
        } newtrans;

        struct {
            uint16			characterCode;
            uint16			glyphIndex;
        } newglyph;

        struct {
            void				(*styleFunc)(fs_GlyphInfoType*);
            voidFunc			traceFunc;
        } gridfit;

        struct {
            int32				bottomClip;
            int32				topClip;
				int32				Component;
				int32*			BitMapBuffer;
		  } scan;

        struct {
            F26Dot6*			x;
            F26Dot6*			y;
				uint8*			onCurve;
				int16*			sp;
				int16*			ep;
				int16				nc;
				int16				fx_use_new_metrics;
				uint32			fx_cell_height;
				uint32			fx_cell_width;
				int32				fx_cell_ascent;
				int32				fx_char_x0;
				int32				fx_char_x1;
				int32				fx_char_y0;
				int32				fx_char_y1;
        } newoutline;

	 } param;
} fs_GlyphInputType;

#ifndef FIXEDSQRT2
#define FIXEDSQRT2 0x00016A0AL
#endif

/* Font scaler trap selctors */
#define OUTLINEFONTTRAP     0xA854
#define FS_OPENFONTS        0x8000
#define FS_INITIALIZE       0x8001
#define FS_NEWSFNT          0x8002
#define FS_NEWTRANS         0x8003
#define FS_NEWGLYPH         0x8004
#define FS_GETAW            0x8005
#define FS_GRIDFITT         0x8006
#define FS_NOGRIDFITT       0x8007
#define FS_FINDBMSIZE       0x8008
#define FS_SIZEOFOUTLINES   0x8009
#define FS_SAVEOUTLINES     0x800a
#define FS_RESTOREOUTLINES  0x800b
#define FS_CONTOURSCAN      0x800c
#define FS_CLOSE            0x800d
#define FS_READMVT          0x800e
#define FS_MAPCHAR_CODES    0x800f

#ifndef FS_ENTRY
#ifdef PC_OS
#ifdef WIN32
#define FS_ENTRY extern "C" __declspec(dllexport) int
#else
#define FS_ENTRY extern "C" int _export
#endif
#else 
#define FS_ENTRY int32
#endif 
#endif 

#ifdef MACINIT
extern FS_ENTRY fs__OpenFonts(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr)
    = {0x303C,FS_OPENFONTS,0xA854};
extern FS_ENTRY fs__Initialize(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr)
    = {0x303C,FS_INITIALIZE,0xA854};
extern FS_ENTRY fs__NewSfnt(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr)
    = {0x303C,FS_NEWSFNT,0xA854};
extern FS_ENTRY fs__NewTransformation(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr)
    = {0x303C,FS_NEWTRANS,0xA854};
extern FS_ENTRY fs__NewGlyph(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr)
    = {0x303C,FS_NEWGLYPH,0xA854};
extern FS_ENTRY fs__GetAdvanceWidth(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr)
    = {0x303C,FS_GETAW,0xA854};
extern FS_ENTRY fs__ContourGridFit(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr)
    = {0x303C,FS_GRIDFITT,0xA854};
extern FS_ENTRY fs__ContourNoGridFit(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr)
    = {0x303C,FS_NOGRIDFITT,0xA854};
extern FS_ENTRY fs__FindBitMapSize(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr)
    = {0x303C,FS_FINDBMSIZE,0xA854};

/* these three optional calls are for caching the outlines */
extern FS_ENTRY fs__SizeOfOutlines(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr)
    = {0x303C,FS_SIZEOFOUTLINES,0xA854};
extern FS_ENTRY fs__SaveOutlines(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr)
    = {0x303C,FS_SAVEOUTLINES,0xA854};
extern FS_ENTRY fs__RestoreOutlines(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr)
    = {0x303C,FS_RESTOREOUTLINES,0xA854};

extern FS_ENTRY fs__ContourScan(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr)
    = {0x303C,FS_CONTOURSCAN,0xA854};
extern FS_ENTRY fs__CloseFonts(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr)
    = {0x303C,FS_CLOSE,0xA854};

#else

/*** Direct Calls to Font Scaler Client Interface, for Clients not using the trap mechanism ***/

FS_ENTRY fs_OpenFonts(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr);
FS_ENTRY fs_Initialize(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr);
FS_ENTRY fs_NewSfnt(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr);
FS_ENTRY fs_NewTransformation(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr);
FS_ENTRY fs_NewGlyph(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr);
FS_ENTRY fs_GetAdvanceWidth(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr);
FS_ENTRY fs_NewOutline(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr);
FS_ENTRY fs_ContourGridFit(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr);
FS_ENTRY fs_ContourNoGridFit(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr);
FS_ENTRY fs_FindBitMapSize(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr);
FS_ENTRY fs_ContourScan(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr);
FS_ENTRY fs_CloseFonts(fs_GlyphInputType* inputPtr, fs_GlyphInfoType* outputPtr);

extern int fs__Contour(fs_GlyphInputType*inputPtr, fs_GlyphInfoType*outputPtr, boolean useHints);

#endif

#ifdef FSCFG_NO_INITIALIZED_DATA
void fs_InitializeData(void);
#endif

#endif
