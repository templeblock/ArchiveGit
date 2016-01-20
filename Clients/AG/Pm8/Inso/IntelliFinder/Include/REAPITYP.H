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
 * REapityp.h - External Reference Engine Type Definitions.                 *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:
    RE external type definitions of Input/Output structures and other
    structures used in the API.

    This replaces the previous file EDtypdef.h

Creation Date:
    19-mar-92   ECH

Change Record:
    Date        Author  Comment
    19-feb-93   JCC     Inflection, RE_FN_V200

*----------------------------------------------------------------------------*/

#ifndef REAPITYP_H

#define REAPITYP_H

/************************************************************************
 *
 * General Type Definitions.
 *
 ************************************************************************/

typedef UINT2B  RE_OPERATION;

typedef FLAG (FAR PASCAL *RE_ABORTPROC)(UINT4B, UINT4B);
/************************************************************************
 *
 * Support Structures.
 *
 ************************************************************************/

/* Status and Error structure.
*/

typedef struct RE_status
{
    RETCODE     nRetCode;
    INT2B       nWarning;
    INT2B       nError;
    INT2B       nSysError;
    UINT2B      wFileNum;
}
RE_STATUS,
FAR *PFRE_STATUS;



/* Table Of Contents Hierarchical Structure element, containing titles.
*/
typedef struct RE_toc_sect
{
    INT2B       nTitleID;
    UCHAR       uLevel;
}
RE_TOC_SECT,
FAR  *PFRE_TOC_SECT;

SLF_TYPDEF_OR (SLF_HDL_OBJ, PFRE_TOC_SECT, ORF_tocsect_re) ORF_TOCSECT_RE;

/* Table Of Contents Hierarchical Structure Header.
*/
typedef struct RE_toc
{
    UINT2B          wSize;
    PFRE_TOC_SECT   aSection;
}
RE_TOC,
FAR  *PFRE_TOC;


/* Table Of Contents Filter Header.
*/

typedef struct RE_toc_filter
{
    INT4B       lSerialNum;
    FLAG FAR    *bTocFilter;
}
RE_TOCFLT,
FAR  *PFRE_TOCFLT;

#ifndef RE_SF_V22 /* RE_NEWTOCHA */
typedef struct RE_ent_filter
{
    INT4B       lSerialNum;
    FLAG FAR    *bEntFilter;
}
RE_ENTFLT,
FAR  *PFRE_ENTFLT;
#endif /* not RE_SF_V22 */


/* Field Selection information.
*/
typedef struct RE_fld
{
    UINT2B      wSize;
    PFUCHAR FAR *pTitles;
    PFINT2B     pGrammar;
}
RE_FLD,
*PRE_FLD,
NEAR *PNRE_FLD,
FAR  *PFRE_FLD;


typedef struct RE_fldfilter
{
    INT4B       lSerialNum;
    FLAG FAR    *aFldFilter;
}
RE_FLDFLT,
FAR  *PFRE_FLDFLT;


/* The structure that holds all of the database filenames
*/
typedef struct RE_filenames
{
    SLPATH FAR *pPath;      /* Path name or volume num. for the dir.    */
    PFCHAR      pDBFile;    /* The name of the single database file     */
}
RE_FILENAMES,
FAR *PFRE_FILENAMES;


/************************************************************************
 *
 * Input/Output Structures.
 *
 ************************************************************************/

/* Application Object Input/Output structure.
*/

typedef struct RE_app_io
{
    INT2B           nAppID;                 /* Output   */
    UCHAR           mAttrib[4];             /* Output   */
    UCHAR           szVersion[RE_VER_LEN];  /* Output   */
    PFRE_STATUS     pStatus;                /* Output   */
}
RE_APP_IO,
*PRE_APP_IO,
NEAR *PNRE_APP_IO,
FAR *PFRE_APP_IO;


/* Database Object Input/Output structure.
*/

