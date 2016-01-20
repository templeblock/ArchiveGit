/*
$Header: /PM8/TT/SFNT.CPP 1     3/03/99 6:35p Gbeddow $
$Log: /PM8/TT/SFNT.CPP $
// 
// 1     3/03/99 6:35p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:15:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:45:12   Fred
// Initial revision.
// 
//    Rev 1.2   21 Apr 1997 08:48:00   Fred
// Fixes for rotated pens and shadows.
// 
//    Rev 1.1   18 Apr 1997 12:49:52   Fred
// Changes for PMG 4.0
// 
//    Rev 1.0   14 Mar 1996 13:21:44   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:42:06   FRED
// Initial revision.
// 
//    Rev 1.1   04 Oct 1995 10:59:30   FRED
// 32 bit TrueType engine
// 
//    Rev 1.0   29 Sep 1995 13:37:00   FRED
// Initial revision.
 * 
 *    Rev 1.43   21 Jan 1993 14:07:38   MACDUFF
 * Restored version 1.41,  because 1.42 failed with composite characters.
 * 
 *    Rev 1.41   01 Jan 1993 10:18:06   FRED
 * Fixed bug where composite glyphs with backgrounds
 * were failing. This problem exists in all shipping
 * TTDOS (builds < 115) and TrueEffects (builds < 305).
 * 
 * 
 *    Rev 1.40   17 Dec 1992 13:31:08   MACDUFF
 * Finds kerning table.
 * 
 *    Rev 1.39   09 Nov 1992 16:04:14   MACDUFF
 * Sets fx_contours_added
 * 
 *    Rev 1.38   07 Oct 1992 10:53:20   MACDUFF
 * Checks MLS_ASM_SFNT_UnfoldCurve
 * 
 *    Rev 1.37   01 Sep 1992 14:55:36   FRED
 * Optimized some of the endcap code.
 * 
 * 
 *    Rev 1.36   30 Aug 1992 15:10:22   FRED
 * Changed Add R,2 to INC R/INC R
 * 
 *    Rev 1.35   29 Aug 1992 10:58:46   FRED
 * Uses new MLS_ASM_... #defines.
 * 
 * 
 *    Rev 1.34   28 Aug 1992 18:26:20   FRED
 * Added the missing parenthesis!
 * 
 * 
 *    Rev 1.33   28 Aug 1992 18:19:46   FRED
 * Added glyph_complete argument to sfnt_ReadSFNT() so that we can tell
 * when to add the background box. It should only be added after any and
 * all composite glyphs have been handled.
 * 
 * 
 *    Rev 1.32   28 Aug 1992 17:32:56   MACDUFF
 * ComputeIndex4 fully assemblerized
 * 
 *    Rev 1.31   28 Aug 1992 09:42:02   FRED
 * Coded more endcap stuff in assembly.
 * 
 * 
 *    Rev 1.30   27 Aug 1992 22:56:10   MACDUFF
 * Reduced size of cmap lookup by eliminating binary search component.
 * Sped up linear search component by coding in assembler.
 * 
 *    Rev 1.29   27 Aug 1992 18:42:50   ADAM
 * si.
 * 
 *    Rev 1.28   27 Aug 1992 18:36:44   FRED
 * Hinting is turned off for fixed pitch background
 * characters. For COURBI, "W", the hinting could cause
 * the glyph to extend outside of the background BBOX
 * which caused the background box to be drawn incorrectly.
 * 
 * 
 *    Rev 1.27   26 Aug 1992 17:40:00   ADAM
 * Fixed end caps with symbol fonts.
 * 
 *    Rev 1.26   26 Aug 1992 14:51:18   FRED
 * Fixed problem with < and > endcaps. The corners
 * could become to sharp and screw up outlines if the
 * space character was skinny.
 * 
 * 
 *    Rev 1.25   26 Aug 1992 14:29:12   FRED
 * More assembly code (left wing endcap).
 * 
 * 
 *    Rev 1.24   26 Aug 1992 08:52:22   FRED
 * Coded more endcap path stuff in assembly.
 * 
 * 
 *    Rev 1.23   26 Aug 1992 01:49:50   FRED
 * Changed some code to assembly.
 * 
 * 
 *    Rev 1.22   25 Aug 1992 18:24:36   FRED
 * Added add_box() function to make endcap code smaller.
 * 
 * Attempt (unfinished) at coding add_box() in assembly.
 * 
 * Fix (hopefully) for background fonts with white foregrounds
 * that were too skinny.
 * 
 * 
 *    Rev 1.21   25 Aug 1992 10:44:16   FRED
 * New code to compute metrics.
 * 
 * 
 *    Rev 1.20   24 Aug 1992 14:24:20   FRED
 * Added "wing" endcaps.
 * 
 * 
 *    Rev 1.19   23 Aug 1992 17:05:00   FRED
 * Support for endcaps.
 * 
 * 
 *    Rev 1.18   20 Aug 1992 08:32:02   MACDUFF
 * Fixed another >> 6.
 * 
 *    Rev 1.17   19 Aug 1992 15:47:06   MACDUFF
 * Uses variable-precision globals.
 * 
 *    Rev 1.16   16 Aug 1992 14:16:08   FRED
 * Fixed bug where bounding box for background fonts
 * with a white pattern were wrong because the winding
 * of the single outline box forced the outline pen to
 * be stroked one pixel out from the true bounding box
 * of the glyph.
 * 
 * 
 *    Rev 1.15   15 Aug 1992 23:54:18   FRED
 * Moved some of the background box code to the
 * "new trans" function in FSCALER.C so that the
 * global bounding box for background fonts can
 * be computed.
 * 
 * 
 *    Rev 1.14   14 Aug 1992 18:17:52   FRED
 * Fixed bug with replacing path data with pillow rectangle.
 * 
 * 
 *    Rev 1.13   14 Aug 1992 17:16:12   FRED
 * Added support for background effects on fixed
 * pitch fonts. Ideally, the background box will be
 * font space width. If the character is too wide,
 * then the box is expanded to include the character
 * and the minimum left an right padding. In MoreFonts
 * and Geronimo, these characters are horizontally
 * scaled so that they will end up at the desired
 * size... TrueType does not yet do this.
 * 
 * 
 *    Rev 1.12   14 Aug 1992 14:44:48   FRED
 * Pillow character has width of 2 * space width.
 * 
 * 
 *    Rev 1.11   14 Aug 1992 12:59:36   FRED
 * Added code to transform '_' into a "pillow" for
 * fonts with a background effect.
 * 
 * 
 *    Rev 1.10   14 Aug 1992 10:15:50   FRED
 * Calls set_background_pattern() to make background plaid work.
 * 
 * 
 *    Rev 1.9   13 Aug 1992 23:52:04   FRED
 * Changes to fix some bugs with pattern alignment.
 * 
 * 
 *    Rev 1.8   13 Aug 1992 22:30:40   FRED
 * No horizontal pattern alignment for woodgrain.
 * 
 * No horizontal pattern alignment > BACKGROUND_PAD_MAXIMUM.
 * 
 * Uses MLS_PATTERN definitions.
 * 
 * 
 *    Rev 1.7   13 Aug 1992 17:14:56   FRED
 * Sets background pattern offset for final vertical
 * pattern alignment.
 * 
 * 
 *    Rev 1.6   13 Aug 1992 12:13:02   FRED
 * First attempt at background pattern alignment.
 * 
 * 
 *    Rev 1.5   12 Aug 1992 17:27:52   FRED
 * Uses new BACKGROUND_PAD definitions.
 * 
 * 
 *    Rev 1.4   12 Aug 1992 11:43:38   FRED
 * Support for background effects. Properly sizes
 * background box for proportional fonts (still
 * have to do fixed pitch) and adds one or two
 * boxes to the glyph depending on the background
 * and fill patterns.
 * 
 * 
 *    Rev 1.3   28 Jul 1992 09:59:28   ADAM
 * Missed one change.
 * 
 *    Rev 1.2   28 Jul 1992 09:51:48   ADAM
 * Made code revisions to match those that were shipped from Microsoft
 * on the 7/24/92 source diskette.
 * 
 *    Rev 1.1   10 Jul 1992 10:56:36   ADAM
*/
/*
    File:       sfnt.c

    Contains:   xxx put contents here (or delete the whole line) xxx

    Written by: xxx put name of writer here (or delete the whole line) xxx

    Copyright:  c 1989-1990 by Apple Computer, Inc., all rights reserved.

    Change History (most recent first):

       <17+>     10/9/90    MR,rb   Remove classification of unused tables in sfnt_Classify
        <17>     8/10/90    MR      Pass nil for textLength parameter to MapString2, checked in
                                    other files to their precious little system will BUILD.  Talk
                                    about touchy!
        <16>     8/10/90    gbm     rolling out Mike's textLength change, because he hasn't checked
                                    in all the relevant files, and the build is BROKEN!
        <15>     8/10/90    MR      Add textLength arg to MapString2
        <14>     7/26/90    MR      don't include toolutil.h
        <13>     7/23/90    MR      Change computeindex routines to call functins in MapString.c
        <12>     7/18/90    MR      Add SWAPW macro for INTEL
        <11>     7/13/90    MR      Lots of Ansi-C stuff, change behavior of ComputeMapping to take
                                    platform and script
         <9>     6/27/90    MR      Changes for modified format 4: range is now times two, loose pad
                                    word between first two arrays.  Eric Mader
         <8>     6/21/90    MR      Add calls to ReleaseSfntFrag
         <7>      6/5/90    MR      remove vector mapping functions
         <6>      6/4/90    MR      Remove MVT
         <5>      5/3/90    RB      simplified decryption.
         <4>     4/10/90    CL      Fixed mapping table routines for double byte codes.
         <3>     3/20/90    CL      Joe found bug in mappingtable format 6 Added vector mapping
                                    functions use pointer-loops in sfnt_UnfoldCurve, changed z from
                                    int32 to int16
         <2>     2/27/90    CL      New error code for missing but needed table. (0x1409)  New
                                    CharToIndexMap Table format.
                                    Assume subtablenumber zero for old sfnt format.  Fixed
                                    transformed component bug.
       <3.2>    11/14/89    CEL     Left Side Bearing should work right for any transformation. The
                                    phantom points are in, even for components in a composite glyph.
                                    They should also work for transformations. Device metric are
                                    passed out in the output data structure. This should also work
                                    with transformations. Another leftsidebearing along the advance
                                    width vector is also passed out. whatever the metrics are for
                                    the component at it's level. Instructions are legal in
                                    components. Instructions are legal in components. Glyph-length 0
                                    bug in sfnt.c is fixed. Now it is legal to pass in zero as the
                                    address of memory when a piece of the sfnt is requested by the
                                    scaler. If this happens the scaler will simply exit with an
                                    error code ! Fixed bug with instructions in components.
       <3.1>     9/27/89    CEL     Removed phantom points.
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
      <y1.1>     5/26/89    CEL     Integrated the new Font Scaler 1.0 into Spline Fonts
       <1.0>     5/25/89    CEL     Integrated 1.0 Font scaler into Bass code for the first time.

    To Do:
        <3+>     3/20/90    mrr     Fixed mapping table routines for double byte codes.
                                    Added support for font program.
                                    Changed count from uint16 to int16 in vector char2index routines.
*/

