/*
$Header: /PM8/TT/FSCALER.CPP 1     3/03/99 6:35p Gbeddow $
$Log: /PM8/TT/FSCALER.CPP $
// 
// 1     3/03/99 6:35p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:15:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:45:08   Fred
// Initial revision.
// 
//    Rev 1.4   01 Jul 1997 09:08:32   Fred
// More changes for PMG 4.0
// 
//    Rev 1.3   02 May 1997 10:14:56   Fred
//  
// 
//    Rev 1.2   21 Apr 1997 08:47:50   Fred
// Fixes for rotated pens and shadows.
// 
//    Rev 1.1   18 Apr 1997 12:49:40   Fred
// Changes for PMG 4.0
// 
//    Rev 1.0   14 Mar 1996 13:21:20   Jay
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:14:58   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:41:44   FRED
// Initial revision.
// 
//    Rev 1.1   04 Oct 1995 10:59:36   FRED
// 32 bit TrueType engine
// 
//    Rev 1.0   29 Sep 1995 13:37:02   FRED
// Initial revision.
 * 
 *    Rev 1.97   28 Apr 1995 13:40:00   FRED
 * Added coordinatePrecision field to output structure
 * in which the number of fraction bits is passed back
 * to the caller after a grid fit call.
 * 
 * 
 *    Rev 1.96   19 Jan 1994 10:46:44   MACDUFF
 * set_precision now uses the stretching values in the input matrix
 * instead of the xStretch and yStretch fields.  xStretch and yStretch
 * should always be set to one; other values will no longer work.
 * 
 *    Rev 1.95   13 Jul 1993 10:42:42   MACDUFF
 * Reformatted the ROTATE_EFFECTS code, since it's now correct and
 * usable (although ROTATE_EFFECTS is currently disabled).
 * 
 *    Rev 1.94   13 Jan 1993 09:50:10   MACDUFF
 * NewSFNT now clears nonScaledAW before calling sfnt_ReadSFNTMetrics.
 * This forces a read of the hmtx table.
 * 
 *    Rev 1.93   12 Jan 1993 11:40:20   MACDUFF
 * Initialize advance width and glyph offset fields in key.  This is for
 * prevention of duplicate reads of hmtx and loca tables, respectively.
 * 
 *    Rev 1.92   12 Jan 1993 09:59:20   MACDUFF
 * cosmetic
 * 
 *    Rev 1.91   11 Jan 1993 11:41:40   MACDUFF
 * Removed unused code in GetAdvanceWidths pertaining to background characters.
 * Note that this means outputPtr->metricinfo.advanceWidth is not set to
 * anything; all clients must look at key instead.
 * 
 *    Rev 1.90   11 Jan 1993 09:21:30   MACDUFF
 * Omit support for trace function if FSCFG_NO_TRACE defined.
 * 
 *    Rev 1.89   07 Jan 1993 10:34:38   MACDUFF
 * Don't do dropout control if fill pattern is not black.  Trying to make
 * dropout control use patterns would slow it down considerably, without
 * much benefit.
 * 
 *    Rev 1.88   18 Nov 1992 08:25:30   MACDUFF
 * Changed some instances of "#ifdef TRUEFX" to "#ifdef ROTATE_EFFECTS".
 * ROTATE_EFFECTS is NOT currently defined.  This is so metrics will be
 * right while rotation is still under development.
 * 
 *    Rev 1.87   11 Nov 1992 18:12:34   MACDUFF
 * set_precision uses xStretch, yStretch values.  This fixes bug.
 * 
 *    Rev 1.86   09 Nov 1992 16:45:24   MACDUFF
 * In TRUEFX, computation of fx_x_pixels_per_em and fx_y_pixels_per_em
 * takes xStretch and yStretch values into account.
 * 
 *    Rev 1.85   09 Nov 1992 13:31:32   MACDUFF
 * Passes more parameters to sc_scan_background.
 * 
 *    Rev 1.84   05 Nov 1992 17:15:30   MACDUFF
 * Fixed error in TRUEFX computation of fx_cell_height, fx_cell_width.
 * Call new functions sc_initialize_buffer and sc_scan_background, rather
 * than using flags with sc_ScanChar to do the same thing.
 * 
 *    Rev 1.83   05 Nov 1992 13:01:18   MACDUFF
 * Added calls to make_static_pen.
 * 
 *    Rev 1.82   21 Oct 1992 14:12:20   MACDUFF
 * Support for new shadow rotation
 * 
 *    Rev 1.81   19 Oct 1992 17:56:48   MACDUFF
 * set_precision uses the transform matrix.
 * Partial support for rotation.
 * 
 *    Rev 1.80   15 Oct 1992 16:55:08   ADAM
 * Point size = 0 returns 1000 line metrics instead of em resolution.
 * 
 *    Rev 1.79   14 Oct 1992 10:53:00   ADAM
 * Full support of EAE metrics. Previously we had no way of just getting
 * the EAE contributions. Now we do.
 * 
 *    Rev 1.78   15 Sep 1992 15:54:56   ADAM
 * Some MLS assembler code was referencing the compilers far procedures.
 * These are no longer available. They are now: NEAR_ followed by the name.
 * 
 *    Rev 1.77   04 Sep 1992 11:23:12   FRED
 * NewTrans() sets the fx_em_cell_height and fx_em_cell_width fields
 * in the Fx structure.
 * 
 * 
 *    Rev 1.76   04 Sep 1992 09:35:00   FRED
 * Added fx_eae_width and fx_em_eae_width to DeltaX values returned by
 * GetAdvanceWidth().
 * 
 * 
 *    Rev 1.75   03 Sep 1992 17:27:28   FRED
 * NewTransform initializes two new fields in the key:
 * 
 *     EAE_width   and   EAE_width_em
 * 
 * These are the adjustments for horizontal
 * metrics at dot and Em resolution.
 * 
 * 
 *    Rev 1.74   01 Sep 1992 01:13:02   FRED
 * Added "L" to number to turn off warning.
 * 
 * 
 *    Rev 1.73   29 Aug 1992 14:50:12   FRED
 * Removed MLS_OUTLINE_contour_inline_pen. MLS_OUTLINE_light
 * is used instead.
 * 
 * 
 *    Rev 1.72   29 Aug 1992 10:59:40   FRED
 * Fixed bug in assembly version of set_precision().
 * 
 * Uses new MLS_ASM_... #defines.
 * 
 * 
 *    Rev 1.71   28 Aug 1992 18:48:56   FRED
 * Fixed bug where _PRECISION was being computed wrong and disabling
 * hinting.
 * 
 * 
 *    Rev 1.70   28 Aug 1992 15:19:48   FRED
 * Recoded the algorithm for setting _PRECISION
 * in assembly.
 * 
 * 
 *    Rev 1.69   28 Aug 1992 11:11:58   FRED
 * Fixed problem where FSCALER.C would not compile
 * with MSL_ASM undefined.
 * 
 * 
 *    Rev 1.68   27 Aug 1992 18:02:38   FRED
 * Sets Fx.fx_is_symbol_font.
 * 
 * Endcaps are only available when a background effect is active.
 * 
 * 
 *    Rev 1.67   27 Aug 1992 17:11:40   ADAM
 * Uses EM width / 4 for the space character in symbol fonts. This approximates
 * SYMBOL.OTL which has a space width of 249/1000.
 * 
 *    Rev 1.66   27 Aug 1992 09:08:00   FRED
 * Fixed bug where the global bounding box was being computed
 * incorrectly. (The symbol.otl expansion was wrong.)
 * 
 *    Rev 1.65   26 Aug 1992 17:38:58   ADAM
 * Fixed end caps with symbol fonts.
 * 
 *    Rev 1.64   26 Aug 1992 14:52:10   FRED
 * For symbol fonts, the underline endcap
 * character to 0xf05f, not 0x005f. The code
 * should now handle this.
 * 
 * 
 *    Rev 1.63   26 Aug 1992 14:33:00   FRED
 * Gets space width for symbol fonts (clientID == 0) using 0xf020
 * instead of 0x0020.
 * 
 * 
 *    Rev 1.62   26 Aug 1992 13:59:30   FRED
 * Fixed spelling error.
 * 
 * 
 *    Rev 1.61   26 Aug 1992 13:47:30   FRED
 * Endcaps for fixed pitch fonts are only (1 X space width).
 * 
 * 
 *    Rev 1.60   25 Aug 1992 23:18:44   FRED
 * Space width * 2 is done in call to ShortMulDiv()
 * for Endcap width. (It was done outside).
 * 
 * 
 *    Rev 1.59   25 Aug 1992 11:38:50   FRED
 * Added assembly code to NewSFnt().
 * 
 * 
 *    Rev 1.58   25 Aug 1992 10:44:48   FRED
 * New code to compute metrics.
 * 
 * 
 *    Rev 1.57   24 Aug 1992 13:25:28   ADAM
 * Allows the client to breath at the head of NewTransform.
 * 
 *    Rev 1.56   23 Aug 1992 17:04:42   FRED
 * Support for endcaps.
 * 
 * 
 *    Rev 1.55   21 Aug 1992 16:56:56   ADAM
 * Removed some parameter missing warning messages.
 * 
 *    Rev 1.54   21 Aug 1992 12:29:48   FRED
 * Fixed bug where CharacterBBox_EAE values were wrong.
 * 
 * 
 *    Rev 1.53   20 Aug 1992 12:11:20   ADAM
 * Defined __FSCALER_C__ to allow FSCONFIG.H to build special code for
 * TurboPascal overlay manager.
 * 
 *    Rev 1.52   20 Aug 1992 17:35:58   FRED
 * Switched back to 0 point size returning Em resolution metrics.
 * 
 *    Rev 1.51   20 Aug 1992 17:13:32   FRED
 * 0 point size now gives 1000 line metrics instead of Em resolution.
 * 
 * 
 *    Rev 1.50   20 Aug 1992 15:22:02   FRED
 * Added function prototypes.
 * 
 * 
 *    Rev 1.49   20 Aug 1992 15:17:24   FRED
 * CharacterBBox_EAE now should work for background fonts.
 * 
 * 
 *    Rev 1.48   20 Aug 1992 14:41:06   FRED
 * Added support for 0 point size by folding the
 * following values into the transformation matrix:
 *    point size = 72
 *    X and Y Resolution = Em Resolution
 * 
 * fs_GetAdvanceWidth sets CharacterBBOX_EAE in key.
 * 
 * 
 *    Rev 1.47   20 Aug 1992 11:23:56   FRED
 * Fixed syntax error.
 * 
 * 
 *    Rev 1.46   20 Aug 1992 11:19:52   FRED
 * Now sets fx_fixed_point_size.
 * 
 *    Rev 1.45   19 Aug 1992 15:45:40   MACDUFF
 * Took out debugging code.
 * 
 *    Rev 1.44   19 Aug 1992 15:44:06   MACDUFF
 * Sets up globals for variable precision math.
 * 
 *    Rev 1.43   18 Aug 1992 18:50:08   MACDUFF
 * Support for global _Precision
 * 
 *    Rev 1.42   18 Aug 1992 16:31:42   FRED
 * Optimized filling with white pattern.
 * 
 * 
 *    Rev 1.41   18 Aug 1992 12:00:26   FRED
 * GetAdvanceWidths() always computes the MLS horizontal advance widths
 * DeltaX_EAE and DeltaX_EAE_em.
 * 
 * 
 *    Rev 1.40   16 Aug 1992 15:35:00   FRED
 * Sets key->DeltaX_EAE instead of outputPtr->metricInfo.advanceWidth.x
 * 
 * 
 *    Rev 1.39   15 Aug 1992 23:56:36   FRED
 * Moved some code from SFNT.C to compute the
 * background box of background fonts to the
 * "new trans" function. This allows us to
 * run less code when a new glyph is loaded,
 * and to set the global bbox after "new trans"
 * is called.
 * 
 * 
 *    Rev 1.38   15 Aug 1992 14:29:32   FRED
 * fs_NewSFnt() now sets Fx.fx_em_resolution.
 * 
 * 
 *    Rev 1.37   15 Aug 1992 14:01:34   FRED
 * Added code to set various metric fields in the key used
 * by the scaling shell:
 * 
 *    SpaceWidth_EAE
 *    SpaceWidth_EAE_em
 *    SpacingType
 *    UnderlineDistance
 *    GlobalBBox_EAE_em
 *    DeltaX_EAE_em
 *    Device advance width in output structure
 * 
 * 
 *    Rev 1.36   14 Aug 1992 18:29:06   FRED
 * Added prototype for compute_eae_values().
 * 
 * 
 *    Rev 1.35   14 Aug 1992 18:23:54   FRED
 * fs_NewTransform() calls compute_eae_values().
 * 
 * 
 *    Rev 1.34   14 Aug 1992 15:01:14   FRED
 * Added code to set fx_is_fixed_pitch field in Fx
 * structure by examining the Panose number in the
 * OS2/Window table.
 * 
 * 
 *    Rev 1.33   14 Aug 1992 13:00:42   FRED
 * Added code to set fx_em_space_width at the end of
 * fs_newSFnt().
 * 
 * 
 *    Rev 1.32   14 Aug 1992 10:57:00   FRED
 * fs_NewGlyph() sets fx_character_code.
 * 
 * 
 *    Rev 1.31   13 Aug 1992 22:30:10   FRED
 * Uses MLS_PATTERN definitions.
 * 
 * 
 *    Rev 1.30   13 Aug 1992 12:14:26   FRED
 * Fixed some warning messages (all we're benign).
 * 
 * fx_orientation is now set in NewTrans instead of ContourScan.
 * 
 * 
 *    Rev 1.29   12 Aug 1992 17:37:36   FRED
 * Added code to expand global font BBOX to
 * MoreFonts/Geronimo SYMBOL.OTL values.
 * 
 * 
 *    Rev 1.28   12 Aug 1992 11:55:16   FRED
 * Fixed bug where unswapped values were being incremented.
 * 
 * 
 *    Rev 1.27   12 Aug 1992 11:46:06   FRED
 * Fixed syntax error.
 * 
 * 
 *    Rev 1.26   12 Aug 1992 11:39:26   FRED
 * In maxProfile structure, bumps maxContours by 2
 * and maxPoints by 8 (for background effects.)
 * 
 * Rearranged special effects code to support
 * background effects.
 * 
 * 
 *    Rev 1.25   11 Aug 1992 12:33:40   FRED
 * Fixed comment.
 * 
 * 
 *    Rev 1.24   11 Aug 1992 12:32:34   FRED
 * Reworked code that calls sc_ScanChar() to handle shadows.
 * 
 * 
 *    Rev 1.23   10 Aug 1992 17:41:52   ADAM
 * Removed ReadTable. Now that the client has access to the offset & length
 * table in the key, it seems unecessary.
 * 
 *    Rev 1.22   10 Aug 1992 14:03:06   ADAM
 * Sets special effects orientation based on the key->imageState flag DEGREE90.
 * key->imageState is set at newTrans time in FSGLUE.C.
 * 
 *    Rev 1.21   08 Aug 1992 16:53:40   FRED
 * Handles contour pens.
 * 
 * 
 *    Rev 1.20   08 Aug 1992 15:35:08   FRED
 * Fixed bug where fx_char_y1 was set incorrectly (was not negated).
 * 
 *    Rev 1.19   08 Aug 1992 15:28:42   FRED
 * Fixed bug with computing fx_cell_width.
 * 
 * 
 *    Rev 1.18   08 Aug 1992 15:06:22   FRED
 * Sets fx_cell_height, fx_cell_ascent, and fx_cell_width from actual
 * font bounding box.
 * 
 * 
 *    Rev 1.17   08 Aug 1992 13:33:08   ADAM
 * Modified fs_newSfnt to save the global bbox into the key structure.
 * 
 *    Rev 1.16   07 Aug 1992 18:49:50   FRED
 * Sets fx_x_pixels_per_em and fx_y_pixels_per_em fields
 * in the Fx structure.
 * 
 * 
 *    Rev 1.15   07 Aug 1992 18:26:56   FRED
 * Fx variables are initialized when the information is available.
 * 
 * Started code that performs mulitple calls to sc_ScanChar to do
 * compound special effects.
 * 
 * 
 *    Rev 1.14   06 Aug 1992 13:52:00   ADAM
 * Allows the client to access the key through the output structure.
 * 
 *    Rev 1.13   04 Aug 1992 12:54:44   ADAM
 * New font scaler function: fs_ReadTable.
 * 
 *    Rev 1.12   01 Aug 1992 16:33:38   ADAM
 * Modified the behavior of fs_FindBitmapSize. It now returns the number
 * of bytes necessary for just 1 row in the YLines MemBase (BITMAP_PTR_2).
 * This is an improvement over the Apple design which forced the client to
 * allocate enough memory for the entire bitmap even if the client prefers
 * to band.
 * 
 *    Rev 1.11   31 Jul 1992 12:42:14   ADAM
 * Made bitmap far.
 * 
 *    Rev 1.10   30 Jul 1992 13:04:44   FRED
 * FIXROUND() is used to convert fixed values for the key fields
 * xPixelsPerEm and yPixelsPerEm.
 * 
 * 
 *    Rev 1.9   30 Jul 1992 12:50:18   FRED
 * Sets key fields xPixelsPerEm and yPixelsPerEm.
 * 
 * 
 *    Rev 1.8   29 Jul 1992 17:15:54   FRED
 * The NewTrans call now copies the xResolution, yResolution,
 * pattern, outline, shadow, and background fields to the key.
 * These values are used for special effects generation.
 * 
 * 
 *    Rev 1.7   14 Jul 1992 15:59:08   ADAM
 * made the PC_OS version perform banding.
 * 
 *    Rev 1.6   14 Jul 1992 11:54:50   ADAM
 * Fixed some PVCS comments
 * 
 *    Rev 1.5   14 Jul 1992 09:23:36   ADAM
 * Some more transformation stuff.
 * 
 *    Rev 1.4   13 Jul 1992 18:30:04   ADAM
 * Was not folding the scaling information into the transformation matrix.
 * Apparently Windows passes a pre-built transformation matrix that includes
 * point size and resolution information.
 * 
 *    Rev 1.3   13 Jul 1992 18:23:58   ADAM
 * Was returning non scaled delta x.
 * This is a good place to return both in the future.
 * 
 *    Rev 1.2   09 Jul 1992 17:36:56   ADAM
 * OpenFonts & Initialize were not being compiled because of PC_OS
*/

