/*----------------------------------------------------------------------*
*                                                                       *
*                   Copyright 1995 INSO Corporation                     *
*                         Common Code Library                           *
*                                                                       *
* This work is protected by United States and international copyright   *
* laws and contains certain trade secret information.  All use of this  *
* work is governed by the terms of agreements between INSO Corporation  *
* and its licensees or potential licensees.  Disclosure or disassembly  *
* of the contents of embodied programs or algorithms is prohibited.     *
* All rights are reserved worldwide.                                    *
*                                                                       *
*-----------------------------------------------------------------------*

*-----------------------------------------------------------------------*
*                                                                       *
*    SFcoldef.h - typedefs and defines for collation                    *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
 
    Creation Date:
    17-may-95   KWB - Created according to $/COMPRESS/DOC/COLLATION.DOC

*-----------------------------------------------------------------------*/

#ifndef SFCOLDEF_H
#define SFCOLDEF_H
#ifdef  SF_COLLATE
/*  Constant Definitions    */

#define SFCOLCLAST      0   /*  Terminator for string of collation units    */

#define SFCOLCMAXLEN    128 /*  Maximum string/sequence length              */

#define SFCOLWIGNORE    0   /*  Weight which is ignored in comparisons      */

#define SFCOLWMAXWEIGHT 3   /*  Maximum number of weights supported         */

#define SFCOLDEFTABSIZE 2500    /*  Size to allocate for default table      */
/*  Type Definitions        */

/*  Collation unit type */
typedef UINT2B  SFCOLCU, * PSFCOLCU, FAR *PFSFCOLCU;

/*  Weight type         */
typedef INT2B   SFCOLW, * PSFCOLW, FAR *PFSFCOLW;

/*  Weight table    */
typedef struct SF_CollationWeightTable
{   UINT2B      wNumWeights,    /*  Number of Weights in the table  */
                wPadding;
    PFSFCOLW    apWeight[SFCOLWMAXWEIGHT];
}   SFCOLWTAB, * PSFCOLWTAB, FAR *PFSFCOLWTAB;

typedef struct SF_CollationState
{   UINT2B  wCUOff,             /*  Offset into translation array   */
            wLength;            /*  Length of translation string    */
}   SFCOLSTATE, * PSFCOLSTATE, FAR *PFSFCOLSTATE;

typedef struct SF_CollationTable
{   UINT2B  wTableFormat,       /*  Table Format Indicator              */
            wNumChars,          /*  length of pOffset & pLength arrays  */ 
            wLenTrans,          /*  Length of translation array         */
            wNumCU;             /*  Number of collation units in array  */
    SFCOLWTAB   eWTab;          /*  Embedded Weight Table               */
    PFUINT2B    pOffset;        /*  Array of offsets into translation   */
    PFUCHAR     pLength;        /*  Array of lengths of translations    */
    PFSFCOLCU   pTranslate;     /*  Translation array                   */
}   SFCOLTAB, *PSFCOLTAB, FAR *PFSFCOLTAB;

SLF_TYPDEF_OR( SLF_HDL_OBJ, PFSFCOLTAB, _OF_SFCOLTAB) ORFSF_COLTAB;

/*  Collation Sequence Type */
typedef struct SF_CollationSequence
{   PFSFCOLTAB  pfTTab;
    UCHAR       uStrLength,
                uSeqLength;
    UCHAR       aString[SFCOLCMAXLEN];
    SFCOLCU     aSequence[SFCOLCMAXLEN];
}   SFCOLSEQ, * PSFCOLSEQ, FAR * PFSFCOLSEQ;

/*  Macros  */

/*  Most of the collation API is implemented through macros so that
 *  minimal code changes will be necessary as we update the collation
 *  module to support other types of code.
 */

#define SFColCGetTranslation(pSeq)  ((pSeq)->pfTTab)
#define SFColCSetTranslation(pSeq, t)   \
(   ((pSeq)->pfTTab = (t)),             \
    SFColCSetString(pSeq, NULL, 0)      \
)

#define SFColCGetString(pSeq)       ((pSeq)->aString)
#define SFColCGetLength(pSeq)       ((pSeq)->uStrLength)
#define SFColCGetCLength(pSeq)      ((pSeq)->uSeqLength)
#define SFColCSetLength(pSeq, wLength)                                  \
        ( ((wLength) <= SFColCGetLength(pSeq)) ?                        \
            SFColCSetString(pSeq, SFColCGetString(pSeq), wLength) :     \
            ERRET                                                       \
        )

#define SFColCGetCU(pSeq, i)    \
    ( ( (UCHAR)(i) > SFColCGetCLength(pSeq)) ? SFCOLCLAST : (pSeq)->aSequence[i])

#define SFColC1GetCU(pSeq, i)   \
    ((pSeq)->aSequence[i])

/*  Get the number of weights present in a given weight table   */
#define SFColWGetNumWeights(pWTab)      ((pWTab)->wNumWeights)

/*  Get the i-th weight for the collation unit given in cu      */
#define SFColWGetWeight(pWTab, cu, i)   ((pWTab)->apWeight[i][cu])

/*  Get the weight table associated with the translation table  */
#define SFColTGetWeightTable(pfTTab)    (&(pfTTab)->eWTab)

/*  The following two macros are set up for readability.  The first macro
 *  calls on the second one with all of its arguments parenthised.  
 *  The second one does the work.  This makes it easier to read the macro
 *  without all of the extra parenthesis.
 *
 *  DO NOT Call this macro with arguments that have side effects (++ and --)
 */

/*  Translate a single character to a collation unit, updating pState
 *  and returning TRUE if the character is to be consumed, or FALSE otherwise.
 */
#define SFColTTranslate(pfTTab, c , pCU, pState)            \
        SFColT1Translate((pfTTab),(c) ,(pCU), (pState))

#define SFColT1Translate(pfTTab, c , pCU, pState)           \
    (   ((pState->wLength == 0) ?                           \
            ((pState->wCUOff = pfTTab->pOffset[c]),         \
             (pState->wLength = pfTTab->pLength[c])         \
            ):                                              \
            0                                               \
        ),                                                  \
        (*pCU = (pfTTab->pTranslate[pState->wCUOff++])),    \
        (--pState->wLength == 0)                            \
    )


/*  Prototypes  */
RETCODE SFColCSetString(PFSFCOLSEQ pfSeq, PFUCHAR pfStr, UINT2B wLength);
RETCODE SFColCAppendCharacter(PFSFCOLSEQ pfSeq, UCHAR c);
INT2B SFCollate(PFSFCOLSEQ pfSeq1, PFSFCOLSEQ pfSeq2, UINT2B nMaxWeights);

/*  Use these functions instead of strcmp when comparing words  */
INT2B SFstrord(PFUCHAR pfStr1, PFUCHAR pfStr2, PFSFCOLTAB pfTab);
INT2B SFstrnord(PFUCHAR pfStr1, PFUCHAR pfStr2, PFSFCOLTAB pfTab, UINT2B n);

PFSFCOLTAB SFInitColTab(PFSFCOLTAB);
PFSFCOLTAB SFInitDefColTab(PFSFCOLTAB);
#endif  /*  SF_COLLATE  */
#endif  /*  SFCOLDEF_H  */