#define BYTEREAD

/** FontScaler's Includes **/
#include "fserror.h"
#include "fscdefs.h"
#include "fontmath.h"
#include "sfnt.h"
#include "fnt.h"
#include "sc.h"
#include "fsglue.h"
#include "privsfnt.h"
#include "effects.h"

/** SFNT Packed format **/
typedef struct {
  int         numberContours;
  int16       * endPoints;             /** vector: indexes into x[], y[] **/
  uint8       * flags;                 /** vector **/
  BBOX        bbox;
} sfnt_PackedSplineFormat;

/* #define GetUnsignedByte( p ) *(((uint8  *)p)++) */
#define GetUnsignedByte( p ) ((uint8)(*p++))

/** <4> **/
#define fsg_MxCopy(a, b)    (*(b) = *(a))

extern void fsg_ShiftChar (fsg_SplineKey *key, F26Dot6 xShift, F26Dot6 yShift, int lastPoint);

#define PRIVATE

/* PRIVATE PROTOTYES */

int sfnt_UnfoldCurve (fsg_SplineKey*key, sfnt_PackedSplineFormat*charData, unsigned length, unsigned *elementCount);

void sfnt_Classify (fsg_SplineKey *key, sfnt_DirectoryEntryPtr dir);
uint16 sfnt_ComputeUnkownIndex (uint8 * mapping, uint16 charCode);
uint16 sfnt_ComputeIndex0 (uint8 * mapping, uint16 charCode);
uint16 sfnt_ComputeIndex2 (uint8 * mapping, uint16 charCode);
uint16 sfnt_ComputeIndex4 (uint8 * mapping, uint16 charCode);
uint16 sfnt_ComputeIndex6 (uint8 * mapping, uint16 charCode);
void sfnt_GetGlyphLocation (fsg_SplineKey*key, uint16 gIndex, uint32*offset, unsigned *length, sfnt_tableIndex* pGlyphTableIndex);

