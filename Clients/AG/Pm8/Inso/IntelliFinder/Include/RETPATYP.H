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
 * REtpatyp.h                                                               *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:


Creation Date:
    17-jUN-92   LT

Change Record:
    Date        Author      Comment

*---------------------------------------------------------------------------*/

#ifndef RETPATYP_H

#define RETPATYP_H

/* Synonym Element Structure Definition.
*/
typedef struct RE_syn_ele
{
   PFUCHAR         pPrimDispStr; /* Primary synonym; for selection, use, or*/
                                 /* lookup; should be concatenated with    */
                                 /* nScndDispStr for display               */

   PFUCHAR         pScndDispStr; /* Extra info. (Cross reference, usage    */
                                 /* note, etc.) associated with            */
                                 /* pPrimDispStr; should be displayed with */
                                 /* pPrimDispStr                           */

   FLAG            bCrossRef;    /* Cross reference is available or not    */

   INT2B           nSynType;     /* Word type (Syn,Ant, Hyper, Hypo, ...)  */
   INT4B           lCREntNum;    /* If bCrossRef is TRUE, this field either*/
                                 /* has entry number if REentlst() may     */
                                 /* return more than one entry number, or  */
                                 /* DUMMYENT (-1) if REentlst() will return*/
                                 /* only one entry number                  */
}
RE_SYN_ELE,
FAR   *PFRE_SYN_ELE;


/* Meaning Element Structure Definition.
*/
typedef struct RE_mng_ele
{
    PFUCHAR     pPrimDispStr;   /* Primary meaning                          */
    PFUCHAR     pScndDispStr;   /* Secondary meaning; it can be attached    */
                                /* to the primary for display               */
    PFUCHAR     pMngTypeStr;    /* Meaning type in native language          */
    PFUCHAR     pInfoDispStr;   /* General info. (Usage note, etc.)         */
                                /* associated with pPrimDispStr             */
 
    INT2B       nMngType;       /* String Code for the string above.        */
    INT2B       nSyns;          /* Number of elements in pSynArray          */
    PFRE_SYN_ELE FAR
                *pSynArray;     /* Array of pointers to synonym elements    */
}
RE_MNG_ELE,
FAR   *PFRE_MNG_ELE;


/* Synonyms Header Structure Definition.
*/
typedef struct RE_syn_hdr
{
   PFUCHAR         pLangStr;      /* Native language string                 */
   PFUCHAR         pHeadword;     /* Headword in the entry text; NULL if    */
                                  /* there is no headword                   */
   INT2B           nMngs;         /* Number of elements in pMngArray        */
   INT2B           nDefMng;       /* Index to pMngArray for the default     */
                                  /* meaning                                */
   INT2B           nDefSyn;       /* Index to pSynArray in the default      */
                                  /* pMngArray for the default              */
   PFRE_MNG_ELE FAR *pMngArray;   /* Array of pointers to meaning elements  */
}
RE_SYN_HDR,
FAR   *PFRE_SYN_HDR;



/* REetpar() I/O structure.
*/
typedef struct RE_etpar_io
{
   PFRE_DB_IO     pDBIO;          /* Input; databse I/O structure.          */ 
   PFUCHAR        pBaseform;      /* Input; REenlst() returned baseform word*/
   PFRE_ENT       pEntList;       /* Input; entry list structure from       */
                                  /*        REentlst()                      */
   INT2B          nReturns;       /* Input; number of elements in pEntList  */
                                  /*        INT2B is compatible with the    */
                                  /*        same field in RE_ENTLIST_IO     */
   INT2B          nEntOffset;     /* Input; offset in pEntList, info. will  */
                                  /*        be used in pSynData             */
   FLAG           bCrossRef;      /* Input; cross reference flag determines */
                                  /*        default meaning and synonyn     */
   PFUCHAR        pCrossWord;     /* Input; ptr. to CRef'd word.            */
   PFUCHAR        pPrevHwd;       /* Input; buffer for prev retn'd headword */ 
   PFRE_STATUS    pStatus;        /* Output; status structure               */
   PFRE_SYN_HDR   pSynData;       /* Output; synonym structure              */
}
RE_ETPAR_IO,
* PRE_ETPAR_IO,
FAR *PFRE_ETPAR_IO,
NEAR *PNRE_ETPAR_IO;



/* The following data structures are used internally by REetpar1() and
 * REetpar2().
*/
typedef struct ET_TokStruct
{              
    UINT2B      nIndex;              /* Index to the entry line buffer            */
    UINT2B      nTokLen;             /* Token length                              */
    SCHAR       nTokType;            /* Token type                                */
    struct ET_TokStruct FAR *lpPrev;
    struct ET_TokStruct FAR *lpNext;
}
ET_TOKNODE,
FAR *PFET_TOKNODE;


/* A node structure for pointer to meanings
*/
typedef struct ET_MngPNode
{
   PFRE_MNG_ELE    pMngArray;     /* Pointer to one meaning element        */
   struct ET_MngPNode FAR *lpNext;
}
ET_MNGPTR,
FAR *PFET_MNGPTR;


/* A node structure for pointer to synonyms
*/
typedef struct ET_SynPNode
{
   PFRE_SYN_ELE   pSynArray;     /* Pointer to one synonym element         */
   struct ET_SynPNode FAR *lpNext;
}
ET_SYNPTR,
FAR *PFET_SYNPTR;


/* An internal data container for miscellaneous data used in parsing.
*/
typedef struct ET_ParseData
{
   
    PFUCHAR     pEntBuff;       /* Pointer to the Text Buffer returned from */
                                /* REentry() that is to be parsed.          */
    PFUCHAR     pPrevUnparsed;  /* Thruput; unparsed and incomplete line    */
                                /*          data in the previous pEntBuff;  */
                                /*          used internally by the parser   */
    FLAG        bEndOfData;     /* TRUE if this is the last buffer of text  */
                                /* for the entry being parsed.              */
}
ET_PARSEDATA,
FAR * PFET_PARSEDATA;



#endif /* RETPATYP_H */

/* End of file - REtpatyp.h */
