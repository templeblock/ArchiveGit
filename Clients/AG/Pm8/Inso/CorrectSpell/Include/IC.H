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
*        IC.h - Defines for the International CorrectSpell(TM) Software *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
        Description:
        This file contains all of the API defines for the 
        International CorrectSpell(TM) Software. 

        This header should be included by all modules of the ICS 
        and by the User Application.

        Cautions:
        None.

        Creation Date:

*-----------------------------------------------------------------------*/
#ifndef IC_H
#define IC_H

/* Task flags */

#define ICINITIALIZE    1       /* Initialize Database  */
#define ICTERMINATE     2       /* Terminate Database   */
#define ICVERIFY        3       /* Perform Verification */
#define ICWILD          5       /* Perform Wildcard Retrieval */
#define ICANAGRAM       6       /* Perform Anagram Processing */
#define ICHYPHENATE     7       /* Perform Pattern Matching Hyphenation */
#define ICUSRWILD       10
#define ICUSRANAGRM     11

/* For Macintosh lock handle, assign pointer OR unlock handle, free pointer */

#define ICUNLOCK        21
#define ICLOCK          22

#define ICCORRECT       100   
#define ICCORMORE       101   

/* Space insertion correction task flag */

#define ICSPACEINSERT   102

/* Personal Dictionary Correction task flags */

#define ICPDCOR         103

/* Unusual combined form Correction task flags */

#define ICUCFCOR        104

/* Initialization status flag definitions (icbuf.h -> init_flags) */

#define ICDLLLDERR  0x8000             /* Error loading DLL                */
#define ICDBNOTSUP  0x4000             /* Code does not supply full functionality 
                                          for the features of the database  */
#define ICHYINTERR  0x2000             /* Hyphenation table Init. Error     */
#define ICLNGEXTERR 0x1000             /* Error init. of external language. */
#define ICWRITEERR  0x0200             /* Write Err                         */
#define ICHYPAVAIL  0x0100             /* Hyphenation initialized           */
#define ICINITOK    0x0080             /* Initialization Success            */
#define ICSEEKERR   0x0040             /* Seek Error                        */
#define ICREADERR   0x0020             /* Read Error                        */
#define ICHEADERR   0x0010             /* Header Mismatch Error             */
#define ICALLOCERR  0x0008             /* Allocation Error                  */
#define ICOPENERR   0x0004             /* File Open Error                   */
#define ICCLOSEERR  0x0002             /* File Close Error                  */
#define ICLANGERR   0x0001             /* Language Mismatch Error           */


/* Dialect status flag definitions (icbuf.h -> dial_flags) */

#define ICMODEERR   0x0020             /* Only 1 mode must be chosen        */
#define ICXTRADIAL  0x0010             /* Warning, multiple dialects selected*/
#define ICNODIAL    0x0008             /* No dialects selected Error        */
#define ICBUSFIN    0x0004             /* Business & finance wrds available */
#define ICLEG       0x0002             /* Legal words available             */
#define ICMEDSCI    0x0001             /* Medical & Science words available */

/* Flags indicating processing during verification (icbuf.h -> process_flags) */

#define ICMUSTUP     0x0800            /* Required sentence initial cap */
#define ICMUSTLOW    0x0400            /* Probably required lower case */
#define ICPERALT     0x0200            /* Neg. Alt. from Pers. Dict.   */
#define ICODDCMPND   0x0100            /* Unusual hard hyphen compound */
#define ICFNDACRONM  0x0080            /* Found acronym w/out periods  */
#define ICFNDHYPALT  0x0040            /* Altrntve hyphenation spelling*/
#define ICPERSONAL   0x0020            /* Found in Personal Dictionary */
#define ICCMPSPLCHG  0x0010            /* Compound spelling change     */
#define ICSOFTHYPH   0x0008            /* At Least one Input Soft Hyph */
#define ICMANDALT    0x0004            /* Mandatory Hyphen Alternative */
#define ICMANDINP    0x0002            /* Mandatory Hyphen Input       */
#define ICSPLWORD    0x0001            /* Split Word Alternative       */

/* Flags indicating errors during verification (icbuf.h -> error_flags) */

#define ICINTPUNCERR     0x20000000        /* Internal Punctuation Error  */
#define IC_PREV_WRD_ERR  0x10000000        /* Previous word had an error  */
#define ICHE_NONCMP      0x08000000        /* Not a compound: break up    */
#define ICHE_6_3         0x04000000        /* 6-3 rule violation          */
#define ICMODSTEM        0x02000000        /* Modified Stem               */
#define ICMODE_ERR       0x01000000        /* error for selected mode (unaccent, io) */
#define ICHHYPERR        0x00800000        /* Hard hyphen error           */
#define ICNUMATTERR      0x00400000        /* Number attachment error     */
#define ICABBRERR        0x00200000        /* Abbrevation error           */
#define ICINVSUFF        0x00100000        /* Invalid Suffix error        */
#define ICMODPOSTCLT     0x00080000        /* Modified Postclitic          */
#define ICINVPUNCT       0x00040000        /* Inappropriate punctuation    */