typedef struct RE_db_io
{
    PFRE_FILENAMES  pFileSpec;      /* Input    */
    UINT2B          mDialect;       /* Input    */                 
#ifndef RE_SF_V201    
    RE_ABORTPROC    fnAbort;
#endif
    INT2B           nAppID;         /* Static   */
    INT2B           nRefType;       /* Output   */
    UCHAR           mAttrib[4];     /* Output   */
    INT2B           nLanguage;      /* Output   */
    UINT2B          mAvailDialect;  /* Output   */
    INT4B           lSerialNum;     /* Output   */
#if ( !defined RE_SF_1RDB || !defined RE_SF_V201 )
    UINT2B          mModelNum;      /* Output   */
#endif
    INT2B           nDBID;          /* Output   */
    INT4B           lLastEntry;     /* Output   */
    INT4B           lMaxEntLen;     
    UINT2B          wMaxLineLen;    /* Output   */
    UINT2B          wMaxWordLen;    /* Output   */
    UINT2B          wMaxEntWordLen; /* Output   */
    UINT2B          wMaxRetEnt;     /* Output   */
    UINT2B          wMaxStrTabLen;  /* Output   */
    INT4B           lRAMSize;
    RE_TOC          eToc;           /* Output = toc section array to show user, elicit settings */
    RE_TOCFLT       eTocFilter;     /* Output = alloc'd array for user on/off log vals 
                                     * ie the TOC settings used to calculate the
                                     * current scope with pEntFilter below
                                     * Note: both can be overridden at operation level
                                     * -- these are the defaults
                                     * The mem alloc'd internally & pointer output,
                                     * but the caller sets values then used as input
                                     */
    RE_FLD          eFld;           /* Output   */
    RE_FLDFLT       eFldFilter;     /* Output   */
#ifndef RE_SF_V22 /* RE_NEWTOCHA */
    PFRE_ENTFLT     pEntFilter;     /* Input = user alloc'd entry filt to map into cur scope *
                                     * ie new default entry filter, combines w eTocFilter
                                     * to calc current scope */
    PFUCHAR         pEntMap;        /* Output = report current scope back to caller */
#endif /* not RE_SF_V22 */
    PFRE_STATUS     pStatus;        /* Output   */
#ifdef RE_DRV_TESTENV
    FLAG            bWBT;           /* Only for WBT, Flag tells us if we're */
#endif
}
RE_DB_IO,
*PRE_DB_IO,
NEAR *PNRE_DB_IO,
FAR *PFRE_DB_IO;


/* Index List Object Input/Output structure.
*/

typedef struct RE_idxlist_io
{
    PFUCHAR         pInWord;        /* Input    */
    INT2B           nDBID;          /* Input    */
    INT2B           nMinLen;        /* Input    */
#ifndef RE_FN_V200
    PFUCHAR         mPOI;           /* Input    */
    UCHAR           uPOS;           /* Input    */
#endif
    PFRE_TOCFLT     pTocFilter;     /* Input  - override   */
    INT4B           lSerialNum;     /* Static   */
    UINT2B          wSize;          /* Static   */
    UINT2B          wListSize;      /* Static   */
    PFRE_STATUS     pStatus;        /* Output   */
    INT2B           nReturns;       /* Output   */
    PFUCHAR         pWordList;      /* Output   */
}
RE_IDXLIST_IO,
*PRE_IDXLIST_IO,
NEAR *PNRE_IDXLIST_IO,
FAR *PFRE_IDXLIST_IO;

/* Entry List Object Input/Output structure.
*/

typedef struct RE_ent
{
    INT4B           lEntNum;            /* Output   */
    PFUCHAR         pWord;              /* Output   */
    UCHAR           uPOS[6];            /* Output   */
#ifndef RE_FN_V200
    UCHAR           mPOI[RE_POI_SIZ];   /* Output   */
#endif
}
RE_ENT,
*PRE_ENT,
FAR *PFRE_ENT;

typedef struct RE_entlist_io
{
    PFUCHAR         pInWord;        /* Input    */
    UINT2B          wPun;           /* Input    */
    UCHAR           uCap;           /* Input    */
    INT2B           nDBID;          /* Input    */
    PFRE_TOCFLT     pTocFilter;     /* Input - override   */
    INT4B           lSerialNum;     /* Static   */
    UINT2B          wSize;          /* Static   */
    PFRE_STATUS     pStatus;        /* Output   */
    PFUCHAR         pBaseform;      /* Output   */
    INT2B           nReturns;       /* Output   */
    PFRE_ENT        pEntList;       /* Output   */
}
RE_ENTLIST_IO,
*PRE_ENTLIST_IO,
NEAR *PNRE_ENTLIST_IO,
FAR *PFRE_ENTLIST_IO;


