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
*    SFctrl.h - Typedef for the control structures.                     *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
 
    Creation Date:
        13-apr-92   ECH & GJC                

    Change Date:
        24-jun-92   ECH     Added SF_ENTLIST.
        03-feb-93   TH/JCC  Inflection support

*-----------------------------------------------------------------------*/
#ifndef SFCTRL_H

#define SFCTRL_H

/*----------------------------------------------------------------------*
 * SF_VARS - The definition for temporary working variables structure.  *
 *           These are allocated external to the SF.                    *
 *----------------------------------------------------------------------*
*/

typedef    struct    SF_vars
{
    PUCHAR    cur_letter;   /* used by SFDECODE                         */
#ifdef SF_ENT_LIST
    PUCHAR    baseform;     /* used by SFDECODE                         */
#endif
#ifdef SF_XTRA_DECODE
    PUCHAR    cap_letter;   /* used by SFDECODE                         */
    PUCHAR    word;         /* used by SFCORREC & SFwild & SFanagram    */
#endif

    PUCHAR    tempword;     /* used by SFACCENT & sfcorrig & sfcorsrt   */
                            /* & SFcltstp                               */

    PUCHAR    capword;      /* used by SFCHKWORD                        */
    PUCHAR    altword;      /* used by SFCHKWORD & SFcltstp             */
    PUCHAR    altword2;     /* used by SFCHKWORD                        */

#ifdef  SF_COLLATE
    PFSFCOLSEQ  pfSeq;      /* Collation sequences used in SFdecode     */
#endif
}
SF_VARS,
*PSF_VARS;


#ifdef SF_ENT_LIST
/*----------------------------------------------------------------------*
 * SF_ENTLIST - Structure that holds all information returned with      *
 *              entry list operations (entry verification & boolean     *
 *              search).                                                *
 *----------------------------------------------------------------------*
*/

typedef struct SF_entlist
{
    INT4B           lEntNum;        /* Output   */
    PFUCHAR         pWord;          /* Output   */
#ifdef RE_SF_ENTRY_VERIFICATION
    UCHAR           uPOS[SFMAX_POS];/* Output   */
#endif
#ifdef SF_INFLECT
    /* Parts of inflection (feature tags).  Although an index line for an
     * inflected form can only have 1 POI & therefore can be represented as
     * an integer, more than 1 line with that form can point to the same 
     * entry & each line can have different POI (even if same baseform & POS).
     * Therefore, need a POI map rather than an integer (which can represent
     * one and only one POI) because an entry can have > 1 POI.
    */
    SF_POI          mPOI;           /* Output   */
#endif
#ifdef RE_SF_PDICTIONARY  /* LRSV23 */
    /* PD is integrated to IF at a low level and mixed lists of IF
     * and PD items are returned.  We need to know which is which
     * because the headword is not stored in the entry for pd items.
     * and we must NOT call the headword from entry code */
     FLAG bIsPDItem;
#endif /* RE_SF_PDICTIONARY */  /* LRSV23 */
}
SF_ENTLIST,
*PSF_ENTLIST,
FAR *PFSF_ENTLIST;

typedef SLF_HDL_OBJ HFSF_ENTLIST;

SLF_TYPDEF_OR (HFSF_ENTLIST, PFSF_ENTLIST, OR_entlist_sf) ORFSF_ENTLIST;
SLF_ITYPDEF_OR (PFSF_ENTLIST, OR_ientlist_sf) ORFSF_IENTLIST;

#endif


#ifdef SF_II
#define MAX_LANG_INFO_SIZE  11  /*  Set to largest size usable by all products  */
#define MAX_INF_PAT_SIZE    3
 
#define NN_INFL    0
#define VB_INFL    1
#define JR_INFL    2
 
typedef struct SF_lmccinfo
{   UCHAR uLeftLen;
    UCHAR aWordData[MAX_LANG_INFO_SIZE];
}    SF_LMCCINFO, FAR * PFSF_LMCCINFO;
 
