/*
$Header: /PM8/TT/SC.CPP 1     3/03/99 6:35p Gbeddow $
$Log: /PM8/TT/SC.CPP $
// 
// 1     3/03/99 6:35p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:15:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:45:12   Fred
// Initial revision.
// 
//    Rev 1.3   01 Jul 1997 09:08:36   Fred
// More changes for PMG 4.0
// 
//    Rev 1.2   02 May 1997 10:15:02   Fred
//  
// 
//    Rev 1.1   18 Apr 1997 12:49:50   Fred
// Changes for PMG 4.0
// 
//    Rev 1.0   14 Mar 1996 13:21:40   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:42:02   FRED
// Initial revision.
// 
//    Rev 1.1   04 Oct 1995 10:59:34   FRED
// 32 bit TrueType engine
// 
//    Rev 1.0   29 Sep 1995 13:37:02   FRED
// Initial revision.
 * 
 *    Rev 1.61   15 Jan 1993 15:40:08   MACDUFF
 * small speed optimization to ISORT.
 * 
 *    Rev 1.60   15 Jan 1993 11:20:24   MACDUFF
 * Added assembler procedure version of ISORT to replace C Macro.  This
 * saves 216 bytes in exchange for slight loss of speed (around 0.2%).
 * 
 *    Rev 1.59   13 Jan 1993 15:48:46   JAY
 * New sc_mark function in assembly.
 * 
 *    Rev 1.58   08 Jan 1993 09:29:00   MACDUFF
 * Coded DDA functions in assembler
 * 
 *    Rev 1.57   15 Dec 1992 12:08:40   MACDUFF
 * Optimized C version of fill_black_rectangle; deleted asm version which
 * didn't help much.
 * 
 *    Rev 1.56   08 Dec 1992 11:32:02   MACDUFF
 * (TRUEFX only) Added fill_black_rectangle and fill_white_rectangle for
 * more efficient stroking of rectangle pens.
 * 
 *    Rev 1.55   02 Dec 1992 18:27:16   MACDUFF
 * In ScanChar, if number of contours is zero, do not return immediately,
 * but do a goto to the code we've added after the character fill is done.
 * 
 *    Rev 1.54   09 Nov 1992 16:04:50   MACDUFF
 * Uses fx_contours_added to avoid filling in the space between the
 * extra background contours.  This replaces code inside of sc_wnNrowfill
 * that checked against the bounding box of the character.
 * 
 *    Rev 1.53   09 Nov 1992 13:29:34   MACDUFF
 * sc_scan_background can now handle rotated fonts: it makes a glyph
 * consisting of the last contour added in sfnt_ReadSfnt and calls
 * sc_ScanChar to fill it.
 * 
 *    Rev 1.52   05 Nov 1992 17:16:26   MACDUFF
 * Stripped sc_initialize_buffer and sc_scan_background out of sc_ScanChar.
 * This is in preparation for background rotation, but is also simpler.
 * 
 *    Rev 1.51   14 Oct 1992 13:18:30   MACDUFF
 * Deleted some commented-out lines of code.
 * 
 *    Rev 1.50   13 Oct 1992 10:11:22   MACDUFF
 * Added one more check for pattern=0, to skip call to pattern processor.
 * 
 *    Rev 1.49   13 Oct 1992 09:19:34   MACDUFF
 * Coded sc_wnNrowFill in asm.  It processes pattern data 16 bits at a
 * time rather than 32 bits at a time, thus fill_pattern_run and
 * PATTERNS.ASM also change.
 * 
 *    Rev 1.48   15 Sep 1992 15:57:22   ADAM
 * Some MLS assembler code was referencing the compilers far procedures.
 * These are no longer available. They are now: NEAR_ followed by the name.
 * 
 *    Rev 1.47   31 Aug 1992 23:13:08   FRED
 * Added "L" to constant to turn off some warnings.
 * 
 * 
 *    Rev 1.46   31 Aug 1992 14:58:36   FRED
 * New version of assembly sc_DrawParabola()... no recursion!
 * 
 * 
 *    Rev 1.45   31 Aug 1992 12:02:12   FRED
 * First version od sc_DrawParabola() in assembly.
 * 
 * 
 *    Rev 1.44   30 Aug 1992 16:45:10   FRED
 * Worked more on assmbly version of sc_DrawParabola -- STILL NOT FINISHED!
 * 
 * 
 *    Rev 1.43   29 Aug 1992 16:38:00   FRED
 * sc_DrawParabola() partially coded in assembly -- NOT FINISHED!
 * 
 * 
 *    Rev 1.42   29 Aug 1992 13:20:36   FRED
 * Coded fill_pattern_run() in assembly.
 * 
 * 
 *    Rev 1.41   27 Aug 1992 18:04:40   FRED
 * If the character is an endcap, then set_background_pattern()
 * is called to set the fill pattern. This will fill endcaps
 * with background plaid instead of foreground plaid.
 * 
 * 
 *    Rev 1.40   25 Aug 1992 18:08:14   FRED
 * Fixed bug with endcaps - printing too thin.
 * 
 * 
 *    Rev 1.39   24 Aug 1992 11:17:58   ADAM
 * More breathing stuff.
 * 
 *    Rev 1.38   23 Aug 1992 17:04:54   FRED
 * Support for endcaps.
 * 
 * 
 *    Rev 1.37   23 Aug 1992 15:07:12   ADAM
 * Breathing stuff.
 * 
 *    Rev 1.36   18 Aug 1992 18:50:38   MACDUFF
 * Support for global _Precision
 * 
 *    Rev 1.35   18 Aug 1992 09:57:28   MACDUFF
 * Uses _PRECISION
 * 
 *    Rev 1.34   16 Aug 1992 14:57:36   FRED
 * Removed "static" from aulMask[] definition so it can be
 * shared with OUTLINE.C
 * 
 * 
 *    Rev 1.33   16 Aug 1992 14:17:52   FRED
 * Fixed bug where bounding box for background fonts
 * with a white pattern were wrong because the winding
 * of the single outline box forced the outline pen to
 * be stroked one pixel out from the true bounding box
 * of the glyph.
 * 
 * 
 *    Rev 1.32   14 Aug 1992 18:24:50   FRED
 * compute_eae_values() is no longer called by FindExtrema. Instead, the
 * call is made by fs_NewTransform(). FindExtrema() assumes the values
 * are already present in the Fx structure.
 * 
 * 
 *    Rev 1.31   14 Aug 1992 10:15:16   FRED
 * Calls set_background_pattern() to make background plaid work.
 * 
 * 
 *    Rev 1.30   13 Aug 1992 23:50:56   FRED
 * Changes to fix some bugs with pattern alignment.
 * 
 * 
 *    Rev 1.29   13 Aug 1992 17:17:22   FRED
 * Sets background pattern offset for vertical alignment.
 * 
 * 
 *    Rev 1.28   12 Aug 1992 11:31:56   FRED
 * Support for background effects.
 * 
 * FindExtrema calls compute_eae_values (EFFECTS.ASM).
 * 
 * 
 *    Rev 1.27   08 Aug 1992 16:16:12   FRED
 * Fixed syntax error.
 * 
 * 
 *    Rev 1.26   08 Aug 1992 16:10:04   FRED
 * Retabbed sc_ScanChar().
 * 
 * Implemented FX_FLAG_fill_stroke and FX_FLAG_stroke_fill.
 * 
 * Uses fx_pattern as fill pattern number. The client specified
 * value is in fx_fill_pattern and fs_ContourScan() initializes
 * fx_pattern as needed.
 * 
 * 
 *    Rev 1.25   07 Aug 1992 18:49:18   FRED
 * Uses new Fx field names.
 * 
 * 
 *    Rev 1.24   07 Aug 1992 18:28:12   FRED
 * Moved top level special effects code out of sc_ScanChar()
 * and into sc_CountourScan() in FSCALER.C
 * 
 * 
 *    Rev 1.23   07 Aug 1992 15:14:28   FRED
 * Uses outline specified on GlobalKey->outline.
 * 
 * 
 *    Rev 1.22   07 Aug 1992 13:38:42   FRED
 * Fixed bug with background filling.
 * 
 * 
 *    Rev 1.21   07 Aug 1992 11:35:58   FRED
 * Calls pattern functions with TrueType Y coordinate. The pattern
 * processors adjust it as needed.
 * 
 * 
 *    Rev 1.20   07 Aug 1992 10:58:22   FRED
 * Now properly initializes the bitmap to zeroes.
 * 
 * 
 *    Rev 1.19   07 Aug 1992 10:52:58   FRED
 * Sets "initialize buffer" flags.
 * 
 * 
 *    Rev 1.18   07 Aug 1992 10:32:52   FRED
 * Added fill_pattern_run() function which fills a run of pattern
 * data in the bitmap.
 * 
 * Looks at FX_FLAG_initialize_buffer and clears the bitmap to
 * zero if it is set.
 * 
 * Looks at FX_FLAG_fill_background and clears the bitmap using
 * the pattern specified in Fx.fx_background_pattern.
 * 
 * 
 *    Rev 1.17   06 Aug 1992 18:11:12   FRED
 * Uses new Fx structure.
 * 
 * 
 *    Rev 1.16   06 Aug 1992 13:31:44   FRED
 * Better pattern generation where pattern data is
 * loaded on demand. Function patterns are only called
 * for the bits that are used.
 * 
 * Bitmap data which is not part of the character is
 * not changed.
 * 
 * Row data is zeroed before being filled. In the future
 * this behavior will be tied to a flag.
 * 
 * 
 *    Rev 1.15   04 Aug 1992 15:42:32   FRED
 * Calls DrawOutline()
 * 
 *    Rev 1.14   01 Aug 1992 16:35:20   ADAM
 * Fixed a comment.
 * 
 *    Rev 1.13   31 Jul 1992 13:41:28   ADAM
 * Fixed comments for rev 1.11 & 1.12.
 * Corrected inverted special effects Y values in nwnfillRow.
 * 
 *    Rev 1.10   31 Jul 1992 12:42:18   ADAM
 * Made bitmap far.
 * 
 *    Rev 1.9   30 Jul 1992 17:56:26   FRED
 * Character BBOX is now passed to initializ_effects_variables().
 * 
 * 
 *    Rev 1.8   30 Jul 1992 11:32:02   FRED
 * Inverts Y coordinate for special effects generation.
 * 
 * 
 *    Rev 1.7   29 Jul 1992 16:43:08   FRED
 * Fixed bug where pattern data was not properly advanced
 * for mulitple row requests.
 * 
 * 
 *    Rev 1.6   29 Jul 1992 13:48:04   FRED
 * Uses new effects code for fill patterns.
 * 
 * 
 *    Rev 1.5   28 Jul 1992 09:52:18   ADAM
 * Made code revisions to match those that were shipped from Microsoft
 * on the 7/24/92 source diskette.
 * 
 *    Rev 1.4   14 Jul 1992 18:03:30   ADAM
 * fixed some comments
 * 
 *    Rev 1.3   13 Jul 1992 18:26:24   ADAM
 * 
 *    Rev 1.2   10 Jul 1992 18:59:38   ADAM
*/

/*
        File:           sc.c

        Contains:       xxx put contents here xxx

        Written by:     xxx put writers here xxx

        Copyright:      c 1990 by Apple Computer, Inc., all rights reserved.

        Change History (most recent first):

        <15>    06/27/91        AC     Put back the banding code
        <14>    05/13/91        AC     Rewrote sc_mark and sc_markRows
        <13>    12/20/90        RB     Add ZERO macro to include 0 degrees in definition of interior
*/