/* Entry Text Object Input/Output structure.
*/

typedef struct RE_entry_io
{
    INT4B           lEntNum;        /* Input    */
    INT2B           nDBID;          /* Input    */
    UINT2B          wPosition;      /* Input - Character position to */
                                    /*         start retrieval   */
    PFRE_FLDFLT     pFldFilter;     /* Input    */
    INT4B           lSerialNum;     /* Static   */
    UINT2B          wSize;          /* Static   */
    INT2B           nToc;           /* Output   */
    PFRE_STATUS     pStatus;        /* Output   */
    INT2B           nReturns;       /* Output   */
    PFUCHAR         pEntBuff;       /* Output   */
#ifdef RE_DRV_TESTENV
    INT2B           nMAXHeadword;   /* running one, headword saves headword */
#endif                              /* length                               */
}
RE_ENTRY_IO,
*PRE_ENTRY_IO,
NEAR *PNRE_ENTRY_IO,
FAR *PFRE_ENTRY_IO;


typedef struct RE_srch
{
    INT4B           lEntNum;        /* Output   */
    PFUCHAR         pWord;          /* Output   */
 /* INT2B           nInfo;             Output   */
}
RE_SRCH,
*PRE_SRCH,
FAR *PFRE_SRCH;

typedef struct RE_search_io
{
    PFUCHAR         pBoolStr;       /* Input    */
    UCHAR           uBoolCap;       /* Input    */
    INT2B           nDBID;          /* Input    */
    PFRE_FLDFLT     pFldFilter;     /* Input    */
    PFRE_TOCFLT     pTocFilter;     /* Input - override    */
 /* PRE_BOOLEXP     pBoolStruct;       Through  */
    INT4B           lSerialNum;     /* Static   */
    UINT2B          wSize;          /* Static   */
    PFRE_STATUS     pStatus;        /* Output   */
    INT2B           nReturns;       /* Output   */
    PFRE_SRCH       pEntList;       /* Output   */
}
RE_SEARCH_IO,
*PRE_SEARCH_IO,
NEAR *PNRE_SEARCH_IO,
FAR *PFRE_SEARCH_IO;


/* String Table Object.
 * The String Table is used to convert integers to strings.  Integers are 
 * used to signify information in many parts of the API and internally.  
 * Integers conserve space & make better comparators than strings, but are
 * cryptic for human viewing.  The String Table is provided to convert
 * integers into human-readable interpretations.  OEMs may pass integers
 * from API functions to REstrtab() and receive a string, with an option of
 * forms (abbreviated, non-abbreviated, verbose, etc.).  The return string
 * should be suitable for inclusion in end-user display.
 *
 * A string has a class and an index.  The class is a grouping, to tie like
 * strings together, such as error messages, markup macros, etc.  Within a
 * class, each string has a unique index.  Indexes may be reused across
 * classes, however.  A string's class must usually be inferred by the OEM.
 * E.g., the pStatus->nSysError is an error message, so its class is
 * inferred to be the Error Message Class.  There are defines for each class.
*/

typedef struct RE_strtab_io
{
    INT2B           nStrClass;      /* Input    */
    INT2B           nStrIndex;      /* Input    */
    INT2B           nLanguage;      /* Input    */
    UINT2B          mDialect;       /* Input    */
    INT2B           nDBID;          /* Input    */
    INT4B           lSerialNum;     /* Static   */
    PFRE_STATUS     pStatus;        /* Output   */
    PFUCHAR         pString;        /* Output   */
}
RE_STRTAB_IO,
*PRE_STRTAB_IO,
NEAR *PNRE_STRTAB_IO,
FAR *PFRE_STRTAB_IO;

typedef struct RE_token
{       PFUCHAR      pText;
        UCHAR        uLength,
                     uStyle,
                     nXRefID;
        FLAG         bBreakFlags,
                     bAttributes;
}
RE_TOKEN,
*PRE_TOKEN,
NEAR *PNRE_TOKEN,
FAR *PFRE_TOKEN;


