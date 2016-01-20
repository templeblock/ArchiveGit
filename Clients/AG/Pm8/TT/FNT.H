/*
$Header: /PM8/TT/FNT.H 1     3/03/99 6:35p Gbeddow $
$Log: /PM8/TT/FNT.H $
// 
// 1     3/03/99 6:35p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:14:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:45:14   Fred
// Initial revision.
// 
//    Rev 1.1   01 Jul 1997 09:08:22   Fred
// More changes for PMG 4.0
// 
//    Rev 1.0   14 Mar 1996 13:21:14   Jay
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:14:52   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:41:38   FRED
// Initial revision.
// 
//    Rev 1.0   29 Sep 1995 13:37:04   FRED
// Initial revision.
 * 
 *    Rev 1.4   15 Jan 1993 09:49:26   MACDUFF
 * Changed MAXBYTE_INSTRUCTIONS when MLS_ASM_FNT_InnerExecute is on,
 * for a smaller jump table.
 * 
 *    Rev 1.3   28 Aug 1992 13:01:48   FRED
 * Defines fnt_PixelSize as _ONE.
 * 
 * 
 *    Rev 1.2   18 Aug 1992 09:57:32   MACDUFF
 * Uses _PRECISION
 * 
 *    Rev 1.1   13 Jul 1992 18:36:12   ADAM
 * PVCS keywords
*/
/*
    File:       fnt.h

    Contains:   xxx put contents here (or delete the whole line) xxx

    Written by: xxx put name of writer here (or delete the whole line) xxx

    Copyright:  c 1987-1990 by Apple Computer, Inc., all rights reserved.

    Change History (most recent first):

       <11+>     9/15/90    MR,rb   Change pvx and pvy to proj.[xy].  Same for freedom vector.
                                    Conditionalize vectors for Fracts or ShortFracts.
        <10>     7/26/90    MR      rearrange local graphic state, remove unused parBlockPtr
         <9>     7/18/90    MR      change loop variable from long to short, and other Ansi-changes
         <8>     7/13/90    MR      Prototypes for function pointers
         <5>      6/4/90    MR      Remove MVT
         <4>      5/3/90    RB      replaced dropoutcontrol with scancontrolin and scancontrol out
                                    in global graphics state
         <3>     3/20/90    CL      fields for multiple preprograms fields for ppemDot6 and
                                    pointSizeDot6 changed SROUND to take D/2 as argument
         <2>     2/27/90    CL      Added DSPVTL[] instruction.  Dropoutcontrol scanconverter and
                                    SCANCTRL[] instruction
       <3.1>    11/14/89    CEL     Fixed two small bugs/feature in RTHG, and RUTG. Added SROUND &
                                    S45ROUND.
       <3.0>     8/28/89    sjk     Cleanup and one transformation bugfix
       <2.2>     8/14/89    sjk     1 point contours now OK
       <2.1>      8/8/89    sjk     Improved encryption handling
       <2.0>      8/2/89    sjk     Just fixed EASE comment
       <1.7>      8/1/89    sjk     Added composites and encryption. Plus some enhancements.
       <1.6>     6/13/89    SJK     Comment
       <1.5>      6/2/89    CEL     16.16 scaling of metrics, minimum recommended ppem, point size 0
                                    bug, correct transformed integralized ppem behavior, pretty much
                                    so
       <1.4>     5/26/89    CEL     EASE messed up on "c" comments
      <,1.3>     5/26/89    CEL     Integrated the new Font Scaler 1.0 into Spline Fonts

    To Do:
*/
/*  rwb 4/24/90 Replaced dropoutControl with scanControlIn and scanControlOut in
        global graphics state. 
        <3+>     3/20/90    mrr     Added support for IDEFs.  Made funcDefs long aligned
                                    by storing int16 length instead of int32 end.
*/

#ifndef __FNT_H__
#define __FNT_H__

#include "fntjmp.h"

#define fnt_pixelSize	_ONE		// ((F26Dot6)(1 << _PRECISION))
#define fnt_pixelShift	_PRECISION

#ifndef MLS_ASM_FNT_InnerExecute
#define MAXBYTE_INSTRUCTIONS 256
#else
#define MAXBYTE_INSTRUCTIONS 143
#endif

#define VECTORTYPE                      ShortFract
#define ONEVECTOR                       ONESHORTFRAC
#define VECTORMUL(value, component)     ShortFracMul((F26Dot6)(value), (ShortFract)(component))
#define VECTORDOT(a,b)                  ShortFracDot((ShortFract)(a),(ShortFract)(b))
#define VECTORDIV(num,denum)            ShortFracDiv((ShortFract)(num),(ShortFract)(denum))
#define VECTORMULDIV(a,b,c)             ShortFracMulDiv((ShortFract)(a),(ShortFract)(b),(ShortFract)(c))
#define VECTOR2FIX(a)                   ((Fixed) (a) << 2)
#define ONESIXTEENTHVECTOR              ((ONEVECTOR) >> 4)

typedef struct VECTOR {
    VECTORTYPE x;
    VECTORTYPE y;
} VECTOR;