/*
	File:		FontScaler.c

	Contains:	xxx put contents here (or delete the whole line) xxx

	Written by:	xxx put name of writer here (or delete the whole line) xxx

	Copyright:	c 1988-1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<11>	11/27/90	MR		Need two scalars: one for (possibly rounded) outlines and cvt,
									and one (always fractional) metrics. [rb]
		<10>	11/21/90	RB		Allow client to disable DropOutControl by returning a NIL
									pointer to memoryarea[7]. Also make it clear that we inhibit
									DOControl whenever we band. [This is a reversion to 8, so mr's
									initials are added by proxy]
		 <9>	11/13/90	MR		(dnf) Revert back to revision 7 to fix a memmory-trashing bug
									(we hope). Also fix signed/unsigned comparison bug in outline
									caching.
		 <8>	11/13/90	RB		Fix banding so that we can band down to one row, using only
									enough bitmap memory and auxillary memory for one row.[mr]
		 <7>	 11/9/90	MR		Add Default return to fs_dropoutval. Continue to fiddle with
									banding. [rb]
		 <6>	 11/5/90	MR		Remove FixMath.h from include list. Clean up Stamp macros. [rb]
		 <5>	10/31/90	MR		Conditionalize call to ComputeMapping (to avoid linking
									MapString) [ha]
		 <4>	10/31/90	MR		Add bit-field option for integer or fractional scaling [rb]
		 <3>	10/30/90	RB		[MR] Inhibit DropOutControl when Banding
		 <2>	10/20/90	MR		Restore changes since project died. Converting to smart math
									routines, integer ppem scaling. [rb]
		<16>	 7/26/90	MR		don't include ToolUtils.h
		<15>	 7/18/90	MR		Fix return bug in GetAdvanceWidth, internal errors are now ints.
		<14>	 7/14/90	MR		remove unused fields from FSInfo
		<13>	 7/13/90	MR		Ansi-C fixes, rev. for union in FSInput
		<11>	 6/29/90	RB		Thus endeth the too long life of encryption
		<10>	 6/21/90	MR		Add calls to ReleaseSfntFrag
		 <9>	 6/21/90	RB		add scanKind info to fs_dropoutVal
		 <8>	  6/5/90	MR		remove fs_MapCharCodes
		 <7>	  6/1/90	MR		Did someone say MVT? Yuck!!! Out, damn routine.
		 <6>	  6/1/90	RB		fixed bandingbug under dropout control
		 <4>	  5/3/90	RB		added dropoutval function.  simplified restore outlines.
									support for new scanconverter in contourscan, findbitmapsize,
									saveoutlines, restoreoutlines.
		 <3>	 3/20/90	CL		Changed to use fpem (16.16) instead of pixelsPerEm (int) Removed
									call to AdjustTransformation (not needed with fpem) Added call
									to RunXFormPgm Removed WECANNOTDOTHIS #ifdef Added
									fs_MapCharCodes
		 <2>	 2/27/90	CL		New error code for missing but needed table. (0x1409).  New
									CharToIndexMap Table format.
									Fixed transformed component bug.
	   <3.6>	11/15/89	CEL		Put an else for the ifdef WeCanNotDoThis so Printer compile
									could use more effecient code.
	   <3.5>	11/14/89	CEL		Left Side Bearing should work right for any transformation. The
									phantom points are in, even for components in a composite glyph.
									They should also work for transformations. Device metric are
									passed out in the output data structure. This should also work
									with transformations. Another leftsidebearing along the advance
									width vector is also passed out. whatever the metrics are for
									the component at it's level. Instructions are legal in
									components. Instructions are legal in components. The
									transformation is internally automatically normalized. This
									should also solve the overflow problem we had. Now it is legal
									to pass in zero as the address of memory when a piece of the
									sfnt is requested by the scaler. If this happens the scaler will
									simply exit with an error code ! Five unnecessary element in the
									output data structure have been deleted. (All the information is
									passed out in the bitmap data structure) fs_FindBMSize now also
									returns the bounding box.
	   <3.4>	 9/28/89	CEL		fs_newglyph did not initialize the output error. Caused routine
									to return error from previous routines.
	   <3.3>	 9/27/89	CEL		Took out devAdvanceWidth & devLeftSideBearing.
	   <3.2>	 9/25/89	CEL		Changed the NEED_PROTOTYPE ifdef to use the NOT_ON_THE_MAC flag
									that existed previously.
	   <3.1>	 9/15/89	CEL		Changed dispatch scheme. Calling conventions through a trap
									needed to match Macintosh pascal. Pascal can not call C unless
									there is extra mucky glue. Bug that caused text not to appear.
									The font scaler state was set up correctly but the sfnt was
									purged. It was reloaded and the clientid changed but was still
									the same font. Under the rules of the FontScaler fs_newsfnt
									should not have to be called again to reset the state. The extra
									checks sent back a BAD_CLIENTID_ERROR so QuickDraw would think
									it was a bad font and not continue to draw.
	   <3.0>	 8/28/89	sjk		Cleanup and one transformation bugfix
	   <2.4>	 8/17/89	sjk		Coded around MPW C3.0 bug
	   <2.3>	 8/14/89	sjk		1 point contours now OK
	   <2.2>	  8/8/89	sjk		Improved encryption handling
	   <2.1>	  8/2/89	sjk		Fixed outline caching bug
	   <2.0>	  8/2/89	sjk		Just fixed EASE comment
	   <1.5>	  8/1/89	sjk		Added composites and encryption. Plus some enhancements.
	   <1.4>	 6/13/89	SJK		Comment
	   <1.3>	  6/2/89	CEL		16.16 scaling of metrics, minimum recommended ppem, point size 0
									bug, correct transformed integralized ppem behavior, pretty much
									so
	   <1.2>	 5/26/89	CEL		EASE messed up on "c" comments
	  <y1.1>	 5/26/89	CEL		Integrated the new Font Scaler 1.0 into Spline Fonts
	   <1.0>	 5/25/89	CEL		Integrated 1.0 Font scaler into Bass code for the first time.

	To Do:
*/
/*		<3+>	 3/20/90	mrr		Conditionalized error checking in fs_SetUpKey.
									Compiler option for stamping memmory areas for debugging
									Removed error field from FSInfo structure.
									Added call to RunFontProgram
									Added private function prototypes.
									Optimizations from diet clinic

*/