voidPtr sfnt_GetDataPtr (fsg_SplineKey *key, uint32 offset, unsigned length, sfnt_tableIndex n);

#define MAX_SFNT_FORMAT2PROC 7

uint16 (*sfnt_Format2Proc [MAX_SFNT_FORMAT2PROC])(uint8 *mapping, uint16 charCode)
#ifndef  FSCFG_NO_INITIALIZED_DATA
= { sfnt_ComputeIndex0, sfnt_ComputeUnkownIndex, sfnt_ComputeUnkownIndex, sfnt_ComputeUnkownIndex, sfnt_ComputeIndex4, sfnt_ComputeUnkownIndex, sfnt_ComputeIndex6 }
#endif 
;

#ifdef SEGMENT_LINK
#pragma segment SFNT_C
#endif


/*                              
 *
 *  sfnt_UnfoldCurve            <3>
 *
 *  ERROR:  return NON-ZERO
 *
 */

#ifndef IN_ASM

#ifndef MLS_ASM_SFNT_UnfoldCurve

PRIVATE int	sfnt_UnfoldCurve (fsg_SplineKey*key,
sfnt_PackedSplineFormat*charData,
unsigned length,
unsigned *elementCount)
{
  register int z;
  register uint8 flag, *byteP, *byteP2;
  register uint8 *p;
  register F26Dot6 *cpPtr;
  fnt_ElementType * elementPtr;
  int numPoints;

  elementPtr  = & (key->elementInfoRec.interpreterElements[GLYPHELEMENT]);

  if (*elementCount == GLYPHELEMENT) 
  {
    key->totalContours = 0;
    fsg_SetUpElement (key, GLYPHELEMENT);
  } 
  else 
  {
      /* # of points in previous component */
    fsg_IncrementElement (key, GLYPHELEMENT, key->numberOfRealPointsInComponent, elementPtr->nc);
  }
  key->totalComponents = *elementCount;

 (*elementCount)++;
  key->glyphLength = length;
  if (length <= 0) 
  {
    elementPtr->nc = 1;
    key->numberOfRealPointsInComponent = 1;
    key->totalContours += 1;

    elementPtr->sp[0] = 0;
    elementPtr->ep[0] = 0;

    elementPtr->onCurve[0] = ONCURVE;
    elementPtr->oox[0] = 0;
    elementPtr->ooy[0] = 0;
    return NO_ERR; /***************** exit here ! *****************/
  }

  elementPtr->nc = charData->numberContours;
  z = (int) elementPtr->nc;
  key->totalContours += z;
  if (z < 0 || z > (int)key->maxProfile.maxContours)
    return CONTOUR_DATA_ERR;

  {   /* <4> */
    register int16*sp = elementPtr->sp;
    register int16*ep = elementPtr->ep;
    int16 * charDataEP = charData->endPoints;
    register LoopCount i;
    *sp++ = 0;
    *ep = SWAPWINC (charDataEP);
    for (i = z - 2; i >= 0; --i)
    {
      *sp++ = *ep++ + 1;
      *ep = SWAPWINC (charDataEP);
    }
    numPoints = *ep + 1;
  }

/* Do flags */
  p = charData->flags;
  byteP = elementPtr->onCurve;
  byteP2 = byteP + numPoints;         /* only need to set this guy once */
  while (byteP < byteP2)
  {
    *byteP++ = flag = GetUnsignedByte (p);
    if (flag & REPEAT_FLAGS) 
    {
      register LoopCount count = GetUnsignedByte (p);
      while (count--) 
      {
	*byteP++ = flag;
      }
    }
  }

/* Do X first */
  z = 0;
  byteP = elementPtr->onCurve;
  cpPtr = elementPtr->oox;
  while (byteP < byteP2)
  {
    if ((flag = *byteP++) & XSHORT) 
    {
      if (flag & SHORT_X_IS_POS)
	z += GetUnsignedByte (p);
      else
	z -= GetUnsignedByte (p);
    } 
    else if (! (flag & NEXT_X_IS_ZERO)) 
    { /* This means we have a int32 (2 byte) vector */
#ifdef BYTEREAD
      z += (int) ((int8)(*p++) << 8);
      z += (uint8) * p++;
#else
      z += * ((int16 *)p);
      p += sizeof (int16);
#endif
    }
    *cpPtr++ = (F26Dot6) z;
  }

/* Now Do Y */
  z = 0;
  byteP = elementPtr->onCurve;
  cpPtr = elementPtr->ooy;
  while (byteP < byteP2)
  {
    if ((flag = *byteP) & YSHORT) 
    {
      if (flag & SHORT_Y_IS_POS)
	z += GetUnsignedByte (p);
      else
	z -= GetUnsignedByte (p);
    } 
    else if (! (flag & NEXT_Y_IS_ZERO)) 
    { /* This means we have a int32 (2 byte) vector */
#ifdef BYTEREAD
      z += (int) ((int8)(*p++) << 8);
      z += (uint8) * p++;
#else
      z += * ((int16 *)p);
      p += sizeof (int16);
#endif
    }
    *cpPtr++ = z;

    *byteP++ = flag & (uint8)ONCURVE; /* Filter out unwanted stuff */
  }

  key->numberOfRealPointsInComponent = numPoints;
  if (numPoints > (int) key->maxProfile.maxPoints)
    return POINTS_DATA_ERR;

  if (! (key->compFlags & NON_OVERLAPPING)) 
  {
    elementPtr->onCurve[0] |= OVERLAP;
  }

  return NO_ERR;
}
#endif		/* MLS_ASM_SFNT_UnfoldCurve */