#endif
 

/*----------------------------------------------------------------------*
 * SF_DCTRL - The definition for the static (real) decode control       *
 *            structure.                                                *
 *----------------------------------------------------------------------*
*/

typedef    struct    SF_ctrl
{
    UINT2B     wInitStat;       /* Initialization status                    */
    UINT2B     wBlockSize;      /* Disk block size                          */
    UINT2B     wQuadSiz;        /* Size of quadgram table                   */
    INT2B      iIdxType;        /* Index Encoding version                   */
    UCHAR      mIdxAttrib[1];   /* Attribute Map for Index Encoding         */
    INT2B      iIdxFormat;      /* Index Format - ordering of fields/states */ 
    INT4B      lDataOffset;     /* Offset to disk data (skipping hdr & tabs)*/

#ifdef  SF_II
    /*  ICS Databases use a 7 byte language info table, II databases use
     *  a 10 byte table, GCS has 42 bytes.  I've put this in the pCtrl
     *  structure so we can work with the language information data
     *  dynamically in a combined product. [KWB]
     */
    UINT2B      wLangInfoSize;  /* Size of data in lang info table          */
#endif
 
    PFUCHAR    pStrings;        /* The decompression codes                  */
    PFUCHAR    pStarts;         /* Pointer to start of code in cmp_strin    */
    PFUCHAR    pLengths;        /* Lengths of decompression strings         */
    PFUCHAR    pPtrTab;         /* Pointer to the Quadgram pointer table    */
#ifdef SF_CIRCLE_INDEX
    PFINT4B    pNPtrTab;        /* Pointer to the Numeric pointer table     */
#endif
    INT2B      iBislmt;         /* Limit of binary search sector            */
    INT2B      iBits;           /* Number of bits in compression code       */
    INT2B      iExplicit;       /* Number of explicit compression codes     */
    UCHAR      iBiskey;         /* Length of Quadgram binary search key     */
    UCHAR      byLang;          /* Language Code - defined in sllang.h      */
    PFUINT4B   att_tab;         /* character attribute table                */
#ifdef  SF_II
    PFVIO      pfVIO;           /* Master Lexicon File I/O pointer          */
#endif
    SL_FHANDLE wFd;             /* Master Lexicon file descriptor           */
    UINT2B     wMaxWordLen;     /* The maximum length of a word             */
    UINT2B     wMaxRetEnt;      /* The maximum number of entries returned.  */
    FLAG       bUseClitics;     /* Use Clitic Processing, prefix & postfix. */
    UINT2B     wLangInfoSiz;    /* Size of language information table       */
    PFUCHAR    wLangInfoTab;    /* Table containing all possible lang bits  */
 
#ifdef SF_COMPOUND_PROCESS
    UINT2B  wCompSiz;           /* Compound table size                      */
    PFUCHAR pCompTab;           /* Pointer to the compound table            */
#endif
 
#ifdef SF_XTRA_DECODE
    UINT2B  wStrSiz;            /* Size of string substitution table        */
    PFUCHAR pStrTab;            /* String substitution table                */
    UINT2B  wStepSiz;           /* Size of step table                       */
    PFUCHAR pStepTab;           /* Pointer to step table                    */
    UCHAR   byTotalSteps;       /* Total number of correction steps         */
    UINT2B  wRiggedSiz;         /* Size of rigged table                     */
    PFUCHAR pRiggedTab;         /* Pointer to rigged table                  */
    PFUCHAR FAR *pGoodString;
    PFUCHAR FAR *pBadString;
    PFUCHAR pLenGood;
    PFUCHAR pLenBad;
#if defined(SF_ICS) || defined(SF_II) || defined(SF_GCS)
    UCHAR   bySubsOptTab[SFMAX_STRING << 1];
#endif
#endif
 
#ifdef SF_XTRA_DECODE
    PFUCHAR pQuadTab;
#endif
 
#ifdef  SF_COLLATE
    PFSFCOLTAB    pfColTab;         /*  Database Collation Table    */
#endif 
}
SF_CTRL,
FAR *PFSF_DCTRL;
 