/*
 * File: sc.c
 *
 * This module scanconverts a shape defined by quadratic B-splines
 *
 * The BASS project scan converter sub ERS describes the workings of this module.
 *
 *
 *  c Apple Computer Inc. 1987, 1988, 1989, 1990.
 *
 * History:
 * Work on this module began in the fall of 1987.
 * Written June 14, 1988 by Sampo Kaasila.
 *
 * Released for alpha on January 31, 1989.
 * 
 * Added experimental non breaking scan-conversion feature, Jan 9, 1990. ---Sampo
 * 
 */

#define multlong(a,b) SHORTMUL(a,b) /* ((a)*(b)) */

#include <stdio.h>

#include    "fscdefs.h"
#include    "fontmath.h"
#include    "sfnt.h"
#include    "fnt.h"
#include    "sc.h"
#include    "fserror.h"
#include    "fsglue.h"

#include		"effects.h"

extern void set_pattern(int16 pattern);
extern FX Fx;


#ifndef PRIVATE
#define PRIVATE 
#endif 

#ifdef FSCFG_BIG_ENDIAN
#define FSCFG_SC_NATIVE_ENDIAN_LONG_BMP
#endif 

#ifndef FSCFG_SC_NATIVE_ENDIAN_LONG_BMP

static uint32 aulInvPixMask[32]
#ifdef PC_OS
= 
{
  0x00000080L, 0x00000040L, 0x00000020L, 0x00000010L,
  0x00000008L, 0x00000004L, 0x00000002L, 0x00000001L,
  0x00008000L, 0x00004000L, 0x00002000L, 0x00001000L,
  0x00000800L, 0x00000400L, 0x00000200L, 0x00000100L,
  0x00800000L, 0x00400000L, 0x00200000L, 0x00100000L,
  0x00080000L, 0x00040000L, 0x00020000L, 0x00010000L,
  0x80000000L, 0x40000000L, 0x20000000L, 0x10000000L,
  0x08000000L, 0x04000000L, 0x02000000L, 0x01000000L
}
#endif 
;

#define MASK_INVPIX(mask,val)   mask = aulInvPixMask[val]

#else

#define MASK_INVPIX(mask,val)   mask = ((uint32)0x80000000L) >> (val)

#endif


#ifdef PC_OS									// Windows uses compile-time initialization
#define SETUP_MASKS()
#endif 

#ifdef FSCFG_SC_NATIVE_ENDIAN_LONG_BMP	// Use runtime shifts, not masks
#define SETUP_MASKS()
#endif 

#ifndef SETUP_MASKS							// General case -- works on any CPU
#define SETUP_MASKS_NEEDED
#define SETUP_MASKS()  if (!fMasksSetUp) SetUpMasks(); else
static void SetUpMasks (void);
static int fMasksSetUp = 0;
#endif 


//----------------------------------------------------------------------------
// define "static" data that will be needed by the DDAs. When FSCFG_REENTRANT
// is not defined, this is made static so that parameters will not have to
// be passed around.  When reentrancy is required, we declare the locals as an
// auto variable and pass a pointer to this auto wherever we need it.
//-----------------------------------------------------------------------------

struct scLocalData
{
	int32 jx, jy, endx, endy, **px, **py;
	int32 wideX, wideY;
	int32 incX, incY;
	int32 **xBase, **yBase;
	int32 marktype;
	int32 **lowRowP, **highRowP;
	int32 r;
};

#ifdef FSCFG_REENTRANT
#define SCP0    struct scLocalData* pLocalSC
#define SCP     struct scLocalData* pLocalSC,
#define SCA0    pLocalSC
#define SCA     pLocalSC,
#define LocalSC (*pLocalSC)
#else 
#define SCP0    void
#define SCP
#define SCA0
#define SCA

struct scLocalData LocalSC;

#endif

// Private prototypes
                         
PRIVATE int sc_mark(SCP F26Dot6 *pntbx, F26Dot6 *pntby, F26Dot6 *pntx, F26Dot6 *pnty, F26Dot6 *pnte, sc_BitMapData *bbox = NULL);
PRIVATE void sortRows(sc_BitMapData *bbox, int32**lowRowP, int32**highRowP);
PRIVATE void sortCols(sc_BitMapData *bbox);
PRIVATE int sc_DrawParabola(F26Dot6 _Ax, F26Dot6 _Ay, F26Dot6 _Bx, F26Dot6 _By, F26Dot6 _Cx, F26Dot6 _Cy, F26Dot6** hX, F26Dot6** hY, uint32* count, int32 inGY);
PRIVATE void sc_wnNrowFill(int32 rowM, int32 nRows, sc_BitMapData *bbox);
PRIVATE void sc_orSomeBits(sc_BitMapData *bbox, int32 scanKind);
PRIVATE int32** sc_lineInit(int32* arrayBase, int32** rowBase, int32 nScanlines, int32 maxCrossings, int32 minScanline);
PRIVATE int32 nOnOff(int32** base, int32 k, int32 val, int32 nChanges);
PRIVATE int32 nUpperXings(int32** lineBase, int32 **valBase, int32 line, int32 val, int32 lineChanges, int32 valChanges, int32 valMin, int32 valMax, int32 lineMax);
PRIVATE int32 nLowerXings(int32** lineBase, int32 **valBase, int32 line, int32 val, int32 lineChanges, int32 valChanges, int32 valMin, int32 valMax, int32 lineMin);
PRIVATE void invpixSegY(int32 llx, uint32 k, uint32* bitmapP);
PRIVATE void invpixSegX(int32 llx, uint32 k, uint32* bitmapP);
PRIVATE void invpixOn(int32 llx, uint32 k, uint32* bitmapP);

PRIVATE void DDA_1_XY(SCP0);
PRIVATE void DDA_2_XY(SCP0);
PRIVATE void DDA_3_XY(SCP0);
PRIVATE void DDA_4_XY(SCP0);
PRIVATE void DDA_1_Y(SCP0);
PRIVATE void DDA_2_Y(SCP0);
PRIVATE void DDA_3_Y(SCP0);
PRIVATE void DDA_4_Y(SCP0);

void fill_pattern_run(int32 x, int32 y, int32 l);

extern int DrawOutline(sc_CharDataType *glyphPtr, sc_GlobalData *scPtr, int32 lowBand, int32 highBand);

// CAUTION. The value of DO_STUBS is chosen to be 5 because the DDA routines
// that are used when stub crossings are needed start after 5 addresses in
// the DDA jump table. PLEASE BE VERY CAREFUL WHEN CHANGING THIS VALUE.

#define DO_STUBS    5           

#if 0
void ValidateData(sc_BitMapData *bbox)
{
	int fValid = 1;

	int32 scanline;
	int32 nIntOn, nIntOff;
	int32 *rowPt, *pOn, *pOff;
	int32 incY, incX;

	int32 ymin = bbox->bounds.yMin;
	int32 ymax = bbox->bounds.yMax - 1;
	int32 xmin = bbox->bounds.xMin;
	int32 xmax = bbox->bounds.xMax - 1;
	int32** xBase = bbox->xBase;
	int32** yBase = bbox->yBase;

	incY = bbox->nYchanges + 2;
	incX = bbox->nXchanges + 2;

	// First do Y scanlines
	rowPt = *(yBase + ymin);
	for (scanline = ymin; scanline <= ymax; ++scanline)
	{
		nIntOn = *rowPt;
		nIntOff = *(rowPt + incY - 1);
		pOn = rowPt + 1;
		pOff = rowPt + incY - 1 - nIntOff;
		if ((nIntOn < 0)
		 || (nIntOn > (int32)bbox->nYchanges)
		 || (nIntOff < 0)
		 || (nIntOff > (int32)bbox->nYchanges)
		 || (nIntOn+nIntOff > (int32)bbox->nYchanges))
		{
			fValid = 0;
			_asm
			{
				int 3
			}
			break;
		}
		rowPt += incY;
	}

	// Next do X scanlines
	rowPt = *(xBase + xmin);
	for (scanline = xmin ; scanline <= xmax; ++scanline)
	{
		nIntOn = *rowPt;
		nIntOff = *(rowPt + incX - 1);
		pOn = rowPt + 1;                              
		pOff = rowPt + incX - 1 - nIntOff;
		if ((nIntOn < 0)
		 || (nIntOn > (int32)bbox->nXchanges)
		 || (nIntOff < 0)
		 || (nIntOff > (int32)bbox->nXchanges)
		 || (nIntOn+nIntOff > (int32)bbox->nXchanges))
		{
			fValid = 0;
			_asm
			{
				int 3
			}
			break;
		}
		rowPt += incX;
	}
}
#endif

// This is the top level call to the scan converter.
// It returns the bitmap
//
// Assumes that (*handle)->bbox.xmin,...xmax,...ymin,...ymax
// are already set by sc_FindExtrema ()
//
// PARAMETERS:
//
// glyphPtr is a pointer to sc_CharDataType
// scPtr is a pointer to sc_GlobalData.
// lowBand   is lowest scan line to be included in the band.
// highBand  is one greater than the highest scan line to be included in the band. <7>
// scanKind contains flags that specify whether to do dropout control and what kind
//      0 -> no dropout control
//      bits 0-15 not equal 0 -> do dropout control
//      if bit 16 is also on, do not do dropout control on 'stubs' 