/** FontScaler's Includes **/

#define __FSCALER_C__

#include "fserror.h"    
#include "fscdefs.h"
#include "fontmath.h"
#include "sfnt.h"
#include "fnt.h"
#include "sc.h"
#include "fsglue.h"
#include "fscaler.h"
#include "privsfnt.h"
#include "effects.h"
#include <math.h>


extern void sfnt_GetGlyphLocation(fsg_SplineKey* key, uint16 gIndex, uint32* offset, unsigned *length, sfnt_tableIndex* pGlyphTableIndex);
extern voidPtr sfnt_GetDataPtr(fsg_SplineKey* key, uint32 offset, unsigned length, sfnt_tableIndex n);


extern void set_pattern(int16 pattern);
extern void set_pen(int16 pen);
extern void make_contour_pens(void);
extern void make_shadow_pens(void);
extern void compute_eae_values(fsg_SplineKey* key);
extern FX Fx;


#define _PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862
double const PI = _PI;
double const PI2 = _PI*2.0;
double const DEG2RADC = _PI/180.0;
double const RAD2DEGC = 180.0/_PI;


#define LOOPDOWN(n)		for (--n; n >= 0; --n)

#define OUTLINEBIT    0x02

#define SETJUMP(key, error)     if ((error = fs_setjmp(key->env)) != 0) return(error)

#ifdef SEGMENT_LINK
#pragma segment FONTSCALER_C
#endif


#define PRIVATE

/* PRIVATE PROTOTYPES */
fsg_SplineKey* fs_SetUpKey(fs_GlyphInputType* inptr, unsigned stateBits, int* error);
int fs__Contour(fs_GlyphInputType *inputPtr, fs_GlyphInfoType *outputPtr, boolean useHints);
int32 fs_SetSplineDataPtrs(fs_GlyphInputType*inputPtr, fs_GlyphInfoType *outputPtr);
int32 fs_SetInternalOffsets(fs_GlyphInputType*inputPtr, fs_GlyphInfoType *outputPtr);
void fs_45DegreePhaseShift(sc_CharDataType *glyphPtr);
int32 fs_dropOutVal(fsg_SplineKey *key);

#ifdef  DEBUGSTAMP

  #define STAMPEXTRA              4
  #define STAMP                   'sfnt'

  void SETSTAMP(Ptr p)
  {
    * ((int32 *) ((p) - STAMPEXTRA)) = STAMP;
  }


  void CHECKSTAMP(Ptr p)
  {
    if (* ((int32 *) ((p) - STAMPEXTRA)) != STAMP)
      Debugger();
  }

#else

  #define STAMPEXTRA    0
  #define SETSTAMP(p)
  #define CHECKSTAMP(p)

#endif

struct fsg_SplineKey * GlobalKey;

#ifdef PC_OS

#ifdef PMW

#define FS_SETUPKEY(state) \
	register fsg_SplineKey*key = (fsg_SplineKey *)inputPtr->memoryBases[KEY_PTR_BASE]; \
	key->GetSfntFragmentPtr = inputPtr->GetSfntFragmentPtr; \
	key->ReleaseSfntFrag = inputPtr->ReleaseSfntFrag;
#define SET_STATE(state)

#else

#define FS_SETUPKEY(state)   register fsg_SplineKey*key = (fsg_SplineKey *)inputPtr->memoryBases[KEY_PTR_BASE]; 
#define SET_STATE(state)

#endif

#else

#define FS_SETUPKEY(state) \
  register fsg_SplineKey*key = fs_SetUpKey(inputPtr, state, &error);\
  if (!key) \
    return error;


void dummyReleaseSfntFrag(voidPtr p);
void dummyReleaseSfntFrag(voidPtr p)
{
}

#define SET_STATE(s)  key->state = (s)
#endif

/*
 *	Set up the key in case memmory has moved or been purged.
 */
#ifndef PC_OS
fsg_SplineKey*fs_SetUpKey(register fs_GlyphInputType*inptr, register unsigned stateBits, int*error)
{
  register fsg_SplineKey*key;

  key = (fsg_SplineKey *)inptr->memoryBases[KEY_PTR_BASE];
  key->memoryBases = inptr->memoryBases;
  key->GetSfntFragmentPtr = inptr->GetSfntFragmentPtr;

#ifdef RELEASE_MEM_FRAG
    if (!(key->ReleaseSfntFrag = inptr->ReleaseSfntFrag))
      key->ReleaseSfntFrag = dummyReleaseSfntFrag;
#else
    key->ReleaseSfntFrag = dummyReleaseSfntFrag;
#endif

  if ((key->state & stateBits) != stateBits)
  {
    *error = OUT_OFF_SEQUENCE_CALL_ERR;
    return 0;
  }

  key->clientID = inptr->clientID;
  *error = NO_ERR;

  return key;
}
#endif

/*** FONT SCALER INTERFACE ***/

/*
 *
 */

FS_ENTRY fs_OpenFonts(fs_GlyphInputType *inputPtr, fs_GlyphInfoType *outputPtr)
{
  outputPtr->memorySizes[KEY_PTR_BASE]                 = fsg_KeySize() + STAMPEXTRA;
  outputPtr->memorySizes[VOID_FUNC_PTR_BASE]           = fsg_InterPreterDataSize() + STAMPEXTRA;
  outputPtr->memorySizes[SCAN_PTR_BASE]                = fsg_ScanDataSize() + STAMPEXTRA;
  return NO_ERR;
}

FS_ENTRY fs_Initialize(fs_GlyphInputType *inputPtr, fs_GlyphInfoType *outputPtr)
{
  register fsg_SplineKey 			*key;

    SETSTAMP(inputPtr->memoryBases[0] + outputPtr->memorySizes[0]);
    SETSTAMP(inputPtr->memoryBases[2] + outputPtr->memorySizes[2]);

    key = (fsg_SplineKey *)inputPtr->memoryBases[KEY_PTR_BASE];
    key->memoryBases = inputPtr->memoryBases;

	GlobalKey = key;

    SET_STATE(INITIALIZED);
    CHECKSTAMP(inputPtr->memoryBases[0] + outputPtr->memorySizes[0]);
    CHECKSTAMP(inputPtr->memoryBases[2] + outputPtr->memorySizes[2]);


	 /* Allow the client access to the Key */

	 outputPtr->Key	= key;


    return NO_ERR;
}

/*
 *	This guy asks for memmory for points, instructions, fdefs and idefs
 */
