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
 * REctrl.h - Control structures for the virtual and real databases.        *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:
   Type definitions for the real and virtual database control structures.
   Type defines for some of the underlying structures as well.
   Type defines for the object referencers for these structures.

Creation Date:
    29-oct-91   ECH     From REdbbufs.h

Change Record:
    Date        Author  Comment
    08-nov-91   JCC     TOCHA support
    08-jan-92   ECH     RE_FN_ENTRY CTO. ECH04
    21-feb-92   RAS     ED names -> RE
    23-mar-92   ECH     Add the API status structure pointer to PVCtrl. ECH05
    26-may-92   ECH     SF_DECODE changes.  ECH06
    19-jun-92   JCC     Single file reference works
    27-aug-92   JCC     Clitic hash table
    16-sep-92   JCC     Partial line retrieval - wkbufSiz removed
    18-sep-92   JCC     str_tab made immediate (ifStrSubsTab)
    21-Oct-92   ECH     Restructure TOCHA.
    10-mar-93   PJM     modified DM_db_ctrl & DM_vdb_ctrl str. declarations
*---------------------------------------------------------------------------*/

#ifndef RECTRL_H
#define RECTRL_H


#include        "sfbfile.h"
/* String table object reference.  Each string of different length possibly, so
 * store pointers to strings.
*/

/******************************************************************
*
*  CONTROL STRUCTURES
*  The control structures for the reference engine have grown
*  to be quite large.  There are three main structures, two defined
*  here and one in the App Manager.  Imbedded structures should be
*  used to isolate fields used for similar functions or actions.
*  This will modularize the data without overhead.
*
*******************************************************************
*/

/*------------------------------------------------------------------
 * GETINFO - Information used for getting data from main entry
 *           bytes by bytes
 *------------------------------------------------------------------
*/

#ifdef RE_SF_ENTRY_VERIFICATION
typedef struct RE_GETINFO
{
    INT4B       entn;           /* Entry number for current data        */
                                /* retrieval                            */
    UINT2B      ent_offset;     /* Offset in the current entry          */
    ORN_BUFF    wkbuf;          /* transfer buffer from internal to OEM */
    UINT2B      wWorkBufSiz;    /* Size in chars of above               */
    INT2B       iRtvStatus;     /* retrieval status - more to decompress? */
    UINT2B      wPosition;      /* Character position to start retrieval  */
}
GETINFO_RE,
NEAR *PN_GETINFO_RE,
FAR  *PF_GETINFO_RE;

/* Object references for getinfo.
*/
SLN_TYPDEF_OR (SLN_HDL_OBJ, PN_GETINFO_RE, ORN_getinfo_re) ORN_GETINFO_RE;
SLF_TYPDEF_OR (SLF_HDL_OBJ, PF_GETINFO_RE, ORF_getinfo_re) ORF_GETINFO_RE;

SLN_ITYPDEF_OR (PN_GETINFO_RE, ORN_igetinfo_re) ORN_IGETINFO_RE;
SLF_ITYPDEF_OR (PF_GETINFO_RE, ORF_igetinfo_re) ORF_IGETINFO_RE;
#endif

/*--------------------------------------------------------------------
 * BOOLINFO - This is the structure used to evaluate a boolean search
 *            expression.  There are a lot of parallel arrays, each
 *            element representing one operand of the expression.
 *---------------------------------------------------------------------
*/

#ifdef RE_SF_BOLSCH
typedef struct RE_BOOLINFO 
{
    ORN_BUFF    prepExp;        /* Preparsed expression.                */
                                /* & of operands of expr. in storage[]  */
    PUCHAR      string[NUM_SEARCH_WORDS];
                                /* Length of each operand.              */
    UCHAR       length[NUM_SEARCH_WORDS];
                                /* Set if word is key (see note).       */
    FLAG        key[NUM_SEARCH_WORDS];
                                /* If word in sblock; 1 bit/word.       */
    ORN_FLAG    sblock_present;
                                /* Set if word present in entry.        */
    UINT2B      present[NUM_SEARCH_WORDS];
    INT2B       num_strings;    /* Number of operands.                  */
                                /* ascii operands.                      */
    UCHAR       storage[NUM_SEARCH_WORDS * 20];
    UCHAR       *next;          /* Where to store op'nd in storage[].   */
                                /* Compiled search request.             */
    UCHAR       command[NUM_SEARCH_WORDS * 3];
}
BOOLINFO_RE,
NEAR *PN_BOOLINFO_RE,
FAR  *PF_BOOLINFO_RE;