int sc_ScanChar (sc_CharDataType *glyphPtr, sc_GlobalData *scPtr, sc_BitMapData *bbox, int32 lowBand, int32 highBand, int32 scanKind, F26Dot6 XOffset, F26Dot6 YOffset)
{
	// If the FX_FLAG_stroke_fill flag is set, then we want to stroke
	// the path before filling it.

	if ((Fx.fx_flags & FX_FLAG_stroke_fill) != 0)
	{
		DrawOutline(glyphPtr, scPtr, lowBand, highBand);
	}

	// If there is a fill pattern, then fill the path.

	set_pattern(Fx.fx_pattern);

	if (Fx.fx_pattern_processor != 0)
	{
		register F26Dot6 *x = glyphPtr->x;
		register F26Dot6 *y = glyphPtr->y;
		register ArrayIndex i, endPt, nextPt;
		register uint8 *onCurve = glyphPtr->onCurve;
		ArrayIndex startPt, j;
		LoopCount ctr;
		sc_GlobalData *p;
		F26Dot6 *xp, *yp, *x0p, *y0p;
		register F26Dot6 xx, yy, xx0, yy0;
		int quit;
		uint32 vecCount;
#ifdef FSCFG_REENTRANT
		struct scLocalData thisLocalSC;
		struct scLocalData* pLocalSC = &thisLocalSC;
#endif 

		SETUP_MASKS();

#ifdef FSCFG_NO_BANDING

		bbox->yBase = sc_lineInit(bbox->yLines, bbox->yBase, bbox->bounds.yMax - bbox->bounds.yMin, bbox->nYchanges, bbox->bounds.yMin);

		if (scanKind)
		{
			bbox->xBase = sc_lineInit(bbox->xLines, bbox->xBase, bbox->bounds.xMax - bbox->bounds.xMin, bbox->nXchanges, bbox->bounds.xMin);
		}

#else

		if (scanKind)
		{
			bbox->xBase = sc_lineInit(bbox->xLines, bbox->xBase, bbox->bounds.xMax - bbox->bounds.xMin, bbox->nXchanges, bbox->bounds.xMin);
			bbox->yBase = sc_lineInit(bbox->yLines, bbox->yBase, bbox->bounds.yMax - bbox->bounds.yMin, bbox->nYchanges, bbox->bounds.yMin);
		}
		else
		{
			bbox->yBase = sc_lineInit(bbox->yLines, bbox->yBase, highBand - lowBand, bbox->nYchanges, lowBand);
		}

#endif

		// At this time set up LocalSC.yBase, LocalSC.xBase, LocalSC.marktype,
		// LocalSC.wideX and LocalSC.wideY in static variables. This will save
		// us from passing these parameters to sc_mark.

		LocalSC.yBase = bbox->yBase;
		LocalSC.xBase = bbox->xBase;
		LocalSC.marktype = (scanKind > 0) ? DO_STUBS : 0;		// just a boolean value
		LocalSC.wideX = bbox->nXchanges + 1;
		LocalSC.wideY = bbox->nYchanges + 1;
		LocalSC.lowRowP = bbox->yBase + lowBand;
		LocalSC.highRowP = bbox->yBase + highBand - 1;

		if (glyphPtr->nc == 0)
		{
			goto scanchar_fill_done;
		}

		p = scPtr;

		// Loop through each contour.
		for (ctr = 0; ctr < glyphPtr->nc; ctr++)
		{
			x0p = xp = p->xPoints;
			y0p = yp = p->yPoints;
			startPt = i = glyphPtr->sp[ctr];
			endPt = glyphPtr->ep[ctr];

			if (startPt == endPt)
			{
				continue;
			}

			quit = 0;
			vecCount = 1;

			// Generate the starting point.
			if (onCurve[i] & ONCURVE) 
			{
				// Start point is oncurve.
				*xp++ = xx = x[i]+XOffset;
				*yp++ = yy = y[i++]+YOffset;
			}

			else 
			{
				// Start point is not oncurve, check if end point is oncurve.
				if (onCurve[endPt] & ONCURVE) 
				{
					// End point is oncurve, start point is not. In this case,
					// back up one point on the contour. Note that "i" remains the
					// index of the old starting point which is the index of the
					// offcurve control point for the first spline.
					startPt = endPt--;
					*xp++ = xx = x[startPt]+XOffset;
					*yp++ = yy = y[startPt]+YOffset;
				}
				else 
				{
					// Both start and end points are not oncurve, generate midpoint to
					// use for "oncurve" starting point. Note that "i" remains as the
					// index of the start point which is the index of the offcurve
					// control point for the first spline.
					*xp++ = xx = (F26Dot6)(((long) x[i]+XOffset + x[endPt]+XOffset + 1) >> 1);
					*yp++ = yy = (F26Dot6)(((long) y[i]+YOffset + y[endPt]+YOffset + 1) >> 1);

					// ***** Jump into the "offcurve" part of the loop below *****
					goto Offcurve;
				}
			}

			// We have initialized the scan array with the starting point, now
			// process the following points in the contour.
			while (true) 
			{
				// Process a run of oncurve points. This is basically all the lines.
				// These are simply copied from the curve data into the vector arrays.
				while (onCurve[i] & ONCURVE)
				{
					// Check if vector array is full (note that vecCount is incremented.)
					if (++vecCount > MAXVECTORS)
					{
						// Ran out of local memory. Consume data and continue.
						if (!sc_mark (SCA x0p, y0p, x0p+1, y0p+1, yp-1))
						{
							return SCAN_ERR;
						}

						x0p = p->xPoints + 2;	// save data in points 0 and 1 (first vector) for final mark
						y0p = p->yPoints + 2;
						*x0p++ = *(xp - 2);		// save last vector to be future previous vector
						*x0p++ = *(xp - 1);
						*y0p++ = *(yp - 2);
						*y0p++ = *(yp - 1);
						xp = x0p;					// start next processing with last vector
						x0p = p->xPoints + 2;
						yp = y0p;
						y0p = p->yPoints + 2;
						vecCount = 5;				// (4 + 1 for next point)
					}

					// Save current oncurve point.
					*xp++ = xx = x[i]+XOffset;
					*yp++ = yy = y[i]+YOffset;

					if (quit) 
					{
						// Done with all points.
						goto sc_exit;
					}
					else 
					{
						// Move to next point (wrap around in contour if necessary.)
						i = i == endPt ? quit = 1, startPt : i + 1;
					}
				}

				// Process a run of offcurve points. Basically, we have one or more
				// quadratic (3 point) splines to process using sc_DrawParabola.
				do 
				{
	Offcurve:
					// We are going to draw a quadratic spline defined by three points:
					// two endpoints and one control point.

					// Save first endpoint for spline.
					xx0 = xx;
					yy0 = yy;

					// Save index of offcurve control point (i).
					j = i;

					// Compute the index of the next point. Wrap around the contour if necessary.
					nextPt = i == endPt ? quit = 1, startPt : i + 1;

					// Check if the next point is on the curve. If so, it is the second endpoint.
					// If not, then we'll have to compute it.
					if (onCurve[nextPt] & ONCURVE) 
					{
						// Next point is oncurve, use it as the second endpoint.
						xx = x[nextPt]+XOffset;
						yy = y[nextPt]+YOffset;
						i = nextPt;
					}
					else 
					{
						// The next point is offcurve, so we use the midpoint of the offcurve
						// control point for this spline and the next offcurve point as the
						// second endpoint for the spline. Note that "i" is not changed since it
						// is the index of the offcurve control point for the next spline.
						xx = (F26Dot6)(((long) x[i]+XOffset + x[nextPt]+XOffset + 1) >> 1);
						yy = (F26Dot6)(((long) y[i]+YOffset + y[nextPt]+YOffset + 1) >> 1);
					}

					// Generate the curve vectors into the current vector array.
					if (sc_DrawParabola(xx0, yy0, x[j]+XOffset, y[j]+YOffset, xx, yy, &xp, &yp, &vecCount, -1L))
					{
						// Not enough room to create parabola vectors. Consume current
						// vectors and try again.
						if (!sc_mark(SCA x0p, y0p, x0p+1, y0p+1, yp-1, bbox))
						{
							return SCAN_ERR;
						}

						x0p = p->xPoints + 2;	// save data in points 0 and 1 (first vector) for final mark
						y0p = p->yPoints + 2;
						*x0p++ = *(xp - 2);		// save last vector to be future previous vector
						*x0p++ = *(xp - 1);
						*y0p++ = *(yp - 2);
						*y0p++ = *(yp - 1);
						xp = x0p;					// start next processing with last vector
						x0p = p->xPoints + 2;
						yp = y0p;
						y0p = p->yPoints + 2;
						vecCount = 5;				// (4 + 1 for next point)

						// recaptured some memory, try again, if still wont work, MAXVEC is too small
						if (sc_DrawParabola(xx0, yy0, x[j]+XOffset, y[j]+YOffset, xx, yy, &xp, &yp, &vecCount, -1L))
						{
							return (SCAN_ERR);
						}
					}

					if (quit) 
					{
						// Done with all points.
						goto sc_exit;
					}

					else 
					{
						// Move to next point (wrap around in contour if necessary.)
						i = i == endPt ? quit = 1, startPt : i + 1;
					}
				} while (!(onCurve[i] & ONCURVE));
			}

	sc_exit:

			// Mark the accumulated vectors.
			if (!sc_mark(SCA x0p, y0p, x0p+1, y0p+1, yp-1, bbox))
			{
				return (SCAN_ERR);
			}

			// Mark the first vector using the last vector as the "previous".
			if (!sc_mark(SCA xp-2, yp-2, p->xPoints, p->yPoints, p->yPoints+1, bbox))
			{
				return (SCAN_ERR);
			}
		} // Contour loop

		// All the contours have been scan converted into the crossing arrays.

		// Sort the row crossings.
		sortRows(bbox, LocalSC.lowRowP, LocalSC.highRowP);

		// If we are doing drop out control, sort the column data (not generated if no dropout control.)
		if (scanKind)
		{
			sortCols(bbox);
		}

		// Take care of problem of very small thin glyphs - always fill at
		// least one pixel. Should this only be turned on if dropout control??

		if (LocalSC.highRowP < LocalSC.lowRowP)
		{
			register int32* p = *LocalSC.lowRowP;
			register int32* s = p + bbox->nYchanges + 1;

			++*p; 
			*(p + *p) = bbox->bounds.xMin;
			++*s; 
			*(s - *s) = bbox->bounds.xMax == bbox->bounds.xMin ? bbox->bounds.xMin + 1 : bbox->bounds.xMax;
			highBand = lowBand + 1;
		}
		else if (bbox->bounds.xMin == bbox->bounds.xMax)
		{
			register int32* p;
			register int32 inc = bbox->nYchanges;

			for (p = *LocalSC.lowRowP; p <= *LocalSC.highRowP; p += inc + 1)
			{
				*p = 1;
				*(p + inc) = bbox->bounds.xMin + 1;
				*(++p) = bbox->bounds.xMin;
				*(p + inc) = 1;
			}
		}

		// Fill the bitmap using the crossing data.
		sc_wnNrowFill(lowBand, highBand - lowBand, bbox);

		// If we're doing dropout control, fix any dropouts.
		if (scanKind)
		{
			sc_orSomeBits(bbox, scanKind);
		}
	}

scanchar_fill_done:

	// If the FX_FLAG_fill_stroke flag is set, then we want to stroke
	// the path after filling it.

	if ((Fx.fx_flags & FX_FLAG_fill_stroke) != 0)
	{
		DrawOutline(glyphPtr, scPtr, lowBand, highBand);
	}

	return (NO_ERR);
}

void sc_initialize_buffer(sc_BitMapData *bbox, int32 lowBand, int32 highBand)
{
	memset(bbox->bitMap, 0, (size_t)((bbox->wide >> 3)*(highBand-lowBand)));
}

// rwb 11/29/90 - modify the old positive winding number fill to be 
// a non-zero winding number fill to be compatible with skia, postscript,
// and our documentation.

#define LARGENUM		0x7fff 
#define NEGONE			((uint32)0xFFFFFFFFL)

#ifndef  FSCFG_SC_NATIVE_ENDIAN_LONG_BMP
uint32 aulMask [32]
#ifdef PC_OS
  =
  {
    0xffffffffL, 0xffffff7fL, 0xffffff3fL, 0xffffff1fL,
    0xffffff0fL, 0xffffff07L, 0xffffff03L, 0xffffff01L,
    0xffffff00L, 0xffff7f00L, 0xffff3f00L, 0xffff1f00L,
    0xffff0f00L, 0xffff0700L, 0xffff0300L, 0xffff0100L,
    0xffff0000L, 0xff7f0000L, 0xff3f0000L, 0xff1f0000L,
    0xff0f0000L, 0xff070000L, 0xff030000L, 0xff010000L,
    0xff000000L, 0x7f000000L, 0x3f000000L, 0x1f000000L,
    0x0f000000L, 0x07000000L, 0x03000000L, 0x01000000L
  }
#endif 
  ;
#define MASK_ON(x)  aulMask[x]
#define MASK_OFF(x) ~aulMask[32-(x)]

#else

#define MASK_ON(x)  (NEGONE >> (x))
#define MASK_OFF(x) (NEGONE << (x))

#endif

// x is pixel position, where 0 is leftmost pixel in scanline. 
// if x is not in the long pointed at by row, set row to the value of
// temp, clear temp, and clear all longs up to the one containing x.
// Then set the bits from x mod 32 through 31 in temp.

#define CLEARUpToAndSetLoOrder(x, lastBit, row, temp)						\
{																							\
	if (x >= lastBit)																	\
	{																						\
		uint32 skip;																	\
																							\
		*row &= ~temp;																	\
		*row |= get_pattern_data(temp, lastBit-32, y);						\
																							\
		skip = (x-lastBit)/32+1;													\
		row += skip;																	\
		lastBit += skip*32;															\
		temp = 0;																		\
	}																						\
																							\
	temp |= MASK_ON(32 + x - lastBit);											\
}