FS_ENTRY fs_NewSfnt(fs_GlyphInputType *inputPtr, fs_GlyphInfoType *outputPtr)
{
  int   error;

  FS_SETUPKEY(INITIALIZED);

#ifdef PC_OS
  key->clientID = inputPtr->clientID;
  key->memoryBases = inputPtr->memoryBases;
#endif

  SETJUMP(key, error);

  CHECKSTAMP(inputPtr->memoryBases[0] + outputPtr->memorySizes[0]);
  CHECKSTAMP(inputPtr->memoryBases[1] + outputPtr->memorySizes[1]);
  CHECKSTAMP(inputPtr->memoryBases[2] + outputPtr->memorySizes[2]);

  sfnt_DoOffsetTableMap(key);					/* Map offset and length table */

  {
    sfnt_FontHeaderPtr fontHead = (sfnt_FontHeaderPtr)sfnt_GetTablePtr(key, sfnt_fontHeader, true);
    sfnt_HorizontalHeaderPtr horiHead = (sfnt_HorizontalHeaderPtr)sfnt_GetTablePtr(key, sfnt_horiHeader, true);

    if (SWAPL(fontHead->magicNumber) != SFNT_MAGIC) 
    { 
      return BAD_MAGIC_ERR; 
    }
    key->emResolution            = SWAPW(fontHead->unitsPerEm);
    key->fontFlags               = SWAPW(fontHead->flags);
    key->numberOf_LongHorMetrics = SWAPW(horiHead->numberOf_LongHorMetrics);
    key->indexToLocFormat        = fontHead->indexToLocFormat;

		/*
		// Save the Em resolution in a spot that the effects code can
		// get to easily.
		*/

		Fx.fx_em_resolution = key->emResolution;

		/*
		// Move the global bbox into the key structure. The resolution of
		// this bbox is defined in emResolution. Note that these values
		// do not include any extended attribute escapements.
		*/

		key->GlobalBBox.xMin	= SWAPW(fontHead->xMin);
		key->GlobalBBox.yMin	= SWAPW(fontHead->yMin);
		key->GlobalBBox.xMax	= SWAPW(fontHead->xMax);
		key->GlobalBBox.yMax	= SWAPW(fontHead->yMax);

		/*
		// Mimic the MoreFonts/Geronimo behavior of make the global BBOX
		// the union of the BBOX read from the font file and the BBOX of
		// SYMBOL.OTL.
		*/

		{
			#define	SYMBOL_OTL_xMin	-166
			#define	SYMBOL_OTL_xMax	 996
			#define	SYMBOL_OTL_yMin	-302
			#define	SYMBOL_OTL_yMax	 968

			#define	THOUSAND2EM(a)	(int32)((int32)((int32)(a)*(int32)(key->emResolution))/1000L)

			int32 n;

			n = THOUSAND2EM(SYMBOL_OTL_xMin);
			if (key->GlobalBBox.xMin > n) key->GlobalBBox.xMin = n;

			n = THOUSAND2EM(SYMBOL_OTL_xMax);
			if (key->GlobalBBox.xMax < n) key->GlobalBBox.xMax = n;

			n = THOUSAND2EM(SYMBOL_OTL_yMin);
			if (key->GlobalBBox.yMin > n) key->GlobalBBox.yMin = n;

			n = THOUSAND2EM(SYMBOL_OTL_yMax);
			if (key->GlobalBBox.yMax < n) key->GlobalBBox.yMax = n;
		}
		
	 RELEASESFNTFRAG(key, horiHead);
    RELEASESFNTFRAG(key, fontHead);
  }

  {
    voidPtr p = sfnt_GetTablePtr(key, sfnt_maxProfile, true);
	 key->maxProfile = * ((sfnt_maxProfileTablePtr) p);

#ifndef FSCFG_BIG_ENDIAN
    {
      int16 * p;
      for (p = (int16 *) & key->maxProfile; p < (int16 *) & key->maxProfile + sizeof(key->maxProfile) / sizeof(int16); p++)
		*p = SWAPW(*p);
    }
#endif

	key->maxProfile.maxContours += 2;
	key->maxProfile.maxPoints += 8;


    RELEASESFNTFRAG(key, p);
  }

  outputPtr->memorySizes[PRIVATE_FONT_SPACE_BASE]	 = fsg_PrivateFontSpaceSize(key) + STAMPEXTRA;
  outputPtr->memorySizes[WORK_SPACE_BASE]           = fsg_WorkSpaceSetOffsets(key) + STAMPEXTRA;

#ifdef FSCFG_USE_GLYPH_DIRECTORY
  key->mappingF = 0;
#else 
  if ((error = sfnt_ComputeMapping(key, inputPtr->param.newsfnt.platformID, inputPtr->param.newsfnt.specificID)) != 0)
    return error;
#endif 

	/*
	// Extract the space width for the new font and save it in the Fx
	// structure.
	*/

	if (Fx.fx_is_symbol_font)
	{
		/*
		// Because of the wildly different space widths of various mfg's
		// symbol fonts, MLS sets the correct space width here.
		*/

		Fx.fx_em_space_width = key->emResolution >> 2;
	}

	else
	{
		uint8 * mappingPtr = (uint8 *)sfnt_GetTablePtr(key, sfnt_charToIndexMap, true);

		uint16 glyphIndex =
			key->mappingF(mappingPtr + key->mappOffset, 0x0020);

		RELEASESFNTFRAG(key, mappingPtr);

		key->nonScaledAW = ADVANCE_WIDTH_UNKNOWN;		/* force read */
		sfnt_ReadSFNTMetrics(key, glyphIndex);

		Fx.fx_em_space_width = key->nonScaledAW;

	}


  SET_STATE(INITIALIZED | NEWSFNT);
  key->scanControl = 0;

/*
	 *	Can't run font program yet, we don't have any memmory for the graphic state
	 *	Mark it to be run in NewTransformation.
	 */
  key->executeFontPgm = true;

#ifdef DEBUGSTAMP
  CHECKSTAMP(inputPtr->memoryBases[0] + outputPtr->memorySizes[0]);
  CHECKSTAMP(inputPtr->memoryBases[1] + outputPtr->memorySizes[1]);
  CHECKSTAMP(inputPtr->memoryBases[2] + outputPtr->memorySizes[2]);
#endif
  return NO_ERR;
}


#define	MAX(a, b)		((a)>(b)?(a):(b))
#define	ABS(a)			((a)<0?-(a):(a))


