/*
$Header: /PM8/TT/PRIVSFNT.H 1     3/03/99 6:35p Gbeddow $
$Log: /PM8/TT/PRIVSFNT.H $
// 
// 1     3/03/99 6:35p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:14:58   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:45:18   Fred
// Initial revision.
// 
//    Rev 1.1   01 Jul 1997 09:08:28   Fred
// More changes for PMG 4.0
// 
//    Rev 1.0   14 Mar 1996 13:21:36   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:42:00   FRED
// Initial revision.
// 
//    Rev 1.0   29 Sep 1995 13:37:08   FRED
// Initial revision.
 * 
 *    Rev 1.4   31 Aug 1992 23:11:08   FRED
 * Added "L" to constant to turn off some warnings.
 * 
 * 
 *    Rev 1.3   28 Aug 1992 18:24:46   FRED
 * Same change, but done correctly (I hope).
 * 
 * 
 *    Rev 1.2   28 Aug 1992 18:22:54   FRED
 * Added glyph_complete parameter to sfnt_ReadSFNT() prototype. This
 * parameter is used to detect when to add the background box to a
 * glyph... only after all compostie glyphs have been processed.
 * 
 * 
 *    Rev 1.1   10 Jul 1992 10:51:58   ADAM
 * PVCS keywords
*/
/*
    File:       private sfnt.h

    Contains:   xxx put contents here xxx

    Written by: xxx put writers here xxx

    Copyright:  c 1990 by Apple Computer, Inc., all rights reserved.

    Change History (most recent first):

        <3+>     7/17/90    MR      Change return types to in for computemapping and readsfnt
         <3>     7/14/90    MR      changed SQRT to conditional FIXEDSQRT2
         <2>     7/13/90    MR      Change parameters to ReadSFNT and ComputeMapping
        <1+>     4/18/90    CL      
         <1>     3/21/90    EMT     First checked in with Mike Reed's blessing.

    To Do:
*/

#ifndef __PRIVSFNT_H__
#define __PRIVSFNT_H__

/* PUBLIC PROTOTYPE CALLS */

/*
 * Below we have private stuff
 * This has nothing to do with the file format.
 */

voidPtr sfnt_GetTablePtr (register fsg_SplineKey *key, register sfnt_tableIndex n, register boolean mustHaveTable);


/*
 * Creates mapping for finding offset table
 */
extern void sfnt_DoOffsetTableMap (fsg_SplineKey *key);

/* perfect spot size (Fixed) */
#ifndef FIXEDSQRT2
#define FIXEDSQRT2 0x00016A0AL
#endif

/*
 * Returns offset and length for table n
 */
extern int sfnt_ComputeMapping (fsg_SplineKey *key, uint16 platformID, uint16 specificID);

extern void sfnt_ReadSFNTMetrics (fsg_SplineKey *key, uint16 glyphIndex);

extern int sfnt_ReadSFNT (fsg_SplineKey *key, unsigned *elementCount, uint16 gIndex, boolean useHints, voidFunc traceFunc, uint16 glyph_complete);

#ifdef  FSCFG_NO_INITIALIZED_DATA
void sfnt_InitializeData (void);
#endif


#ifndef FS_COPY_INSTRUCTIONS
#define FS_COPY_INSTRUCTIONS(instrPtr, sizeOfInstructions, p) (p)=(instrPtr)
#endif 

#ifndef FS_RELEASE_INSTRUCTION_COPY
#define FS_RELEASE_INSTRUCTION_COPY(p)
#endif

#endif