// x is pixel position, where 0 is leftmost pixel in scanline. 
// if x is not in the long pointed at by row, set row to the value
// of temp, set all bits in temp, and set all bits in all longs up
// to the one containing x. Then clear the bits from x mod 32
// through 31 in temp.

#define SETUpToAndClearLoOrder(x, lastBit, row, temp)						\
{																							\
	if (x >= lastBit)																	\
	{																						\
		*row &= ~temp;																	\
		*row |= get_pattern_data(temp, lastBit-32, y);						\
																							\
		row++;																			\
		lastBit += 32;																	\
																							\
		while (x >= lastBit)															\
		{																					\
			*row = get_pattern_data(0xffffffffL, lastBit-32, y);			\
																							\
			row++;																		\
			lastBit += 32;																\
		}																					\
																							\
		temp = NEGONE;																	\
	}																						\
																							\
	temp &= MASK_OFF(lastBit - x);												\
}

// Winding number fill of nRows of a glyph beginning at rowM,
// using two sorted arrays of onTransitions and offTransitions. 

PRIVATE void sc_wnNrowFill(int32 rowM, int32 nRows, sc_BitMapData *bbox)
{
	uint32   longsWide = bbox->wide >> 5;
	uint32   lineWide  = bbox->nYchanges + 2;
	uint32*  rowB      = bbox->bitMap;
	int32*   lineB     = * (bbox->yBase + rowM + nRows - 1);
	int32    xMin      = bbox->bounds.xMin;
	int32		y;

	y = rowM+nRows-1;

	while (nRows-- > 0)
	{
		uint32* row = rowB;
		int32* line = lineB;

		// The old FILLONEROW (row, longsWide, line, lineWide, xMin) macro
		// was too big for some compilers, so we expand its contents here.

		{
			register int32 moreOns, moreOffs;
			register int32 *onTp, *offTp;
			uint32 temp;
			uint32* rowEnd = row + longsWide;
			int32 on, off, x, lastBit;
			int windNbr, stop;

			lastBit  = 32 + xMin;
			windNbr  = 0;
			temp     = 0;
			moreOns  = *line;
			onTp     = line+1;
			offTp    = line + lineWide - 1;
			moreOffs = *offTp;
			offTp   -= moreOffs;

			on  = *onTp;
			off = *offTp;

			while (moreOns || moreOffs)
			{
				stop = 0;

				if (on < off)
				{
					x    = on;
					stop = 1;
				}

				if (on > off)
				{
					x    = off;
					stop = -1;
				}

				if (stop)
				{
					windNbr += stop;

					if (windNbr == stop)
					{
						CLEARUpToAndSetLoOrder(x, lastBit, row, temp)
					}

					else
					{
						if (windNbr == 0)
						{
							SETUpToAndClearLoOrder(x, lastBit, row, temp)
						}
					}
				}

				if (stop >= 0)
				{
					--moreOns;

					if (moreOns)
						on = *(++onTp);
					else
						on = LARGENUM;
				}

				if (stop <= 0)
				{
					--moreOffs;

					if (moreOffs)
						off = *(++offTp);
					else
						off = LARGENUM;
				}
			}

			if (row < rowEnd)
			{
				*row &= ~temp;
				*row |= get_pattern_data(temp, lastBit-32, y);
			}
		}

		rowB  += longsWide;
		lineB -= lineWide;

		y--;
	}
}

#undef NEGONE


// Sort the values stored in locations pBeg to pBeg+nVal in ascending order

#define ISORT(pBeg, pVal)                                           \
{                                                                   \
	register int32 *pj = pBeg;                                       \
	register int32 nVal = *pVal - 2;                                 \
	for (; nVal >= 0; --nVal)                                        \
	{                                                                \
		register int32 v;                                             \
		register int32 *pk, *pi;                                      \
		                                                              \
		pk = pj;                                                      \
		pi = ++pj;                                                    \
		v = *pj;                                                      \
		while (*pk > v && pk >= pBeg)                                 \
			*pi-- = *pk--;                                             \
		*pi = v;                                                      \
	}                                                                \
}

// rwb 4/5/90 Sort OnTransition and OffTransitions in Xlines arrays

PRIVATE void sortCols(sc_BitMapData *bbox)
{
	register int32 nrows = bbox->bounds.xMax - bbox->bounds.xMin - 1;
	register int32 *p = bbox->xLines;
	register uint32 n = bbox->nXchanges + 1;
	
	for (; nrows >= 0; --nrows)
	{
		ISORT(p + 1, p);
		p += n;
		ISORT(p - *p, p);
		++p;
	}
}

// rwb 4/5/90 Sort OnTransition and OffTransitions in Ylines arrays

PRIVATE void sortRows(sc_BitMapData* bbox, int32** lowRowP, int32** highRowP)
{
	register uint32 n = bbox->nYchanges + 1;
	int32 *p, *pend;
	
	if (highRowP < lowRowP) 
		return;
	p = *lowRowP;
	pend = *highRowP;
	do
	{
		ISORT(p + 1, p);
		p += n;
		ISORT(p - *p, p);
		++p;
	} while (p <= pend);
}

// 4/4/90 Version that distinguishes between On transitions and Off transitions.
// 3/23/90 
//       A procedure to find and mark all of the scan lines (both row and column) that are
// crossed by a vector.  Many different cases must be considered according to the direction
// of the vector, whether it is vertical or slanted, etc.  In each case, the vector is first
// examined to see if it starts on a scan-line.  If so, special markings are made and the
// starting conditions are adjusted.  If the vector ends on a scan line, the ending 
// conditions must be adjusted.  Then the body of the case is done.
//       Special adjustments must be made when a vector starts or ends on a scan line. Whenever
// one vector starts on a scan line, the previous vector must have ended on a scan line.
// Generally, this should result in the line being marked as crossed only once (conceptually
// by the vector that starts on the line.  But, if the two lines form a vertex that
// includes the vertex in a colored region, the line should be marked twice.  If the 
// vertex is also on a perpendicular scan line, the marked scan line should be marked once
// on each side of the perpendicular line.  If the vertex defines a point that is jutting
// into a colored region, then the line should not be marked at all. In order to make
// these vertex crossing decisions, the previous vector must be examined.
//
//       Because many vectors are short with respect to the grid for small resolutions, the
// procedure first looks for simple cases in which no lines are crossed.
//
// xb, x0, and x1 are x coordinates of previous point, current point and next point
// similaryly yb, y0 and y1
//       ybase points to an array of pointers, each of which points to an array containing
// information about the glyph contour crossings of a horizontal scan-line.  The first
// entry in these arrays is the number of ON-transition crossings, followed by the y
// coordinates of each of those crossings.  The last entry in each array is the number of
// OFF-transtion crossings, preceded by the Y coordinates for each of these crossings.
//       LocalSC.xBase contains the same information for the column scan lines.

//-----------------------------------------------------------------------------
// define the DDA table.
//-----------------------------------------------------------------------------

#define DDA_MAX_FUNCTIONTABLE 10
void (* DDAFunctionTable [DDA_MAX_FUNCTIONTABLE])(SCP0) =
{ 
	DDA_1_Y, DDA_2_Y, DDA_3_Y, DDA_4_Y, DDA_4_Y, 
	DDA_1_XY, DDA_2_XY, DDA_3_XY, DDA_4_XY, DDA_4_XY
};

#define DROUND(a)			((a + HALFM) & INTPART)
#define RSH(a)				((a)>>PIXSHIFT)
#define LSH(a)				((a)<<PIXSHIFT)
#define LINE(a)			(!(((a) & FRACPART) ^ HALF))
#define SET(p,val)		{register int32 *row = *p; ++*row; *(row+*row)=val;}
#define OFFX(val)			{register int32 *s = *LocalSC.px+LocalSC.wideX; ++*s; *(s-*s) = val;}
#define OFFY(val)			{register int32 *s = *LocalSC.py+LocalSC.wideY; ++*s; *(s-*s) = val;}
#define BETWEEN(a,b,c)	(a < b && b < c )
#define EQUAL(a,b,c)		(a == b && a == c) 
#define EPSILON			0x1

//---------------------LINE ORIENTATION MACROS--------------------------------

#define F_Q1				0x0001
#define F_Q2				0x0002
#define F_Q3				0x0004
#define F_Q4				0x0008
#define F_INTERIOR		0x0010  
#define F_V_LINEAR		0x0020
#define F_H_LINEAR		0x0040
#define QUADRANT_BITS	(F_Q1|F_Q2|F_Q3|F_Q4)
#define TOP_TO_BOT		BETWEEN(y1,y0,yb)          
#define BOT_TO_TOP		BETWEEN(yb,y0,y1)
#define LFT_TO_RGHT		BETWEEN(xb,x0,x1)
#define RGHT_TO_LFT		BETWEEN(x1,x0,xb)
#define INTERIOR			(SCMR_Flags&F_INTERIOR)
#define HORIZ				(SCMR_Flags&F_V_LINEAR)
#define VERT				(SCMR_Flags&F_H_LINEAR)
#define QUAD_1OR2			(SCMR_Flags&(F_Q1|F_Q2))
#define QUAD_3OR4			(SCMR_Flags&(F_Q3|F_Q4))
#define QUAD_2OR3			(SCMR_Flags&(F_Q2|F_Q3))
#define QUAD_1OR4			(SCMR_Flags&(F_Q1|F_Q4))
#define QUAD_1				(SCMR_Flags&F_Q1)
#define QUAD_2				(SCMR_Flags&F_Q2)
#define QUAD_3				(SCMR_Flags&F_Q3)
#define QUAD_4				(SCMR_Flags&F_Q4)