FS_ENTRY fs_NewTransformation(fs_GlyphInputType *inputPtr, fs_GlyphInfoType *outputPtr)
{
	Fixed xStretch, yStretch;
	int16 error;

	FS_SETUPKEY(INITIALIZED | NEWSFNT);

	SETJUMP(key, error);

	xStretch = inputPtr->param.newtrans.transformMatrix->transform[0][0];
	yStretch = inputPtr->param.newtrans.transformMatrix->transform[1][1];

	/*
	// Copy some of the input parameters into the Fx structure where
	// we have easy access to them.
	*/

	Fx.fx_integer_point_size = HIWORD(inputPtr->param.newtrans.pointSize);
	Fx.fx_x_resolution       = inputPtr->param.newtrans.xResolution;
	Fx.fx_y_resolution       = inputPtr->param.newtrans.yResolution;

	Fx.fx_Fill					 = inputPtr->param.newtrans.Fill;
	Fx.fx_Outline				 = inputPtr->param.newtrans.Outline;
	Fx.fx_Shadow				 = inputPtr->param.newtrans.Shadow;

	Fx.fx_effects_rotation   = inputPtr->param.newtrans.effects_rotation;

	Fx.fx_dRotation = (((double)Fx.fx_effects_rotation)/65536.0)*DEG2RADC;
	Fx.fx_dCosRotation = cos(Fx.fx_dRotation);
	Fx.fx_dSinRotation = sin(Fx.fx_dRotation);

	// If the point size is zero, then choose a point size and
	// resolution that will force all metrics to Em resolution:
	//
	//		X resolution = Em Resolution
	//		Y resolution = Em Resolution
	//		Point Size   = POINTSPERINCH

	if (Fx.fx_integer_point_size == 0)
	{
		inputPtr->param.newtrans.pointSize   = (uint32)POINTSPERINCH << 16;
		inputPtr->param.newtrans.xResolution = 1000;
		inputPtr->param.newtrans.yResolution = 1000;
	}

#ifdef PC_OS
	/* Setup the font offsets */
	if (key->offset_storage == 0)
	{
		int8 **pu = (int8 **) &key->offset_storage, **puEnd;
		int8 *base = inputPtr->memoryBases[PRIVATE_FONT_SPACE_BASE];

		while (pu <= (int8 **) &key->offset_PreProgram)
		{
			*pu++ = base + (unsigned)(long)*pu;
		}

		pu    = (int8 **) &key->elementInfoRec.offsets;
		puEnd = (int8 **) ((int8 *) pu + sizeof(fsg_OffsetInfo) * MAX_ELEMENTS);
		base  = key->memoryBases[WORK_SPACE_BASE];

		while (pu < puEnd)
		{
			*pu++ = base + (unsigned)(long)*pu;
		}
	}
#endif

	/* Load the font program and pre program if necessary */
	if (key->executeFontPgm)
	{
		fnt_GlobalGraphicStateType *globalGS = (fnt_GlobalGraphicStateType *) FONT_OFFSET(key->memoryBases[PRIVATE_FONT_SPACE_BASE], key->offset_globalGS);

		fsg_SetUpProgramPtrs(key, globalGS, FONTPROGRAM);
		fsg_SetUpProgramPtrs(key, globalGS, PREPROGRAM);
	}

	key->currentTMatrix = *inputPtr->param.newtrans.transformMatrix;
	key->fixedPointSize = inputPtr->param.newtrans.pointSize;
	key->pixelDiameter  = inputPtr->param.newtrans.pixelDiameter;


	/*
	// Fold the point size and resolution into the matrix.
	*/

	Fixed scale;

	scale = ShortMulDiv(key->fixedPointSize, inputPtr->param.newtrans.yResolution, POINTSPERINCH);
	key->currentTMatrix.transform[0][1] = FixMul( key->currentTMatrix.transform[0][1], scale );
	key->currentTMatrix.transform[1][1] = FixMul( key->currentTMatrix.transform[1][1], scale );
	key->currentTMatrix.transform[2][1] = FixMul( key->currentTMatrix.transform[2][1], scale );


	scale = ShortMulDiv(key->fixedPointSize, inputPtr->param.newtrans.xResolution, POINTSPERINCH);
	key->currentTMatrix.transform[0][0] = FixMul( key->currentTMatrix.transform[0][0], scale );
	key->currentTMatrix.transform[1][0] = FixMul( key->currentTMatrix.transform[1][0], scale );
	key->currentTMatrix.transform[2][0] = FixMul( key->currentTMatrix.transform[2][0], scale );

	Fx.fx_x_pixels_per_em = FIXROUND(FixMul(scale, xStretch));
	Fx.fx_y_pixels_per_em = FIXROUND(FixMul(scale, yStretch));

	/*
	// Modifies key->fpem and key->currentTMatrix.
	*/

	fsg_ReduceMatrix(key);

	key->ScalingMatrix = key->currentTMatrix;

	/* get premultipliers if any, also called in sfnt_ReadSFNT */
	fsg_InitInterpreterTrans(key, &key->interpScalarX, &key->interpScalarY, &key->metricScalarX, &key->metricScalarY);

	/*************************************************************************/
	/*  At this point, we have                                               */
	/*     fixedPointSize = user defined fixed                               */
	/*     metricScalarX  = fixed scaler for scaling metrics                 */
	/*     interpScalarX  = fixed scaler for scaling outlines/CVT            */
	/*     pixelDiameter  = user defined fixed                               */
	/*     currentTMatrix = 3x3 user transform and non-squareness resolution */
	/*************************************************************************/

	/*
	//	This guy defines FDEFs and IDEFs.  The table is optional.
	*/

	if (key->executeFontPgm)
	{
		if ((error = fsg_RunFontProgram(key)) != 0)
			return error;
		key->executeFontPgm = false;
	}

#ifndef FSCFG_NO_TRACE
	if (!(key->executePrePgm = (boolean) !inputPtr->param.newtrans.traceFunc))
	{
		/*
		// Do this now so we do not confuse font editors.
		// Run the pre program and scale the control value table.
		// Sets key->executePrePgm to false.
		*/

		if ((error = fsg_RunPreProgram(key, inputPtr->param.newtrans.traceFunc)) != 0)
			return error;
	}
#else
	key->executePrePgm = 1;
#endif



	SET_STATE(INITIALIZED | NEWSFNT | NEWTRANS);

	outputPtr->scaledCVT = (F26Dot6 *) FONT_OFFSET(key->memoryBases[PRIVATE_FONT_SPACE_BASE], key->offset_controlValues);






	/*
	// Initialize the fx_cell_ascent, fx_cell_height, fx_cell_width,
	// fx_em_cell_height, and fx_em_cell_width values in the Fx structure.
	// These values are based on the black version of the font with no
	// special effects.
	*/

	Fx.fx_em_cell_height = key->GlobalBBox.yMax-key->GlobalBBox.yMin;
	Fx.fx_em_cell_width  = key->GlobalBBox.xMax-key->GlobalBBox.xMin;
	Fx.fx_cell_ascent = FIXROUND(LongMulDiv(key->metricScalarY, key->GlobalBBox.yMax, key->emResolution));
	Fx.fx_cell_height = FIXROUND(LongMulDiv(key->metricScalarY, key->GlobalBBox.yMax-key->GlobalBBox.yMin, key->emResolution));
	Fx.fx_cell_width  = FIXROUND(LongMulDiv(key->metricScalarX, key->GlobalBBox.xMax-key->GlobalBBox.xMin, key->emResolution));

	/*
	// Initialize and contour and shadow pens required to build the glyph.
	// The calls to set_pen() are not because we are about to draw anything
	// (thats done by ContourScan()), but because shadow pens depend on
	// a what outline pen is being used.
	*/

	if (IS_CONTOUR_OUTLINE(Fx.fx_Outline.m_nStyle))
	{
		make_contour_pens();
	}

	if (IS_SHADOW(Fx.fx_Shadow.m_nStyle))
	{
		if (IS_CONTOUR_OUTLINE(Fx.fx_Outline.m_nStyle))
		{
			set_pen((int16)MLS_OUTLINE_contour_outline_pen);
		}

		else
		{
			set_pen(Fx.fx_Outline.m_nStyle);
		}

		make_shadow_pens();
	}






	/*
	// We have all the information needed for computing the EAE
	// values.
	//
	// Note that EAE's are glyph independent (same for A, B, ...). That's why
	// we can compute them in NewTrans().
	*/

	compute_eae_values(key);

	/*
	// Initialize the following values in the key for the client:
	//
	//		SpaceWidth_EAE
	//
	//			The dot resolution space width with dot EAE's.
	//
	//		SpacingType
	//
	//			0 => fixed pitch
	//			1 => portrait
	//
	//		UnderlineDistance
	//
	//			The position to draw an underline for the typeface.
	//			This value is typically negative (below the baseline).
	//
	//		GlobalBBox_EAE
	//
	//			The global BBOX for the typeface at dot resolution
	//			with dot EAE's.
	*/

	/*
	// SpaceWidth_EAE - Use the Em space width saved by the NewSFnt() call
	// scaled to the requested size and adjusted by the EAE width value.
	*/

	key->SpaceWidth_EAE = FIXROUND(LongMulDiv(key->metricScalarX, Fx.fx_em_space_width, key->emResolution))+Fx.fx_eae_width;

	/*
	// UnderlineDistance - This is a canned value (MLS_UNDERLINE_DISTANCE)
	// scaled to the proper value.
	*/

	key->UnderlineDistance = FIXROUND(LongMulDiv(key->metricScalarY, MLS_UNDERLINE_DISTANCE, 1000));

	/*
	// The X coordinates are the scaled global BBOX X coordinates
	// adjusted by the dot resolution EAE's.
	*/

	key->GlobalBBox_EAE.xMin = FIXROUND(LongMulDiv(key->metricScalarX, key->GlobalBBox.xMin, key->emResolution))-Fx.fx_eae.xMin;
	key->GlobalBBox_EAE.xMax = FIXROUND(LongMulDiv(key->metricScalarX, key->GlobalBBox.xMax, key->emResolution))+Fx.fx_eae.xMax;
	key->GlobalBBox_EAE.yMin = Fx.fx_cell_ascent-Fx.fx_cell_height-Fx.fx_eae.yMin;
	key->GlobalBBox_EAE.yMax = Fx.fx_cell_ascent+Fx.fx_eae.yMax;

#ifdef DEBUGSTAMP
	CHECKSTAMP(inputPtr->memoryBases[0] + outputPtr->memorySizes[0]);
	CHECKSTAMP(inputPtr->memoryBases[2] + outputPtr->memorySizes[2]);
	CHECKSTAMP(inputPtr->memoryBases[3] + outputPtr->memorySizes[3]);
	CHECKSTAMP(inputPtr->memoryBases[4] + outputPtr->memorySizes[4]);
#endif		

	return NO_ERR;
}


/*
 * Compute the glyph index from the character code.
 */
FS_ENTRY fs_NewGlyph(fs_GlyphInputType *inputPtr, fs_GlyphInfoType *outputPtr)
{
	int error;
	FS_SETUPKEY(0);
	SETJUMP(key, error);

#ifdef DEBUGSTAMP
	CHECKSTAMP(inputPtr->memoryBases[0] + outputPtr->memorySizes[0]);
	CHECKSTAMP(inputPtr->memoryBases[2] + outputPtr->memorySizes[2]);
	CHECKSTAMP(inputPtr->memoryBases[3] + outputPtr->memorySizes[3]);
	CHECKSTAMP(inputPtr->memoryBases[4] + outputPtr->memorySizes[4]);
#endif

	SET_STATE(INITIALIZED | NEWSFNT | NEWTRANS);  /* clear all other bits */

	/* Initialize advance width and glyph loc (1/12/93 MH) */
	key->nonScaledAW = ADVANCE_WIDTH_UNKNOWN;
	key->GlyphOffset = GLYPH_OFFSET_UNKNOWN;

	if (inputPtr->param.newglyph.characterCode != NONVALID)
	{
		int32 xmin, ymin, xmax, ymax;

		/*
		// Save the character code in the Fx structure so we can get
		// to it easily.
		*/

		Fx.fx_character_code = inputPtr->param.newglyph.characterCode;

		/*
		// Set the MLS dot resolution character bounding box in
		// Character_BBox_EAE. The bounding box data is read from
		// the glyph table and scaled using the current transformation.
		// The result is adjusted by the dot EAE values. Note that
		// this bounding box may be inaccurate because the glyph
		// has not been adjusted with the hinting program. If a dot
		// accurate bounding box is needed, then FindBitmapSize() must
		// be called.
		*/

		uint8 * mappingPtr = (uint8 *)sfnt_GetTablePtr(key, sfnt_charToIndexMap, true);

		uint16 glyphIndex = key->mappingF(mappingPtr + key->mappOffset, inputPtr->param.newglyph.characterCode);

		unsigned length;
		uint32 offset;
		sfnt_tableIndex glyphTableIndex;

		outputPtr->glyphIndex = glyphIndex;
		key->glyphIndex = glyphIndex;

		RELEASESFNTFRAG(key, mappingPtr);

		/*
		// The character does not exist. Attempt to read its bounding
		// box from the glyph table.
		*/

		/*
		// Get the location of the glyph table.
		*/

		sfnt_GetGlyphLocation(key, key->glyphIndex, &offset, &length, &glyphTableIndex);

		/*
		// Make sure the glyph was found.
		*/

		if (length >= 5*sizeof(uint16))
		{
			voidPtr glyphDataPtr;
			int16 *p;

			/*
			// Read in the data. Only the first five words are read. The
			// first word is the number of contours which is followed by
			// the bounding box: xmin, ymin, xmax, ymax.
			*/

			glyphDataPtr = sfnt_GetDataPtr(key, offset, 5*sizeof(uint16), glyphTableIndex);

			p = ((int16 *)glyphDataPtr)+1;

			/*
			// Unpack the bounding box (the data's in 68000 format).
			*/

			xmin = SWAPWINC(p);
			ymin = SWAPWINC(p);
			xmax = SWAPWINC(p);
			ymax = SWAPWINC(p);

			/*
			// Free up the glyph data, we're all done with it.
			*/

			RELEASESFNTFRAG(key, glyphDataPtr);

			/*
			// Initialize the bounding box. Simply scale the values
			// from the glyph table and adjust them with the dot EAE values.
			*/

			xmin = FIXROUND(LongMulDiv(key->metricScalarX, xmin, key->emResolution));
			xmax = FIXROUND(LongMulDiv(key->metricScalarX, xmax, key->emResolution));

			key->CharacterBBox_EAE.yMin = FIXROUND(LongMulDiv(key->metricScalarY, ymin, key->emResolution));
			key->CharacterBBox_EAE.yMax = FIXROUND(LongMulDiv(key->metricScalarY, ymax, key->emResolution));
		}

		/*
		// We can now set the X coordinates of CharacterBBox_EAE using
		// the xmin and xmax values computed above. These are the
		// final values except for the EAE adjustments.
		*/

		key->CharacterBBox_EAE.xMin = xmin-Fx.fx_eae.xMin;
		key->CharacterBBox_EAE.xMax = xmax+Fx.fx_eae.xMax;

		/*
		// Adjust the Y coordinates by the EAE values.
		*/

		key->CharacterBBox_EAE.yMin -= Fx.fx_eae.yMin;
		key->CharacterBBox_EAE.yMax += Fx.fx_eae.yMax;

		/*
		// Initialize the effects variables that depend on the
		// character bounding box. Note that the Y coordinates
		// are inverted to correspond to the MoreFonts/Geronimo
		// convention. These values are used by the function
		// patterns which receive all coordinates in this
		// orientation.
		*/

		Fx.fx_char_x0 =  key->CharacterBBox_EAE.xMin;
		Fx.fx_char_y0 = -key->CharacterBBox_EAE.yMax;
		Fx.fx_char_x1 =  key->CharacterBBox_EAE.xMax;
		Fx.fx_char_y1 = -key->CharacterBBox_EAE.yMin;
	}

	else
	{
		key->glyphIndex = outputPtr->glyphIndex = inputPtr->param.newglyph.glyphIndex;
	}

#ifdef DEBUGSTAMP
	CHECKSTAMP(inputPtr->memoryBases[0] + outputPtr->memorySizes[0]);
	CHECKSTAMP(inputPtr->memoryBases[2] + outputPtr->memorySizes[2]);
	CHECKSTAMP(inputPtr->memoryBases[3] + outputPtr->memorySizes[3]);
	CHECKSTAMP(inputPtr->memoryBases[4] + outputPtr->memorySizes[4]);
#endif

	return NO_ERR;
}