/*
 * Internal routine (make this an array and do a look up?)
 */
PRIVATE void sfnt_Classify (register fsg_SplineKey *key,
register sfnt_DirectoryEntryPtr dir)
{
  ArrayIndex  Index = -1;

  switch (SFNT_SWAPTAG(dir->tag))
  {
    case tag_FontHeader:
      Index = sfnt_fontHeader;
      break;
    case tag_HoriHeader:
      Index = sfnt_horiHeader;
      break;
    case tag_IndexToLoc:
      Index = sfnt_indexToLoc;
      break;
    case tag_MaxProfile:
      Index = sfnt_maxProfile;
      break;
    case tag_ControlValue:
      Index = sfnt_controlValue;
      break;
    case tag_PreProgram:
      Index = sfnt_preProgram;
      break;
    case tag_GlyphData:
      Index = sfnt_glyphData;
      break;
    case tag_HorizontalMetrics:
      Index = sfnt_horizontalMetrics;
      break;
    case tag_CharToIndexMap:
      Index = sfnt_charToIndexMap;
      break;
    case tag_FontProgram:
      Index = sfnt_fontProgram;      /* <4> */
      break;
#ifdef PC_OS
    case tag_Postscript:
      Index = sfnt_Postscript;
      break;
    case tag_HoriDeviceMetrics:
      Index = sfnt_HoriDeviceMetrics;
      break;
    case tag_LinearThreeshold:
      Index = sfnt_LinearThreeShold;
      break;
    case tag_NamingTable:
      Index = sfnt_Names;
      break;
    case tag_OS_2:
      Index = sfnt_OS_2;
      break;
#endif
#ifdef FSCFG_USE_GLYPH_DIRECTORY
    case tag_GlyphDirectory:
      Index = sfnt_GlyphDirectory;
      break;
#endif 
	 case tag_Kerning:
		Index = sfnt_Kerning;
		break;
  }
  if (Index >= 0)
  {
    key->offsetTableMap[Index].Offset = SWAPL (dir->offset);
    key->offsetTableMap[Index].Length = (unsigned) SWAPL (dir->length);
  }
}

#endif


/*
 * Creates mapping for finding offset table     <4>
 */
void sfnt_DoOffsetTableMap (register fsg_SplineKey *key)
{
  register LoopCount i;
  sfnt_OffsetTablePtr sfntDirectory;
  LoopCount cTables;

  if ((sfntDirectory = (sfnt_OffsetTablePtr) GETSFNTFRAG (key, key->clientID, 0, sizeof (sfnt_OffsetTable))) != 0)
  {
    cTables = (LoopCount) SWAPW (sfntDirectory->numOffsets) - 1;
    RELEASESFNTFRAG(key, sfntDirectory);
    sfntDirectory = (sfnt_OffsetTablePtr) GETSFNTFRAG (key, key->clientID, 0, sizeof (sfnt_OffsetTable) + sizeof (sfnt_DirectoryEntry) * cTables);
  }

  if (!sfntDirectory)
    fs_longjmp (key->env, NULL_SFNT_DIR_ERR); /* Do a gracefull recovery  */

    /* Initialize */
  MEMSET ((void *)key->offsetTableMap, 0, sizeof (key->offsetTableMap));
  {
    register sfnt_DirectoryEntryPtr dir = &sfntDirectory->table[cTables];
    for (i = cTables; i >= 0; --i, --dir)
      sfnt_Classify (key, dir);
  }

#ifdef FSCFG_USE_GLYPH_DIRECTORY
    key->offsetTableMap[sfnt_BeginningOfFont].Offset = 0;
    key->offsetTableMap[sfnt_BeginningOfFont].Length = ~0;
#endif

  RELEASESFNTFRAG(key, sfntDirectory);
}


/*
 * Use this function when only part of the table is needed.
 *
 * n is the table number.
 * offset is within table.
 * length is length of data needed.
 * To get an entire table, pass length = -1     <4>
 */

voidPtr sfnt_GetTablePtr (register fsg_SplineKey *key, register sfnt_tableIndex n, register boolean mustHaveTable)
{
  unsigned length = (unsigned)(key->offsetTableMap[n].Length);
  register voidPtr fragment;
  int Ret;

  if (length)
  {
    if ((fragment = GETSFNTFRAG (key, key->clientID, key->offsetTableMap[n].Offset, length)) != 0)
      return fragment;
    else
      Ret = CLIENT_RETURNED_NULL;
  } 
  else 
  {
    if (mustHaveTable)
      Ret = MISSING_SFNT_TABLE; /* Do a gracefull recovery  */
    else
      return (voidPtr) 0;
  }

  fs_longjmp (key->env, Ret); /* Do a gracefull recovery  */

  return NULL;
}