PRIVATE int sc_mark(SCP F26Dot6 *pntbx, F26Dot6 *pntby, F26Dot6 *pntx, F26Dot6 *pnty, F26Dot6 *pnte, sc_BitMapData *bbox /*=NULL*/)
{
	int32 onrow, oncol, Shift;
	int32** pend;
	F26Dot6  x0, y0, x1, y1, xb, yb, rx0, ry0, rx1, ry1, dy, dx; 
	int32 rhi, rlo;

	//--------------------SET UP A FLAG BYTE AND EQUATES FOR IT------------------------

	register int SCMR_Flags;

	//---------------------------------------------------------------------------------
	// Except on the PC, this code supports banding when stub and drop out 
	// controls are not required. For these cases, return if the band will
	// not include any part of the glyph.
	//---------------------------------------------------------------------------------

#ifndef FSCFG_NO_BANDING
	if (LocalSC.marktype != DO_STUBS && LocalSC.highRowP < LocalSC.lowRowP)
		return 1;
#endif
 
	//---------------------------------------------------------------------------------
	// loop through all the points in the contour.
	//---------------------------------------------------------------------------------

	x0 = *pntbx;
	y0 = *pntby;
	x1 = *pntx++;
	y1 = *pnty++;

	// In side the loop, we will set (xb,yb) to (x0,y0) and (x0,y0) to (x1,y1).
	// For the very first pass through the loop, if (xb,yb) is = (x0,y0) and
	// (LINE y0) is true or we are doing stub_control and (LINE x0) is true, 
	// we will run into a degenerate case where we will have two instead of
	// 3 distinct points. Detect and fail this case. We should look into 
	// this after 3.1 ships.

	if ((x0 == x1) && (y0 == y1) && (LINE(y1) || (LocalSC.marktype == DO_STUBS && LINE(x1))))
		return 0;
		
	while (pnty <= pnte)
	{
		xb = x0;
		yb = y0;
		x0 = x1;
		y0 = y1;
		x1 = *pntx++;
		y1 = *pnty++;

		//-----------------------------------------------------------------------------
		// get the next set of points.
		//-----------------------------------------------------------------------------
		
		//-----------------------------------------------------------------------------
		// scan convert this line.
		//-----------------------------------------------------------------------------

		SCMR_Flags = 0;
		dy = y1 - y0;
		dx = x1 - x0;
		if (!dy && !dx)
		{
			// we have a point and will ignore this. However, for the next 
			// iteration we will not update (xb,yb) so that we don't get 
			// (xb,yb) = (x0,y0) and end up being with 2 instead of 3 
			// distinct points.

			x0 = xb;
			y0 = yb;
			continue;
		}

		rx0 = DROUND(x0);
		LocalSC.jx = RSH(rx0);
		ry0 = DROUND(y0);
		LocalSC.jy = RSH(ry0);
		rx1 = DROUND(x1);
		LocalSC.endx = RSH(rx1);
		ry1 = DROUND(y1);
		LocalSC.endy = RSH(ry1);
		LocalSC.py = LocalSC.yBase + LocalSC.jy;
		pend = LocalSC.yBase + LocalSC.endy;
		LocalSC.px = LocalSC.xBase + LocalSC.jx;
		onrow = false;
		oncol = false;

    	//------------------SET UP THE QUADRANT THAT THE LINE IS IN---------------------
    
		if (dx > 0 && dy >=0) SCMR_Flags |= F_Q1;
		else
			if (dx <= 0 && dy > 0)  SCMR_Flags |= F_Q2;
			else    
				if (dx < 0 && dy <= 0) SCMR_Flags |= F_Q3;
				else SCMR_Flags |= F_Q4;

		//------------------------------------------------------------------------------

		LocalSC.py = LocalSC.yBase + LocalSC.jy;
		LocalSC.px = LocalSC.xBase + LocalSC.jx;
    
		//------------------------------------------------------------------------------
		// for platforms where we do banding, we will set onrow and oncol only 
		// if the starting point is in the band.
		//------------------------------------------------------------------------------
		
#ifndef FSCFG_NO_BANDING
		if ((LocalSC.marktype == DO_STUBS)
		 || ((QUAD_1OR2 && LocalSC.py >= LocalSC.lowRowP)
		  || (QUAD_3OR4 && LocalSC.py <= LocalSC.highRowP)))
#endif
		{
			if LINE (y0) onrow = true;
			if LINE (x0) oncol = true;
		}

		//------------------------------------------------------------------------------
		// for the platforms where we do banding, find out if the band totaly 
		// excludes the current line.
		//------------------------------------------------------------------------------
#ifndef FSCFG_NO_BANDING
		if (LocalSC.marktype != DO_STUBS)
			if (QUAD_1OR2 && (LocalSC.py > LocalSC.highRowP || pend < LocalSC.lowRowP))
				continue;
			else if (QUAD_3OR4 && (LocalSC.py < LocalSC.lowRowP || pend > LocalSC.highRowP))
				continue;

#endif 
    
		//--------------------------------------------------------------------------------
		// compute some other flags.
		//--------------------------------------------------------------------------------
    
		if ((long)(x0-xb)*dy < (long)(y0-yb)*dx)
			SCMR_Flags |= F_INTERIOR ;
		if (EQUAL (yb, y0, y1))
			SCMR_Flags |= F_V_LINEAR ;
		if (EQUAL (xb, x0, x1))
			SCMR_Flags |= F_H_LINEAR ;
    
		//--------------------------------------------------------------------------------
		// Now handle the cases where the starting point falls on a row scan line
		// and maybe also on a coloumn scan line.
		//
		// first consider the intersections with row scan lines only. After this we
		// will consider the intersections with coloumn scan lines. It is not
		// worth while to set the coloumn intersections yet, because in any case we
		// will have to set the intersetions when a vertex does not lie on a row
		// but does lie on a coloumn.
		//--------------------------------------------------------------------------------
    
		Shift = 0;
		if (onrow) 
		{
			if (oncol) Shift = 1 ;
    
			if ((INTERIOR || VERT) && (((yb > y0) && QUAD_1OR2) || ((yb < y0) && QUAD_3OR4)))
			{
				SET(LocalSC.py, LocalSC.jx)
				OFFY(LocalSC.jx+Shift)
			}
			else
			{
				if ((INTERIOR && QUAD_1OR2) || BOT_TO_TOP || (HORIZ && (xb > x0) && QUAD_1))
				{
					SET(LocalSC.py, LocalSC.jx)
				}
				else
				{
					if ((INTERIOR && QUAD_3OR4) || TOP_TO_BOT || (HORIZ && (x0 > xb) && QUAD_3))
					{
						OFFY(LocalSC.jx+Shift)
					}
				}
			}
		}
		
		//------------------------------------------------------------------------------
		// now handle the coloumn intersections.
		//------------------------------------------------------------------------------
    
		Shift = 0 ;
		if (oncol && LocalSC.marktype == DO_STUBS)
		{
			if (onrow) Shift = 1 ;
    
			if ((INTERIOR || HORIZ) && (((xb > x0) && QUAD_1OR4) || ((xb < x0) && QUAD_2OR3)))
			{
				SET(LocalSC.px, LocalSC.jy)
				OFFX(LocalSC.jy+Shift)
			}
			else
			{
				if ((INTERIOR && QUAD_2OR3) || RGHT_TO_LFT || (VERT && (yb > y0) && QUAD_2))
				{
					SET(LocalSC.px, LocalSC.jy)
				}
				else
				{
					if ((INTERIOR && QUAD_1OR4) || LFT_TO_RGHT || (VERT && (y0 > yb) && QUAD_4))
					{
						OFFX(LocalSC.jy+Shift)
					}
				}
			}
		}
		
		//------------------------------------------------------------------------------
		// Now handle horizontal and vertical lines.
		//------------------------------------------------------------------------------
    
		//-----------------------horizontal line----------------------------------------
    
		if (LocalSC.endy == LocalSC.jy)
		{
			if (LocalSC.marktype != DO_STUBS)
			{
				continue;
			}
			else
			{
				if (QUAD_2OR3)
				{
					if LINE(x1)
					{
						++LocalSC.endx;
					}
					pend = LocalSC.xBase + LocalSC.endx;
					--LocalSC.px;
					while (LocalSC.px >= pend)
					{   
						SET(LocalSC.px, LocalSC.jy)
						--LocalSC.px;
					}
					continue;
				}
				else
				{
					if (onrow && QUAD_1)
					{
						++LocalSC.jy;
					}
					if (oncol)
					{
						++LocalSC.px;
						pend = LocalSC.xBase + LocalSC.endx;
						while (LocalSC.px < pend)
						{
							OFFX(LocalSC.jy)
							++LocalSC.px;
						}
						continue;
					}
				}
			}
		}
		
		//-----------------------vertical line------------------------------------------
    
		if (LocalSC.endx == LocalSC.jx)
		{
			if (QUAD_1OR2)
			{
				pend = LocalSC.yBase + LocalSC.endy;
				
				//------------------------------------------------------------------------
				// adjust the ending condition when banding is being done.
				//------------------------------------------------------------------------
            
#ifndef FSCFG_NO_BANDING
				if (LocalSC.marktype != DO_STUBS && pend > LocalSC.highRowP)
				{
					pend = LocalSC.highRowP + 1;
				}
#endif
				//------------------------------------------------------------------------

				if (onrow)
				{
					++LocalSC.py;
				}
				while (LocalSC.py < pend)							// note oncol can't be true
				{
					//---------------------------------------------------------------------
					// take care of banding when we support it
					//---------------------------------------------------------------------

#ifndef FSCFG_NO_BANDING 
					if (LocalSC.py >= LocalSC.lowRowP)
					{
						SET(LocalSC.py, LocalSC.jx)
					}
#else
					SET(LocalSC.py, LocalSC.jx)
#endif
					++LocalSC.py;
				}
				continue;
			}
			else
			{
				if (QUAD_4 && oncol)
				{
					++LocalSC.jx;
				}
				if LINE (y1)
				{
					++LocalSC.endy;
				}
				pend = LocalSC.yBase + LocalSC.endy;

				//------------------------------------------------------------------------
				// adjust the ending condition when banding is being done.
				//------------------------------------------------------------------------
				
#ifndef FSCFG_NO_BANDING
				if (LocalSC.marktype != DO_STUBS && pend < LocalSC.lowRowP)
					pend = LocalSC.lowRowP;
#endif
				//------------------------------------------------------------------------

				--LocalSC.py;
				while (LocalSC.py >= pend)
				{
                //--------------------------------------------------------------------
                // take care of banding when we support it
                //--------------------------------------------------------------------

#ifndef FSCFG_NO_BANDING 
					if (LocalSC.py <= LocalSC.highRowP)
					{
						OFFY(LocalSC.jx)
					}
#else
						OFFY(LocalSC.jx)
#endif
					--LocalSC.py;
				}
				continue ;
			}
		}
		
		//------------------SET UP INITIAL CONDITIONS FOR THE DDA-----------------------
    
		if (QUAD_1OR2)
		{
			LocalSC.incY = LSH(dy);
    
			if (!onrow)
			{
				rhi = multlong((ry0 - y0 + HALF), dx);
			}
			else
			{
				rhi = LSH(dx);
				++LocalSC.jy;
				++LocalSC.py;
			}
    
			if (QUAD_1)
			{
				LocalSC.incX = LSH(dx) ;
				if (!oncol)
				{
					rlo = multlong((rx0 - x0 + HALF), dy);
				}
				else
				{
					rlo = LocalSC.incY;
					++LocalSC.jx;
					++LocalSC.px;
				}
				    
				LocalSC.r = rhi - rlo;
			}
			else					// 2nd quadrant
			{
				LocalSC.incX = -LSH(dx);
				if (!oncol)
				{
					rlo = multlong((rx0 - x0 - HALF), dy);
				}
				else
				{
					rlo = -LocalSC.incY;
				}
				if LINE(x1)
				{
					++LocalSC.endx;
				}
				    
				LocalSC.r = rlo - rhi + EPSILON;
			}
		}
		else						// 3rd and 4th Quadrants
		{
			LocalSC.incY = -LSH(dy);
				    
			if (!onrow)
			{
				rhi = multlong((ry0 - y0 - HALF), dx);
			}
			else
			{
				rhi = -LSH(dx);
			}
				    
			if (QUAD_3)
			{
				LocalSC.incX = -LSH(dx);
				if (!oncol)
				{
					rlo = multlong((rx0 - x0 - HALF), dy);
				}
				else
				{
					rlo = LocalSC.incY;
				}
				    
				if LINE (y1) ++LocalSC.endy;
				if LINE (x1) ++LocalSC.endx;
				    
				LocalSC.r = rhi - rlo;
			}
			else					// 4th quadrant
			{
				LocalSC.incX = LSH(dx);
				if (!oncol)
				{
					rlo = multlong((rx0 - x0 + HALF), dy);
				}
				else
				{
					rlo = -LocalSC.incY;
					LocalSC.jx++;
					LocalSC.px++;
				}
				    
				if LINE (y1) ++LocalSC.endy;
				LocalSC.r = rlo - rhi + EPSILON;
			}
		}
		
		//------------------------------------------------------------------------------
		// set up the address of the DDA function and call it.
		//------------------------------------------------------------------------------
    
		(*DDAFunctionTable[((SCMR_Flags & QUADRANT_BITS) >> 1) + LocalSC.marktype])(SCA0);

		//------------------------------------------------------------------------------
		// go on to the next line.
		//------------------------------------------------------------------------------
	}
	
	return 1;
}

