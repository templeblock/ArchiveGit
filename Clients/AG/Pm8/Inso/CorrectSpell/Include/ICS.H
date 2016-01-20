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
*        ICS.H - Defines for the International CorrectSpell(TM) Software *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
        Description:
        This header contains all defines for the International 
        CorrectSpell(TM) Software. 
        This header should be included by all modules of the ICS. 

        Cautions:
        None.

*-----------------------------------------------------------------------*/
#ifndef ICS_H
#define ICS_H

/* Product identification information */

#define  ICPRODCODE    6        /* The product code        */

#define  ICIDXTYPLO    5        /* The earliest file structure type that
                                    this version of ICS will work with.   */

#define  ICIDXTYPHI    6        /* The latest file structure type that
                                    this version of ICS will work with.   */
/* 6 type databases have the skipping table at the end of each sector */

#define IC_MAX_ENTRIES  5

/* Location of information in the headers */

/* Size of the headers */
#define ICHSIZE 1024

/* Header Format */

/* International CorrectSpell(TM) specific information */

/* Maximum word length of words stored in RAM */
#define ICRAMLEN 32

/* Maximum number of rehashes for the lmcc table */
#define ICLMREHASH 50

/* Maximum number of possible tries to find a postclitic */
#define ICPOSTCOUNT 8

/* Maximum length of a possible postclitic */
#define ICPOSTMAX SFPOSTMAX

/* Maximum length of a possible preclitic */
#define ICPREMAX SFPREMAX

/* Maximum rehashes for cache table */
#define MAX_CACHE_REHASH 10

/* Length of one cache init entry */
#define CACHE_INIT_ENTRY_SIZE 6

/* Flags indicating process of verification (icctlbuff.h -> verify_flags) */

#define ICNFCCHVER   0x0400            /* Found in cache as "non-word" */
#define ICNFCCHADD   0x0200            /* Added to cache as "non-word" */
#define ICFNDHE      0x0100            /* Found by Helyes-e          */
#define ICFNDRAM     0x0080            /* Found in RAM               */
#define ICFNDDISK    0x0040            /* Found on Disk              */
#define ICFNDCACHE   0x0020            /* Found in the Cache Table   */
#define ICFNDMORFO   0x0010            /* Found in Morpho Verfier    */
#define ICFNDCOMP    0x0008            /* Found Through Compounding  */
#define ICFNDALT     0x0004            /* Found Alternative Spelling */
#define ICFNDINV     0x0002            /* Found but Invalid Spelling */
#define ICADDCACHE   0x0001            /* Added to the Cache Table   */

/* Attributes describing the input word (sfctrl.h -> attribute) */
#define ICAPOS       0x0400            /* Word internal apostrophe     */
#define ICPRECLT     0x0100            /* Pre  Characters Stripped     */
#define ICPOSTCLT    0x0080            /* Post Characters Stripped     */
#define ICDOUBLES    0x0040            /* Uppercase Double SS          */
#define ICUSERWILD   0x0020            /* Personal Dictionary Wildcard */
#define ICREMACCENT  0x0010            /* Modified accent to verify    */
#define ICMLTACC     0x0008            /* Multiple Accents in word     */
#define ICEXCACC     0x0004            /* Exception Accent             */
#define ICFNDACCENT  0x0002            /* Found through Accent Folding */
#define ICORDNUM     0x0001            /* Ordinal Number Identified    */

/* Status codes for Compounding (pctlbuff->pcmpbuff-> comp_flags) */

#define IC_MAND_HYP_COMP   0x8000      /* Compound Mandatory Hyphen     */
#define IC_DIAER_COMP      0x4000      /* Diaeresis Compound            */
#define IC_OK_TRPLE_COMP   0x2000      /* Valid Triple Compound         */
#define IC_OK_DBLE_COMP    0x1000      /* Valid Double Compound         */
#define IC_LEADHYP_COMP    0x0800      /* Valid Leading Hyphen Compound */
#define IC_TRAILHYP_COMP   0x0400      /* Valid trailing Hyphen Compound*/
#define IC_COMP_NORMAL     0x0200      /* Valid Compound                */
#define IC_EXCEPTION_COMP  0x0100      /* Exceptional Compound          */
#define IC_HYPHEN_ADD_COMP 0x0080      /* Hyphen Added for Verificaiton */
#define IC_DBL_DEL_COMP    0x0040      /* Deleted Char to make double   */
#define IC_DBL_ADD_COMP    0x0020      /* Added Char to make triple     */
#define IC_LNK_DEL_COMP    0x0010      /* Removed Invalid Link          */
#define IC_LNK_ADD_COMP    0x0008      /* Added Valid Link              */
#define IC_NF_LNK_ADD_COMP 0x0002      /* Failed Link Added             */
#define IC_LINK_ERR_COMP   0x0001      /* Link Error Compound           */

/* The size of the stack depends on the maximum nesting level */
#define ICMAX_LMCC 15

/* Longest link in any of the tables */
#define IC_MAX_LINK_LEN 5

/* Task flags for Identifying a valid LMCC (ICcmplmcc)                        */
#define IC_LMCC1   1                   /* Check for LMCC Initial cap */
#define IC_LMCCEND 3                   /* End marker                 */

/* LMCC Supplementary Hyphen Positions */
#define ICLMCCLAST 0x0001              /* LMCC hyphen point after last char */
#define ICLMCCSCND 0x0002              /* LMCC hyphen point after second */
                                       /* to last char */
#define ICLMCCTHRD 0x0004              /* LMCC hyphen point after third  */
                                       /* to last char */

#define ICRETWILD 9

/* Application ID for Personal Dictionary support */
#define ICPDWPID    1

/* Size of a cache table page */
#define PAGESIZE 32768

/* Sentence position for capcode checking */
#define IC_MUSTBESTART  1     /* Current word starts a new sentence */
#define IC_MAYBESTART   2     /* May or may not be a new sentence   */

#ifdef ICS_UCF_CORRECT
#define MAXHH 6  /* maximum num of hh/slash in a word */
#endif

#endif /* ICS_H */

/* End of file ICS.h */