PRIVATE voidPtr sfnt_GetDataPtr (register fsg_SplineKey *key,
register uint32 offset,
register unsigned length,
register sfnt_tableIndex n)
{
  register voidPtr fragment;
  register int     Ret;

  if (key->offsetTableMap[n].Length)
  {
    if ((fragment = GETSFNTFRAG (key, key->clientID, offset + key->offsetTableMap[n].Offset, length)) != 0)
      return fragment;
    else
      Ret = CLIENT_RETURNED_NULL; /* Do a gracefull recovery  */
  } 
  else
    Ret = MISSING_SFNT_TABLE;

  fs_longjmp (key->env, Ret); /* Do a gracefull recovery  */

  return NULL;
}


/*
 * This, is when we don't know what is going on
 */
PRIVATE uint16 sfnt_ComputeUnkownIndex (uint8 * mapping, uint16 gi)
{
  return 0;
}


/*
 * Byte Table Mapping 256->256          <4>
 */
#ifndef IN_ASM
PRIVATE uint16 sfnt_ComputeIndex0 (uint8 * mapping, register uint16 charCode)
{
  return (charCode < 256 ? mapping[charCode] : 0);
}

#if 0
/*
 * High byte mapping through table
 *
 * Useful for the national standards for Japanese, Chinese, and Korean characters.
 *
 * Dedicated in spirit and logic to Mark Davis and the International group.
 *
 *  Algorithm: (I think)
 *      First byte indexes into KeyOffset table.  If the offset is 0, keep going, else use second byte.
 *      That offset is from beginning of data into subHeader, which has 4 words per entry.
 *          entry, extent, delta, range
 *
 */

typedef struct {
  uint16  firstCode;
  uint16  entryCount;
  int16   idDelta;
  uint16  idRangeOffset;
} sfnt_subHeader;

typedef struct {
  uint16  subHeadersKeys [256];
  sfnt_subHeader  subHeaders [1];
} sfnt_mappingTable2;

PRIVATE uint16 sfnt_ComputeIndex2 (uint8 * mapping, uint16 charCode)
{
  register uint16 index, mapMe;
  uint16   highByte;
  sfnt_mappingTable2 *Table2 = (sfnt_mappingTable2 *) mapping;
  sfnt_subHeader *subHeader;

/* mapping */
  index = 0;  /* Initially assume missing */
  highByte = charCode >> 8;

  if (Table2->subHeadersKeys [highByte])
    mapMe = charCode & 0xff; /* We also need the low byte. */
  else
    mapMe = highByte;

  subHeader = (sfnt_subHeader *) ((char *) &Table2->subHeaders + SWAPW (Table2->subHeadersKeys [highByte]));

  mapMe -= SWAPW (subHeader->firstCode);         /* Subtract first code. */
  if (mapMe < SWAPW (subHeader->entryCount))
  {  /* See if within range. */
    uint16 glyph;

    if (glyph = * ((uint16 *) ((char *) &subHeader + SWAPW (subHeader->idRangeOffset)) + mapMe))
      index = glyph + (uint16) SWAPW (subHeader->idDelta);
  }
  return index;
}

#endif


#define maxLinearX2 16
#define BinaryIteration \
        newP = (uint16 *) ((int8 *)tableP + (range >>= 1)); \
        if (charCode > (uint16) SWAPW (*newP)) tableP = newP;

/*
 * Segment mapping to delta values, Yack.. !
 *
 * In memory of Peter Edberg. Initial code taken from code example supplied by Peter.
 */
#ifndef MLS_ASM_SFNT_ComputeIndex4

PRIVATE uint16 sfnt_ComputeIndex4 (uint8 * mapping, register uint16 charCode)
{
/* Do not change these declarations if MLS_ASM is defined! -- MH 8/27/92 */
  register uint16 *tableP;
  register uint16 idDelta;
  register uint16 offset, segCountX2, index;

/* mapping */
  tableP = (uint16 *)mapping;

  index = 0; /* assume missing initially */
  segCountX2 = SWAPWINC (tableP);

  if (segCountX2 < maxLinearX2) 
  {
    tableP += 3; /* skip binary search parameters */
  } 
  else 
  {
/* start with unrolled binary search */
    register uint16 *newP;
    register int16  range;      /* size of current search range */
    register uint16 selector;   /* where to jump into unrolled binary search */
    register uint16 shift;      /* for shifting of range */

    range       = SWAPWINC (tableP); /* == 2**floor (log2 (segCount)) == largest power of two <= segCount */
    selector    = SWAPWINC (tableP); /* == 2* log2 (range) */
    shift       = SWAPWINC (tableP); /* == 2* (segCount-range) */
/* tableP points at endCount[] */

    if (charCode >= (uint16) SWAPW (* ((uint16 *) ((int8 *)tableP + range))))
      tableP = (uint16 *) ((int8 *)tableP + shift); /* range to low shift it up */

    switch (selector >> 1)
    {
    case 15: 
      BinaryIteration;
    case 14: 
      BinaryIteration;
    case 13: 
      BinaryIteration;
    case 12: 
      BinaryIteration;
    case 11: 
      BinaryIteration;
    case 10: 
      BinaryIteration;
    case 9: 
      BinaryIteration;
    case 8: 
      BinaryIteration;
    case 7: 
      BinaryIteration;
    case 6: 
      BinaryIteration;
    case 5: 
      BinaryIteration;
    case 4: 
      BinaryIteration;
    case 3:
    case 2:  /* drop through */
    case 1:
    case 0:
      break;
    }
  }
/* Now do linear search */
  for (; charCode > (uint16) SWAPW (*tableP); tableP++)
    ;

tableP++;  /*??? Warning this is to fix a bug in the font */

/* End of search, now do mapping */

  tableP = (uint16 *) ((int8 *)tableP + segCountX2); /* point at startCount[] */
  if (charCode >= (uint16) SWAPW (*tableP))
  {
    offset = charCode - (uint16) SWAPW (*tableP);
    tableP = (uint16 *) ((int8 *)tableP + segCountX2); /* point to idDelta[] */
    idDelta = (uint16) SWAPW (*tableP);
    tableP = (uint16 *) ((int8 *)tableP + segCountX2); /* point to idRangeOffset[] */
    if ((uint16) SWAPW (*tableP) == 0)
    {
      index   = charCode + idDelta;
    } 
    else 
    {
      offset += offset; /* make word offset */
      tableP  = (uint16 *) ((int8 *)tableP + (uint16) SWAPW (*tableP) + offset); /* point to glyphIndexArray[] */
      index   = (uint16) SWAPW (*tableP) + idDelta;
    }
  }
  return index;
}