//-------------------END OF SC_MARK-------------------------------------------
    
//----------------------------------------------------------------------------
// DDA for 1st quadrant with markings for x and y scan lines
//----------------------------------------------------------------------------

void DDA_1_XY(SCP0)
{
	for (;;)
	{
		if (LocalSC.r > 0)
		{
			if (LocalSC.jx == LocalSC.endx) break;
			OFFX(LocalSC.jy)  
			LocalSC.px++;
			LocalSC.jx++;
			LocalSC.r -= LocalSC.incY;
		}
		else
		{
			if (LocalSC.jy == LocalSC.endy) break;
			SET(LocalSC.py, LocalSC.jx) 
			LocalSC.jy++;
			LocalSC.py++;
			LocalSC.r += LocalSC.incX;
		}           
	}
}

//----------------------------------------------------------------------------
// DDA for 2nd quadrant with markings for x and y scan lines
//----------------------------------------------------------------------------

void DDA_2_XY(SCP0)
{
	for (;;)
	{
		if (LocalSC.r > 0)
		{
			if (LocalSC.jx == LocalSC.endx) break;
			--LocalSC.jx;
			--LocalSC.px;
			SET(LocalSC.px, LocalSC.jy)
			LocalSC.r -= LocalSC.incY;
		}
		else
		{
			if (LocalSC.jy == LocalSC.endy) break;
			SET(LocalSC.py, LocalSC.jx) 
			LocalSC.jy++;
			LocalSC.py++;
			LocalSC.r += LocalSC.incX;
		}          
	}
}

//----------------------------------------------------------------------------
// DDA for 3rd quadrant with markings for x and y scan lines
//----------------------------------------------------------------------------

void DDA_3_XY(SCP0)
{
	for (;;)
	{
		if (LocalSC.r > 0)
		{
			if (LocalSC.jx == LocalSC.endx) break;
			--LocalSC.jx;
			--LocalSC.px;
			SET(LocalSC.px, LocalSC.jy)
			LocalSC.r -= LocalSC.incY;
		}
		else
		{
			if (LocalSC.jy == LocalSC.endy) break;
			LocalSC.jy--;
			LocalSC.py--;
			OFFY(LocalSC.jx);
			LocalSC.r += LocalSC.incX;
		}          
	}
}

//----------------------------------------------------------------------------
// DDA for 4th quadrant with markings for x and y scan lines
//----------------------------------------------------------------------------

void DDA_4_XY(SCP0)
{
	for (;;)
	{
		if (LocalSC.r > 0)
		{
			if (LocalSC.jx == LocalSC.endx) break;
			OFFX(LocalSC.jy)
			LocalSC.px++;
			LocalSC.jx++;
			LocalSC.r -= LocalSC.incY;
		}
		else
		{
			if (LocalSC.jy == LocalSC.endy) break;
			LocalSC.jy--;
			LocalSC.py--;
			OFFY(LocalSC.jx)
			LocalSC.r += LocalSC.incX;
		}          
	}
}

//----------------------------------------------------------------------------
// DDA for 1st quadrant with markings for y scan lines only
//----------------------------------------------------------------------------

void DDA_1_Y(SCP0)
{
	for (;;)
	{
		if (LocalSC.r > 0)
		{
			if (LocalSC.jx == LocalSC.endx) return;
			LocalSC.jx += 1;
			LocalSC.r -= LocalSC.incY;
		}
		else
		{
			if (LocalSC.jy == LocalSC.endy) return;
			//------------------------------------------------------------------------
			// for platforms for which we support banding, include extra code
			//------------------------------------------------------------------------
#ifndef FSCFG_NO_BANDING
			if (LocalSC.py > LocalSC.highRowP)
				return;
			if (LocalSC.py >= LocalSC.lowRowP)
#endif
			//------------------------------------------------------------------------
				SET(LocalSC.py, LocalSC.jx)
			LocalSC.jy++;
			LocalSC.py++;
			LocalSC.r += LocalSC.incX;
		}          
	}
}

//----------------------------------------------------------------------------
// DDA for 2nd quadrant with markings for y scan lines only
//----------------------------------------------------------------------------

void DDA_2_Y(SCP0)
{
	for (;;)
	{
		if (LocalSC.r > 0)
		{
			if (LocalSC.jx == LocalSC.endx) return;
			LocalSC.jx -= 1;
			LocalSC.r -= LocalSC.incY;
		}
		else
		{
			if (LocalSC.jy == LocalSC.endy) return;
			//------------------------------------------------------------------------
			// for platforms for which we support banding, include extra code
			//------------------------------------------------------------------------
#ifndef FSCFG_NO_BANDING
			if (LocalSC.py > LocalSC.highRowP)
				return;
			if (LocalSC.py >= LocalSC.lowRowP)
#endif
			//------------------------------------------------------------------------
				SET(LocalSC.py, LocalSC.jx)
			LocalSC.jy++ ;
			LocalSC.py++ ;
			LocalSC.r += LocalSC.incX ;
		}          
	}
}

//----------------------------------------------------------------------------
// DDA for 3rd quadrant with markings for y scan lines only
//----------------------------------------------------------------------------

void DDA_3_Y(SCP0)
{
	for (;;)
	{
		if (LocalSC.r > 0)
		{
			if (LocalSC.jx == LocalSC.endx) return;
			LocalSC.jx -= 1;
			LocalSC.r -= LocalSC.incY;
		}
		else
		{
			if (LocalSC.jy == LocalSC.endy) return;
			LocalSC.jy--;
			LocalSC.py--;
			//------------------------------------------------------------------------
			// for platforms for which we support banding, include extra code
			//------------------------------------------------------------------------
#ifndef FSCFG_NO_BANDING
			if (LocalSC.py < LocalSC.lowRowP)
				return;
			if (LocalSC.py <= LocalSC.highRowP)
#endif
			//------------------------------------------------------------------------
				OFFY(LocalSC.jx) 
			LocalSC.r += LocalSC.incX;
		}          
	}
}

//----------------------------------------------------------------------------
// DDA for 4th quadrant with markings for y scan lines only
//----------------------------------------------------------------------------

void DDA_4_Y(SCP0)
{
	for (;;)
	{
		if (LocalSC.r > 0)
		{
			if (LocalSC.jx == LocalSC.endx) return;
			LocalSC.jx += 1;
			LocalSC.r -= LocalSC.incY;
		}
		else
		{
			if (LocalSC.jy == LocalSC.endy) return;
			LocalSC.jy--;
			LocalSC.py--;
			//------------------------------------------------------------------------
			// for platforms for which we support banding, include extra code
			//------------------------------------------------------------------------
#ifndef FSCFG_NO_BANDING
			if (LocalSC.py < LocalSC.lowRowP)
				return;
			if (LocalSC.py <= LocalSC.highRowP)
#endif
			//------------------------------------------------------------------------
				OFFY (LocalSC.jx) 
			LocalSC.r += LocalSC.incX;
		}          
	}
}

//------------------------END  OF DDAS -----------------------------------------

#undef  DROUND
#undef  RSH
#undef  LSH
#undef  LINE
#undef  SET
#undef  OFFY
#undef  BETWEEN
#undef  INTERIOR
#undef  TOP_TO_BOT  
#undef  BOT_TO_TOP  
#undef  LFT_TO_RGHT
#undef  RGHT_TO_LFT

// new version 4/4/90 - winding number version assumes that the On transitions are
// int the first half of the array, and the Off transitions are in the second half.  Also
// assumes that the number of on transitions is in array[0] and the number of off transitions
// is in array[n].

// New version 3/10/90 
// Using the crossing information, look for segments that are crossed twice.  First
// do Y lines, then do X lines.  For each found segment, look at the three lines in
// the more positive adjoining segments.  If there are at least two crossings
// of these lines, there is a dropout that needs to be fixed, so fix it.  If the bit on 
// either side of the segment is on, quit; else turn the leastmost of the two pixels on.

PRIVATE void sc_orSomeBits(sc_BitMapData *bbox, int32 scanKind)
{
	int32 ymin, ymax, xmin, xmax;
	register int32 **yBase, **xBase;
	register int32 scanline, coordOn, coordOff, nIntOn, nIntOff;
	uint32 *bitmapP;
	uint32 *scanP;
	int32 *rowPt, longsWide, *pOn, *pOff, *pOff2;
	int32 index, incY, incX;
	int32 upper, lower;

	scanKind &= STUBCONTROL;
	ymin = bbox->bounds.yMin;
	ymax = bbox->bounds.yMax - 1;
	xmin = bbox->bounds.xMin;
	xmax = bbox->bounds.xMax - 1;
	xBase = bbox->xBase;
	yBase = bbox->yBase;
	longsWide = bbox->wide >> 5;
	if (longsWide == 1) 
		bitmapP = bbox->bitMap + bbox->high - 1;
	else 
		bitmapP = bbox->bitMap + longsWide * (bbox->high - 1);

	// First do Y scanlines

	scanP = bitmapP;
	incY = bbox->nYchanges + 2;
	incX = bbox->nXchanges + 2;
	rowPt = * (yBase + ymin);
	for (scanline = ymin; scanline <= ymax; ++scanline)
	{
		nIntOn = *rowPt;
		nIntOff = * (rowPt + incY - 1);
		pOn = rowPt + 1;
		pOff = rowPt + incY - 1 - nIntOff;
		while (nIntOn--)
		{
			coordOn = *pOn++;
			index = nIntOff;
			pOff2 = pOff;
			while (index-- && ((coordOff = *pOff2++) < coordOn))
				;

			if (coordOn == coordOff)	// This segment was crossed twice
			{
				if (scanKind)
				{
					upper = nUpperXings(yBase, xBase, scanline, coordOn, incY - 2, incX - 2,  xmin, xmax + 1, ymax);
					lower = nLowerXings(yBase, xBase, scanline, coordOn, incY - 2, incX - 2,  xmin, xmax + 1, ymin);
					if (upper < 2 || lower < 2) 
						continue;
				}
				if (coordOn > xmax) 
					invpixOn (xmax - xmin, longsWide, scanP);
				else if (coordOn == xmin) 
					invpixOn (0, longsWide, scanP);
				else 
					invpixSegY (coordOn - xmin - 1, longsWide, scanP);
			}
		}
		rowPt += incY;
		scanP -= longsWide;
	}

	// Next do X scanlines

	rowPt = * (xBase + xmin);
	for (scanline = xmin ; scanline <= xmax; ++scanline)
	{
		nIntOn = *rowPt;
		nIntOff = * (rowPt + incX - 1);
		pOn = rowPt + 1;                              
		pOff = rowPt + incX - 1 - nIntOff;
		while (nIntOn--)
		{
			coordOn = *pOn++;
			index = nIntOff;
			pOff2 = pOff;
			while (index-- && ((coordOff = *pOff2++) < coordOn))
				;
			if (coordOn == coordOff)
			{
				if (scanKind)
				{
					upper = nUpperXings (xBase, yBase, scanline, coordOn, incX - 2, incY - 2, ymin, ymax + 1, xmax);
					lower = nLowerXings (xBase, yBase, scanline, coordOn, incX - 2, incY - 2,  ymin, ymax + 1, xmin);
					if (upper < 2 || lower < 2) 
						continue;
				}
				if (coordOn > ymax) 
					invpixOn (scanline - xmin, longsWide, bitmapP - longsWide * (ymax - ymin));
				else if (coordOn == ymin) 
					invpixOn (scanline - xmin, longsWide, bitmapP);
				else 
					invpixSegX (scanline - xmin, longsWide, bitmapP - longsWide * (coordOn - ymin - 1));
			}
		}
		rowPt += incX;
	}
}