SLN_TYPDEF_OR (SLN_HDL_OBJ, PN_BOOLINFO_RE, ORN_boolinfo_re) ORN_BOOLINFO_RE;
SLF_TYPDEF_OR (SLF_HDL_OBJ, PF_BOOLINFO_RE, ORF_boolinfo_re) ORF_BOOLINFO_RE;

SLN_ITYPDEF_OR (PN_BOOLINFO_RE, ORN_iboolinfo_re) ORN_IBOOLINFO_RE;
SLF_ITYPDEF_OR (PF_BOOLINFO_RE, ORF_iboolinfo_re) ORF_IBOOLINFO_RE;
#endif


/*---------------------------------------------------------------------
 * BOOLSTAT - Data that was previously declared as static.  The fields
 *            in this structre represent variables that must retain
 *            their value from call to call.
 *---------------------------------------------------------------------
*/

#ifdef RE_SF_BOLSCH
typedef struct RE_BOOLSTAT
{
    INT2B       sblock_ptr;     /* Block of n entries (member of starts[])  */
                                /* Formatted input expression               */
    INT4B       file_loc;       /* Start of sblock, disk data, n-bit codes. */
    INT4B       final;          /* End of sblock, disk data, n-bit codes.   */
    INT4B       current_entry;  /* Being decompressed during full eval.     */
    ORN_CHAR    ok_codes;       /* Interesting codes.                       */
    ORN_BUFF    decomp_buff;    /* Sliding window of decompressed data.     */
    ORN_BUFF    headwd_buff;    /* Save for output if match.                */
    INT2B       headwd_len;     /* Length of above word.                    */
}
BOOLSTAT_RE,
NEAR *PN_BOOLSTAT_RE,
FAR  *PF_BOOLSTAT_RE;

SLN_TYPDEF_OR (SLN_HDL_OBJ, PN_BOOLSTAT_RE, ORN_bstat_re) ORN_BSTAT_RE;
SLF_TYPDEF_OR (SLF_HDL_OBJ, PF_BOOLSTAT_RE, ORF_bstat_re) ORF_BSTAT_RE;

SLN_ITYPDEF_OR (PN_BOOLSTAT_RE, ORN_ibstat_re) ORN_IBSTAT_RE;
SLF_ITYPDEF_OR (PF_BOOLSTAT_RE, ORF_ibstat_re) ORF_IBSTAT_RE;
#endif


/*---------------------------------------------------------------------
 * TOC leafnode (headword) list retrieval data struct., for getting a list
 * of headwords from definitions within a subsection of the reference work.
 * Used by REtoclst().
 *---------------------------------------------------------------------
*/

typedef struct _RE_TOCLST
{
    INT4B       lStart;     /* First hwd in range to retrieve.              */
    INT4B       lStop;      /* Last hwd in reange to retrieve.              */
#ifdef RE_FN_ENTRY
    ORF_GETINFO_RE
                oRtvDat;    /* Decompression work buffer & status.          */
#endif
}
TOCLST_RE,
FAR *PF_TOCLST_RE,
NEAR *PN_TOCLST_RE;

/* Object Referencer Declerations.
*/
SLF_TYPDEF_OR (SLF_HDL_OBJ, PF_TOCLST_RE, ORF_toclst_re) ORF_TOCLST_RE;
SLF_TYPDEF_OR (SLF_HDL_OBJ, PF_TOCNODE_RE, ORF_tocnode_re) ORF_TOCNODE_RE;


/*---------------------------------------------------------------------
 * STRINGTAB - An array of string pointers.
 *---------------------------------------------------------------------
*/

typedef PFUCHAR FAR *PFSTRINGTAB;

SLF_TYPDEF_OR (SLF_HDL_OBJ, PFSTRINGTAB, ORF_stringtab_re) ORF_STRINGTAB_RE;