typedef SLF_HDL_OBJ HFSF_DCTRL;
 
SLF_TYPDEF_OR (HFSF_DCTRL, PFSF_DCTRL, OR_sf_dctrl) ORFSF_DCTRL;
 
/*-----------------------------------------------------------------------
 * SF_DVCTRL - The definition for the dynamic (virtual) decode control
 *             structure.
 *-----------------------------------------------------------------------
*/
 
typedef struct SF_vctrl
{
    INT2B     iTask;        /* Task to perform:                         */
    INT2B     iFileType;    /* Decode file (ICS index, RE index, Bool)  */
    UINT2B    mCurrDialect; /* Chosen dialect for this database & task. */
 
    PFUCHAR   pDecoSector;  /* The DISK sector to be decoded            */
    INT2B     iLastBlock;   /* Last block read - redundant read guard   */
    INT2B     iDbFlags;     /* Status flags returned from lexicon       */
    PFINT2B   pLangIndx;    /* Array of language information indices    */
    PSF_VARS  pVars;        /* Working variables                        */
 
    INT2B     iNumAlt;      /* The number of alternatives stored        */
    INT2B     iMaxAlt;      /* The maximum number of alternatives to ret*/
    INT2B     iNextAlt;     /* The next available character             */
 
    PFUCHAR   pList;        /* List of returns                          */
    UINT2B     iListSize;    /* Size of list                             */
    PFINT2B   pCorPtr;      /* Array of indexes to list or returns      */
 
    PFUCHAR   pCapCode;     /* Dictionary capitalization code.          */
    PFSF_MAP  pCapMap;      /* Dictionary capitalization map.           */
 
    UCHAR     byInCapCode;  /* Input capitalization code                */
    SF_MAP    iInCapMap;    /* Input capitalization map                 */
 
    INT2B     iNumEnt;      /* Number of entries (caps).                */
    INT2B     iNumBaseEnts; /* Number of entries retrieved by an initial*/
                            /* call to SFdecode(), i.e., not baseforms  */
 
#ifdef SF_BASEFORM
    PFUCHAR   pBaseform;    /* Null-terminated baseform of the inWord.  */
                            /* List of baseforms to look up.            */
    PFUCHAR   pBaseToDo[SFMAX_BASEFORM + 1];
                            /* Parts of speech for each of the prev.    *
                             * 0th elem of 2nd dim. holds amt. of POSs; *
                             * is not a POS code, thus size is max + 1. *
                            */
    UCHAR     uPOSToDo[SFMAX_BASEFORM][SFMAX_POS + 1];
    UCHAR     uPOSOff;      /* Which baseform decode is currently doing */
 
#ifdef SF_INFLECT
                            /* POIs of each baseform                    */
    SF_POI    mBasesPOI[SFMAX_BASEFORM];
#endif
#endif /* SF_BASEFORM */
 
#ifdef SF_ENT_LIST
    PFSF_ENTLIST  pEntList; /* Entry list retrieval array (searches too)*/
#endif
 
#ifdef SF_BASE_CLITIC
    PFSF_CLITIC pClitic;    /* Pointer to clitic structure              */
#endif
 
#ifdef SF_XTRA_DECODE
    PFUCHAR pQuadgram;      /* Pointer to present quadgram              */
    UCHAR   cSectors [SFMAX_QUADS + 1][4];
                            /* Quadgrams to examine for correction      */
    INT2B   iNumSectors;    /* Number of quadgrams to examine           */
    INT2B   iLSize;
    INT2B   iFOffset;
    INT2B   iLOffset;
    INT2B   iROffset;
#endif
 
#ifdef SF_XTRA_DECODE
    INT2B   iDeltaLen;      /* Maximum difference in length allowed     */
    INT2B   iLevTotal;      /* Total number corrections total word      */
    INT2B   iLevVowel;      /* Number of vowel corrections word         */
    INT2B   iLevCon;        /* Number of consonant corrections word     */
    INT2B   iLevString;     /* Number of string corrections word        */
    INT2B   iInitChar;      /* Number of initial characters assumed     */
    INT2B   iPass;          /* Correction pass number                   */
                            /* 1 - simple: transposition, single <->    */
                            /*         geminate                         */
                            /* 2 - normal: all types                    */
                            /* 3 - string substitution on 1st or 2nd cha*/
                            /* 4 - accent folding                       */
                            /* 5 - exception words thru->through etc    */
                            /* 6 - two phonetic errors in initial       */
                            /*     quadgram, one at beginning           */
                            /* 7 - length 4, 5 or 6 characters          */
    SCHAR   cStepNum;       /* Step number being done.                  */
    SCHAR   cExecMax;       /* Maximum step to be executed.             */
    INT2B   iLenDictword;   /* Length of original dictionary word       */
    INT2B   iLenInpword;    /* Length of original input word            */
    INT2B   iQlevTotal;     /* Total number of corrections initial      */
    INT2B   iQlevVowel;     /* Number of vowel corrections initial      */
    INT2B   iQlevCon;       /* Number of consonant corrections init     */
    INT2B   iQlevString;    /* Number of string corrections initial     */
    INT2B   wNumSubs;       /* Number of string substitution pairs      */
    UCHAR   bySubsTab[SFMAX_STRING << 1];
                            /* String substitution for input            */
    FLAG    bInitTry;       /* Initial try has been done                */
    PFINT2B pSim;           /* Similarity Array for sorting cor. rets.  */
#endif                      /* Correction                               */
 
#ifdef SF_PD
    FLAG       bPDUse;
    VOID FAR   *pPDStructs;  /* Pointer to PD wordIO            */
    UINT2B     wPDDID;       /* PD wDID currently being processed */
    UCHAR      byPDCorOrder; /* COrrection Order for Personal Dictionary SD's */
    UINT2B     wPID;         /* Process ID for PD instance */
    UCHAR      byPDNegWrds;  /* TRUE if neg_wrd processing for PD is necessary */
#endif
 
#ifdef SF_ANAGRAM
    UCHAR   byLettersInp[SF_LETTER_BUF_SZ];
    UINT2B  wMinAnaSiz;     /* Minimum for shortest anagram.            */
#endif
 
#ifdef SF_WILDCARD
    INT2B   iWildMin;       /* The min. possible length of pattern.     */
    INT2B   iWildMax;       /* The max. possible length of pattern.     */
#ifdef SF_WILDPAGE
    INT2B      iSubAction;  /* Start, forward, backward, stop           */
    INT2B      wcRange;     /* search range in blocks of current expression */
    INT2B      wc1stSect;   /* starting sector of an expression         */
    ORF_2BINT  wcSects;     /* Sectors of batches of output sets        */
    ORF_2BINT  wcMatches;   /* #s of 1st matches in 1 sectors of batches of output sets */
    INT2B      wcCurBatch;  /* index into above arrays                  */
    FLAG       bWCLast;     /* we are at the last page                  */
#endif
#endif
 
    SF_MAP  iShmap;         /* 64 bit soft hyphenation input map        */
    SF_MAP  iHypmap;        /* 64 bit database mand/hyphenation map     */
    INT2B   iLangDialect;   /* Language Dialect defined in sl.h         */
    INT2B   iLangMode;      /* Language Mode defined in sl.h            */
 
    INT2B   iAttribute;     /* Attribute flags about the root word      */
                            /* See sfdef.h for value descriptions       */
    PFUCHAR pXtraBits;      /* Linguistic information for English and    */
                            /* additional lmcc hyphen point for cmp lang */
    PFUCHAR pFilter;        /* Filter byte                              */
 
    UCHAR   iPrefixStrip;   /* Number of prefix chars stripped in Czech */

    PFINT2B pDbBits;        /* Database bits for each cap code word     */
    PFUCHAR pRegLink;       /* Compound Lang. Regular link low order 4
                               bits hihg order bits are empty           */
    PFUCHAR pExcLink;       /* Compound Lang. Exceptional link, null
                               and terminal double                      */
 
#ifdef SF_BOOLSEARCH
    PFFLAG  pSblockPresent; /* Set if the word is present in the entry  */
    UCHAR   byArgbit;       /* Which operand to use in a Bool search.   */
#endif
 
#ifdef SF_TABLE_OF_CONTENTS
    UINT2B  bSomeExcl;       /* are some excluded - re_newtocha */
    PFUCHAR pExcl;          /* Pointer to exclusion filter.             */
#endif
 
#ifdef SF_RE
    INT2B   iWordNum;       /* Word number within sector                */
#endif
#ifdef SF_CLOSEST_MATCH
    INT2B   iStartWordNum;  /* First word to get from sector            */
    INT2B   iWordsToGet;    /* Number of words to return                */
    INT4B   iStNum;         /* ST number of returned word               */
#endif
#ifdef SF_CIRCLE_INDEX
    INT2B   iNthWord;       /* Offset of word in the block to retrieve. */
    INT2B   iEntOff;        /* Offset into pEntList to place the rets.  */
#endif
 
#ifdef SF_INFLECT
    UCHAR   uInPOS;         /* POS to match inflections to                  */
    SF_POI  mInPOI;         /* POI map to match inflections to              */
    UCHAR   uOutPOI;        /* POI of returned word (integer)               */
    UCHAR                   /* word-init qgrams of a baseform's irreg infls */
        pInflQuad[SFMAXINFLQUAD][SFQUADSIZE];
    INT2B   iInflQuadNum;   /* Current irregular inflection's w.i.q.        */
#endif
 
#ifdef  SF_INTERRUPT
    SF_ABORTPROC    fnAbortProc;
    INT2B           wBlocksToRead;
    INT2B           wBlocksRead;
    FLAG            bUserAbort;
#endif
 
#ifdef SF_II
/* Using Word Data Table of II */
    VFHDL   vfhdl;              /* Handle to use for mapping data       */
    PFUINT2B  aPosSeq;  /* POS Combo tags, unique POS sequences */
    UCHAR   bLangInfoModified;  /*  LangInfo table is modified  */
    UINT2B (FAR * aInflPat)[MAX_INF_PAT_SIZE];  /*  Inflection Pattern Data  */
    PFUINT2B  aDervPat; /*Derivation pattern data */
#ifdef SF_COMPOUND_PROCESS
    PFSF_LMCCINFO pfLMCC; /* far pointer to LMCC structure */
    UCHAR uNumLMCCs;
    UCHAR uMaxLMCCs;
#endif /* SF_COMPOUND_PROCESS */
    PFUCHAR pfWDTEntry; /* Word data table entry for a given word */
#endif /* SF_II*/
}
SF_VCTRL,
FAR *PFSF_DVCTRL;
 