typedef struct RE_token_io
{       INT4B             lEntNum;
        INT2B             nDBID;
        PFRE_FLDFLT       pFldFilter;
        FLAG              bFlags;
        UCHAR             nSearchTerms;
        UINT2B            wTextLen;
        PFUCHAR           pTermList[8];      /* Should be NUM_SEARCH_WORDS */
        RE_OPERATION      nSearchOp;
        INT4B             lSerialNum;
        UINT2B            wSize,
                          wStartToken;
        INT2B             nToc;
        PFRE_STATUS       pStatus;
        INT2B             nReturns;
        PFUCHAR           pTextBuf;
        PFRE_TOKEN        pTokenBuf;
}
RE_TOKEN_IO,
*PRE_TOKEN_IO,
NEAR *PNRE_TOKEN_IO,
FAR *PFRE_TOKEN_IO;


/* The I/O structure for initializing/using an IF PD.
*/
typedef struct _PD_DB_IO
{
    /* Input
    */
    INT2B   nAppID;                             /* the IF application ID    */
    INT2B   nIFDBID;                            /* the parent IF DB         */
    UCHAR   sPdOpenFileName[REMAXFILENAME];     /* the input disk filename  */
    SLPATH  xPdOpenPath;                        /* the disk file location   */
    UCHAR   sPdCloseFileName[REMAXFILENAME];    /* the input disk filename  */
    SLPATH  xPdClosePath;                       /* the disk file location   */
    SLPATH  xTempPath;                          /* disk location for temp files*/

    /* Creation Fields -- Set by OEM at PD creation, then autofilled in IO STRUCT.
    */
    UCHAR   sCreatorName [REPD_MAXWORD+4];      /* field for user to self identify*/
    UCHAR   sCreationDate[REMAXDATE];           /* dd-mm-yyyy*/
    UCHAR   sPdName[REPD_MAXWORD+4];            /* field for OEM to uniquely identify each PD */
    UCHAR   sPdDisplayName[REPD_MAXWORD+4];     /* the name to display in menus in the UI*/

    /* Fields for READ-WRITE databases only.
    */
    UCHAR   sEditDate[REMAXDATE];   /*dd-mm-yyyy*/
    UINT2B  wVersionNumber;

    /* Output & Input.
    */
    INT2B   nPDID;

    /* Output
    */
    UINT2B  wIFModelNum;            /* from target IF DB (almanac, dictionary, etc.)*/
    INT4B   lIFSerialNum;           /* from target IF DB (for future reference)     */
    INT2B   nIFLanguage;            /* the language of the IF DB            */ 
    UINT2B  wAccessMode;            /* read only or read/write              */
    UINT2B  wSaveStatus;            /* whether the data loaded was the result of QUICKSAVE */
    INT4B   lTotalEntriesRDIndex;   /* rd index subpd                       */
    INT4B   lTotalEntriesRDText;    /* rd text subpd                        */
    INT4B   lTotalEntriesSDIndex;   /* sd index subpd                       */
    INT4B   lTotalEntriesSDText;    /* sd text subpd                        */
    UINT2B  wMaxWordLen;
    UINT2B  wMaxLineLen;
    UINT2B  wMaxEntWordLen;
    UINT4B  dwMaxRetEnt;

    PFRE_STATUS  pStatus; 
}
REPD_DB_IO,
*PREPD_DB_IO,
NEAR *PNREPD_DB_IO,
FAR  *PFREPD_DB_IO;

typedef struct REPD_edit_io
{
    INT2B       nPDID;
    INT4B       lEntNum;
    UCHAR       sHeadword[REPD_MAXWORD+4];
    UINT2B      wPOS;
    UINT2B      wSize;
    PFUCHAR     pEditText;
    UCHAR       uPercentFull;
    PFRE_STATUS pStatus;
} 
REPD_EDIT_IO,
*PREPD_EDIT_IO,
NEAR *PNREPD_EDIT_IO,
FAR  *PFREPD_EDIT_IO;

#endif /* REAPITYP_H */

/* End of file - REapityp.h
*/