#else

/*
 * Assembler version, skips the binary search
 */

PRIVATE uint16 sfnt_ComputeIndex4 (uint8 * mapping, uint16 charCode)
{
/*		uint16 *tableP			in SI */
/*		uint16 idDelta;		in BX */
/*		uint16 offset;			in DI */
/*		uint16 segCountX2;	in CX */
/*		charCode is in DX most of the time */

	uint16 index;
	asm {
		/* tableP = (uint16 *)mapping; */
		mov	si, mapping	

		/* index = 0; */
		mov	word ptr index, 0

		/* segCountX2 = SWAPWINC(tableP) */
		cld
		lodsw
		xchg	ah, al
		mov	cx, ax

		/* tableP += 3 */
		add	si, 6

		/* Linear search */
		mov	dx, charCode
	}
l_search:
	asm {
		lodsw
		xchg	ah, al
		cmp	ax, dx
		jb		short l_search

		/* Do mapping */

		/* tableP = (uint16 *) ((int8 *)tableP + segCountX2); */
		add	si, cx

		/* if (charCode >= (uint16) SWAPW(*tableP)) */
		mov	ax, word ptr [si]
		xchg	ah, al
		cmp	ax, dx
		ja		l_cmap4_1

			/* offset = charCode - (uint16) SWAPW(*tableP) */
			mov	ax, word ptr [si]
			xchg	ah, al
			sub	dx, ax			/* DX no longer contains charCode */
			mov	di, dx

			/* tableP = (uint16 *) ((int8 *)tableP + segCountX2); */
			add	si, cx

			/* idDelta = (uint16) SWAPW(*tableP); */
			mov	ax, word ptr [si]
			xchg	ah, al
			mov	bx, ax

			/* tableP = (uint16 *) ((int8 *)tableP + segCountX2); */
			add	si, cx

			/* if ((uint16) SWAPW(*tableP) == 0) */
			mov	ax, word ptr [si]
			xchg	ah, al
			or		ax, ax
			jne	l_cmap4_2

				/* index = charCode + idDelta */
				mov	ax, charCode
				jmp	l_cmap4_3
			}

l_cmap4_2:
		/* else */
		asm {
				/* offset += offset */
				add	di, di

				/* tableP = (uint16 *) ((int8 *)tableP +
									(uint16) SWAPW(*tableP) + offset); */
				mov	ax, word ptr [si]
				xchg	ah, al
				mov	dx, si
				add	dx, ax
				add	dx, di
				mov	si, dx

				/* index = (uint16) SWAPW(*tableP) + idDelta; */
				mov	ax, word ptr [si]
				xchg	ah, al
			}
l_cmap4_3:
		asm {
				add	ax, bx
				mov	word ptr index, ax
		}
l_cmap4_1:
	asm {
			/* return index */
			mov	ax, index
	}
}

#endif



/*
 * Trimmed Table Mapping
 */

typedef struct {
  uint16  firstCode;
  uint16  entryCount;
  uint16  glyphIdArray [1];
} sfnt_mappingTable6;

PRIVATE uint16 sfnt_ComputeIndex6 (uint8 * mapping, uint16 charCode)
{
  register sfnt_mappingTable6 *Table6 = (sfnt_mappingTable6 *) mapping;

/* mapping */
  charCode  -= SWAPW (Table6->firstCode);
  return (charCode < (uint16) SWAPW (Table6->entryCount) ? (uint16) SWAPW (Table6->glyphIdArray [charCode]) : 0);
}

#endif




/*
 * Sets up our mapping function pointer.
 */
int	sfnt_ComputeMapping (register fsg_SplineKey *key, uint16 platformID, uint16 specificID)
{
  sfnt_char2IndexDirectoryPtr table = (sfnt_char2IndexDirectoryPtr)sfnt_GetTablePtr (key, sfnt_charToIndexMap, false);
  uint8 * mapping = (uint8 *)table;
  uint16 format;
  boolean found = false;
  int   Ret = NO_ERR;

  if (!table)	
  {
    /* If no "cmap" is present, permits access to characters by glyph index */
    key->mappingF = sfnt_ComputeUnkownIndex;
    return NO_ERR;
  }

  platformID = (uint16) SWAPW (platformID);
  specificID = (uint16) SWAPW (specificID);

/* mapping */
  {
    register sfnt_platformEntryPtr plat = (sfnt_platformEntryPtr) table->platform;    /* <4> */
    for (; plat < &table->platform [SWAPW (table->numTables)]; ++plat)
    {
      if (plat->platformID == platformID && plat->specificID == specificID)
      {
        found = true;
        key->mappOffset = (unsigned) SWAPL (plat->offset);  /* skip header */
        break;
      }
    }
  }

  if (!found)
  {
    Ret = OUT_OF_RANGE_SUBTABLE;
    format = (uint16) -1;
  }
  else
  {
    format = * (uint16 *)(mapping + key->mappOffset);     /* back up for header */
    format = SWAPW (format);
    key->mappOffset += sizeof (sfnt_mappingTable);
  }

#ifndef NOT_ON_THE_MAC
  switch (format)
  {
  case 0:
    key->mappingF = sfnt_ComputeIndex0;
    break;
#if 0
  case 2:
    key->mappingF = sfnt_ComputeIndex2;
    break;
#endif
  case 4:
    key->mappingF = sfnt_ComputeIndex4;
    break;
  case 6:
    key->mappingF = sfnt_ComputeIndex6;
    break;
  default:
    key->mappingF = sfnt_ComputeUnkownIndex;
    break;
  }
#else
  key->mappingF = format <= 6 ? sfnt_Format2Proc [format]: sfnt_ComputeUnkownIndex;
#endif
  RELEASESFNTFRAG(key, table);
  return Ret;
}


