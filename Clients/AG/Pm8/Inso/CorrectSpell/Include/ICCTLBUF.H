/****************************************************************************
 *                   Copyright  1995  INSO Corporation                      *
 *                International CorrectSpell[TM] Software                   *
 *                                                                          *
 * This work is protected by United States and international copyright laws *
 * and contains certain trade secret information.  All use of this work is  *
 * governed by the terms of agreements between INSO Corporation and its     *
 * licensees or potential licensees.  Disclosure or disassembly of the      *
 * contents of embodied programs or algorithms is prohibited.  All rights   *
 * are reserved worldwide.                                                  *
 ****************************************************************************
 
*-----------------------------------------------------------------------*
*                                                                       *
*    ICctlbuf.h - Typedef for the control buffer.                       *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
    Description:
    This header contains the typdef for the control buffer for the 
    International CorrectSpell(TM) Software.  This header should be 
    included by all modules of the ICS and by the User Application.

    Cautions:
    None.

*-----------------------------------------------------------------------*/
#ifndef ICCTLBUF_H
#define ICCTLBUF_H

#include "ics.h"

#ifdef ICS_PDICTIONARY
#include "pdlibhdr.h"
#endif

#ifdef ICS_HYPHENATION
#include "ih.h"
#include "ihctlbuf.h"
#include "ihbuff.h"
#include "ihhypext.h"
#else 
typedef VOID FAR *PFIHBUFF;
#endif

#ifdef ICS_FINNISH_MORPHO
#include "cmtmp.h"
#include "cmics2.h"
#if defined(WINDOWS) || defined(WIN32)
#define   ICFINDLLFUN "Morfo" /* function export name from Finnish DLL */
#endif
#ifdef OS2
#define   ICFINDLLFUN "MORFO" /* function export name from Finnish DLL */
#endif
#endif

#if defined ICS_HUNGARIAN || defined ICS_POLISH
#include "he_ic.h"
#if defined(WINDOWS) || defined(WIN32)
#define   ICHUNDLLFUN "IChu"  /* name of entry function to Hungarian DLL */
#endif
#ifdef OS2
#define   ICHUNDLLFUN "ICHU" /* function export name from Hungarian DLL */
#endif
#endif

/*---------------------------------------------------------------------
 * STRINGPTR - An array of string pointers.
 *---------------------------------------------------------------------
*/
typedef PFUCHAR FAR *PFSTRINGPTR;

SLF_TYPDEF_OR (SLF_HDL_OBJ, PFSTRINGPTR, _ORF_STRINGPTR_IC) ORF_STRINGPTR_IC;

#ifdef ICS_PREV_WORD_CHECK
/**********************************************************************
   previous word structure information
 **********************************************************************/

typedef    struct    _PREVWORDBUF
{
     UCHAR prev_word[ICMAX + 1];      /* Previous word        */
     UCHAR prev_strip[ICSTRIPMAX+1];  /* Previous poststrip   */
     UCHAR uPrevCapCode;              /* Input capcode of previous word */
     INT2B prev_bits;                 /* Previous db bits     */
}
PREVWORDBUFF,
FAR * PFPREVBUFF;
#endif

#ifdef SF_IPR
typedef struct _IPRBUFF 
{
    UINT2B  pDbBits[IC_MAX_ENTRIES]; /* Database Bits for the proof reader */
                             /*   CD_OD        xx000000 00000000   */
                             /*   Ligature     00xx0000 00000000   */
                             /*   Gender       0000xx00 00000000   */
                             /*   Number       000000xx 00000000   */
                             /*   Abbreviation 00000000 x0000000   */
                             /*   Rank 1       00000000 0x000000   */
                             /*   Lex NP       00000000 00x00000   */
                             /*   Ambiguous    00000000 000x0000   */
                             /*   LMCC         00000000 0000x000   */
                             /*   RMCC         00000000 00000x00   */
                             /*   A/AN         00000000 000000xx   */
#ifdef ICS_CLITIC_PROCESS
    INT2B   nClitData;       /* Clitic Information                 */                               
#endif

#ifdef ICS_COMPOUND_PROCESS
    INT2B   nCompData;       /* Compound Information               */
                             /*   SFLMCC and SFRMCC are used.      */
                             /*   See sfdef.h.                     */
#endif

    UCHAR   uLink_pos;       /* position of link (== lmcc length)  */
    UCHAR   uLmcc_cap;       /* dictionary cap code of lmcc        */
    UCHAR   uRmcc_pos;       /* position of link                   */
    UCHAR   pWord[ICMAX + 1];/* Verified word                      */
                             /* Capitalization codes.              */
    UCHAR   pCapCode[IC_MAX_ENTRIES];
    UCHAR   uNumEnt;
} 
IPRBUFF, 
NEAR *PIPRBUFF,
FAR *PFIPRBUFF;
#endif   /* If ICS IPR */