// Pixel oring to fix dropouts   *** inverted bitmap version ***
// See if the bit on either side of the Y line segment is on, if so return,
// else turn on the leftmost bit.
//
// Bitmap array is always K longs wide by H rows high.
//
// Bit locations are numbered 0 to H-1 from top to bottom
// and from 0 to 32*K-1 from left to right; bitmap pointer points to 0,0, and
// all of the columns for one row are stored adjacently.

PRIVATE void invpixSegY(int32 llx, uint32 k, uint32* bitmapP)
{
	uint32 maskL, maskR;
	
	llx += 1 ;
	MASK_INVPIX(maskR, llx & 0x1f);
	bitmapP += (llx >> 5);
	
	if (*bitmapP & maskR) 
	{
		return;
	}
	
	if (llx &= 0x1f)
	{
		MASK_INVPIX(maskL, llx - 1);
	}
	else
	{
		MASK_INVPIX(maskL, 31);
		--bitmapP;
	}
	
	*bitmapP |= maskL;
}

// Pixel oring to fix dropouts   *** inverted bitmap version ***
// See if the bit on either side of the X line segment is on, if so return,
// else turn on the bottommost bit.
//
// Temporarily assume bitmap is set up as in Sampo Converter. 
// Bitmap array is always K longs wide by H rows high.
//
// For now, assume bit locations are numbered 0 to H-1 from top to bottom
// and from 0 to 32*K-1 from left to right; and that bitmap pointer points to 0,0, and
// all of the columns for one row are stored adjacently.

PRIVATE void invpixSegX(int32 llx, uint32 k, uint32* bitmapP)
{
	register uint32 maskL;
	
	bitmapP -= k;
	MASK_INVPIX(maskL, llx & 0x1f);
	bitmapP += (llx >> 5);
	
	if (*bitmapP & maskL) 
	{
		return;
	}
	
	bitmapP += k;
	*bitmapP |= maskL;
}


// Pixel oring to fix dropouts    ***inverted bitmap version ***
// This code is used to orin dropouts when we are on the boundary of the bitmap.
// The bit at llx, lly is colored.
//
// Temporarily assume bitmap is set up as in Sampo Converter. 
// Bitmap array is always K longs wide by H rows high.
//
// For now, assume bit locations are numbered 0 to H-1 from top to bottom
// and from 0 to 32*K-1 from left to right; and that bitmap pointer points to 0,0, and
// all of the columns for one row are stored adjacently.

PRIVATE void invpixOn(int32 llx, uint32 k, uint32* bitmapP)
{
	uint32 maskL;
	
	MASK_INVPIX (maskL, llx & 0x1f);
	bitmapP += (llx >> 5);
	*bitmapP |= maskL;
}

// Initialize a two dimensional array that will contain the coordinates of 
// line segments that are intersected by scan lines for a simple glyph.  Return
// a biased pointer to the array containing the row pointers, so that they can
// be accessed without subtracting a minimum value.
// Always reserve room for at least 1 scanline and 2 crossings 

PRIVATE int32** sc_lineInit(int32* arrayBase, int32** rowBase, int32 nScanlines, int32 maxCrossings, int32 minScanline)
{
	int32** bias;
	register int32 count = nScanlines;
	if (count) 
		--count;
	bias = rowBase - minScanline;
	maxCrossings += 1;
	for (; count >= 0; --count)
	{
		*rowBase++ = arrayBase;
		*arrayBase = 0;
		arrayBase += maxCrossings;
		*arrayBase++ = 0;
	}
	return bias;
}

// Check the kth scanline (indexed from base) and count the number of onTransition and
// offTransition contour crossings at the line segment val.  Count only one of each
// kind of transition, so maximum return value is two. 

PRIVATE int32 nOnOff(int32** base, int32 k, int32 val, int32 nChanges)
{
	register int32* rowP = *(base + k);
	register int32* endP = (rowP + *rowP + 1);
	register int32 count = 0;
	register int32 v;
	
	while (++rowP < endP)
	{
		if ((v = *rowP) == val)
		{
			++count;
			break;
		}
		if (v > val) 
		{
			break;
		}
	}
	
	rowP = *(base + k) + nChanges + 1;
	endP = (rowP - *rowP - 1);
	
	while (--rowP > endP)
	{
		if ((v = *rowP) == val) 
		{
			return ++count;
		}
		if (v < val) 
		{
			break;
		}
	}
	
	return count;
}

// 8/22/90 - added valMin and valMax checks
// See if the 3 line segments on the edge of the more positive quadrant are cut by at
// least 2 contour lines.

PRIVATE int32 nUpperXings(int32** lineBase, int32** valBase, int32 line, int32 val, int32 lineChanges, int32 valChanges, int32 valMin, int32 valMax, int32 lineMax)
{
	register int32 count = 0;
	
	if (line < lineMax) 
		count += nOnOff(lineBase, line + 1, val, lineChanges);
	if (count > 1) 
		return (int)count;
	else if (val > valMin) 
		count += nOnOff(valBase, val - 1, line + 1, valChanges);
	if (count > 1) 
		return (int)count;
	else if (val < valMax) 
		count += nOnOff(valBase, val, line + 1, valChanges);
	return (int)count;
}

// See if the 3 line segments on the edge of the more negative quadrant are cut by at
// least 2 contour lines.

PRIVATE int32 nLowerXings(int32** lineBase, int32** valBase, int32 line, int32 val, int32 lineChanges, int32 valChanges, int32 valMin, int32 valMax, int32 lineMin)
{
	register int32 count = 0;
	
	if (line > lineMin) 
		count += nOnOff(lineBase, line - 1, val, lineChanges);
	if (count > 1) 
		return (int)count;
	if (val > valMin) 
		count += nOnOff(valBase, val - 1, line, valChanges);
	if (count > 1) 
		return (int)count;
	if (val < valMax) 
		count += nOnOff(valBase, val, line, valChanges);
	return (int)count;
}

// Finds the extrema of a character.
//
// PARAMETERS:
//
// bbox is the output of this function and it contains the bounding box.
//
// revised for new scan converter 4/90 rwb 

int sc_FindExtrema(sc_CharDataType *glyphPtr, sc_BitMapData *bbox)
{
	register F26Dot6 *x, *y;
	register F26Dot6 tx, ty, prevx, prevy;
	F26Dot6 xmin, xmax, ymin, ymax;
	ArrayIndex point, endPoint, startPoint;
	int ctr;
	uint32 nYchanges, nXchanges, nx;
	int32 posY, posX;
	int firstTime = true;

	nYchanges = nXchanges = 0;
	xmin = xmax = ymin = ymax = 0;

	for (ctr = 0; ctr < glyphPtr->nc; ctr++)
	{
		endPoint = glyphPtr->ep[ctr];
		startPoint = glyphPtr->sp[ctr];
		x = &(glyphPtr->x[startPoint]);
		y = &(glyphPtr->y[startPoint]);
		
		if (startPoint == endPoint)
		{
			continue;	// We need to do this for anchor points for composites
		}
		
		if (firstTime)
		{
			xmin = xmax = *x;
			ymin = ymax = *y;
			firstTime = false;
		}
		
		posY = (int)(*y >= (ty = * (y + endPoint - startPoint)));
		posX = (int)(*x >= (tx = * (x + endPoint - startPoint)));

		for (point = startPoint; point <= endPoint; ++point)
		{
			prevx = tx;
			prevy = ty;
			tx = *x++;
			ty = *y++;
			if (tx > prevx) 
			{
				if (!posX) 
				{
					++nXchanges;
					posX = true;
				}
			}
			else if (tx < prevx) 
			{
				if (posX) 
				{
					++nXchanges;
					posX = false;
				}
			}
			else if (ty == prevy)
			{
				int32 j = point - 2 - startPoint;
				register F26Dot6 *newx = x-3;
				register F26Dot6 *oldx = newx++;
				register F26Dot6 *newy = y-3;
				register F26Dot6 *oldy = newy++;
				register uint8 *newC = &(glyphPtr->onCurve[point-2]);
				register uint8 *oldC = newC++;
				
				*(newC + 1) |= ONCURVE;
				for (; j >= 0; --j)
				{
					*newx-- = *oldx--;
					*newy-- = *oldy--;
					*newC-- = *oldC--;
				}
				++startPoint;
			}

			if (ty > prevy) 
			{
				if (!posY) 
				{
					++nYchanges;
					posY = true;
				}
			}
			else if (ty < prevy) 
			{
				if (posY) 
				{
					++nYchanges;
					posY = false;
				}
			}
			if (tx > xmax) 
				xmax = tx;
			else if (tx < xmin) 
				xmin = tx;
			if (ty > ymax) 
				ymax = ty;
			else if (ty < ymin) 
				ymin = ty;
		}
		
		glyphPtr->sp[ctr] = startPoint < endPoint ? startPoint : endPoint;
		if (nXchanges & 1) 
			++nXchanges;
		if (nYchanges & 1) 
			++nYchanges;	// make even
	}

	xmax += HALF;  
	xmax >>= PIXSHIFT;
	ymax += HALF;  
	ymax >>= PIXSHIFT;
	xmin += HALFM; 
	xmin >>= PIXSHIFT;
	ymin += HALFM; 
	ymin >>= PIXSHIFT;

#if 0
	if (((F26Dot6)(int16)xmin != xmin)
	 || ((F26Dot6)(int16)ymin != ymin)
	 || ((F26Dot6)(int16)xmax != xmax)
	 || ((F26Dot6)(int16)ymax != ymax))
	{
		return POINT_MIGRATION_ERR;
	}
#endif

	xmin -= Fx.fx_eae.xMin;
	xmax += Fx.fx_eae.xMax;
	ymin -= Fx.fx_eae.yMin;
	ymax += Fx.fx_eae.yMax;

	bbox->bounds.xMax = (int)xmax;	// bitmap boundaries
	bbox->bounds.xMin = (int)xmin;
	bbox->bounds.yMax = (int)ymax;
	bbox->bounds.yMin = (int)ymin;

	bbox->high = (int)(ymax - ymin);
	nx = (int)(xmax - xmin);			// width is rounded up to be a long multiple
	bbox->wide = (nx + 31) & ~31;		// also add 1 when already an exact long multiple

	//------------------------------------------------------------------------------
	// make the width atleast 32 pels wide so that we do not allocate zero
	// memory for the bitmap                                                          
	//------------------------------------------------------------------------------
	
	if (bbox->wide == 0)
	{
		bbox->wide = 32;
	}
	
	//------------------------------------------------------------------------------

	if (nXchanges == 0) 
		nXchanges = 2;
	if (nYchanges == 0) 
		nYchanges = 2;

	// PMGTODO: The "4" is because the value computed by sc_FindeExtrema seems to be
	// low in some cases. So, for now we compensate by adding an empirically derived
	// kludge constant. This nees to be fixed to determine why the computed value
	// is not accurate.
	bbox->nXchanges = nXchanges+4;

	bbox->nYchanges = nYchanges;
	
	return NO_ERR;
}