SLF_ITYPDEF_OR (PFSTRINGTAB, ORF_istringtab_re) ORF_ISTRINGTAB_RE;


/*------------------------------------------------------------------------
 * STRXLT - String Translation Table, for getting strings corresponding to
 *      integers coming out of the API/Database.  An array of these is paired
 *      with a block of concatenated strings.
 *------------------------------------------------------------------------
*/
typedef struct _STRXLT_RE
{

    INT2B   nClass;     /* Class of a string (grouping)                 */
    INT2B   nIndex;     /* Index of a string (unique within class)      */
    UINT2B  nStr1Off;   /* Offset into strblk of 1st string of class/index  */
    UINT2B  nStr2Off;   /* Offset into strblk of 2nd string of class/index  */

}
STRXLT_RE,
FAR * PF_STRXLT_RE;

SLF_TYPDEF_OR (SLF_HDL_OBJ, PF_STRXLT_RE, _OF_STRXLT_RE) OF_STRXLT_RE;


/*------------------------------------------------------------------------
 * FILEINFO - A structure that mirrors the OEM input structure.  It keeps
 *            contains strings for the names of the input files.
 *------------------------------------------------------------------------
*/

typedef struct RE_FILEINFO
{
    SLPATH  path;               /* The directory path for the db files  */
    UCHAR   fname[SLFNAMEMAX];  /* Physical file's name + extension     */
}
FILEINFO_RE,
FAR *PF_FILEINFO_RE;

/*------------------------------------------------------------------------
 * POSCACHE - A structure that caches previous states during sequential 
 *                          processing.
 *            (used in Tokenizer and Table-of-Contents functionality)
 *------------------------------------------------------------------------
 */
typedef struct
{        UINT2B        wCurFilled,                /*        Number of entries currently in the cache        */
                        wPosShift,                /*        Shift to get cache index from position      */
                        wMaxSave,                /*        Maximum Number of that can be saved                        */
                        wSizeEntry;                /*        Size of a cache entry                                                */
        VOID FAR *pCacheEntry;        /*        Pointer to the first cache entry                        */
}
POSCACHE_RE,
FAR *PF_POSCACHE_RE;
SLF_TYPDEF_OR(SLF_HDL_OBJ, PF_POSCACHE_RE, ORs_poscache_re) ORF_POSCACHE_RE;

/*------------------------------------------------------------------------
 * COMPPOS - A structure that describes the position during decompression
 *           of a file.  dwCodeOffset is the code being unravelled, and
 *           wByteOffset is the number of bytes to skip.
 *------------------------------------------------------------------------
 */
typedef struct RE_COMPPOS
{        INT2B        nCodeOffset,  /* Codes to skip (offset from dwFirstCode)  */
                      nByteOffset,  /* Bytes to skip                            */
                      nIsMarkup;    /* Type of Markup                           */
}
COMPPOS_RE,
FAR* PF_COMPPOS_RE;

#ifdef        RE_SF_TOKEN

/*------------------------------------------------------------------------
 * FSAENTRY - A structure that contains one entry in a state for the FSA. 
 *            wClassOffset points to a string which contains the matching
 *            characters for this state, and wNewState contains the next
 *                          state to goto when the input character matches one of those
 *                          pointed to by wClassOffset.
 *------------------------------------------------------------------------
 */
typedef struct RE_FSAENTRY
{        UINT2B        wClassOffset,
                        wNewState;
}
FSAENTRY_RE,
FAR *PF_FSAENTRY_RE;

/*------------------------------------------------------------------------
 * FSATABLE - A structure that contains the state machine for FSA token 
 *            matcher.  pEntry[0] is the first entry of the first state.
 *                          Each state is terminated by an entry whose wClassOffset
 *                          member points to a NIL string.  The wClassOffset member of
 *                          each entry is an index into pClassTab, which is a table of
 *                          strings shared by all states.
 *------------------------------------------------------------------------
 */
typedef struct RE_FSATABLE
{   UINT2B        wNumEntrys,
                        wClassTabLen;
        PFUCHAR        pClassTab;
        FSAENTRY_RE
                        pEntry[1];
}
FSATABLE_RE,
FAR *PF_FSATABLE_RE;