/* note:  there is a hole here for a couple of new error codes!             */
#define ICDBLEWRD        0x00008000        /* Double word found Error       */
#define ICORDERR         0x00004000        /* Ordinal number Error          */
#define ICAPOSALT        0x00002000        /* Apostrophe Alternative Change */
#define ICFINCHAR        0x00001000        /* Removed Final Char to Verify  */
#define ICINITERR        0x00000800        /* Database Initialization Error */
#define ICCAPERR         0x00000400        /* Capitalization Error          */
#define ICCMPERR         0x00000200        /* Compounding Error             */
#define ICLOCKERR        0x00000100        /* Dereferencing unlocked handles*/
#define ICINVPRE         0x00000080        /* Invalid Precharacters         */
#define ICINVTRL         0x00000040        /* Invalid Trailing Characters   */
#define ICNEGWORD        0x00000020        /* Negative Personal Dictionary word*/
#define ICNOTFRE         0x00000010        /* Not a Free Standing Word      */
#define ICSFTHYPERR      0x00000008        /* Input Soft Hyphen Error       */
#define ICACCERR         0x00000004        /* Accent Error                  */
#define ICPSTINV         0x00000002        /* Inappropriate postclitic      */
#define ICPREINV         0x00000001        /* Inappropriate preclitic       */

/* Flags indicating errors of the previous word (picbuff->lPrevWrdErr) */
/* Need to be checked only if error_flag is set to IC_PREV_WRD_ERR     */

/* 1-28 are for Greek only */
#define IC_APHAERESIS_ERR       1           /* The previous word should end in a vowel or diphthong*/
#define IC_NU_NUDROP_ERR        2           /* The previous word should end with the letter nu (-Á)*/
#define IC_TIN_NUDROP_ERR       3           /* should be full form Ó·Á */
#define IC_DEN_NUDROP_ERR       4           /* should be full form ›ﬁÁ */
#define IC_MIN_NUDROP_ERR       5           /* should be full form Ê·Á */
#define IC_AUTIN_NUDROP_ERR     6           /* should be full form ÷ÚÓûÁ */
#define IC_TON_NUDROP_ERR       7           /* should be full form ÓÈÁ */
#define IC_ENAN_NUDROP_ERR      8           /* should be full form ùÁ÷Á */
#define IC_SE_ELISION_ERR       9           /* should be full form Ïﬁ */
#define IC_ME_ELISION_ERR      10           /* should be full form Êﬁ */
#define IC_APO1_ELISION_ERR    11           /* should be full form ÷Í¢ */
#define IC_APO2_ELISION_ERR    12           /* should be alternative form ÷Û'*/
#define IC_GIA_ELISION_ERR     13           /* should be full form ÿ„õ */
#define IC_KATA1_ELISION_ERR   14           /* should be full form ‰÷Óõ */
#define IC_KATA2_ELISION_ERR   15           /* should be alternative form ‰÷‚'*/
#define IC_META1_ELISION_ERR   16           /* should be full form ÊﬁÓõ */
#define IC_META2_ELISION_ERR   17           /* should be alternative form Êﬁ‚'*/
#define IC_PARA_ELISION_ERR    18           /* should be full form Í÷Îõ */
#define IC_DIA_ELISION_ERR     19           /* should be full form ›„õ */
#define IC_EPI1_ELISION_ERR    20           /* should be full form ﬁÍü */
#define IC_EPI2_ELISION_ERR    21           /* should be alternative form ﬁÛ' */
#define IC_USTERA_ELISION_ERR  22           /* should be full form £ÏÓﬁÎ÷ */
#define IC_MESA_ELISION_ERR    23           /* should be full form ÊùÏ÷ */
#define IC_ALLA_ELISION_ERR    24           /* should be full form ÷ÂÂõ */
#define IC_NA_ELISION_ERR      25           /* should be full form Á÷ */
#define IC_THA_ELISION_ERR     26           /* should be full form ‚÷ */
#define IC_TAU_ELISION_ERR     27           /* should be full form of Ó' corrsponding to ÓÈ, Ó÷ or ÓÈÚ*/
#define IC_EINAI_ELISION_ERR   28           /* should be full form ﬁüÁ÷„ */

/* English */
#define IC_A_ERROR         29         /* Word cannot be preceded by 'a'  */
#define IC_AN_ERROR        30         /* Word cannot be preceded by 'an' */
                                    
/* Return Codes */

#define ICRETALT         2             /* Alternative spelling returned */
#define ICRETINVALID     3             /* Word Found but Invalid        */
#define ICRETPREPROC     4             /* Word Verified by Preprocessor */
#define ICRETFOUND      10             /* Word Found                    */
#define ICRETNOTFOUND   11             /* Word Not Found                */

/* Correction Return Codes    */

#define ICRETCONT       20             /* Continue with correction.  More 
                                          correction steps can be applied  */
#define ICRETDONE       21             /* All corrections strategies allowed
                                          for this task have been exhausted*/
#define ICRETFULL       22             /* The number of correction alternatives
                                          specfied by ICCORMAX has been reached*/

/* TRUE, FALSE, ERRET, and OKRET also used by the system. */




/* Product identification information */

#define  ICSVERSION     3
#define  ICSRELMAJOR    4
#define  ICSRELMINOR    0

/* System dependent file control defines */

#ifdef IBMPC
#define ICFNAMEMAX 64
#else
#define ICFNAMEMAX 128
#endif

/* The maximum length of a word, the buffers are defined as max + 1 */
#define ICMAX           64

/* The maximum number of chars stripped by parser and pre processor */
#define ICSTRIPMAX      ICMAX

/* The maximum number of hard hyphens or slashes in a word */
#define ICMAXHH          6                          

#endif /* IC_H */

/* End of file IC.h */