typedef struct {
    F26Dot6  *x; /* The Points the Interpreter modifies */
    F26Dot6  *y; /* The Points the Interpreter modifies */
    F26Dot6  *ox; /* Old Points */
    F26Dot6  *oy; /* Old Points */
    F26Dot6  *oox; /* Old Unscaled Points, really ints */
    F26Dot6  *ooy; /* Old Unscaled Points, really ints */
    uint8 *onCurve; /* indicates if a point is on or off the curve */
    int16 *sp;  /* Start points */
    int16 *ep;  /* End points */
    uint8  *f;  /* Internal flags, one byte for every point */
    int16 nc;  /* Number of contours */
} fnt_ElementType;

typedef struct {
    int32 start;        /* offset to first instruction */
    uint16 length;      /* number of bytes to execute <4> */
    uint16 pgmIndex;    /* index to appropriate preprogram for this func (0..1) */
} fnt_funcDef;

/* <4> pretty much the same as fnt_funcDef, with the addition of opCode */
typedef struct {
    int32 start;
    uint16 length;
    uint8  pgmIndex;
    uint8  opCode;
} fnt_instrDef;

typedef struct {
    Fract x;
    Fract y;
} fnt_FractPoint;

/***************** This is stored as FractPoint[] and distance[]
typedef struct {
    Fract x, y;
    int16 distance;
} fnt_AngleInfo;
*******************/

typedef struct {
    uint8 *    Instruction;
    unsigned  Length;
} fnt_pgmList;

struct fnt_LocalGraphicStateType;
typedef void (*FntTraceFunc)(struct fnt_LocalGraphicStateType*, uint8*);

#ifdef FSCFG_REENTRANT
typedef void (*FntFunc)(struct fnt_LocalGraphicStateType*);
typedef void (*FntMoveFunc)(struct fnt_LocalGraphicStateType*, fnt_ElementType*, ArrayIndex, F26Dot6);
typedef F26Dot6 (*FntProject)(struct fnt_LocalGraphicStateType*, F26Dot6, F26Dot6);
typedef void (*InterpreterFunc)(struct fnt_LocalGraphicStateType*, uint8*, uint8*);
typedef F26Dot6 (*FntRoundFunc)(struct fnt_LocalGraphicStateType*, F26Dot6, F26Dot6);
#else 
typedef void (*FntMoveFunc)(fnt_ElementType*, ArrayIndex, F26Dot6);
typedef void (*FntFunc)(void);
typedef F26Dot6 (*FntProject)(F26Dot6 x, F26Dot6 y);
typedef void (*InterpreterFunc)(uint8*, uint8*);
typedef F26Dot6 (*FntRoundFunc)(F26Dot6 xin, F26Dot6 engine);
#endif 



typedef struct {

/* PARAMETERS CHANGEABLE BY TT INSTRUCTIONS */
    F26Dot6 wTCI;                   /* width table cut in */
    F26Dot6 sWCI;                   /* single width cut in */
    F26Dot6 scaledSW;               /* scaled single width */
    int32 scanControl;              /* controls kind and when of dropout control */
    int32 instructControl;          /* controls gridfitting and default setting */
    
    F26Dot6 minimumDistance;        /* moved from local gs  7/1/90  */
    FntRoundFunc RoundValue;        /*                              */
        F26Dot6 periodMask;                     /* ~(gs->period-1)                              */
        VECTORTYPE period45;                       /*                                                              */
        int16   period;                         /* for power of 2 periods               */
        int16   phase;                          /*                                                              */
        int16   threshold;                      /* moved from local gs  7/1/90  */

        int16 deltaBase;
        int16 deltaShift;
        int16 angleWeight;
        int16 sW;                               /* single width, expressed in the same units as the character */
        int8 autoFlip;                          /* The auto flip Boolean */
    int8 pad;   
#ifndef FSCFG_NOPAD_PARAMETER_BLOCK_4
    int16 pad2;   
#endif 
} fnt_ParameterBlock;               /* this is exported to client */

#define MAXANGLES       20
#define ROTATEDGLYPH    0x100
#define STRETCHEDGLYPH  0x200
#define NOGRIDFITFLAG   1
#define DEFAULTFLAG     2

typedef enum {
    PREPROGRAM,
    FONTPROGRAM,
    MAXPREPROGRAMS
} fnt_ProgramIndex;

typedef struct fnt_ScaleRecord {
    Fixed fixedScale;       /* Slow Scale */
    int denom;            /* Fast and Medium Scale */
    int numer;              /* Fast and Medium Scale */
    int shift;              /* Fast Scale */
} fnt_ScaleRecord;

typedef F26Dot6 (*GlobalGSScaleFunc)(fnt_ScaleRecord*, F26Dot6);

