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
 * REdecomp.h - Internal defines used by decompression routines.            *
 *                                                                          *
 *--------------------------------------------------------------------------*


    Description:
    This header contains defines for the Reference Engine Software that 
    pertain to decompression of data (during normal decompression and
    searching).

    Creation Date:
    14-jun-90   JCC

Change Record:
    11-mar-91   JCC     temporary ifdef for large DB p.o.s. processing
    01-apr-91   JCC     single-char bool operators moved to REDEFINE.H
    14-nov-91   JCC     bring in defines from REDECOMP.C, RERTVDAT.H
    21-jan-92   JCC     ditto, from REDBDEFS.H
    21-feb-92   RAS     ED names -> RE
    09-jun-92   ECH     New ST delimeter.  ECH06
    23-jun-92   ECH     Added DF fields (SODF_STR, EODF_LEN, etc.)
    26-aug-92   JCC     Character macro content defines

*---------------------------------------------------------------------------*/

#ifndef REDECOMP_H

#define REDECOMP_H

/* REdecomp Related Defines.
*/

/* Work area sizes.
*/
#define BUFF_SIZE       256     /* in bytes of disk input */
#define DECOMP_SIZE     256     /* size of sliding decompression window */
#define RESTACK_SIZE    100     /* max # of right-hand codes to save    */

/* Decompression task flags.  These are passed as parameters to REdecomp.
*/
#define RE_ALL          1
#define RE_HEADWORD     2
#define RE_FIND_PART    3
#define RE_GET_PART     4
#define RE_PRONUNCIATION 5
#define RE_FIELDSEL     6
#define RE_HWDLIST      7


/* higher level entry decompression defines.
*/

/* Values for ->iRtvSatus, other than standard RE_END_OF_DATA and
 * RE_NO_ERROR.
*/
#define RE_GETDAT_CONT  1       /* cont'ing regular (entire) entry rtv'l*/
#define RE_GETPRT_CONT  2       /* cont'ing part-of-speech retrieval    */

/* Data format defines.
*/

/* The char used in the DB for marking syllable breaks in headwords
*/
#define SYLLABLE_BREAK  D_MIDDLE_DOT

/* Note: these could be substituted with the RE_WD field in the db's grammar.
 * To use that, reading in the grammar will have to be linked to TOCHA
 * functionality as well as field selection.
*/
#define SOWD_STR        "<WD>"  /* Start Of headWorD field delimiter    */
#define SOWD_LEN        4       /* length of above, w/o EOS             */
#define EOWD_STR        "</WD>" /* End Of  headWorD field delimiter     */
#define EOWD_LEN        5       /* length of above, w/o EOS             */

#define SOIWD_STR       "<IWD>" /* Start of Indo-Eroupean Headword.     */
#define SOIWD_LEN       5       /* Length of the above string.          */

#define EOLN_STR        "\012"    /* End Of  headWorD field delimiter     */
#define EOLN_LEN        1       /* length of above, w/o EOS             */

#define SODS_STR        "<DS>"  /* Start Of DeFinition field delimiter  */
#define SODS_LEN        4       /* length of above, w/o EOS             */
#define EODS_STR        "\012"  /* End Of  DeFinition field delimiter   */
#define EODS_LEN        1       /* length of above, w/o EOS             */

#define EOSIMP_STR      "ÿ"     /* End Of WD field delimiter in Simps   */
#define EOSIMP_LEN      1       /* length of above, w/o EOS             */

#define SOUP_STR        "<UP>"  /* Start Of sUPerscript field delimiter */
#define SOUP_LEN        4       /* length of above, w/o EOS             */
#define EOUP_STR        "</UP>" /* End Of sUPerscript field delimiter   */
#define EOUP_LEN        5       /* length of above, w/o EOS             */

#define SOCM_STR        "<{"    /* Start of a character macro.          */
#define SOCM_LEN        2
#define EOCM_STR        "}>"    /* End of a character macro.            */
#define EOCM_LEN        2

/* Character macro contents
*/
#define SZCM_STR        "sz"    /* s/z macro, for British English DBs   */
#define SZCM_LEN        2

