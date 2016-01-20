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
 * REdefine.h - OEM input defines for the Reference Engine.                 *
 *                                                                          *
 *--------------------------------------------------------------------------*


    Description:
    This header contains definitions internal to the Reference Engine.

    Creation Date:
    12-apr-89   JGH

Change Record:
    26-apr-90   JCC     do add'l steps after a hit
    03-may-90   JCC     do a minimum # of steps, as per KCSL fax of today
    09-may-90   JCC     hash index table for boolean searching
    22-jun-90   JCC     Added VALID_CAP macro, bool search
    06-feb-91   JCC     max headword length in DB header, not a define
    01-apr-91   JCC     single-char bool operators here for OEM changeability
    24-may-91   JCC     scrollable wildcarding
    21-feb-92   RAS     ED names -> RE
    02-apr-92   ECH     Trimmed for the new API.  ECH05
    21-aug-92   ECH     Moved API defines to REapidef.h.

*---------------------------------------------------------------------------*/

#ifndef REDEFINE_H

#define REDEFINE_H

#define RE_TOC_CACHE_SIZ (1024) /* //64 //18 //10 */ 

/* Comment the following out to disable the toc cache */
#define RE_TOC_CACHE_ENABLE


/* Maximum lengths of input expressions.
*/
#define RE_MAX_HRT_ENTRIES  8   /* Max number of entry numbers returned     */


#define RE_ALT_INPUT_MAX_DIF 3      /* Maximum difference in bytes between  */
                                    /* the length of input and the length   */
                                    /* of the returned spelling alt. for    */
                                    /* that input                           */
/* Maximum length of a filename
*/
#define FNAMEMAX            128     /* filename plus path                   */
#define FNAMEXT             14      /* fname 8 + dot 1 plus ext. 3 + null 1 */
                                    /* = 13 + 1 for Mac pascal string pad   */
                                    /* = 14                                 */

#define PDMAXALT 20 /* max number of spelling alts returned from the PD */
#define REPDPID 1   /* hard code this PD process ID for now */

/* Size of the SF header for all RE databases.
*/
#define RE_SF_HEADER_SIZ    1024

/* An entry number used for stubs
*/
#define DUMMYENT            -1

/* An index number used for stubs
*/
#define DUMMYIDX            -1

/* Special cases for Capcode filtering in REhrtini()
 * Use RE_DEFAULT for normal CapCode Filtrering.
*/
#define RE_GERMTHES_CAPCODE 1       /* German Thesaurus.                    */

/* Mode of opereation for REgetini() and REgetstp().
*/
#define RE_GETDAT_GET       1
#define RE_BOLSCH_GET       2

/* Part of speech test macros
*/
#define REisverb(pos) (POS_TO_POSCLASS_RE (pos) == VERB_POSCLASS_RE)

#define REMAXCACHEPOS       40            /* Cache 40 positions within the entry    */
#define REFSA_ACCEPT        0x2000        /* Accept state flag for fsa tokenizer    */
#define REFSA_MAXSTRING     16            /* Reserve sixteen one byte codes         */

#define REFSA_STRING        0             /* Text                                   */
#define REFSA_SGML          1             /* An SGML Token                          */
#define REFSA_MACRO         2             /* A character macro                      */
#define REFSA_PARTWORD      3             /* Interior Part of a word                */
#define REFSA_WORD          4             /* The final part of a word               */
#define REFSA_SGMLATTR      14            /* The attribute part of an SGML Macro    */
#define REFSA_ERROR         15            /* A Tokenizing Error                     */

#define REFSA_FIRSTID       0x1000        /* First Token Identifier                 */
#define REFSA_FCHMACRO      0x1C00        /* Character Macro Token                  */
#define REFSA_LCHMACRO      0x1FFF        /* Range (first & last)                   */
#define REFSA_NEWLINE       REFSA_FIRSTID /* A newline                              */

#define REGRM_FIRSTID       0x4000        /* First NonTerminal ID                   */
                                          
#define REGRM_NILTOKEN      0xFFFF        /* Tokens in the grammar                  */
#define REGRM_EOFTOKEN      0xFFFE
#define REGRM_DATTOKEN      0x0001

#define REACT_STOP          0x00
#define REACT_PUSH          0x01
#define REACT_POP           0x02
#define REACT_SETATTRIB     0x03
#define REACT_DELATTRIB     0x04
#define REACT_ADDATTRIB     0x05
#define REACT_XREFON        0x06
#define REACT_XREFOFF       0x07
#define REACT_NOBREAK       0x08
#define REACT_HIDEEMIT      0x09
#define REACT_NEWLINE       0x0A
#define REACT_USELINE       0x0B
#define REACT_HIDELINE      0x0C
                         /* 0x0D - 0x0E available
                         */     
#define REACT_EXSETSTYLE    0x0F                                 
#define REACT_SETSTYLE      0x10

#define REACT_EMITNUM       0x20
#define REACT_EMITUROM      0x30
#define REACT_EMITLROM      0x40
#define REACT_EMITULET      0x50
#define REACT_EMITLLET      0x60
                         /* 0x70 - 0x7F available
                         */
#define REACT_SETNUM        0x80
#define REACT_ADDNUM        0x90
#define REACT_DELNUM        0xA0
                         /* 0xB0 - 0xBF available
                         */
#define REACT_EMITSTR       0xC0 
                         /* 0xC0 - 0xFF are taken for strings
                         */

#define RE_BOLD_ATR         0x01
#define RE_ITALIC_ATR       0x02
#define RE_ULINE_ATR        0x04
#define RE_STRIKE_ATR       0x08
#define RE_SUPER_ATR        0x10   
#define RE_SUB_ATR          0x20
#define RE_SMCAPS_ATR       0x40
#define RE_INVISIBLE_ATR    0x80

#define RE_LINE_BRK         0x01
#define RE_WORD_BRK         0x02
#define RE_STYLE_BRK        0x04
#define RE_ATTRIB_BRK       0x08
#define RE_SRCHWRD_BRK      0x10
#define RE_KEYWD_BRK        0x20
#define RE_CHMACRO_BRK      0x40
#define RE_SRCHWRD_FLG      0x80
#define RE_MAKEWORD_BRK     0x80        /* Used internally */

#define REPRS_NUMREGISTERS  16
#define REPRS_STACKSIZE     64
#define REPRS_MAXSAVE       16

/* For Circular Indexes, the two highest bits in the POS code.
*/
#define RE_PRIM_CIRCLE_FORM 0x80
#define RE_SCND_CIRCLE_FORM 0x40


#define QA_ASSERT
#ifdef QA_ASSERT
#include "assert.h"
#define SF_ASSERT(_expression) assert(_expression)
#else /* not qa_assert */
#define SF_ASSERT(_expression)
#endif /* not qa_assert */

#ifdef RE_SF_PDICTIONARY  /* LRSV23 */

/* the number of extra bytes in PD for our implementation */
#define RE_EXTRA_BYTES 12

/* the max number of PDs that can be attached to an IF instance */
#define RE_MAX_PD 12
#define RE_MAX_PD_SUBPDS 2
#define RE_MAX_PD_TABLE (RE_MAX_PD * RE_MAX_PD_SUBPDS)

/* modes for PD edit text syntax checking */
#define PROC_INDEX 1
#define PROC_DSDS 2
#define VERIFY 3

#endif /* RE_SF_PDICTIONARY */ /* LRSV23 */

#endif /* REDEFINE_H */

/* End of file - REdefine.h
*/