/*
 * this call is optional.
 *
 * can be called right after fs_NewGlyph()
 */
FS_ENTRY fs_GetAdvanceWidth(fs_GlyphInputType *inputPtr, fs_GlyphInfoType *outputPtr)
{
	int error;
	FS_SETUPKEY(INITIALIZED | NEWSFNT | NEWTRANS);
	SETJUMP(key, error);

	sfnt_ReadSFNTMetrics(key, key->glyphIndex);

#if 0
	outputPtr->metricInfo.advanceWidth.x = key->nonScaledAW;
#else
	outputPtr->metricInfo.advanceWidth.y = 0;

	if (key->identityTransformation)
	{
		outputPtr->metricInfo.advanceWidth.x = ShortMulDiv( key->metricScalarX, key->nonScaledAW, key->emResolution );
	}

	else
	{
		outputPtr->metricInfo.advanceWidth.x = FixDiv( key->nonScaledAW, key->emResolution );
		fsg_FixXYMul( &outputPtr->metricInfo.advanceWidth.x, &outputPtr->metricInfo.advanceWidth.y, &key->currentTMatrix );
	}
#endif

	// Set the delta X values for the character:
	//
	//		DeltaX_EAE is the scaled Em resolution Delta X.

	key->DeltaX_EAE = FIXROUND(ShortMulDiv(key->metricScalarX, key->nonScaledAW, key->emResolution))+Fx.fx_eae_width;

	return NO_ERR;
}


int fs__Contour(fs_GlyphInputType*inputPtr, fs_GlyphInfoType*outputPtr, boolean useHints)
{
#ifndef PC_OS
  register int8				*workSpacePtr;
#endif
  register fsg_OffsetInfo		*offsetPtr;
  int	error;

  FS_SETUPKEY(INITIALIZED | NEWSFNT | NEWTRANS);
  SETJUMP(key, error);

#ifdef DEBUGSTAMP
  CHECKSTAMP(inputPtr->memoryBases[0] + outputPtr->memorySizes[0]);
  CHECKSTAMP(inputPtr->memoryBases[2] + outputPtr->memorySizes[2]);
  CHECKSTAMP(inputPtr->memoryBases[3] + outputPtr->memorySizes[3]);
  CHECKSTAMP(inputPtr->memoryBases[4] + outputPtr->memorySizes[4]);
#endif
/* potentially do delayed pre program execution */
  if (key->executePrePgm /*&& useHints*/)
  {
/* Run the pre program and scale the control value table */
    key->executePrePgm = false;
    if ((error = fsg_RunPreProgram(key, 0)) != 0)
      return error;
  }

	/* Never do hints if F26dot6 precision is not 6 */
	if (_PRECISION != 6)
		useHints = false;


  if ((error = fsg_GridFit(key, inputPtr->param.gridfit.traceFunc, useHints)) != 0) 	/* THE CALL */
    return error;

#ifndef PC_OS
  workSpacePtr        = key->memoryBases[WORK_SPACE_BASE];
#endif
  offsetPtr           = &(key->elementInfoRec.offsets[1]);

  outputPtr->xPtr     = (F26Dot6 *) FONT_OFFSET(workSpacePtr, offsetPtr->x);
  outputPtr->yPtr     = (F26Dot6 *) FONT_OFFSET(workSpacePtr, offsetPtr->y);
  outputPtr->startPtr = (int16 *) FONT_OFFSET(workSpacePtr, offsetPtr->sp);
  outputPtr->endPtr   = (int16 *) FONT_OFFSET(workSpacePtr, offsetPtr->ep);
  outputPtr->onCurve  = (uint8 *) FONT_OFFSET(workSpacePtr, offsetPtr->onCurve);
  outputPtr->numberOfContours	 = key->elementInfoRec.interpreterElements[GLYPHELEMENT].nc;

#if 1
  {
    register metricsType*metric = &outputPtr->metricInfo;
    int      numPts = outputPtr->endPtr[outputPtr->numberOfContours-1] + 1 + PHANTOMCOUNT;
    register unsigned index1 = numPts-PHANTOMCOUNT + RIGHTSIDEBEARING;
    register unsigned index2 = numPts-PHANTOMCOUNT + LEFTSIDEBEARING;

    metric->devAdvanceWidth.x     = DOT6TOFIX(outputPtr->xPtr[index1] - outputPtr->xPtr[index2]);
    metric->devAdvanceWidth.y     = DOT6TOFIX(outputPtr->yPtr[index1] - outputPtr->yPtr[index2]);
  }

#endif

  outputPtr->scaledCVT = (F26Dot6 *) FONT_OFFSET(key->memoryBases[PRIVATE_FONT_SPACE_BASE], key->offset_controlValues);

  outputPtr->outlinesExist = (uint16) (key->glyphLength != 0);

  SET_STATE(INITIALIZED | NEWSFNT | NEWTRANS | GOTGLYPH);

#ifdef DEBUGSTAMP
  CHECKSTAMP(inputPtr->memoryBases[0] + outputPtr->memorySizes[0]);
  CHECKSTAMP(inputPtr->memoryBases[1] + outputPtr->memorySizes[1]);
  CHECKSTAMP(inputPtr->memoryBases[2] + outputPtr->memorySizes[2]);
  CHECKSTAMP(inputPtr->memoryBases[3] + outputPtr->memorySizes[3]);
  CHECKSTAMP(inputPtr->memoryBases[4] + outputPtr->memorySizes[4]);
#endif
  return NO_ERR;
}

FS_ENTRY fs_ContourNoGridFit(fs_GlyphInputType *inputPtr, fs_GlyphInfoType *outputPtr)
{
  return fs__Contour(inputPtr, outputPtr, false);
}


FS_ENTRY fs_ContourGridFit(fs_GlyphInputType *inputPtr, fs_GlyphInfoType *outputPtr)
{
  return fs__Contour(inputPtr, outputPtr, true);
}

// Let the client specify the outline data.
//
// Pointers to the parts of the outline data are saved in the input structure.
// The outline data memory is owned by the client.

FS_ENTRY fs_NewOutline(fs_GlyphInputType *inputPtr, fs_GlyphInfoType *outputPtr)
{
	int error;
	FS_SETUPKEY(0);
	SETJUMP(key, error);

	register fnt_ElementType  *elementPtr;
	elementPtr = &(key->elementInfoRec.interpreterElements[GLYPHELEMENT]);
	elementPtr->x = inputPtr->param.newoutline.x;
	elementPtr->y = inputPtr->param.newoutline.y;
	elementPtr->ox = NULL;
	elementPtr->oy = NULL;
	elementPtr->oox = NULL;
	elementPtr->ooy = NULL;
	elementPtr->onCurve = inputPtr->param.newoutline.onCurve;
	elementPtr->sp = inputPtr->param.newoutline.sp;
	elementPtr->ep = inputPtr->param.newoutline.ep;
	elementPtr->f = NULL;
	elementPtr->nc = inputPtr->param.newoutline.nc;

	if (inputPtr->param.newoutline.fx_use_new_metrics)
	{
		Fx.fx_cell_height = inputPtr->param.newoutline.fx_cell_height;
		Fx.fx_cell_width = inputPtr->param.newoutline.fx_cell_width;
		Fx.fx_cell_ascent = inputPtr->param.newoutline.fx_cell_ascent;

		Fx.fx_em_cell_height = FIXROUND(LongMulDiv(key->emResolution, Fx.fx_cell_height, key->metricScalarY));
		Fx.fx_em_cell_width = FIXROUND(LongMulDiv(key->emResolution, Fx.fx_cell_width, key->metricScalarX));

		Fx.fx_char_x0 = inputPtr->param.newoutline.fx_char_x0;
		Fx.fx_char_x1 = inputPtr->param.newoutline.fx_char_x1;
		Fx.fx_char_y0 = inputPtr->param.newoutline.fx_char_y0;
		Fx.fx_char_y1 = inputPtr->param.newoutline.fx_char_y1;
	}

	return NO_ERR;
}