typedef struct fnt_GlobalGraphicStateType {
    F26Dot6* stackBase;             /* the stack area */
    F26Dot6* store;                 /* the storage area */
    F26Dot6* controlValueTable;     /* the control value table */
    
    uint16  pixelsPerEm;            /* number of pixels per em as an integer */
    uint16  pointSize;              /* the requested point size as an integer */
    Fixed   fpem;                   /* fractional pixels per em    <3> */
    F26Dot6 engine[4];              /* Engine Characteristics */
    
    fnt_ParameterBlock defaultParBlock; /* variables settable by TT instructions */
    fnt_ParameterBlock localParBlock;

    /* Only the above is exported to Client throught FontScaler.h */

/* VARIABLES NOT DIRECTLY MANIPULABLE BY TT INSTRUCTIONS  */
    
    fnt_funcDef* funcDef;           /* function Definitions identifiers */
    fnt_instrDef* instrDef;         /* instruction Definitions identifiers */
        GlobalGSScaleFunc ScaleFuncXBase;
        GlobalGSScaleFunc ScaleFuncYBase;
        GlobalGSScaleFunc ScaleFuncX;
        GlobalGSScaleFunc ScaleFuncY;
        GlobalGSScaleFunc ScaleFuncCVT;
        fnt_pgmList pgmList[MAXPREPROGRAMS];  /* each program ptr is in here */
    
/* These are parameters used by the call back function */
        fnt_ScaleRecord   scaleXBase;
        fnt_ScaleRecord   scaleYBase;
        fnt_ScaleRecord   scaleX;
        fnt_ScaleRecord   scaleY;
        fnt_ScaleRecord   scaleCVT;

        Fixed  cvtStretchX;
        Fixed  cvtStretchY;

    int8   identityTransformation;  /* true/false  (does not mean identity from a global sense) */
    int8   non90DegreeTransformation; /* bit 0 is 1 if non-90 degree, bit 1 is 1 if x scale doesn't equal y scale */
    Fixed  xStretch;            /* Tweaking for glyphs under transformational stress <4> */
        Fixed  yStretch;                        /* Tweaking for glyphs under transformational stress <4> */
    
    int8 init;                      /* executing preprogram ?? */
    uint8 pgmIndex;                 /* which preprogram is current */
    LoopCount instrDefCount;        /* number of currently defined IDefs */
        int    squareScale;
	/* int16  pad2; */
#ifdef DEBUG
    sfnt_maxProfileTable*   maxp;
    uint16                  cvtCount;
#endif

} fnt_GlobalGraphicStateType;

/* 
 * This is the local graphics state  
 */
typedef struct fnt_LocalGraphicStateType {
    fnt_ElementType *CE0, *CE1, *CE2;   /* The character element pointers */
    VECTOR proj;                        /* Projection Vector */
    VECTOR free;                        /* Freedom Vector */
    VECTOR oldProj;                     /* Old Projection Vector */
    F26Dot6 *stackPointer;

    uint8 *insPtr;                      /* Pointer to the instruction we are about to execute */
    fnt_ElementType *elements;
    fnt_GlobalGraphicStateType *globalGS;
        FntTraceFunc TraceFunc;

    ArrayIndex Pt0, Pt1, Pt2;           /* The internal reference points */
    int16   roundToGrid;            
    LoopCount loop;                     /* The loop variable */ 
    uint8 opCode;                       /* The instruction we are executing */
    uint8 padByte;
    int16 padWord;

    /* Above is exported to client in FontScaler.h */

    VECTORTYPE pfProj; /* = pvx * fvx + pvy * fvy */

    FntMoveFunc MovePoint;
    FntProject Project;
    FntProject OldProject;
    InterpreterFunc Interpreter;
#ifdef FSCFG_REENTRANT
        F26Dot6 (*GetCVTEntry) (struct fnt_LocalGraphicStateType*,ArrayIndex);
        F26Dot6 (*GetSingleWidth) (struct fnt_LocalGraphicStateType*);
#else 
        F26Dot6 (*GetCVTEntry) (ArrayIndex n);
        F26Dot6 (*GetSingleWidth) (void);
#endif 
    FntMoveFunc ChangeCvt;
    Fixed       cvtDiagonalStretch;

    jmp_buf env;        /* always be at the end, since it is unknown size */
    /* int16   pad2; */

} fnt_LocalGraphicStateType;

/*
 * Executes the font instructions.
 * This is the external interface to the interpreter.
 *
 * Parameter Description
 *
 * elements points to the character elements. Element 0 is always
 * reserved and not used by the actual character.
 *
 * ptr points at the first instruction.
 * eptr points to right after the last instruction
 *
 * globalGS points at the global graphics state
 *
 * TraceFunc is pointer to a callback functioned called with a pointer to the
 *      local graphics state if TraceFunc is not null. The call is made just before
 *      every instruction is executed.
 *
 * Note: The stuff globalGS is pointing at must remain intact
 *       between calls to this function.
 */
extern int fnt_Execute(fnt_ElementType *elements, uint8 *ptr, register uint8 *eptr,
                            fnt_GlobalGraphicStateType *globalGS, voidFunc TraceFunc);

extern int fnt_SetDefaults (fnt_GlobalGraphicStateType *globalGS);

#define GLOBALGSTATE(key)           (fnt_GlobalGraphicStateType*) FONT_OFFSET (key->memoryBases[PRIVATE_FONT_SPACE_BASE], key->offset_globalGS)

#ifdef FSCFG_NO_INITIALIZED_DATA
void fnt_InitializeData (void);
#endif

#endif