#ifdef ICS_COMPOUND_PROCESS

/**********************************************************************
   Compound Processing Structures 
 **********************************************************************/

/**********************************************************************
   LMCC Structure Information
 **********************************************************************/

typedef struct lmcc 
{
    UCHAR   lmcc_alt[ICMAX + 1];      /* LMCC Alternate Spelling  */
    CHAR    lmcc_hyp;                 /* Extra LMCC hyphen point  */
    INT2B   length;                   /* Length of the lmcc       */
    INT2B   link_code;                /* The link code            */
    INT2B   zero_link;                /* Zero linking element     */
    INT2B   exc_code;                 /* Exception code           */
    INT2B   dbl_code;                 /* Double/triple code       */
    UCHAR   cap_code;                 /* Cap code of lmcc         */
    SF_MAP  cap_map;                  /* LMCC capitalization map  */
    INT2B   db_flags;                 /* Mandatory hyphen code & Complex word */
    INT2B   exc_zero;                 /* Zero exception link flag */
    SF_MAP  hmap;                     /* LMCC hyphen map          */
} 
LMCC, 
NEAR *PLMCC;

/**********************************************************************
   LMCC and RMCC Structure Information
 **********************************************************************/

typedef    struct    _ICCMPBUFF
{
    LMCC    stk[ICMAX_LMCC];          /* lmcc stack                */
    INT2B   link_pos;                 /* position of link          */
    INT2B   rmcc_pos;                 /* position of rmcc          */
    UCHAR   lmcc_cap;                 /* lmcc cap code             */
    SF_MAP  lmcc_cmap;                /* LMCC capitalization map  */
    SF_MAP  lmcc_hmap;                /* lmcc hyphen map           */
    INT2B   lmcc_db_flags;            /* lmcc database flags       */
    CHAR    lmcc_extrahyp;            /* Extra 3 bits of hyphenation for LMCCs */
    UCHAR   lmcc_alt[ICMAX + 1];      /* lmcc alternate spelling   */
    UCHAR   rmcc_cap;                 /* rmcc cap code             */
    SF_MAP  rmcc_hmap;                /* rmcc hyphen map           */
    INT2B   rmcc_db_flags;            /* rmcc database flags       */
    CHAR    rmcc_extrahyp;            /* Extra 3 bits of hyphenation for RMCCs */
    UCHAR   rmcc_alt[ICMAX + 1];      /* lmcc alternate spelling   */
    UCHAR   alt[ICMAX + 1];           /* alternate string          */
    INT2B   alt_link_pos;             /* position of link in alt-string */
    INT2B   alt_rmcc_pos;             /* position of rmcc in alt-string */
    INT2B   comp_flags;               /* Compounding flags         */
} 
ICCMPBUFF, 
NEAR *PICCMPBUFF,
FAR *PFICCMPBUFF;
#else
typedef VOID NEAR *PICCMPBUFF;
#endif    /* If Compound Process */


/* The definition for the control buffer */