SLF_TYPDEF_OR (SLF_HDL_OBJ, PF_FSATABLE_RE, ORSfsaTableRe) ORF_FSATABLE_RE;

typedef struct RE_TOKENINFO
{
    UINT4B  dwEntryNum;
    UINT2B  wTokenNum;
    UINT2B  wBufLen;
    UCHAR   pBuffer[1];
}
TOKENINFO_RE,
NEAR *PN_TOKENINFO_RE,
FAR  *PF_TOKENINFO_RE;

SLN_TYPDEF_OR (SLN_HDL_OBJ, PN_TOKENINFO_RE, ORN_tokeninfo_re) ORN_TOKENINFO_RE;

/*------------------------------------------------------------------------
 * PARSETBL - A structure that contains the predictive parsing table
 *                  structures. 
 *------------------------------------------------------------------------
 */

typedef struct RE_GOTO
{        UINT2B        wTokenOff,        /*        Offset to tokens in TokenTab        */
                        wProdOff;        /*        Offset to production in ProdTab        */
}        GOTO_RE, NEAR * PN_GOTO_RE, FAR *PF_GOTO_RE;

typedef struct RE_NONTERM
{        UINT2B        nProd,                /*        Number of productions                                */
                        wGotoOff,        /*        Offset to Productions in GotoTab        */
                        wExpCmdOff,        /*        Offset to expand command in CmdTab        */
                        wRedCmdOff;        /*        Offset to reduct command in CmdTab        */
}        NONTERM_RE, NEAR * PN_NONTERM_RE, FAR * PF_NONTERM_RE;

/*        Define types for Production, Token Class and Command Tables        */
typedef UINT2B PROD_RE, FAR *PF_PROD_RE;
typedef UINT2B TCLS_RE, FAR *PF_TCLS_RE;
typedef UCHAR CMD_RE, FAR *PF_CMD_RE;

typedef struct RE_PARSETBL
{        UINT2B                wNumNT,                        /*        Number of Non-Terminals                */
                                wNumGoto,                /*        Number of Goto Tbl Entries        */
                                wLenProdTab,        /*        Length of ProdTab                        */
                                wLenCmdTab,                /*        Length of CmdTab                        */
                                wLenTokenTab;        /*        Length of TokenTab                        */
        PF_GOTO_RE        pGotoTab;                /*        Ptr to Goto Function Table        */
        PF_PROD_RE        pProdTab;                /*        Ptr to Production List Tbl        */
        PF_TCLS_RE        pTokenTab;                /*        Ptr to Token List (class)        */
        PF_CMD_RE        pCmdTab;                /*        Ptr to list of cmds                        */
        NONTERM_RE        pNonTerm[1];        /*        Variable Len Array of NT        */
}        PARSETBL_RE,
        NEAR *PN_PARSETBL_RE,
        FAR *PF_PARSETBL_RE;        

typedef RE_TOKEN PQ_ELEMENT_RE,
FAR * PFPQ_ELEMENT_RE;

SLF_TYPDEF_OR(SLF_HDL_OBJ, PF_PARSETBL_RE, ORSparseTblRE) ORF_PARSETBL_RE;

/*        The following structure saves the state of the parse after each call to
 *        to REtoken().
 */
typedef struct RE_ParseState
{
        INT2B        Stack[REPRS_STACKSIZE];
        INT2B        Top;          /* Current top of stack                  */
        INT2B        Symbol;       /* Current grammar symbol                */
        INT2B        nInToken;     /* Count of tokens read from REtknize    */
        INT2B        nOutToken;    /* Count of tokens written by REtoken    */
        INT2B        nXRefID;
        UCHAR        Attrib[8];                
        UCHAR        Numbers[REPRS_NUMREGISTERS];
        UCHAR        OldStyle;
        UCHAR        bHideEmit;    /* Hide the next EMIT command, if TRUE.  */            
        UCHAR        bUseLine;     /* Pass/Hide entry file newline chars.   */
        UCHAR        bNoBreak;     /* NoBreak FLAG set in AEG               */
}        
PARSE_RE,
FAR *PF_PARSE_RE;

#endif