// This function break up a parabola defined by three points (A,B,C) and breaks it
// up into straight line vectors given a maximium error. The maximum error is
// 1/resolution * 1/ERRDIV. ERRDIV is defined in sc.h.
//
//           
//         B *-_
//          /   `-_
//         /       `-_
//        /           `-_ 
//       /               `-_
//      /                   `* C
//   A *
//
// PARAMETERS:
//
// Ax, Ay contains the x and y coordinates for point A. 
// Bx, By contains the x and y coordinates for point B. 
// Cx, Cy contains the x and y coordinates for point C.
// hX, hY are handles to the areas where the straight line vectors are going to be put.
// count is pointer to a count of how much data has been put into *hX, and *hY.
//
// F (t) = (1-t)^2 * A + 2 * t * (1-t) * B + t * t * C, t = 0... 1 =>
// F (t) = t * t * (A - 2B + C) + t * (2B - 2A) + A  =>
// F (t) = alfa * t * t + beta * t + A
// Now say that s goes from 0...N, => t = s/N
// set: G (s) = N * N * F (s/N)
// G (s) = s * s * (A - 2B + C) + s * N * 2 * (B - A) + N * N * A
// => G (0) = N * N * A
// => G (1) = (A - 2B + C) + N * 2 * (B - A) + G (0)
// => G (2) = 4 * (A - 2B + C) + N * 4 * (B - A) + G (0) =
//           3 * (A - 2B + C) + 2 * N * (B - A) + G (1)
//
// D (G (0)) = G (1) - G (0) = (A - 2B + C) + 2 * N * (B - A)
// D (G (1)) = G (2) - G (1) = 3 * (A - 2B + C) + 2 * N * (B - A)
// DD (G)   = D (G (1)) - D (G (0)) = 2 * (A - 2B + C)
// Also, error = DD (G) / 8 .
// Also, a subdivided DD = old DD/4.
 
PRIVATE int sc_DrawParabola(F26Dot6 _Ax, F26Dot6 _Ay, F26Dot6 _Bx, F26Dot6 _By, F26Dot6 _Cx, F26Dot6 _Cy, F26Dot6** hX, F26Dot6** hY, uint32* count, int32 inGY)
{
	int nsqs;
	register int32 GX, GY, _DX, _DY, DDX, DDY;
	register F26Dot6 *xp, *yp;
	register int32 tmp;
	int32 i;

	// Start calculating the first and 2nd order differences
	GX = _Bx; // GX = Bx
	DDX = (_DX = (_Ax - GX)) - GX + _Cx; // = alfa-x = half of ddx, DX = Ax - Bx
	GY = _By; // GY = By
	DDY = (_DY = (_Ay - GY)) - GY + _Cy; // = alfa-y = half of ddx, DY = Ay - By
	// The calculation is not finished but these intermediate results are useful

	if (inGY < 0) 
	{
		// calculate amount of steps necessary = 1 << GY
		// calculate the error, GX and GY used a temporaries
		GX  = DDX < 0 ? -DDX : DDX;
		GY  = DDY < 0 ? -DDY : DDY;

		// approximate GX = sqrt (ddx * ddx + ddy * ddy) = Euclididan distance, DDX = ddx/2 here
		GX += GX > GY ? GX + GY : GY + GY; // GX = 2*distance = error = GX/8

		// error = GX/8, but since GY = 1 below, error = GX/8/4 = GX >> 5, => GX = error << 5
#ifdef ERRSHIFT
		for (GY = 1; GX > (PIXELSIZE << (5 - ERRSHIFT)); GX >>= 2)  
		{
#else
		for (GY = 1; GX > (PIXELSIZE << 5) / ERRDIV; GX >>= 2) 
		{
#endif
			GY++; // GY used for temporary purposes
		}

		// Now GY contains the amount of subdivisions necessary, number of vectors == (1 << GY)
		if (GY > MAXMAXGY)
		{
			GY = MAXMAXGY; // Out of range => Set to maximum possible.
		}

		i = 1 << GY;

		if ((*count = *count + i)  > MAXVECTORS) 
		{
			// Overflow, not enough space => return 1
			return (1);
		}
	}
	else
	{
		GY = inGY;
		i = 1 << GY;
	}

	if (GY > MAXGY) 
	{
		F26Dot6 MIDX, MIDY;

		DDX = GY - 1; // DDX used as a temporary

		// Subdivide, this is nummerically stable.

		MIDX = (F26Dot6) (((long) _Ax + _Bx + _Bx + _Cx + 2) >> 2);
		MIDY = (F26Dot6) (((long) _Ay + _By + _By + _Cy + 2) >> 2);
		_DX  = (F26Dot6) (((long) _Ax + _Bx + 1) >> 1);
		_DY  = (F26Dot6) (((long) _Ay + _By + 1) >> 1);
		sc_DrawParabola (_Ax, _Ay, (F26Dot6)_DX, (F26Dot6)_DY, MIDX, MIDY, hX, hY, count, DDX);
		_DX  = (F26Dot6) (((long) _Cx + _Bx + 1) >> 1);
		_DY  = (F26Dot6) (((long) _Cy + _By + 1) >> 1);
		sc_DrawParabola (MIDX, MIDY, (F26Dot6)_DX, (F26Dot6)_DY, _Cx, _Cy, hX, hY, count, DDX);

		return (0);
	}

	nsqs = (int)(GY + GY); // GY = n shift, nsqs = n*n shift

	// Finish calculations of 1st and 2nd order differences

	_DX  = DDX - (_DX << ++GY); // alfa + beta * n
	DDX += DDX;
	_DY  = DDY - (_DY <<   GY);
	DDY += DDY;

	xp = *hX;
	yp = *hY;

	GY = (long) _Ay << nsqs; // Ay * (n*n)
	GX = (long) _Ax << nsqs; // Ax * (n*n)
	// GX and GY used for real now

	// OK, now we have the 1st and 2nd order differences,
	// so we go ahead and do the forward differencing loop.
	tmp = 1L << (nsqs-1);

	do
	{
		GX += _DX;  // Add first order difference to x coordinate
		*xp++ = (F26Dot6)((GX + tmp) >> nsqs);
		_DX += DDX; // Add 2nd order difference to first order difference.

		GY += _DY;  // Do the same thing for y.
		*yp++ = (F26Dot6)((GY + tmp) >> nsqs);
		_DY += DDY;
	} while (--i);

	*hX = xp; // Done, update pointers, so that caller will know how much data we put in.
	*hY = yp;

	return (0);
}

#ifndef PC_OS
#ifndef FSCFG_SC_NATIVE_ENDIAN_LONG_BMP
// SetUpMasks() loads two arrays of 32-bit masks at runtime so
// that the byte layout of the masks need not be CPU-specific
//
// It is conditionally compiled because the arrays are unused 
// in a "BITMAPS_AS_LONGS" (e.g. Motorola) configuration and initialized 
// at compile time for Intel order in the PC_OS (Windows) 
// configuration.
//
// We load the arrays by converting the Big-Endian value of
// the mask to the "native" representation of that mask.  The
// "native" representation can be applied to a "native" byte
// array to manipulate more than 8 bits at a time of an output
// bitmap.

static void SetUpMasks (void)
{
	register int i;
	uint32 ulMaskI = (unsigned long)(-1L);
	uint32 ulInvPixMaskI = (unsigned long)(0x80000000L);

	for (i=0; i<32; i++, ulMaskI>>=1, ulInvPixMaskI>>=1)
	{
		aulMask[i] = (uint32) SWAPL(ulMaskI);
		aulInvPixMask[i] = (uint32) SWAPL(ulInvPixMaskI);
	}
	fMasksSetUp = true;
}
#endif
#endif

void fill_pattern_run(int32 x, int32 y, int32 l)
{
	if ((y >= Fx.fx_low_row) && (y < Fx.fx_high_row))
	{
		int32 xmin;
		int32 xmax;

		xmin = Fx.fx_bitmap->bounds.xMin;
		xmax = Fx.fx_bitmap->bounds.xMax;

		if (x < xmin)
		{
			l += (x-xmin);
			x = xmin;
		}

		if ((x+l) > xmax)
		{
			l = xmax-x;
		}

		if (l > 0)
		{
			uint32 wide = Fx.fx_bitmap->wide >> 5;
			uint32 offset = (x-xmin) >> 5;
			uint32* row = Fx.fx_bitmap->bitMap+offset+((Fx.fx_high_row-1-y)*wide);
			int32 last = xmin+((offset+1) << 5);
			uint32 temp;
			
			temp = MASK_ON(32+x-last);

			x = x+l;

			if (x >= last)
			{
				*row &= ~temp;
				*row |= get_pattern_data(temp, last-32, y);

				row++;
				last += 32;

				while (x >= last)
				{
					*row = get_pattern_data(0xffffffffL, last-32, y);
					row++;
					last += 32;
				}

				temp = 0xffffffffL;
			}

			if ((temp &= MASK_OFF(last-x)) != 0)
			{
				*row &= ~temp;
				*row |= get_pattern_data(temp, last-32, y);
			}
		}
	}
}


// fill_black_rectangle
// fill_white_rectangle
//
// These functions are based on fill_pattern_run.  They are optimized
// for fast drawing of rectangle pens in outline.c.  The speed gain comes
// from eliminating redundant computations for each row.

void fill_black_rectangle(int32 x, int32 y, int32 l, int32 nrows)
{
	int32 xmin;
	int32 xmax;

	xmin = Fx.fx_bitmap->bounds.xMin;
	xmax = Fx.fx_bitmap->bounds.xMax;

	if (x < xmin)
	{
		l += (x-xmin);
		x = xmin;
	}

	if ((x+l) > xmax)
	{
		l = xmax-x;
	}

	if (l > 0)
	{
		uint32 wide = Fx.fx_bitmap->wide >> 5;
		uint32 offset = (x-xmin) >> 5;
		uint32* rowbase = Fx.fx_bitmap->bitMap+offset+((Fx.fx_high_row-1-y)*wide);
		int32 lastsave = xmin+((offset+1) << 5);
		uint32 first_mask = MASK_ON((x-xmin) & 31);
		int32 xsave = x+l;
		
		while (nrows--)
		{
			register uint32 *row = rowbase;
			register uint32 temp;
			int32 last;

			last = lastsave;
			temp = first_mask;

 			x = xsave;

			if (x >= last)
			{
				*row |= temp;

				row++;
				last += 32;

				while (x >= last)
				{
					*row = 0xffffffff;
					row++;
					last += 32;
				}

				temp = 0xffffffff;
			}

			if ((temp &= ~(MASK_ON((x-xmin) & 31))) != 0)
			{
				*row |= temp;
			}

			rowbase += wide;
		}
	}
}

void fill_white_rectangle(int32 x, int32 y, int32 l, int32 nrows)
{
	int32 xmin;
	int32 xmax;

	xmin = Fx.fx_bitmap->bounds.xMin;
	xmax = Fx.fx_bitmap->bounds.xMax;

	if (x < xmin)
	{
		l += (x-xmin);
		x = xmin;
	}

	if ((x+l) > xmax)
	{
		l = xmax-x;
	}

	if (l > 0)
	{
		uint32 wide = Fx.fx_bitmap->wide >> 5;
		uint32 offset = (x-xmin) >> 5;
		uint32 *rowbase = Fx.fx_bitmap->bitMap+offset+((Fx.fx_high_row-1-y)*wide);
		int32 lastsave = xmin+((offset+1) << 5);
		uint32 first_mask = MASK_ON((x-xmin) & 31);
		int32 xsave = x+l;

		while (nrows--)
		{
			register uint32 *row = rowbase;
			register uint32 temp;
			int32 last;

			last = lastsave;
			temp = first_mask;

			x = xsave;

			if (x >= last)
			{
				*row &= ~temp;

				row++;
				last += 32;

				while (x >= last)
				{
					*row = 0;
					row++;
					last += 32;
				}

				temp = 0xffffffff;
			}

			if ((temp &= ~(MASK_ON((x-xmin) & 31))) != 0)
			{
				*row &= ~temp;
			}

			rowbase += wide;
		}
	}
}