/*
 *
 */

void sfnt_ReadSFNTMetrics (fsg_SplineKey*key, register uint16 glyphIndex)
{
  register sfnt_HorizontalMetricsPtr  horizMetricPtr;
  register uint16                     numberOf_LongHorMetrics = key->numberOf_LongHorMetrics;

  horizMetricPtr  = (sfnt_HorizontalMetricsPtr)sfnt_GetTablePtr (key, sfnt_horizontalMetrics, true);

  if (glyphIndex < numberOf_LongHorMetrics) 
  {
    key->nonScaledAW    = SWAPW (horizMetricPtr[glyphIndex].advanceWidth);
    key->nonScaledLSB   = SWAPW (horizMetricPtr[glyphIndex].leftSideBearing);
  } 
  else 
  {
    int16 * lsb = (int16 *) & horizMetricPtr[numberOf_LongHorMetrics]; /* first entry after[AW,LSB] array */

    key->nonScaledAW    = SWAPW (horizMetricPtr[numberOf_LongHorMetrics-1].advanceWidth);
    key->nonScaledLSB   = SWAPW (lsb[glyphIndex - numberOf_LongHorMetrics]);
  }
  RELEASESFNTFRAG(key, horizMetricPtr);
}

PRIVATE void sfnt_GetGlyphLocation (fsg_SplineKey*key, uint16 gIndex, uint32*offset, unsigned *length, sfnt_tableIndex* pGlyphTableIndex)
{
  void * indexPtr;

#ifdef FSCFG_USE_GLYPH_DIRECTORY
  uint32 * offsetPtr;
  uint16 * lengthPtr;
  uint16 gSize;

  indexPtr = sfnt_GetTablePtr (key, sfnt_GlyphDirectory, false);
  if  (indexPtr)
  {
    offsetPtr = (uint32 *)((char *)indexPtr+(gIndex*(sizeof(int32)+sizeof(uint16))));
    lengthPtr = (uint16 *)(offsetPtr+1);

    *offset = SWAPL(*offsetPtr);
    *length = (*offset == 0L) ? 0 : (unsigned) SWAPW(*lengthPtr);
    *pGlyphTableIndex = sfnt_BeginningOfFont;

    RELEASESFNTFRAG(key, indexPtr);
    return;
  }
#endif 

  indexPtr = sfnt_GetTablePtr (key, sfnt_indexToLoc, true);

  if (SWAPW (key->indexToLocFormat) == SHORT_INDEX_TO_LOC_FORMAT)
  {
    register uint16 *shortIndexToLoc = (uint16 *)indexPtr;
    shortIndexToLoc += gIndex;
    *offset = (uint32) (unsigned) SWAPW (*shortIndexToLoc) << 1; shortIndexToLoc++;
    *length = (unsigned) (((uint32) (unsigned) SWAPW (*shortIndexToLoc) << 1) - *offset);
  }
  else
  {
    register uint32 *longIndexToLoc = (uint32 *)indexPtr;
    longIndexToLoc += gIndex;
    *offset = SWAPL (*longIndexToLoc); longIndexToLoc++;
    *length = (unsigned) (SWAPL (*longIndexToLoc) - *offset);
  }

  *pGlyphTableIndex = sfnt_glyphData;
  RELEASESFNTFRAG(key, indexPtr);
}


/*
 *  <4>
 */
