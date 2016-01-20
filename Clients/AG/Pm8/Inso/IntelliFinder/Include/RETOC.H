/****************************************************************************
 *                     Copyright 1995 INSO Corporation                      *
 *                    IntelliFinder(TM) Reference Engine                    *
 *                                                                          *
 * This work is protected by United States and international copyright laws *
 * and contains certain trade secret information.  All use of this work is  *
 * governed by the terms of agreements between INSO Corporation and its     *
 * licensees or potential licensees.  Disclosure or disassembly of the      *
 * contents of embodied programs or algorithms is prohibited.  All rights   *
 * are reserved worldwide.                                                  *
 ****************************************************************************/

/*--------------------------------------------------------------------------*
 *                                                                          *
 * REtoc.h - Table of contents header file.                                 *
 *                                                                          *
 *--------------------------------------------------------------------------*/
#ifndef RETOC_H
#define RETOC_H


/* Define the following to do performance testing in rewindrv.c, rewindlg.c
*/
/* #define RE_PERF_TEST
*/
/* Define the foll to make toc function no-op, ie just return immediately.
*/
/* #define RE_NOOP_TEST
*/
/* Define the following to activate dead, unfinished code to handle
 *   filter overrides first specified then postponed during development
 */
/* #define RE_OVERRIDE
*/

/* the following is used to correct bitmap offset which have 1 added on
 * the dbtoctab side to avoid 0, which is meaningful
 */
#define OFFSETCORRECTOR0 0
#define OFFSETCORRECTOR 1
#define TRUEOFFSET(_offs) (_offs - OFFSETCORRECTOR)

/* to enumerate the entries */
#define ENTTYPE  INT4B
/* to enumerate the bytes in the bitfield array */
#define BFTYPE   UINT2B

#define ENTBUFSIZE 65535
#define MAXENTVAL   ( sizeof(UCHAR) * ENTBUFSIZE ) 
/* the foll was in orig code, not used pervasively 
 * we now check against it but could use out-of-memory condition
 * in future versions
 */
#define MAXSECT  3140

#define BITSPERCELL 8                                    
/* use of Cmemcpy below limits size of buffer to 64k * 8bits */
/*#define CELLARRAYSIZE (1024L * 32L)*/
#define CELLARRAYSIZE ENTBUFSIZE
#define BITFIELDSIZE (CELLARRAYSIZE * BITSPERCELL)

/* the foll used to store arbitrary bitmaps in byte-oriented arrays */
#define ENTRY_BM_SIZE(_entries) ((UINT2B)(_entries/BITSPERCELL + 1))
#define SET_BM_START(_entries) ((UINT2B)(_entries/BITSPERCELL))
#define SET_BM_STOP(_entries) ((UINT2B)(_entries/BITSPERCELL ))
#define SET_BM_SIZE(_start, _stop) \
                    (SET_BM_STOP(_stop)-SET_BM_START(_start)+1U)
/*---------------------------------------------------------------------
 * Table of contents information.  Contains the static data describing a
 * section of a dictionary's table of contents.
 *---------------------------------------------------------------------
*/
/* now in retoc.h for access by the toc tools */
/* the order of fields is assumed by code -- should not be changed */
/*  and new fields are added at end to help w backward compatibility */
/* the foll depends on field order in TOCNODE_RE */
/* use these to find code you have to change if struct changes */
#define RE_BMJUMP 14
#define RE_SIBJUMP 12
#define RE_NEXTSIBSIZ 2
#define RE_LEVVISSIZ 1

typedef struct RE_toc_node
{
    INT2B nTitleID;
    UCHAR uLevel;
    UCHAR uVisible;
    INT4B lStart;
    INT4B lStop;
    INT2B nNextSib;
    INT4B lBitmap;                 
}
TOCNODE_RE,
FAR *PF_TOCNODE_RE,
NEAR *PN_TOCNODE_RE;

/* cf compatibility on p 12 of spec */
#define RE_COMPATIBILITY_BIT 0x01

/* Editorial lockouts.
*/
#define RE_VISIBLE_NODE 0x20
#define RE_ALL_VALUE_NODE 0x10
#define RE_NO_ROOT_NODE 0x08
#define RE_NO_CANNOT_NODE 0x04
#define RE_NO_MUST_NODE 0x02
#define RE_BASE_NODE 0x01
#define RE_HIDDEN_NODE 0x00

/* check for sums of above values */
#define MAXSYMBOLSUM 63

/* maintain a stack to circumvent recursive calls in processing
 * the tocha tree structure
 */
#define MAXSTACKLEVELS 20
#define RE_TOCBLD_PROCESSING_FINISHED         -1

typedef struct RE_toc_context
{   
    UINT2B node; /* index in array of nodes */
    UCHAR type; /* log value of this node */
    UCHAR level; /* toc tree depth */
    INT2B processing;  /* which log val being processed, mustbe, canbe, or cantbe */
    UINT2B start;  /* range of sections including embedded ones */
    UINT2B stop; 
    ORF_BUFF mBitMap; /* we allocate a temporary bitmap for complex nodes */
}
TOCCONTEXT_RE,
FAR *PF_TOCCONTEXT_RE,
NEAR *PN_TOCCONTEXT_RE;

#endif /* RETOC_H */

/* End of file - REtoc.h
*/