FS_ENTRY fs_FindBitMapSize(fs_GlyphInputType *inputPtr, fs_GlyphInfoType *outputPtr)
{
  register fnt_ElementType  *elementPtr;
#ifndef PC_OS
  register int8             *workSpacePtr;
#endif
  sc_CharDataType           charData;
  register sc_BitMapData    *bitRecPtr;
  uint32                    scan, byteWidth;
  uint32                    numPts;
  int32                     nx;
  FS_MEMORY_SIZE	    		 size;
  int                       error;

  FS_SETUPKEY(INITIALIZED | NEWSFNT | NEWTRANS | GOTGLYPH);
  SETJUMP(key, error);

#ifdef DEBUGSTAMP
  CHECKSTAMP(inputPtr->memoryBases[0] + outputPtr->memorySizes[0]);
  CHECKSTAMP(inputPtr->memoryBases[1] + outputPtr->memorySizes[1]);
  CHECKSTAMP(inputPtr->memoryBases[2] + outputPtr->memorySizes[2]);
  CHECKSTAMP(inputPtr->memoryBases[3] + outputPtr->memorySizes[3]);
  CHECKSTAMP(inputPtr->memoryBases[4] + outputPtr->memorySizes[4]);
#endif
#ifdef USE_OUTLINE_CACHE
  key->outlineIsCached = false;
#endif

  elementPtr    = &(key->elementInfoRec.interpreterElements[GLYPHELEMENT]);
#ifndef PC_OS
  workSpacePtr  = key->memoryBases[WORK_SPACE_BASE];
#endif
  bitRecPtr     = &(key->bitMapInfo);

  charData      = *elementPtr;

  if (key->phaseShift)
    fs_45DegreePhaseShift(&charData);

  error = sc_FindExtrema(&charData,  bitRecPtr);
#ifndef PC_OS
  if (error)
    return error;
#endif

  scan          = bitRecPtr->high;
  byteWidth     = bitRecPtr->wide >> 3;

  {
    BitMap * bm  = &outputPtr->bitMapInfo;
    bm->baseAddr = 0;
    bm->rowBytes = byteWidth;
#ifdef PC_OS
    bm->bounds   = bitRecPtr->bounds;
#else
    bm->bounds.left   = bitRecPtr->bounds.xMin;
    bm->bounds.right  = bitRecPtr->bounds.xMax;
    bm->bounds.top    = bitRecPtr->bounds.yMin;
    bm->bounds.bottom = bitRecPtr->bounds.yMax;
#endif
  }






  numPts = charData.ep[charData.nc-1] + 1 + PHANTOMCOUNT;
  {
    register metricsType*metric = &outputPtr->metricInfo;
    register uint32 index1 = numPts-PHANTOMCOUNT + RIGHTSIDEBEARING;
    register uint32 index2 = numPts-PHANTOMCOUNT + LEFTSIDEBEARING;
    register Fixed tmp32;

    metric->devAdvanceWidth.x     = DOT6TOFIX(charData.x[index1] - charData.x[index2]);
    metric->devAdvanceWidth.y     = DOT6TOFIX(charData.y[index1] - charData.y[index2]);
    index1 = numPts - PHANTOMCOUNT + LEFTEDGEPOINT;
    metric->devLeftSideBearing.x  = DOT6TOFIX(charData.x[index1] - charData.x[index2]);
    metric->devLeftSideBearing.y  = DOT6TOFIX(charData.y[index1] - charData.y[index2]);

    outputPtr->metricInfo.advanceWidth.y = 0;
    if (key->identityTransformation)
	 {
        outputPtr->metricInfo.advanceWidth.x = ShortMulDiv( key->metricScalarX, key->nonScaledAW, key->emResolution );
	 }
    else
	 {
        outputPtr->metricInfo.advanceWidth.x = FixDiv( key->nonScaledAW, key->emResolution );
        fsg_FixXYMul( &outputPtr->metricInfo.advanceWidth.x, &outputPtr->metricInfo.advanceWidth.y, &key->currentTMatrix );
    }

    index2 = numPts - PHANTOMCOUNT + ORIGINPOINT;
    metric->leftSideBearing.x = DOT6TOFIX(charData.x[index1] - charData.x[index2]);
    metric->leftSideBearing.y = DOT6TOFIX(charData.y[index1] - charData.y[index2]);

/* store away sidebearing along the advance width vector */
    metric->leftSideBearingLine = metric->leftSideBearing;
    metric->devLeftSideBearingLine = metric->devLeftSideBearing;

/* Add vector to left upper edge of bitmap for ease of positioning by client */
    tmp32 = ((Fixed) (bitRecPtr->bounds.xMin) << 16) - DOT6TOFIX(charData.x[index1]);
    metric->leftSideBearing.x     += tmp32;
    metric->devLeftSideBearing.x  += tmp32;
    tmp32 = ((Fixed) (bitRecPtr->bounds.yMax) << 16) - DOT6TOFIX(charData.y[index1]);
    metric->leftSideBearing.y		 += tmp32;
    metric->devLeftSideBearing.y	 += tmp32;
  }

  SET_STATE(INITIALIZED | NEWSFNT | NEWTRANS | GOTGLYPH | SIZEKNOWN);

/* get memory for bitmap in bitMapRecord */
  if (scan == 0) 
    ++scan;

  outputPtr->memorySizes[BITMAP_PTR_1] = (FS_MEMORY_SIZE) SHORTMUL(scan, byteWidth) + STAMPEXTRA;

/* get memory for yLines & yBase in bitMapRecord */
  size = (bitRecPtr->nYchanges + 2) * sizeof(int32) + sizeof(int32 *);
  outputPtr->memorySizes[BITMAP_PTR_2] = size;

  if (fs_dropOutVal(key))
  {
		/* get memory for xLines and xBase - used only for dropout control */
		nx = (bitRecPtr->bounds.xMax - bitRecPtr->bounds.xMin);
		if (nx == 0)
		 ++nx;
		size 	 = (nx * (((FS_MEMORY_SIZE) bitRecPtr->nXchanges + 2) * sizeof(int32) + sizeof(int32 *)));
  }
  else 
    size = 0;

  outputPtr->memorySizes[BITMAP_PTR_3] = size;

#ifdef DEBUGSTAMP
  CHECKSTAMP(inputPtr->memoryBases[0] + outputPtr->memorySizes[0]);
  CHECKSTAMP(inputPtr->memoryBases[1] + outputPtr->memorySizes[1]);
  CHECKSTAMP(inputPtr->memoryBases[2] + outputPtr->memorySizes[2]);
  CHECKSTAMP(inputPtr->memoryBases[3] + outputPtr->memorySizes[3]);
  CHECKSTAMP(inputPtr->memoryBases[4] + outputPtr->memorySizes[4]);
#endif
  return NO_ERR;
}