/* Part of speech tag defines
*/
#define POS_CLASS_OFFSET 12     /* from the beginning of p.o.s. tag     */
#define POS_CLASS_LEN   4       /* of string of p.o.s. class            */
#define POS_TAG_LEN     16      /* of p.o.s. tag string (macro + class) */
#define POS_END_LEN     8       /* </PSSEG>                             */
                                /* starts part of speech field          */
#define POS_MACRO       "<PSSEG><PS>"   


/* Boolean search defines
*/

/* Values for bsrchStatus.  RE_END_OF_DATA is another, for completion of 
 * search.
*/
#define RE_BOOLSRCH_INI     1   /* starting a new boolean search        */
#define RE_BOOLSRCH_CONT    2   /* continuation of search on same exp.  */
#define RE_BOOLSRCH_RSTRT   3   /* restart search with same expression  */

/* These are the constants used for the evaluation of boolean expressions.
 * The expression will be translated into a prefix format with one byte
 * for each operator and operand.  The operator byte identifies the type
 * of operation and the left & right operand types.
*/
#define CALCULATED      ((UCHAR) 1)     /* Low order 2 bits of op. byte */
#define CONSTANT        ((UCHAR) 0)
#define OR              ((UCHAR) 0x00)  /* Values for operator bytes    */
#define AND             ((UCHAR) 0x04)
#define NOT             ((UCHAR) 0x08)
#define BOOLEQUAL       ((UCHAR) 0x0C)

#define OR_OPLEN        1       /* length of '|' operator in bool expr. */
#define AND_OPLEN       1       /* length of '&' operator in bool expr. */
#define NOT_OPLEN       1       /* length of '!' operator in bool expr. */
#define OR_OP_STRLEN    2       /* length of 'OR'  op. in bool expr.    */
#define AND_OP_STRLEN   3       /* length of 'AND' op. in bool expr.    */
#define NOT_OP_STRLEN   3       /* length of 'NOT' op. in bool expr.    */

/* Operators that user will put into an expression.
*/
#define OR_OP_STR       "OR"
#define AND_OP_STR      "AND"
#define NOT_OP_STR      "NOT"

#define RE_BOOLEXP_PLEVEL_MAX   10  /* max level of parenthification in a
                                     * boolean expression
                                    */
#define RE_BOOLEXP_MAXOPS       8   /* max operands in a boolean expression */
#define NUM_SEARCH_WORDS        8   /* Max operands in a bool expression    */

/*
 * decompression defines
*/
/* right table code val means left code is single character 
*/
#define SINGLE_CHAR_CODE -1

/* hashing algorithm defines
*/
#define REHASH_TRIES    50          /* For fitting string into hash table */


/*
 * Generic arithmetic macros used during decompression.  Can be useful in
 * other places as well.
*/
/* 
 * Macros for performing modulus arithmetic & other arithmetic
*/
#define MOD8_MASK       0x0007  /* mask by to perform modulus 8 function */

/* masking (2**n - 1) to a quantity yields the modulus 2**n of the quantity.
 * ex: modulus 64 of 69 -> 64 = 2**6 -> 64 - 1 = 63 = 0x003F -> 69 & 3F = 5
*/
#define MOD_2_TO_N(x,n) (x & (TWO_TO_N(n) - 1))
#define MOD8(n)         (n & MOD8_MASK)   /* modulus 8 function */

#define TWO_TO_N(n)     (1 << n)          /* power of 2 */

/*
 * bit & byte operations
*/

#ifndef WIN32
#define MAKEWORD(lsb,msb)   ((INT2B) (lsb | msb << 8)) /* construct word from 2 byte values */
#endif
#define BITS_TO_BYTES(n)    (n >> 3)    /* divide n bits by 8 to get # bytes */
#define BYTES_TO_BITS(n)    (n << 3)    /* mult n bytes by 8 to get # bits */
#define GET_LOW_N_BITS(b,n) (b & ((1 << n) - 1))  /* return lower n bits of b */
#define B_RIGHT_OFF 8       /* subtract from to bits right offset into byte */
#define BITMASK(n)          ((CHAR)(1 << n))    /* mask bit #n w/in a byte */

#endif /* REDECOMP_H */

/* End of file - REdecomp.h
*/