typedef    struct    _ICCTLBUFF
{
    INT2B   verify_flags;     /* Indicates the process by which a word */
                              /* verified.  See ic.h for description   */
    INT2B   iLside;           /* Beginning of stem in input word       */
    INT2B   iRside;           /* Ending of stem in input word          */
    INT2B   iLastMode;        /* Language Mode defined in sllang.h    
                                 saved from last call to ICspl().
                                 If different then clear the cache.    */
    UINT2B  vtab_siz;         /* Primary Verification table size       */
    UINT2B  stab_siz;         /* Secondary Verification table size     */
    UINT2B  iDbClass;         /* Database Version */

    INT4B   cchinit_siz;      /* Cache initialization data size        */

#ifdef ICS_CACHE_CHECK
    INT2B   cache_rehash;     /* Max number of rehashes for cache tab  */
    INT4B   cache_ents;       /* Cache Number of Entries               */
    ORF_BUFF ofcache_tab[4];  /* Pointer to the Cache table            */

#ifdef ICS_BACK_COMPATIBLE
#if (defined ICS_COMPOUND_PROCESS || defined ICS_CLITIC_PROCESS || defined SF_IPR)
    ORF_BUFF ofcachescnd_tab; /* Pointer to the Cache secondary table  */
#endif
#endif

    INT2B   iLastDialect;     /* Language Dialect defined in sllang.h    
                                 saved from last call to ICspl().
                                 If different then clear the cache.    */
#endif

#ifdef ICS_BACK_COMPATIBLE
#ifdef ICS_RAM_VERIFICATION
    ORF_BUFF ofPram_tab;      /* The Primary RAM verification table */
#if (defined ICS_COMPOUND_PROCESS || defined ICS_CLITIC_PROCESS || defined SF_IPR)
    ORF_BUFF ofSram_tab;      /* The Secondary RAM verification table */
#endif
#endif
#endif

#ifdef ICS_PREV_WORD_CHECK
    ORF_BUFF ofdbl_tab;       /* Pointer to double word table          */
                          /* Information about previous word */
    PFPREVBUFF pfprev_buff; 

#endif
        
#ifdef ICS_COMPOUND_PROCESS
    UINT2B   uMinLmcc;        /* Shortest lmcc */     
    UINT2B   uMaxLmcc;        /* Longest lmcc */     
    UINT2B   exc_siz;         /* Exception table size               */
    ORF_BUFF oflink_tab;      /* Pointer to the link table             */
    ORF_BUFF ofzero_tab;      /* Table used to set the zero link and   */
                              /* the exception zero link variables     */
    ORF_BUFF ofexc_tab;       /* Pointer to the exception table        */
#endif

    ORF_BUFF  ofPtrTab;       /* Object ref. to pointer table          */
    ORF_BUFF  ofStrings;      /* Object ref. to compression strings    */
    ORF_BUFF  ofStarts;       /* Object ref. to start of compression strings  */
    ORF_BUFF  ofLengths;      /* Object ref. to length of compression strings */

#ifdef SF_COMPOUND_PROCESS
    ORF_BUFF  ofCompTab;      /* Object ref. to compound table         */
#endif

#ifdef SF_XTRA_DECODE
    ORF_BUFF  ofStepTab;      /* Object ref. to step table             */
    ORF_BUFF  ofStrTab;       /* Object ref. to string substitution table */
    ORF_BUFF  ofRiggedTab;    /* Object ref. to rigged table           */
    ORF_STRINGPTR_IC ofBadString;  /* Object ref. to left hand strings */
    ORF_STRINGPTR_IC ofGoodString; /* Object ref. to right hand strings */
    ORF_BUFF  ofQuadTab;      /* Object ref. to quadgram table         */
#endif

#ifdef ICS_CLITIC_PROCESS
    ORF_BUFF ofCliticTab;     /* Object ref. to clitic table           */
#endif

  PFIHBUFF      phypbuff;     /* Pointer to hyphenation buffer        */

#ifdef ICS_FINNISH_MORPHO
  PFRET_BUF  pmbuf;           /* Pointer to Finnish morpho. buffer    */
#endif

#if defined ICS_HUNGARIAN || defined ICS_POLISH
  PFHUBUFF  pfHUbuff;        /* pointer to Hungarian buffer           */
#endif

#ifdef ICS_PDICTIONARY
  PFPD_WORD_IO pWordIo;   /* Personal Dictionary I/O Structure */
#ifdef PD_ALTWORD_SUPPORT
  PFPD_ALT_IO pAltIo;   /* Personal Dictionary I/O Structure */
#endif
#endif

  PFSF_DCTRL    pDCtrl;       /* Static control     - Not For OEM use   */
  PFSF_DVCTRL   pDVCtrl;      /* Dynamic control    - Not For OEM use   */
  PICCMPBUFF    pcmpbuff;     /* Compounding buffer - Not For OEM use   */

#ifdef SF_IPR
  PFIPRBUFF     piprbuff;     /* Pointer to the IPR buffer         */
#endif

  ORF_BUFF      oflanginfo_tab;/* Array containing all possible lang. info */

  FLAG caller_id;
  FLAG colon_flag;        /* Trailing Colon flag              */

#ifdef MAC_HANDLES
  FLAG bHndlLck;          /* True if handles are currently locked */
#endif

#ifdef ICS_CORRECTION
  UCHAR        byLastCorTask; /* Last correction task performed */
  UCHAR        corword [ICMAX + 1];
#endif


} 
ICCTLBUFF, 
FAR *PICCTLBUFF,             /* Buffer and pointer to a buffer        */
FAR *PFICCTLBUFF;

/**********************************************************************
   If Clitic Processing is defined: Spanish flags for clitics. 
   Used in ICclt, ICcltstm and ICcltacc.
 **********************************************************************/

#ifdef ICS_CLITIC_PROCESS

typedef struct _ICSPFLAGS 
{
    FLAG stem_s;               /* stem ends in 's' */
    FLAG modstem;              /* stem ends in 's'; added for ver. only */
    FLAG foldacc;              /* accent folding */
} 
ICSPFLAGS, 
NEAR *PICSPFLAGS;

#endif   /* If Clitic Process */

#endif /* ICCTLBUF_H */

/* End of file ICctlbuf.h */