#ifdef RE_SF_PDICTIONARY  /* LRSV23 */
/* the following is used to correlate internal PD entry numbers with
 * positions in the IF DB STN space 
 * The PDID is the index in the array.  Each index increment corresponds
 * to the max pd range.
 */
typedef struct _STNTAB_RE
{
    /* the array index is the pdid returned from pd*/
    INT2B nEntryType; /* sd or rd */
    INT2B nIFPDID;
    UINT2B wTextPDID; /* when deleting IF # must check sd, rd both */
    UINT2B wPrecedence;  /* lrsv23-1 */
}STNTAB_RE,
 FAR *PF_STNTAB_RE;
#endif /* RE_SF_PDICTIONARY */ /* LRSV23 */

/*------------------------------------------------------------------------
 * VDB_CTRL - The following structure is the main control structure for
 *            an INSTANCE of a database.  In it, or below it, all instance
 *            related data from the predecary structures is located.
 *------------------------------------------------------------------------
*/

typedef struct RE_vdb_ctrl
{
    PFRE_STATUS status;         /* Pointer to the API status stru, for  */
                                /* all error reporting.                 */
    FLAG        fldflt;         /* True if there is an active FLDSEL flt*/

#ifdef RE_SF_ENTRY_VERIFICATION
    ORF_GETINFO_RE
                oGetInfo;        /* Obj reference to data retrieval stru */
    /* Variables for saving work from previous call in REdecomp().
    */
    UINT4B      dwEntryNum;     /* Previous call was for this entry     */
    INT4B       dwCodeSave;     /* Offset in codes into the DB          */
    INT2B       wByteSave;      /* Offset in chars into the present code*/
#endif
#ifdef RE_SF_TOKEN
        ORN_TOKENINFO_RE
                oTokenInfo;
#endif

    INT2B       block;          /* Block that was last used             */
    INT2B       nextblk;        /* No. of block remaining to be checked */

    SL_FHANDLE  fd;             /* Physical file descriptor             */

                                /* Table of dictionary entry numbers    */
    UCHAR       letters_inp[RE_LETTER_BUF_SZ];

    ORF_BUFF    oCapCode;       /* Array of cap codes.                  */
    ORFSF_MAP   oCapMap;        /* Array of cap maps.                   */

    INT4B       dwCodeOffset;   /* Offset of entry into definition file */
    INT2B       num_alt;        /* The number of alternatives stored    */

    INT2B       len_dict;       /* Length of original dictionary word   */
    INT2B       len_inp;        /* Length of original input word        */

    ORF_BUFF    list;           /* Correction alternatives list.        */
    UINT2B      list_size;      /* Number of characters in ->list       */

    INT2B       wild_min;       /* The min. possible length of pattern  */
    INT2B       wild_max;       /* The max. possible length of pattern  */

#ifdef RE_SF_OTHER_DECODE

    ORF_2BINT   cor;            /* pointers to correction alternatives  */

    INT2B       init_char;      /* Number of initial chars assumed      */

    INT2B       delta_len;      /* Maximum difference in length allowed */
    INT2B       lev_vowel;      /* Number vowel corrections word        */
    INT2B       lev_con;        /* Number consonant corrections word    */
    INT2B       lev_string;     /* Number string corrections word       */
    INT2B       num_sectors;    /* Number of quadgrams to examine       */
#endif 

    INT2B       lev_total;      /* Total number corrections total word  */

#ifdef RE_SF_FLDSEL
    INT2B        numFields;     /* size of below                        */
    ORF_FIELD_RE fields;        /* Field structs and grammar            */
#endif

#ifdef RE_SF_BOLSCH

    ORN_BOOLINFO_RE
                oBoolInfo;      /* Boolean search control structure.    */
    ORN_BSTAT_RE
                oBoolStat;      /* Boolean static data.                 */
                                /* A boolean expression string          */
    CHAR        boolStr[RE_BOOLEXP_MAXLEN];
    UCHAR       boolWhole;      /* whole word or substring search       */
    UCHAR       boolCap;        /* Capitalization option                */
    INT2B       bsrchStatus;    /* State of the search operation.       */
#endif

#ifdef RE_FN_TOCHA
    ORF_FLAG    oDefFilter;      /* Default filter.                     */
    ORF_BUFF    oDefScope;       /* Default Scope.                      */
#ifdef RE_OVERRIDE
    ORF_BUFF    oOvrScope;       /* Override Scope                      */
#endif /* re_override */

    PFUCHAR     pCurScope;       /* Pointer to current exclusion stru.      */
    FLAG        bAllSelected;    /* are all entries selected in cur scope   */
    ORF_TOCLST_RE
                oTocLst;         /* Headword listing structure.             */
    INT4B       lNumTocEntry;    /* Count of number of entries valid in TOC */
    INT4B       lCurTocEntry;    /* COunt of entries already found in TOC   */

    /* Control of the internal, SP-independent stack for processing TOC
    */
    INT2B       nTOCStackCount;
    ORF_BUFF    aTOCStack;
    INT4B       lCacheOff;     /* The offset where the cache starts in the table */
    ORF_BUFF    oTocBitCache;  /* Memory for the TOC bitfield cache (if virtual) */
#endif /* RE_FN_TOCHA */

    ORFSF_DVCTRL    oDVCtrl;                /* Decode Structure (Dynamic, Virtual)  */
    ORN_BFILE       oBitFile;               /* Bit File Structure                                        */
    ORF_POSCACHE_RE oPosCache;              /* Cache of saved entry positions                 */

#ifdef RE_SF_TOKEN
    ORF_POSCACHE_RE oParseCache;            /* Cache of saved parse positions                */
    PARSE_RE        ParseState;             /* Current Parsing State                                */
    PQ_ELEMENT_RE   Queue[RE_PQ_MAXLEN];    /* pattern-matching queue       */
    INT2B       PHeadPQ;        /* current head position of the pattern matching queue */
    INT2B       PTailPQ;        /* current tail position of the pattern matching queue */
    INT2B       nSizePQ;        /* number of tokens currently in the queue  */ 
#endif
    UINT4B      dwFirstCode;    /* Offset to first code in an entry                */
    ORF_BUFF    oDecodeSector;  /* Buffer for decode.                       */

#ifdef RE_SF_PDICTIONARY 
    INT2B       nNextPrecedence;  /* the next available precedence no */
    STNTAB_RE   aPDstnTable[RE_MAX_PD_TABLE]; /* STN to PD no. translation table */
#endif /* RE_SF_PDICTIONARY */


#ifdef        RE_SF_INTERRUPT
        SF_ABORTPROC        fnAbortProc;
#endif
}
VDB_CTRL_RE,
FAR *PFVDB_CTRL_RE;