typedef SLF_HDL_OBJ HFSF_DVCTRL;
 
SLF_TYPDEF_OR (HFSF_DVCTRL, PFSF_DVCTRL, OR_sf_dvctrl) ORFSF_DVCTRL;
SLEXTERN UINT4B SFU4MASK[33];
SLEXTERN UINT2B SFU2MASK[17];
 
#ifdef SF_II
/*
This section contains macros and defines related to the II's Word Data Table.
They are implemented such that any changes to the table organization will
only affect this file, within reasonable changes.
Last time we changed inflection pattern from 8-bit to 10-bit which
incvolved redefining type from UCHAR to UINT2B and had effect on code too!
The byte positions in table are not defined but they are hard coded because
that change could easily be incorporated in macro. All the macros start with
WDT to identify the word data table relation and have Get as common too
(unspecified).
Thus the macros are WDTXXXX. They all accept a PFUCHAR to Word Data Table.
 
XXXX could be CapCode, Dialect, HasMandHyphen, IsDerivation, POSCombo,
NounInflectPat, VerbInflectPat, AdInflectPat, DervPat, LMCCLinkLen,
ErrPosition, Field, RestWordFreq, IsLMCC, IsRMCC, IsDouble, IsCompound,
IsNotStandAlone, IsSplitWord etc.
 
The macros can be easily modified  w/o affectiing the code.
 
Author:    Hemant Pathak
Started:   9th May 1994
Modified:
 
Mailny used by: SFdecode (IIdecode) and  SFdecbit.
 
*/
 