/* rwb - 4/21/90 - fixed to better work with caching */
FS_ENTRY fs_ContourScan(fs_GlyphInputType *inputPtr, fs_GlyphInfoType *outputPtr)
{
  register sc_BitMapData		*bitRecPtr;
  register fnt_ElementType		*elementPtr;
  sc_GlobalData					* scPtr;
  sc_CharDataType					charData;
  int32								scanControl;
  int32								lowBand, highBand;
  uint32								nx, ny;
  int									error;

  FS_SETUPKEY(INITIALIZED | NEWSFNT | NEWTRANS | GOTGLYPH | SIZEKNOWN);
  SETJUMP(key, error);

#ifdef DEBUGSTAMP
  CHECKSTAMP(inputPtr->memoryBases[0] + outputPtr->memorySizes[0]);
  CHECKSTAMP(inputPtr->memoryBases[2] + outputPtr->memorySizes[2]);
  CHECKSTAMP(inputPtr->memoryBases[3] + outputPtr->memorySizes[3]);
  CHECKSTAMP(inputPtr->memoryBases[4] + outputPtr->memorySizes[4]);
#endif

  bitRecPtr = &key->bitMapInfo;
  elementPtr = &(key->elementInfoRec.interpreterElements[GLYPHELEMENT]);
  charData = *elementPtr;
  scPtr = (sc_GlobalData *)key->memoryBases[SCAN_PTR_BASE];

  nx = bitRecPtr->bounds.xMax - bitRecPtr->bounds.xMin;
  if (nx == 0) 
    ++nx;

  scanControl = fs_dropOutVal( key );


/* If topclip <= bottom clip there is no banding by convention  */
  highBand = inputPtr->param.scan.topClip;
  lowBand  = inputPtr->param.scan.bottomClip;

  if (highBand <= lowBand)
  {
    highBand = bitRecPtr->bounds.yMax;
    lowBand = bitRecPtr->bounds.yMin;
  }
  else if (highBand != bitRecPtr->bounds.yMax || lowBand != bitRecPtr->bounds.yMin)
  {
#ifdef FSCFG_NO_BANDING
    return SCAN_ERR;
#endif 
  }
    /* check for out of bounds band request                                                         <10> */
  if (highBand > bitRecPtr->bounds.yMax)
    highBand = bitRecPtr->bounds.yMax;
  if (lowBand < bitRecPtr->bounds.yMin)
    lowBand = bitRecPtr->bounds.yMin;

/*
// 11/16/90 rwb - We now allow the client to turn off DOControl
// by returning a NIL pointer to the memory area used by DOC.
// This is done so that in low memory conditions, the client can
// get enough memory to print something.  We also always turn off
// DOC if the client has requested banding.  Both of these conditions
// may change in the future.  Some versions of TT may simply return
// an error condition when the NIL pointer to memoryarea 7 is
// provided.  We also need to rewrite the scan converter routines
// that fill the bitmap under dropout conditions so that they use
// noncontiguous memory for the border scanlines that need to be
// present for doing DOC.  This will allow us to do DOC even though
// we are banding, providing there is enough memory.  By preflighting
// the fonts so that the request for memory for areas 6 and 7 from
// findBitMapSize is based on actual need rather than worse case
// analysis, we may also be able to reduce the memory required to
// do DOC in all cases and particulary during banding.
*/

    /* inhibit DOControl if banding or BITMAP_PTR_3 = NUL */

  if ((highBand < bitRecPtr->bounds.yMax || lowBand > bitRecPtr->bounds.yMin)
   || (key->memoryBases[BITMAP_PTR_3] == 0))
  {
	  scanControl = 0;
  }

	/* MH 1/7/93: inhibit DropOutControl if using a non-black fill pattern */

  if (Fx.fx_Fill.m_nPattern != FillFormat::Black)
	{
		scanControl = 0;
	}


  bitRecPtr->bitMap       = (uint32*)(inputPtr->param.scan.BitMapBuffer);


  if( scanControl )
  {
    bitRecPtr->xLines    = (int32*) key->memoryBases[BITMAP_PTR_3];
    bitRecPtr->xBase     = (int32**)((char *) bitRecPtr->xLines + (bitRecPtr->nXchanges + 2) * nx * sizeof(int32));
    ny                   = bitRecPtr->bounds.yMax - bitRecPtr->bounds.yMin;
  }
  else
  {
    ny                   = highBand - lowBand;
  }

  if (ny == 0)
    ++ny;

  bitRecPtr->yLines    = (int32*) key->memoryBases[BITMAP_PTR_2];
  bitRecPtr->yBase     = (int32**) ((char *) bitRecPtr->yLines + ((FS_MEMORY_SIZE)bitRecPtr->nYchanges + 2) * (FS_MEMORY_SIZE)ny * sizeof(int32));





	/*
	// Here is where the top level special effects stuff goes.
	// One or more calls to sc_ScanChar are made with different
	// patterns, pens, and flags to achieve the desired result.
	*/

	Fx.fx_bitmap         = bitRecPtr;
	Fx.fx_low_row        = lowBand;
	Fx.fx_high_row       = highBand;

	Fx.fx_flags          = FX_FLAG_stroke_fill;
	Fx.fx_pattern        = FillFormat::None;
	Fx.fx_stroke_pattern = FillFormat::Black;

	sc_initialize_buffer(bitRecPtr, lowBand, highBand);

	uint32 dwComponent = inputPtr->param.scan.Component;

	switch (dwComponent)
	{
		case COMPONENT_WhiteOutMask:
		{
			if (IS_SHADOW(Fx.fx_Shadow.m_nStyle))
			{
				set_pen((int16)MLS_OUTLINE_shadow_pen);

				if ((error = sc_ScanChar(&charData, scPtr, bitRecPtr, lowBand, highBand, scanControl, 0, 0)) != 0)
				{
					return (error);
		 		}
			}

			if (IS_CONTOUR_OUTLINE(Fx.fx_Outline.m_nStyle))
			{
				set_pen((int16)(!IS_3D_SHADOW(Fx.fx_Shadow.m_nStyle) ? MLS_OUTLINE_contour_outline_pen : MLS_OUTLINE_contour_gap_pen));
			}
			else
			{
				set_pen(Fx.fx_Outline.m_nStyle);
			}

			Fx.fx_pattern = FillFormat::Black;

			if ((error = sc_ScanChar(&charData, scPtr, bitRecPtr, lowBand, highBand, scanControl, 0, 0)) != 0)
			{
				return (error);
		 	}

			break;
		}

		case COMPONENT_ShadowMask:
		case COMPONENT_Shadow:
		{
			if (!IS_SHADOW(Fx.fx_Shadow.m_nStyle))
			{
				return SCAN_ERR;
			}

			F26Dot6 XOffset = 0;
			F26Dot6 YOffset = 0;

			if (Fx.fx_Shadow.m_nStyle != ShadowFormat::Drop)
			{
				set_pen((int16)MLS_OUTLINE_shadow_pen);
			}
			else
			{
				set_pen(OutlineFormat::None);
				Fx.fx_pattern = FillFormat::Black;
				XOffset = -(Fx.fx_shadow_pen.nXOffset << 6);
				YOffset =  (Fx.fx_shadow_pen.nYOffset << 6);
			}

			if ((error = sc_ScanChar(&charData, scPtr, bitRecPtr, lowBand, highBand, scanControl, XOffset, YOffset)) != 0)
			{
				return (error);
		 	}

			break;
		}

		case COMPONENT_FillMask:
		case COMPONENT_Fill:
		{
			set_pen(OutlineFormat::None);
			Fx.fx_pattern = (dwComponent == COMPONENT_Fill) ? Fx.fx_Fill.m_nPattern : FillFormat::Black;

			if ((error = sc_ScanChar(&charData, scPtr, bitRecPtr, lowBand, highBand, scanControl, 0, 0)) != 0)
			{
				return (error);
		 	}

			break;
		}

		case COMPONENT_OutlineMask:
		case COMPONENT_Outline:
		{
			if (Fx.fx_Outline.m_nStyle == OutlineFormat::None)
			{
				return SCAN_ERR;
			}

			if (IS_CONTOUR_OUTLINE(Fx.fx_Outline.m_nStyle))
			{
				if (dwComponent == COMPONENT_Outline)
				{
					if (!IS_3D_SHADOW(Fx.fx_Shadow.m_nStyle))
					{
						set_pen((int16)MLS_OUTLINE_contour_outline_pen);

						if ((error = sc_ScanChar(&charData, scPtr, bitRecPtr, lowBand, highBand, scanControl, 0, 0)) != 0)
						{
							return (error);
		 				}
					}

					Fx.fx_pattern = FillFormat::White;
					Fx.fx_stroke_pattern = FillFormat::White;
					set_pen((int16)MLS_OUTLINE_contour_gap_pen);

					if ((error = sc_ScanChar(&charData, scPtr, bitRecPtr, lowBand, highBand, scanControl, 0, 0)) != 0)
					{
						return (error);
					}

					Fx.fx_pattern = FillFormat::None;

					if (Fx.fx_Outline.m_nStyle == OutlineFormat::Contour2)
					{
						Fx.fx_stroke_pattern = FillFormat::Black;
						set_pen(OutlineFormat::Light);

						if ((error = sc_ScanChar(&charData, scPtr, bitRecPtr, lowBand, highBand, scanControl, 0, 0)) != 0)
						{
							return (error);
						}
					}
				}
				else
				{
					Fx.fx_pattern = FillFormat::White;
					set_pen((int16)(!IS_3D_SHADOW(Fx.fx_Shadow.m_nStyle) ? MLS_OUTLINE_contour_outline_pen : MLS_OUTLINE_contour_gap_pen));

					if ((error = sc_ScanChar(&charData, scPtr, bitRecPtr, lowBand, highBand, scanControl, 0, 0)) != 0)
					{
						return (error);
					}
				}
			}
			else
			{
				set_pen(Fx.fx_Outline.m_nStyle);

				if ((error = sc_ScanChar(&charData, scPtr, bitRecPtr, lowBand, highBand, scanControl, 0, 0)) != 0)
				{
					return (error);
		 		}
			}

			break;
		}

		default:
		{
			return SCAN_ERR;
		}
	}

	register BitMap* bm = &outputPtr->bitMapInfo;
	bm->baseAddr = bitRecPtr->bitMap;
	bm->rowBytes = bitRecPtr->wide >> 3;
#ifdef PC_OS
	bm->bounds   = bitRecPtr->bounds;
#else
	bm->bounds.left   = bitRecPtr->bounds.xMin;
	bm->bounds.right  = bitRecPtr->bounds.xMax;
	bm->bounds.top    = bitRecPtr->bounds.yMin;
	bm->bounds.bottom = bitRecPtr->bounds.yMax;
#endif

#ifdef DEBUGSTAMP
	CHECKSTAMP(inputPtr->memoryBases[0] + outputPtr->memorySizes[0]);
	CHECKSTAMP(inputPtr->memoryBases[2] + outputPtr->memorySizes[2]);
	CHECKSTAMP(inputPtr->memoryBases[3] + outputPtr->memorySizes[3]);
	CHECKSTAMP(inputPtr->memoryBases[4] + outputPtr->memorySizes[4]);
#endif
	return NO_ERR;
}

FS_ENTRY fs_CloseFonts(fs_GlyphInputType *inputPtr, fs_GlyphInfoType *outputPtr)
{
  return NO_ERR;
}


PRIVATE void fs_45DegreePhaseShift(sc_CharDataType *glyphPtr)
{
  F26Dot6 * x = glyphPtr->x;
  int16 count = glyphPtr->ep[glyphPtr->nc-1];
  for (; count >= 0; --count)
  {
    (*x)++;
    ++x;
  }
}


/* Use various spline key values to determine if dropout control is to be activated
 * for this glyph, and if so what kind of dropout control.
 * The use of dropout control mode in the scan converter is controlled by 3 conditions.
 * The conditions are: Is the glyph rotated?, is the glyph stretched?,
 * is the current pixels per Em less than a specified threshold?
 * These conditions can be OR'd or ANDed together to determine whether the dropout control
 * mode ought to be used.

Six bits are used to specify the joint condition.  Their meanings are:

BIT		Meaning if set
8		Do dropout mode if other conditions don't block it AND
			pixels per em is less than or equal to bits 0-7
9		Do dropout mode if other conditions don't block it AND
			glyph is rotated
10		Do dropout mode if other conditions don't block it AND
			glyph is stretched
11		Do not do dropout mode unless ppem is less than or equal to bits 0-7
			A value of FF in 0-7  means all sizes
			A value of 0 in 0-7 means no sizes
12		Do not do dropout mode unless glyph is rotated	
13		Do not do dropout mode unless glyph is stretched
			
In other words, we do not do dropout control if:
No bits are set,
Bit 8 is set, but ppem is greater than threshold
Bit 9 is set, but glyph is not rotated
Bit 10 is set, but glyph is not stretched
None of the conditions specified by bits 11-13 are true.

For example, 0xA10 specifies turn dropout control on if the glyph is rotated providing
that it is also less than 0x10 pixels per em.  A glyph is considered stretched if
the X and Y resolutions are different either because of the device characteristics
or because of the transformation matrix.  If both X and Y are changed by the same factor
the glyph is not considered stretched.

 */

PRIVATE int32 fs_dropOutVal(fsg_SplineKey *key)
{
  register int32 condition = key->scanControl;
  if (! (condition & 0x3F00)) 
    return 0;
  if ((condition & 0xFFFF0000L) == NODOCONTROL) 
    return 0;
  {
    register int32 imageState = key->imageState;
    if ((condition & 0x800) && ((imageState & 0xFF) > (condition & 0xFF))) 
      return 0;
    if ((condition & 0x1000) && ! (imageState & ROTATED)) 
      return 0;
    if ((condition & 0x2000) && ! (imageState & STRETCHED)) 
      return 0;
    if ((condition & 0x100) && ((imageState & 0xFF) <= (condition & 0xFF))) 
      return condition;
    if ((condition & 0x100) && ((condition & 0xFF) == 0xFF)) 
      return condition;
    if ((condition & 0x200) && (imageState & ROTATED)) 
      return condition;
    if ((condition & 0x400) && (imageState & STRETCHED)) 
      return condition;
    return 0;
  }
}


#ifdef  FSCFG_NO_INITIALIZED_DATA
void fs_InitializeData(void)
{
	fnt_InitializeData();
	sfnt_InitializeData();
	sc_InitializeData();
}
#endif