int	sfnt_ReadSFNT (register fsg_SplineKey *key,
unsigned   *elementCount,
register uint16 gIndex,
boolean useHints,
voidFunc traceFunc,
uint16 glyph_complete)
{
	unsigned				sizeOfInstructions = 0;
	uint8 *			instructionPtr;
	unsigned				length;
	uint32				offset;
	int					result  = NO_ERR;
	int16 *			shortP;
	void *			glyphDataPtr = 0;  /* to signal ReleaseSfntFrag if we never asked for it */
	Fixed					ignoreX, ignoreY;
	sfnt_tableIndex	glyphTableIndex;
	sfnt_PackedSplineFormat	charData;

	sfnt_ReadSFNTMetrics (key, gIndex);
	sfnt_GetGlyphLocation (key, gIndex, &offset, &length, &glyphTableIndex);

	if (length > 0)
	{
		glyphDataPtr = sfnt_GetDataPtr (key, offset, length, glyphTableIndex);

		shortP = (int16 *)glyphDataPtr;

		charData.numberContours = SWAPWINC (shortP);
		charData.bbox.xMin = SWAPWINC (shortP);
		charData.bbox.yMin = SWAPWINC (shortP);
		charData.bbox.xMax = SWAPWINC (shortP);
		charData.bbox.yMax = SWAPWINC (shortP);
	}

	else
	{
		charData.numberContours = 1;
		MEMSET ((void *)&charData.bbox, 0, sizeof (charData.bbox));
	}

	if (charData.numberContours >= 0) /* Not a component glyph */
	{
		key->lastGlyph = ! (key->weGotComponents && (key->compFlags & MORE_COMPONENTS));

		if (length > 0)
		{
			charData.endPoints = shortP;
			shortP += charData.numberContours;
			sizeOfInstructions = SWAPWINC (shortP);
			instructionPtr = (uint8 *)shortP;
			charData.flags = (uint8 *)shortP + sizeOfInstructions;
		}

		if (!(result = sfnt_UnfoldCurve (key, &charData, length, elementCount)))
		{
			uint8 * p;

do_grid_fit:

			FS_COPY_INSTRUCTIONS (instructionPtr, sizeOfInstructions, p);
			result = fsg_InnerGridFit (key, useHints, traceFunc, &charData.bbox, sizeOfInstructions, p, charData.numberContours < 0);
			FS_RELEASE_INSTRUCTION_COPY (p);
		}
  	}

  	else
	{
		if ((key->weGotComponents = (charData.numberContours == -1)) != 0)
		{
			uint16 flags;
			boolean transformTrashed = false;
			fnt_GlobalGraphicStateType * globalGS = GLOBALGSTATE (key);

			/* save the base scaling factors in case it will be used */
			globalGS->scaleXBase = globalGS->scaleX;
			globalGS->scaleYBase = globalGS->scaleY;
			globalGS->ScaleFuncXBase = globalGS->ScaleFuncX;
			globalGS->ScaleFuncYBase = globalGS->ScaleFuncY;

			do
			{
				transMatrix ctmSaveT, localSaveT;
				uint16 glyphIndex;
				int16 arg1, arg2;

				flags       = (uint16)SWAPWINC (shortP);
				glyphIndex  = (uint16)SWAPWINC (shortP);

				if (flags & ARG_1_AND_2_ARE_WORDS)
				{
					arg1    = SWAPWINC (shortP);
					arg2    = SWAPWINC (shortP);
				}

				else
				{
					int8 * byteP = (int8 *)shortP;

					if (flags & ARGS_ARE_XY_VALUES)
					{
						/* offsets are signed */
						arg1 = *byteP++;
						arg2 = *byteP;
					}
					else
					{
						/* anchor points are unsigned */
						arg1 = (uint8) * byteP++;
						arg2 = (uint8) * byteP;
					}

					++shortP;
				}

				if (flags & (WE_HAVE_A_SCALE | WE_HAVE_AN_X_AND_Y_SCALE | WE_HAVE_A_TWO_BY_TWO))
				{
					fsg_MxCopy (&key->currentTMatrix, &ctmSaveT);
					fsg_MxCopy (&key->localTMatrix, &localSaveT);
					transformTrashed = true;

					if (flags & WE_HAVE_A_TWO_BY_TWO)
					{
						register Fixed multiplier;
						transMatrix mulT;

						multiplier  = SWAPWINC (shortP); /* read 2.14 */
						mulT.transform[0][0] = (multiplier << 2); /* turn into 16.16 */

						multiplier  = SWAPWINC (shortP); /* read 2.14 */
						mulT.transform[0][1] = (multiplier << 2); /* turn into 16.16 */

						multiplier  = SWAPWINC (shortP); /* read 2.14 */
						mulT.transform[1][0] = (multiplier << 2); /* turn into 16.16 */

						multiplier  = SWAPWINC (shortP); /* read 2.14 */
						mulT.transform[1][1] = (multiplier << 2); /* turn into 16.16 */

						fsg_MxConcat2x2 (&mulT, &key->currentTMatrix);
						fsg_MxConcat2x2 (&mulT, &key->localTMatrix);
					}

					else
					{
						Fixed xScale, yScale;

						xScale  = (Fixed)SWAPWINC (shortP); /* read 2.14 */
						xScale <<= 2; /* turn into 16.16 */

						if (flags & WE_HAVE_AN_X_AND_Y_SCALE)
						{
							yScale  = (Fixed)SWAPWINC (shortP); /* read 2.14 */
							yScale <<= 2; /* turn into 16.16 */
						}
						else
						{
							yScale = xScale;
						}

						fsg_MxScaleAB (xScale, yScale, &key->currentTMatrix);
						fsg_MxScaleAB (xScale, yScale, &key->localTMatrix);
					}

					fsg_InitInterpreterTrans (key, &key->interpLocalScalarX, &key->interpLocalScalarY, &ignoreX, &ignoreY); /*** Compute global stretch etc. ***/
					key->localTIsIdentity = false;
				}

				key->compFlags = flags;
				key->arg1 = arg1;
				key->arg2 = arg2;

				result = sfnt_ReadSFNT (key, elementCount, glyphIndex, useHints, traceFunc, 0);

				if (transformTrashed)
				{
					fsg_MxCopy (&ctmSaveT, &key->currentTMatrix);
					fsg_InitInterpreterTrans (key, &key->interpLocalScalarX, &key->interpLocalScalarY, &ignoreX, &ignoreY); /*** Compute global stretch etc. ***/

					fsg_MxCopy (&localSaveT, &key->localTMatrix);
					transformTrashed = false;
				}
			} while (flags & MORE_COMPONENTS && result == NO_ERR);

			/* Do Final Composite Pass */
			sfnt_ReadSFNTMetrics (key, gIndex); /* read metrics again */

			if (flags & WE_HAVE_INSTRUCTIONS)
			{
				sizeOfInstructions = (int) (uint16)SWAPWINC (shortP);
				instructionPtr = (uint8 *)shortP;
			}

			goto do_grid_fit;
		}

		else
		{
			result = UNKNOWN_COMPOSITE_VERSION;
		}
	}

	if (glyphDataPtr)
	{
		RELEASESFNTFRAG(key, glyphDataPtr);
	}

	return (result);
}


#ifdef  FSCFG_NO_INITIALIZED_DATA
        void sfnt_InitializeData (void)
        {
            sfnt_Format2Proc[0] = sfnt_ComputeIndex0;
            sfnt_Format2Proc[1] = sfnt_ComputeUnkownIndex;
            sfnt_Format2Proc[2] = sfnt_ComputeUnkownIndex;
            sfnt_Format2Proc[3] = sfnt_ComputeUnkownIndex;
            sfnt_Format2Proc[4] = sfnt_ComputeIndex4;
            sfnt_Format2Proc[5] = sfnt_ComputeUnkownIndex;
            sfnt_Format2Proc[6] = sfnt_ComputeIndex6;
        }
#endif 