SLEXTERN UCHAR  FAR SFaCompLookup[];
 
#define WDTCapCode(Wdt_Ptr)         ((Wdt_Ptr[0]) & 7)
#define WDTDialect(Wdt_Ptr)         ((Wdt_Ptr[0]) & 0xf0)
#define WDTHasMandHyphen(Wdt_Ptr)   (Wdt_Ptr[1] & 0x80)
#define WDTIsDerivation(Wdt_Ptr)    (Wdt_Ptr[1] & 0x40)
#define WDTPOSCombo(Wdt_Ptr)        (((Wdt_Ptr[2] << 2) & 0x0300) | (Wdt_Ptr[3]))
#define WDTNounInflectPat(Wdt_Ptr)  (((Wdt_Ptr[2] << 4) & 0x0300) | (Wdt_Ptr[4]))
#define WDTVerbInflectPat(Wdt_Ptr)  (((Wdt_Ptr[2] << 6) & 0x0300) | (Wdt_Ptr[5]))
#define WDTAdInflectPat(Wdt_Ptr)    (((Wdt_Ptr[2]<< 8 ) &0x0300) | (Wdt_Ptr[6]))
#define WDTDervPat(Wdt_Ptr)         (((Wdt_Ptr[7] << 4) & 0x0ff0) | ((Wdt_Ptr[8] >> 4) &0x000f ))
#define WDTLMCCLinkLen(Wdt_Ptr)     ((Wdt_Ptr[9] ) & 7)
#define WDTErrPosition(Wdt_Ptr)     ((Wdt_Ptr[9] >>3 ) & 0x1f)
#define WDTField(Wdt_Ptr)           ((Wdt_Ptr[10]) & 0x3f) &0x0f /*time being 4-bit */
#define WDTWordFreq(Wdt_Ptr)        ((Wdt_Ptr[1]) & 0x0f)
#define WDTIsRestricted(Wdt_Ptr)    (WDTWordFreq (Wdt_Ptr) == 15)
#define WDTCompIndex(Wdt_Ptr)       ((Wdt_Ptr[8]) & 0x0f)
#define WDTIsLMCC(Wdt_Ptr)          (SFaCompLookup[WDTCompIndex(Wdt_Ptr)] & 0x08 )
#define WDTIsRMCC(Wdt_Ptr)          (SFaCompLookup[WDTCompIndex(Wdt_Ptr)] & 0x04 )
#define WDTIsCompound(Wdt_Ptr)      (SFaCompLookup[WDTCompIndex(Wdt_Ptr)] & 0x02 )
#define WDTIsSplitWord(Wdt_Ptr)     (SFaCompLookup[WDTCompIndex(Wdt_Ptr)] & 0x01 )
#define WDTIsDouble(Wdt_Ptr)        (SFaCompLookup[WDTCompIndex(Wdt_Ptr)] & 0x00 )
#define WDTIsNotStandAlone(Wdt_Ptr) (SFaCompLookup[WDTCompIndex(Wdt_Ptr)] & 0x00 )
 
#endif /*SF_II*/
 
 
#endif /* SFCTRL_H */
 
/* End of file - SFctrl.h */