typedef SLF_HDL_OBJ HFVDB_CTRL_RE;

SLF_TYPDEF_OR (HFVDB_CTRL_RE, PFVDB_CTRL_RE, OR_vdb_ctrl_re) ORFVDB_CTRL_RE;


/***********************************************************************
*
*  The following structure contains all DATABASE related data.  The
*  data in this structure, or the strucutures below it, pertains to
*  real databases and is unaffected by the manipulations on any virtual
*  database.
*
***********************************************************************/

typedef struct DM_db_ctrl
{
    FILEINFO_RE fileNames;      /* The names of the db files.           */

    OF_LOGFILTAB_RE ofLogFilTab;/* logical files comprising this refwork*/
    INT2B       wLogFilTabSiz;  /* size of above                        */

#ifdef RE_SF_PDICTIONARY  /* LRSV23 */
    INT2B       nPDAPPID;       /* the id of the single open pd app     */
#endif /* RE_SF_PDICTIONARY */ /* LRSV23 */
#ifndef RE_SF_1RDB
    INT4B       lSerialNum;     /* A unique integer for each real db.   */
    UINT2B      mModelNum;      /* An integer for the electronic work.  */
#endif
    INT2B       nRefType;       /* The type of reference work (gen.org) */
    UCHAR       mAttrib[4];     /* DB Attribute Bitmaps. type dependant */
    INT2B       nIdxEntMap;     /* The entry naming type.               */
    INT2B       nLanguage;      /* Language of this reference work      */
    UINT2B      mAvailDialect;  /* Available dialects for this ref. work*/
    FLAG        bSrchIdx;       /* Is there a Boolean Search Index?     */

    ORF_BUFF    starts;         /* Starting points for the search       */

/* #ifdef RE_FN_STRTAB  */
    OF_STRXLT_RE ofStrXltTab;   /* String Translation Table             */
    UINT2B      nNumXltStrs;    /* Number of Xlatable strings in above  */
    ORF_BUFF    ofStrXltBlk;    /* Strings pointed to by above          */
    UINT2B      wMaxStrXltStr;  /* Longest string in the table          */
/* #endif    RE_FN_STRTAB   */

    INT4B       lRAMSize;  

/* JCCRE9 */
    /* Next four elements are used to search the index list for a wordform.
     * Index list is made of sectors.  Each sector starts with a word, then
     * following words use that as a seed.  Sectors are alpha-ordered.
     * ptr_tab contains the 1st 'n' characters of the 1st ("key") word of
     * each sector.  Is most efficient to restart encoding with new key (seed)
     * every so often, to make getting a word far away from 'aardvark' quick.
     * biskey is the value of 'n', so ptr_tab is made of biskey-character 
     * substrings.  bislmt is the number of n-char substrings in ptr_tab and
     * therefore the number of sectors in the index list.  bislmt's
     * value grows when there are more words in wordlist or more info 
     * encoded for each wordform.  bislmt is used when doing a binsrch on
     * ptr_tab to find the sector in which a wordform might appear.  
     * block_siz is the # bytes per sector (new key/seed is used when size of 
     * encoded data > block_siz).  The matching index into ptr_tab from a 
     * binsrch is multiplied by block_siz to get to the sector of encoded 
     * data in the disk file.
    */
    ORF_BUFF    idx_ptr_tab;    /* 1st 'n' chars of ea. sector's 1st wrd*/
#ifdef RE_SF_CIRCLE_INDEX
    ORF_4BINT   idx_nptr_tab;   /* Table w/ index record offset/block   */
#endif
    INT2B       idx_biskey;     /* Value of 'n' above                   */
    INT2B       idx_bislmt;     /* # sects in idx list (# lmts,  ptrtab)*/
    INT2B       idx_block_siz;  /* # bytes per sector in idx list       */

    ORF_BUFF    idx_strings;    /* The decompression codes              */
    INT2B       idx_strings_siz;/* The size of above table              */
    ORF_BUFF    idx_starts;     /* Pointer to start of code in cmp_strin*/
    ORF_BUFF    idx_lengths;    /* Lengths of decompression strings     */
    INT2B       idx_num_codes;  /* Size of above two tables             */
    INT2B       idx_bits;       /* Number of bits in compression code   */
    INT2B       idx_explicit;   /* Number of explicit compression codes */

#ifdef RE_SF_BOLSCH
    /* Next four elements are used to search the index list for a wordform.
     * See note above for main index list.
    */
    ORF_BUFF    src_ptr_tab;    /* 1st 'n' chars of ea. sector's 1st wrd*/
    INT2B       src_biskey;     /* Value of 'n' above                   */
    INT2B       src_bislmt;     /* # sects in idx list (# elmnts ptrtab)*/
    INT2B       src_block_siz;  /* # bytes per sector in idx list       */

    ORF_BUFF    src_strings;    /* The decompression codes              */
    INT2B       src_strings_siz;/* The size of above table              */
    ORF_BUFF    src_starts;     /* Pointer to start of code in cmp_strin*/
    ORF_BUFF    src_lengths;    /* Lengths of decompression strings     */
    INT2B       src_num_codes;  /* Size of above two tables             */
    INT2B       src_bits;       /* Number of bits in compression code   */
    INT2B       src_explicit;   /* Number of explicit compression codes */
#endif  /* RE_SF_BOLSCH */

    UINT2B      decomp_siz;     /* Size of above table                  */

    INT4B       end_pos;        /* Max file offset in definitions file  */
    INT4B       last_etn;       /* Last entry number in this database   */

    ORF_BUFF    left;           /* Definitions decompression table      */
    ORF_BUFF    right;          /* Definitions decompression table      */

    INT2B       num_subs;       /* Number of string substitution pairs  */

    INT2B       maxWordLen;     /* Maximum length of a word in idx list */
    INT4B       maxEntLen;      /* Maximum length of an entry           */
    INT2B       maxLineLen;     /* Maximum length of a line in entries  */
    INT2B       maxEntWordLen;  /* Max. len of a word returned w/entnum */
    INT2B       maxRetEnt;      /* Max. # of entnums returned w/input wd*/

#ifdef RE_SF_OTHER_DECODE
    ORF_BUFF
                quad_tab;       /* Pointer to quadgram table            */

    INT2B       l_size;         /* Size of left side table              */
    INT2B       f_offset;       /* Offset of table of first quadgrams   */
    INT2B       l_offset;       /* Offset of left side table            */
    INT2B       r_offset;       /* Offset of right side table           */

#endif /* RE_SF_OTHER_DECODE */

#ifdef RE_FN_ALTERN
    ORF_BUFF    rigged_tab;     /* Rigged table.                        */
    ORF_IBUFF   ifStrSubsTab;   /* String substitution table            */
    ORF_BUFF    step_tab;       /* Correction Step Table                */
    ORF_STRINGTAB_RE
                goodstring;     /* Right hand strings                   */
    ORF_STRINGTAB_RE
                badstring;      /* Left hand strings                    */
    ORF_BUFF    len_good;       /* Length of each left hand string      */
    ORF_BUFF    len_bad;        /* Length of each right hand string     */
#endif /* RE_FN_ALTERN */

#ifdef RE_CLITIC_PROCESS
    UINT2B      wCltTabSiz; /* Size, in bytes, of the clitic hash table */
    ORF_BUFF    ofCltTab;   /* Hash table of clitics                    */
#endif  /* RE_CLITIC_PROCESS */    

    INT2B       num_bits;       /* Number of bits in decompression      */
    INT2B       search_shift;   /* Log2 of number of st nums per block  */
    INT2B       numStartBlks;   /* Number of starts blocks in database  */

#ifdef RE_SF_FLDSEL
    INT4B       fld_pos;        /* Absolute position of field sel data  */
    INT2B       numFields;      /* Size of the fields & names arrays    */
    ORF_FLDNAME_RE
                fldNames;       /* Names of the fields in this DB.      */
#endif

#if defined(RE_FN_TOCHA) || defined(RE_SF_TOKEN)
    INT2B       nNumTocs;       /* Size of toc hierarchy tree - # nodes */
    UINT2B      wVisTocs;       /* Number of visible nodes.             */
    ORF_TOCNODE_RE
                oTocStru;       /* Internal TOC Hierarchy.              */
    ORF_TOCSECT_RE
                oTocSect;       /* DBIO TOC Visible Hierarchry.         */
#endif
#ifdef RE_FN_TOCHA 
    INT4B       lTocFieldSize;  /* The size of the entire bitfield table */
    INT4B       lCompOff;       /* The offset in the table where compression starts */
    UINT2B      wBitMapSiz;     /* Largest bitmap size including compressed */
    ORF_BUFF    oTocBitField;   /* Memory for TOC bitfield table, if not virtual. */
    UINT2B      wBitCacheSiz;   /* Size of the cache */
    INT4B       lBitFileOffset; /* The offset in the data file where bitfield begins */
#endif  

    INT2B       def_explicit;   /* Number of explicit definition codes  */
    ORF_BUFF    def_strings;    /* Explicit codes for definitions       */
    INT2B       def_strings_siz;/* Size of above explicit codes         */
    INT2B       st_separator;   /* Code number separating entries       */

#ifdef RE_SF_TOKEN
    ORF_FSATABLE_RE
                oFsaTab;
    ORF_PARSETBL_RE
                oParseTab;
#endif
    INT4B       lEntryOffset;   /* Offset to start of entry file        */

    ORFSF_DCTRL oDCtrl;         /* Decode structure (Static, Real)      */
#ifdef RE_DRV_TESTENV
    FLAG        bWBT;
#endif
}
DB_CTRL_RE,
FAR *PFDB_CTRL_RE;

typedef SLF_HDL_OBJ HFDB_CTRL_RE;

SLF_TYPDEF_OR (HFDB_CTRL_RE, PFDB_CTRL_RE, OR_db_ctrl_re) ORFDB_CTRL_RE;

#endif /* RECTRL_H */

/* End of file - REctrl.h
*/
